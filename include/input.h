#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "vectors.h"

struct input_set {
    bool up;
    bool down;
    bool left;
    bool right;

    struct vec2 cursor_pos;
};

extern struct input_set input;

void input_init(GLFWwindow* window);
void input_set_dimensions(GLuint x, GLuint y);

#endif