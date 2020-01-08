#include "number_display.h"

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#include "kge_util.h"
#include "shader.h"
#include "texture.h"
#include "vao.h"
#include "draw.h"

GLuint tex_n[10];
GLuint tex_blank;

void number_display_init()
{
    tex_n[0] = texture_load("res/tga/n0.tga");
    tex_n[1] = texture_load("res/tga/n1.tga");
    tex_n[2] = texture_load("res/tga/n2.tga");
    tex_n[3] = texture_load("res/tga/n3.tga");
    tex_n[4] = texture_load("res/tga/n4.tga");
    tex_n[5] = texture_load("res/tga/n5.tga");
    tex_n[6] = texture_load("res/tga/n6.tga");
    tex_n[7] = texture_load("res/tga/n7.tga");
    tex_n[8] = texture_load("res/tga/n8.tga");
    tex_n[9] = texture_load("res/tga/n9.tga");

    tex_blank = texture_load("res/tga/nblank.tga");
}

void number_display_refresh(struct number_display *nd)
{
    nd->group.draws = nd->draws;
    nd->group.count = nd->digits;
    for (GLint i = 0; i < nd->digits; i++) {
        GLfloat offset = i / 2.0f;
        if (nd->origin_right)
            offset -= (nd->digits - 1) / 2.0f;
        nd->group.draws[i] = (struct draw){
            .vao = vaos[VAO_TALL],
            .tex = tex_blank,
            .pos = (struct vec3){ nd->pos.x + offset, nd->pos.y, nd->pos.z },
        };
    }
    // Apply value textures
    GLint value = nd->value;
    for (GLint i = nd->digits - 1; i >= 0; i--) {
        if (value == 0 && i != nd->digits - 1)
            nd->group.draws[i].tex = tex_blank;
        else
            nd->group.draws[i].tex = tex_n[value % 10];
        value /= 10;
    }
}