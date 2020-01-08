#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>

#define VAO_QUAD 0
#define VAO_TALL 1
#define VAO_WIDE 2
#define VAOS_MAX 3

extern GLuint vaos[VAOS_MAX];

extern void vaos_init();
extern void vaos_deinit();

#endif
