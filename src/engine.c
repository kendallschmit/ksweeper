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
#include "number_display.h"

GLFWwindow *window;

struct vec2 window_wh;
struct vec3 camera_pos;
GLint dots_per_unit;

// Dealing with games
struct game game;
struct number_display time_display;
struct number_display bomb_display;
struct draw win_icon;

static void fit_window()
{
    camera_pos.x = game.center.x;
    camera_pos.y = game.center.y + 0.5;
    draw_set_position(camera_pos);
    glfwSetWindowSize(window, game.w * dots_per_unit, (game.h + 1) * dots_per_unit);
}

static void new_game(GLint w, GLint h, GLint bombs)
{
    game_reset(&game);
    game_start(&game, w, h, bombs);
    fit_window();
}

static void init_hud()
{
    time_display = (struct number_display){
        .digits = 4,
        .origin_right = true,
    };
    bomb_display = (struct number_display){
        .digits = 3,
    };
    win_icon = (struct draw){
        .vao = vaos[VAO_WIDE],
        .tex = texture_win,
    };
}

static void resize_hud()
{
    GLfloat right = (window_wh.x / dots_per_unit) / 2 - 0.25f;
    GLfloat left = (window_wh.x / dots_per_unit) / -2 + 0.25f;
    GLfloat top = (window_wh.y / dots_per_unit) / 2 - 0.5f;
    time_display.pos = (struct vec3){ right - 0.25, top, -1 };
    bomb_display.pos = (struct vec3){ left + 0.25, top, -1 };
    bomb_display.pos = (struct vec3){ left + 0.25, top, -1 };
    win_icon.pos = (struct vec3){ 0, top, -1 };
}

// GLFW callbacks
static void error_callback(int error, const char* description);
static void window_size_callback(GLFWwindow *window, int x, int y);

// Engine
static void engine_init();
static void engine_deinit();

static void engine_step(struct game *game);
static void render_update(struct draw_group *group);

static struct vec2 screen_to_world(struct vec2 point);

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void window_size_callback(GLFWwindow *window, int x, int y)
{
    draw_set_dimensions(x, y, dots_per_unit);
    window_wh = (struct vec2){ x, y };
    glViewport(0, 0, x, y);
    resize_hud();
}

static void engine_init()
{
    // Default engine values
    dots_per_unit = 40;

    // Set up random
    srand(time(NULL));

    // Set up OpenGL context
    glfwMakeContextCurrent(window); // Get context for window
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress); // Load functions
    glfwSwapInterval(1); // Vsync

    // Init engine components
    shader_init();
    vaos_init();
    texture_init();
    draw_init(5, 1.5, 0.1, 1000, dots_per_unit, 1000);
    input_init(window);

    // Window dimensions
    int winx, winy;
    glfwGetWindowSize(window, &winx, &winy);
    draw_set_dimensions((GLfloat)winx, (GLfloat)winy, dots_per_unit);
    window_wh = (struct vec2){ winx, winy };
    glfwSetWindowSizeCallback(window, window_size_callback);

    // Set up static game stuff
    game_init();
    number_display_init();
}

static void engine_deinit()
{
    // Deinit engine stuff
    shader_deinit();
    vaos_deinit();
}

static void engine_step(struct game *game)
{
    // Input
    input_reset();
    glfwPollEvents();

    // Panning with middle click
    //struct vec2 drag = input.drag;
    //drag.x /= dots_per_unit;
    //drag.y /= -dots_per_unit;
    //camera_pos.x -= drag.x;
    //camera_pos.y -= drag.y;
    //draw_set_position(camera_pos);

    // New game with 'e'asy, 'm'edium, or e'x'pert
    if (input.e.press) {
        new_game(8, 8, 10);
    }
    if (input.m.press) {
        new_game(16, 16, 40);
    }
    if (input.x.press) {
        new_game(30, 16, 99);
    }

    // Fit window to game with 'f'
    if (input.f.press) {
        fit_window();
    }

    // Zoom with mouse wheel
    //if (input.scroll.y > 0) {
    //    dots_per_unit -= 2;
    //    if (dots_per_unit < 12)
    //        dots_per_unit += 2;
    //    draw_set_dimensions(window_wh.x, window_wh.y, dots_per_unit);
    //    resize_hud();
    //}
    //else if (input.scroll.y < 0) {
    //    dots_per_unit += 2;
    //    if (dots_per_unit > 64)
    //        dots_per_unit -= 2;
    //    draw_set_dimensions(window_wh.x, window_wh.y, dots_per_unit);
    //    resize_hud();
    //}

    if (!game->over) {
        // Game interactions
        struct vec2 mouse_pos = screen_to_world(input.cursor_pos);
        GLint tile_x = roundf(mouse_pos.x);
        GLint tile_y = roundf(mouse_pos.y);
        // Visual press if click is held
        if (input.mousel.state)
            game_press(game, tile_x, tile_y);
        else
            game_unpress(game);
        // Left click to reveal tile
        if (input.mousel.release)
            game_reveal(game, roundf(mouse_pos.x), roundf(mouse_pos.y));
        // Right click to flag tile
        if (input.mouser.release)
            game_flag(game, roundf(mouse_pos.x), roundf(mouse_pos.y));
        // Show numbers at top of screen
        time_display.value = (GLint)(glfwGetTime() - game->start_time);
        number_display_refresh(&time_display);
        bomb_display.value = game->bombs - game->flag_count;
        number_display_refresh(&bomb_display);
        // Set win texture
        win_icon.tex = texture_blankwin;
    }
    else {
        win_icon.tex = game->win ? texture_win : texture_lose;
        if (input.mousel.release
                && input.cursor_pos.y < dots_per_unit
                && input.cursor_pos.x > window_wh.x / 2 - dots_per_unit * 1
                && input.cursor_pos.x < window_wh.x / 2 + dots_per_unit * 1) {
            game_restart(game);
            kprint("Restarting game");
        }
    }
    // Clear and draw
    glClear(GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_ACCUM_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT);
    //draw_list(background_objs.draws, background_objs.count, PROJECTION_ORTHOGRAPHIC, true, true);
    //draw_list(foreground_objs.draws, foreground_objs.count, PROJECTION_ORTHOGRAPHIC, true, true);
    draw_list(game->draw_group.draws, game->draw_group.count, PROJECTION_ORTHOGRAPHIC, false, false, true);
    draw_list(time_display.group.draws, time_display.group.count, PROJECTION_ORTHOGRAPHIC, false, false, false);
    draw_list(bomb_display.group.draws, time_display.group.count, PROJECTION_ORTHOGRAPHIC, false, false, false);
    draw_list(&win_icon, 1, PROJECTION_ORTHOGRAPHIC, false, false, false);
    // Push to display
    glfwSwapBuffers(window);
}

static struct vec2 screen_to_world(struct vec2 point)
{
    point.x -= window_wh.x / 2;
    point.y -= window_wh.y / 2;
    point.x /= dots_per_unit;
    point.y /= dots_per_unit;
    point.y *= -1;
    point.x += camera_pos.x;
    point.y += camera_pos.y;
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
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    window = glfwCreateWindow(320, 240, "ksweeper", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }

    // Init, loop, deinit
    engine_init();
    new_game(8, 8, 10);
    init_hud();
    resize_hud();
    while (!glfwWindowShouldClose(window)) {
        engine_step(&game);
    }
    engine_deinit();

    // Cleanup GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}