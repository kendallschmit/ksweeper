#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "vectors.h"

struct input_button {
    bool state;
    bool press;
    bool release;
};

struct input_set {
    struct input_button up;
    struct input_button down;
    struct input_button left;
    struct input_button right;

    struct input_button e;
    struct input_button m;
    struct input_button x;
    struct input_button f;

    struct input_button mousel;
    struct input_button mouser;
    struct input_button mousem;

    struct vec2 drag;
    struct vec2 cursor_pos;
    struct vec2 scroll;
};

extern struct input_set input;

extern void input_init(GLFWwindow* window);

extern void input_reset();

#endif