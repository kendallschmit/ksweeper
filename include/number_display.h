#ifndef NUMBER_DISPLAY_H
#define NUMBER_DISPLAY_H

#include <stdbool.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine.h"
#include "vectors.h"
#include "draw.h"

#define NUMBBER_DISPLAY_MAX_DIGITS

struct number_display {
    GLint value;
    GLint digits;
    bool origin_right;

    struct vec3 pos;
    struct draw_group group;
    struct draw draws[10];
};

void number_display_init();

void number_display_refresh(struct number_display *nd);
void number_display_destroy(struct number_display *nd);

#endif