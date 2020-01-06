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

struct object_group background_objs = { 0 };
struct object_group foreground_objs = { 0 };

GLuint tex_akko;
GLuint tex_ritsu;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void engine_thread_fn(GLFWwindow *window)
{
    // Set up game
    srand(time(NULL));

    // Set up OpenGL context
    glfwMakeContextCurrent(window); // Get context for window
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress); // Load functions
    glfwSwapInterval(1); // Vsync

    shader_init(); // Init shaders
    vaos_init(); // Init shaders
    draw_init(5, 1.5, 0.1, 1000, 6, 1000); // Init draw

    // Window dimensions
    int winx, winy;
    glfwGetWindowSize(window, &winx, &winy);
    draw_set_dimensions((GLfloat)winx, (GLfloat)winy);
    //glfwSetWindowSizeCallback(window, window_size_callback);

    tex_akko = texture_load("res/tga/akko.tga");
    tex_ritsu = texture_load("res/tga/ritsu128.tga");

    // Done with initialization
    kprint("Done initializing");

    // Only orthographic object is player for now
    kprint("Set up player");
    struct kge_obj *player = &foreground_objs.objs[0];
    player->draw = &foreground_objs.draws[0];
    *player->draw = (struct draw){
        .vao = vaos[VAO_QUAD],
        .tex = tex_akko,
    };
    foreground_objs.count = 1;
    kprint("Done setting up player")

    // Loop
    while (!glfwWindowShouldClose(window)) {
        // Input
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT
                | GL_DEPTH_BUFFER_BIT
                | GL_ACCUM_BUFFER_BIT
                | GL_STENCIL_BUFFER_BIT);
        draw_list(background_objs.draws, background_objs.count,
                PROJECTION_PERSPECTIVE, true, true);
        draw_list(foreground_objs.draws, foreground_objs.count,
                PROJECTION_ORTHOGRAPHIC, true, true);

        glfwSwapBuffers(window);
    }

    // Deinit engine stuff
    shader_deinit();
    vaos_deinit();
}

extern int engine_go()
{
    // Subscribe to GLFW errors
    glfwSetErrorCallback(error_callback);
    // Get context with GLFW
    if (!glfwInit())
        return -1;
    // OpenGL 4.1, no backward/forward compatibility
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // Make the window
    GLFWwindow *window = glfwCreateWindow(640, 480, "Your Mom", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    // Register key-press callback
    //glfwSetKeyCallback(window, input_key_callback);

    // Run
    engine_thread_fn(window);

    // Cleanup GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}