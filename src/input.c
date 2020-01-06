#include "input.h"

#include "engine.h"
#include "kge_util.h"

struct input_set input = { 0 };

static struct vec2i window_wh;

static void key_callback(
        GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }
    bool *val;
    switch (key) {
        case GLFW_KEY_UP:
            val = &input.up;
            break;
        case GLFW_KEY_DOWN:
            val = &input.down;
            break;
        case GLFW_KEY_LEFT:
            val = &input.left;
            break;
        case GLFW_KEY_RIGHT:
            val = &input.right;
            break;
        default:
            return;
    }
    if (action == GLFW_PRESS) {
        *val = true;
    }
    else if (action == GLFW_RELEASE) {
        *val = false;
    }
}

static void cursor_pos_callback(
        GLFWwindow* window, double x, double y)
{
    x -= window_wh.x / 2;
    y -= window_wh.y / 2;
    x /= DOTS_PER_UNIT;
    y /= DOTS_PER_UNIT;
    y *= -1;
    input.cursor_pos.x = (GLfloat)x;
    input.cursor_pos.y = (GLfloat)y;
}


void input_init(GLFWwindow* window)
{
    // Register key-press callback
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
}

void input_set_dimensions(GLuint x, GLuint y)
{
    window_wh.x = x;
    window_wh.y = y;
}