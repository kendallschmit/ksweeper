#ifndef KTEX_H
#define KTEX_H

#include <glad/glad.h>

#include "tga_names.h"

#define DECLARE_TEXTURE(name) GLuint texture_ ## name;

FOR_EACH_tga(DECLARE_TEXTURE)

void texture_init();

GLuint texture_load_tga(const char *path);

#endif