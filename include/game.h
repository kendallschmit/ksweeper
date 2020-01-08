#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine.h"
#include "vectors.h"
#include "draw.h"

#define TILE_STATE_HIDDEN 0
#define TILE_STATE_REVEAL 1
#define TILE_STATE_FLAG 2
#define TILE_STATE_MARK 3
struct tile {
    struct draw *draw;
    GLint n;
    GLint state;
    bool bomb;
    bool pressed;
    GLint x;
    GLint y;
};

struct game {
    GLint h;
    GLint w;
    GLint bombs;
    struct tile *tiles;
    struct draw_group draw_group;

    struct tile *pressed;
    struct tile *extra_pressed[8];

    struct tile *overclick_tile;
    double overclick_time;

    double start_time;
    GLint flag_count;
    GLint reveal_count;
    bool over;
    bool win;

    struct vec2 center;
};

void game_init();

void game_start(struct game *game, GLint h, GLint w, GLint bombs);
void game_reset(struct game *game);

void game_press(struct game *game, GLint x, GLint y);
void game_unpress(struct game *game);
void game_reveal(struct game *game, GLint x, GLint y);
void game_flag(struct game *game, GLint x, GLint y);

#endif