#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#include "resgen_tga.h"

#define DECLARE_TEXTURE(name) GLuint texture_ ## name;

resgen_tga_for_each(DECLARE_TEXTURE)

void texture_init();

#endif