#include "engine.h"

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "kge_util.h"
#include "vectors.h"
#include "shader.h"
#include "texture.h"
#include "vao.h"
#include "draw.h"
#include "input.h"

#include "game.h"

struct vec2 window_wh;

// GLFW callbacks
static void error_callback(int error, const char* description);
static void window_size_callback(GLFWwindow *window, int x, int y);

// Engine
static void engine_init(GLFWwindow *window);
static void engine_deinit(GLFWwindow *window);

static void engine_step(GLFWwindow *window, struct game *game);
static void render_update(struct draw_group *group);

static struct vec2 screen_to_world(struct vec2 point);

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void window_size_callback(GLFWwindow *window, int x, int y)
{
    draw_set_dimensions(x, y);
    window_wh = (struct vec2){ x, y };
    glViewport(0, 0, x, y);
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
    window_wh = (struct vec2){ winx, winy };
    glfwSetWindowSizeCallback(window, window_size_callback);
}

static void engine_deinit(GLFWwindow *window)
{
    // Deinit engine stuff
    shader_deinit();
    vaos_deinit();
}

static void engine_step(GLFWwindow *window, struct game *game)
{
        // Input
        input_reset();
        glfwPollEvents();
        // GAMEPLAY HAPPENS HERE?
        //struct kge_obj *player = &foreground_objs.objs[0];
        //struct vec2 drag = input.drag;
        //drag.x /= DOTS_PER_UNIT;
        //drag.y /= -DOTS_PER_UNIT;
        //player->pos.x += drag.x;
        //player->pos.y += drag.y;

        struct vec2 mouse_pos = screen_to_world(input.cursor_pos);
        GLint tile_x = roundf(mouse_pos.x);
        GLint tile_y = roundf(mouse_pos.y);
        // Visual press if click is held
        if (input.mousel.state || input.mouser.state)
            game_press(game, tile_x, tile_y);
        else
            game_unpress(game);
        // Left click to reveal tile
        if (input.mousel.release)
            game_reveal(game, roundf(mouse_pos.x), roundf(mouse_pos.y));
        // Right click to flag tile
        if (input.mouser.release)
            game_flag(game, roundf(mouse_pos.x), roundf(mouse_pos.y));

        // Clear and draw
        glClear(GL_COLOR_BUFFER_BIT
                | GL_DEPTH_BUFFER_BIT
                | GL_ACCUM_BUFFER_BIT
                | GL_STENCIL_BUFFER_BIT);
        //draw_list(background_objs.draws, background_objs.count, PROJECTION_ORTHOGRAPHIC, true, true);
        //draw_list(foreground_objs.draws, foreground_objs.count, PROJECTION_ORTHOGRAPHIC, true, true);
        draw_list(game->draw_group.draws, game->draw_group.count, PROJECTION_ORTHOGRAPHIC, false, false);
        // Push to display
        glfwSwapBuffers(window);
}

static struct vec2 screen_to_world(struct vec2 point)
{
    point.x -= window_wh.x / 2;
    point.y -= window_wh.y / 2;
    point.x /= DOTS_PER_UNIT;
    point.y /= DOTS_PER_UNIT;
    point.y *= -1;
    return point;
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

    game_init();
    struct game game;
    game_start(&game, 9, 9, 9);
    while (!glfwWindowShouldClose(window)) {
        engine_step(window, &game);
    }
    engine_deinit(window);

    // Cleanup GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}