#include "input.h"

#include "engine.h"
#include "kge_util.h"

struct input_set input = { 0 };

static void key_callback(
        GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }
    struct input_button *val;
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
        val->state = true;
        val->press = true;
    }
    else if (action == GLFW_RELEASE) {
        val->state = false;
        val->release = true;
    }
}

static void cursor_pos_callback(
        GLFWwindow* window, double x, double y)
{
    if (input.mousem.state) {
        input.drag.x += x - input.cursor_pos.x;
        input.drag.y += y - input.cursor_pos.y;
    }
    input.cursor_pos = (struct vec2){ x, y };
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    struct input_button *val;
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            val = &input.mousel;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            val = &input.mouser;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            val = &input.mousem;
            break;
        default:
            return;
    }
    if (action == GLFW_PRESS) {
        val->state = true;
        val->press = true;
    }
    else if (action == GLFW_RELEASE) {
        val->state = false;
        val->release = true;
    }
}

void reset_button(struct input_button *b)
{
    b->press = false;
    b->release = false;
}

void input_init(GLFWwindow* window)
{
    // Register key-press callback
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
}

void input_reset()
{
    reset_button(&input.up);
    reset_button(&input.down);
    reset_button(&input.left);
    reset_button(&input.right);

    reset_button(&input.mousel);
    reset_button(&input.mouser);
    reset_button(&input.mousem);
    input.drag = (struct vec2){ 0 };
}