#include "texture.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "kge_util.h"

#include "tga.h"

struct reader {
    FILE *file;
    uint8_t *buf;
    size_t len;
    size_t pos;
};
size_t reader_read(uint8_t *dest, size_t size, size_t nmemb, struct reader *r)
{
    if (r->file != NULL) {
        return fread(dest, size, nmemb, r->file);
    }
    size_t nbytes = size * nmemb;
    if (nbytes > r->len - r->pos) {
        kprint("Not enough bytes");
        return 0;
    }
    memcpy(dest, &r->buf[r->pos], nbytes);
    r->pos += nbytes;
    return nmemb;
}

static GLuint texture_from_tga(struct reader *r, const char *name);

#define LOAD_TGA(name) do { \
        struct reader r = { .buf = tga_ ## name, .len = sizeof(tga_ ## name)}; \
        texture_ ## name = texture_from_tga(&r, "tga_" # name);\
    } while (0);
extern void texture_init()
{
    FOR_EACH_tga(LOAD_TGA)
}

extern GLuint texture_load_tga(const char *path)
{
    FILE *f = fopen(path, OPEN_FLAGS_READ_BINARY);
    if (f == NULL) {
        kprint("Unable to open tga at \"%s\"", path);
        return 0;
    }
    struct reader r = { .file = f };
    GLuint tex = texture_from_tga(&r, path);
    fclose(f);
    return tex;
}

static GLuint texture_from_tga(struct reader *r, const char *name)
{
    kprint("Loading tga \"%s\"", name);
    uint8_t header_bytes[18];
    if (reader_read(header_bytes, sizeof(uint8_t), 18, r) != 18) {
        kprint("Unable to read header for \"%s\"", name);
        return 0;
    }
    uint8_t id_len = header_bytes[0];
    uint8_t color_map_type = header_bytes[1];
    uint8_t image_type = header_bytes[2];
    uint16_t colomap_fei = (uint16_t)header_bytes[4] * 256 + header_bytes[3];
    uint16_t colomap_len = (uint16_t)header_bytes[6] * 256 + header_bytes[5];
    uint8_t colomap_bpp = header_bytes[7];
    uint16_t width = (uint16_t)header_bytes[13] * 256 + header_bytes[12];
    uint16_t height = (uint16_t)header_bytes[15] * 256 + header_bytes[14];
    uint8_t pixel_depth = header_bytes[16];
    uint8_t img_descriptor = header_bytes[17];
    //kprint("id_len: %" PRIu8 "", id_len);
    //kprint("color_map_type: %" PRIu8 "", color_map_type);
    //kprint("image_type: %" PRIu8 "", image_type);
    //kprint("colomap_fei: %" PRIu16 "", colomap_fei);
    //kprint("colomap_len: %" PRIu16 "", colomap_len);
    //kprint("colomap_bpp: %" PRIu8 "", colomap_bpp);
    //kprint("width: %" PRIu16 "", width);
    //kprint("height: %" PRIu16 "", width);
    //kprint("pixel_depth: %" PRIu8 "", pixel_depth);
    //kprint("img_descriptor: %" PRIu8 "", img_descriptor);
    if (id_len > 0 && (reader_read(NULL, sizeof(uint8_t), id_len, r) != id_len)) {
        kprint("Unable to read id of \"%s\"", name);
        return 0;
    }
    if (colomap_len > 0 && (reader_read(NULL, sizeof(uint8_t), colomap_len, r)
            != colomap_len)) {
        kprint("Unable to read colomap of \"%s\"", name);
        return 0;
    }
    GLfloat pix[width * height * 4];
    size_t px = 0;
    while (px < width * height) {
        uint8_t l; // Run length
        if (reader_read(&l, 1, 1, r) != 1) {
            kprint("Unable to read run length in \"%s\"", name);
            return 0;
        }
        if (l & 0x80) { // If it is a run length
            l &= 0x7f;
            l++;
            uint8_t v[4];
            if (reader_read(v, 1, 4, r) != 4) {
                kprint("Unable to read pixel value in \"%s\"", name);
                return 0;
            }
            while (l--) {
                pix[px * 3 + 0] = (GLfloat)v[2] / 255;
                pix[px * 3 + 1] = (GLfloat)v[1] / 255;
                pix[px * 3 + 2] = (GLfloat)v[0] / 255;
                px++;
            }
        }
        else { // If it is raw
            l &= 0x7f;
            l++;
            uint8_t v[4];
            while (l--) {
                if (reader_read(v, 1, 4, r) != 4) {
                    kprint("Unable to read pixel value in \"%s\"", name);
                    return 0;
                }
                pix[px * 3 + 0] = (GLfloat)v[2] / 255;
                pix[px * 3 + 1] = (GLfloat)v[1] / 255;
                pix[px * 3 + 2] = (GLfloat)v[0] / 255;
                px++;
            }
        }
    }
    GLuint tex; // Name of tex
    glGenTextures(1, &tex); // Generate texture
    glBindTexture(GL_TEXTURE_2D, tex); // Make active TEXTURE_2D
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
            GL_RGB, GL_FLOAT, pix);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    kprint("Loaded tga \"%s\"", name);
    return tex;
}