#include "engine.h"

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "kge_util.h"
#include "vectors.h"
#include "shader.h"
#include "texture.h"
#include "vao.h"
#include "draw.h"
#include "input.h"

struct object_group background_objs = { 0 };
struct object_group foreground_objs = { 0 };

GLuint tex_akko;
GLuint tex_ritsu;

// GLFW callbacks
static void error_callback(int error, const char* description);
static void window_size_callback(GLFWwindow *window, int x, int y);

// Engine
static void engine_init(GLFWwindow *window);
static void engine_deinit(GLFWwindow *window);

static void engine_step(GLFWwindow *window);
static void render_update(struct object_group *group);

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void window_size_callback(GLFWwindow *window, int x, int y)
{
    draw_set_dimensions(x, y);
    input_set_dimensions(x, y);
    glViewport(0, 0, x, y);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void engine_init(GLFWwindow *window)
{
    // Set up game
    srand(time(NULL));

    // Set up OpenGL context
    glfwMakeContextCurrent(window); // Get context for window
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress); // Load functions
    glfwSwapInterval(1); // Vsync

    // Init engine components
    shader_init();
    vaos_init();
    draw_init(5, 1.5, 0.1, 1000, DOTS_PER_UNIT, 1000);
    input_init(window);

    // Window dimensions
    int winx, winy;
    glfwGetWindowSize(window, &winx, &winy);
    draw_set_dimensions((GLfloat)winx, (GLfloat)winy);
    input_set_dimensions((GLfloat)winx, (GLfloat)winy);
    glfwSetWindowSizeCallback(window, window_size_callback);

    tex_akko = texture_load("res/tga/akko.tga");
    tex_ritsu = texture_load("res/tga/ritsu128.tga");

    // Only orthographic object is player for now
    struct kge_obj *player = &foreground_objs.objs[0];
    player->draw = &foreground_objs.draws[0];
    *player->draw = (struct draw){
        .vao = vaos[VAO_QUAD],
        .tex = tex_akko,
    };
    foreground_objs.count = 1;
}

static void engine_deinit(GLFWwindow *window)
{
    // Deinit engine stuff
    shader_deinit();
    vaos_deinit();
}

static void engine_step(GLFWwindow *window)
{
        // Input
        glfwPollEvents();

        // GAMEPLAY HAPPENS HERE?
        struct kge_obj *player = &foreground_objs.objs[0];
        player->pos.x = input.cursor_pos.x;
        player->pos.y = input.cursor_pos.y;

        // Update draw structs
        render_update(&background_objs);
        render_update(&foreground_objs);
        // Clear and draw
        glClear(GL_COLOR_BUFFER_BIT
                | GL_DEPTH_BUFFER_BIT
                | GL_ACCUM_BUFFER_BIT
                | GL_STENCIL_BUFFER_BIT);
        draw_list(background_objs.draws, background_objs.count,
                PROJECTION_ORTHOGRAPHIC, true, true);
        draw_list(foreground_objs.draws, foreground_objs.count,
                PROJECTION_ORTHOGRAPHIC, true, true);
        // Push to display
        glfwSwapBuffers(window);
}

static void render_update(struct object_group *group)
{
    for (GLuint i = 0; i < group->count; i++) {
        struct kge_obj *o = &group->objs[i];
        o->draw->pos.x = (GLfloat)o->pos.x;
        o->draw->pos.y = (GLfloat)o->pos.y;
        o->draw->pos.z = (GLfloat)o->pos.z;
    }
}

extern int engine_go()
{
    // Subscribe to GLFW errors
    glfwSetErrorCallback(error_callback);
    // Get context with GLFW
    if (!glfwInit())
        return -1;
    // OpenGL 4.1, no backward/forward compatibility. Only needed for macOS?
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // Make the window
    GLFWwindow *window = glfwCreateWindow(640, 480, "Your Mom", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Init, loop, deinit
    engine_init(window);
    while (!glfwWindowShouldClose(window)) {
        engine_step(window);
    }
    engine_deinit(window);

    // Cleanup GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}