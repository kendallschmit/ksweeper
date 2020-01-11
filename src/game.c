#include "game.h"

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "kge_util.h"
#include "shader.h"
#include "texture.h"
#include "vao.h"
#include "draw.h"

GLuint tex_digits[9];

void tile_update(struct game *game, struct tile *t);
void tile_update_all(struct game *game);
void reveal(struct game *game, struct tile *t);
void reveal_at(struct game *game, GLint x, GLint y);
void super_reveal(struct game *game, struct tile *t);
void tile_flag(struct game *game, struct tile *t);

void victory_check(struct game *game);
void count_bombs(struct game *game);

struct tile *tile_at(struct game *game, GLint x, GLint y)
{
    if (x < 0 || y < 0 || x >= game->w || y >= game->h)
        return NULL;
    return &game->tiles[x + game->w * y];
}

struct draw *draw_at(struct game *game, GLint x, GLint y)
{
    if (x < 0 || y < 0 || x > game->w || y > game->h)
        return NULL;
    return &game->draw_group.draws[x + game->w * y];
}

void tile_update(struct game *game, struct tile *t)
{
    switch (t->state) {
    case TILE_STATE_HIDDEN:
        if (game->over && t->bomb) {
            t->draw->tex = texture_bomb;
        }
        else if (t->pressed) {
            t->draw->tex = texture_pressed;
        }
        else {
            t->draw->tex = texture_hidden;
        }
        break;
    case TILE_STATE_REVEAL:
        if (t->bomb) {
            t->draw->tex = texture_bomb_explode;
        }
        else {
            t->draw->tex = tex_digits[t->n];
        }
        break;
    case TILE_STATE_FLAG:
        if (game->over && !t->bomb) {
            t->draw->tex = texture_wrong_flag;
        }
        else {
            t->draw->tex = texture_flag;
        }
        break;
    case TILE_STATE_MARK:
        if (game->over && t->bomb) {
            t->draw->tex = texture_bomb;
        }
        else {
            t->draw->tex = texture_mark;
        }
        break;
    default:
        kprint("Invalid value for t->state (%d)", t->state);
        exit(1);
    }
}

void tile_update_all(struct game *game)
{
    for (GLint i = 0; i < game->h * game->w; i++) {
        tile_update(game, &game->tiles[i]);
    }
}

void reveal(struct game *game, struct tile *t)
{
    if (t->state != TILE_STATE_HIDDEN)
        return;
    t->state = TILE_STATE_REVEAL;

    if (game->reveal_count == 0) {
        // Add bombs
        kprint("Add bombs")
        GLint bomblist[game->w * game->h];
        GLint bomblist_count = 0;
        for (GLint x = 0; x < game->w; x++) {
            for (GLint y = 0; y < game->h; y++) {
                if (x >= t->x - 1 && x <= t->x + 1) {
                    if (y >= t->y - 1 && y <= t->y + 1) {
                        continue;
                    }
                }
                bomblist[bomblist_count] = x + y * game->w;
                bomblist_count++;
            }
        }
        while (bomblist_count > game->bombs) {
            GLint i = randi(0, bomblist_count);
            while (i < bomblist_count - 1) {
                bomblist[i] = bomblist[i + 1];
                i++;
            }
            bomblist_count--;
        }
        for (GLint i = 0; i < bomblist_count; i++) {
            game->tiles[bomblist[i]].bomb = true;
        }
        // Count bombs
        count_bombs(game);
        // Set start time
        game->start_time = glfwGetTime();
    }
    if (t->bomb) {
        game->over = true;
        tile_update_all(game);
        return;
    }
    game->reveal_count++;
    if (t->n == 0) {
        reveal_at(game, t->x - 1, t->y);
        reveal_at(game, t->x + 1, t->y);
        reveal_at(game, t->x, t->y - 1);
        reveal_at(game, t->x, t->y + 1);
        reveal_at(game, t->x - 1, t->y - 1);
        reveal_at(game, t->x + 1, t->y - 1);
        reveal_at(game, t->x - 1, t->y + 1);
        reveal_at(game, t->x + 1, t->y + 1);
    }
    tile_update(game, t);
    victory_check(game);
}

void reveal_at(struct game *game, GLint x, GLint y)
{
    struct tile *t = tile_at(game, x, y);
    if (t != NULL)
        reveal(game, t);
}

void super_reveal(struct game *game, struct tile *t)
{
    // Count flags
    GLint flag_count = 0;
    struct tile *neighbors[] = {
        tile_at(game, t->x - 1, t->y),
        tile_at(game, t->x + 1, t->y),
        tile_at(game, t->x, t->y - 1),
        tile_at(game, t->x, t->y + 1),
        tile_at(game, t->x - 1, t->y - 1),
        tile_at(game, t->x + 1, t->y - 1),
        tile_at(game, t->x - 1, t->y + 1),
        tile_at(game, t->x + 1, t->y + 1),
    };
    for (GLint i = 0; i < 8; i++) {
        struct tile *n = neighbors[i];
        if (n == NULL)
            continue;
        if (n->state == TILE_STATE_MARK)
            return;
        if (n->state == TILE_STATE_FLAG)
            flag_count++;
    }
    if (flag_count == t->n) {
        for (GLint i = 0; i < 8; i++) {
            struct tile *n = neighbors[i];
            if (n == NULL)
                continue;
            reveal(game, n);
        }
    }
}

void tile_flag(struct game *game, struct tile *t)
{
    if (t->state == TILE_STATE_HIDDEN) {
        t->state = TILE_STATE_FLAG;
        game->flag_count++;
    }
    else if (t->state == TILE_STATE_FLAG) {
        t->state = TILE_STATE_MARK;
        game->flag_count--;
    }
    else if (t->state == TILE_STATE_MARK) {
        t->state = TILE_STATE_HIDDEN;
    }
    else {
        return;
    }
    tile_update(game, t);
    victory_check(game);
}

void victory_check(struct game *game)
{
    if (!game->over && game->reveal_count == game->w * game->h - game->bombs) {
        game->win = true;
        game->over = true;
    }
}

void count_bombs(struct game *game)
{
    kprint("Count bombs");
    for (GLint x = 0; x < game->w; x++) {
        for (GLint y = 0; y < game->h; y++) {
            struct tile *t = tile_at(game, x, y);
            struct tile *neighbors[] = {
                tile_at(game, x - 1, y),
                tile_at(game, x + 1, y),
                tile_at(game, x, y - 1),
                tile_at(game, x, y + 1),
                tile_at(game, x - 1, y - 1),
                tile_at(game, x + 1, y - 1),
                tile_at(game, x - 1, y + 1),
                tile_at(game, x + 1, y + 1),
            };
            t->n = 0;
            for (GLint i = 0; i < 8; i++) {
                if (neighbors[i] != NULL && neighbors[i]->bomb) {
                    t->n++;
                }
            }
        }
    }
}

void overclick(struct game *game, GLint x, GLint y)
{
    struct tile *t = tile_at(game, x, y);
    double current_time = glfwGetTime();
    if (game->overclick_tile == t && current_time - game->overclick_time < 0.3)
        super_reveal(game, t);
    game->overclick_tile = t;
    game->overclick_time = current_time;
}

void game_init()
{
    tex_digits[0] = texture_zero;
    tex_digits[1] = texture_one;
    tex_digits[2] = texture_two;
    tex_digits[3] = texture_three;
    tex_digits[4] = texture_four;
    tex_digits[5] = texture_five;
    tex_digits[6] = texture_six;
    tex_digits[7] = texture_seven;
    tex_digits[8] = texture_eight;
}

void game_start(struct game *game, GLint w, GLint h, GLint bombs)
{
    game_reset(game);
    // Init struct
    *game = (struct game){
            .w = w,
            .h = h,
            .bombs = bombs,
            .center = (struct vec2){ (w - 1) / 2.0, (h - 1) / 2.0 },
    };
    // Allocate tiles and draws
    game->tiles = malloc(sizeof (struct tile) * w * h);
    struct draw *draws = malloc(sizeof (struct draw) * w * h);
    if (game->tiles == NULL || draws == NULL) {
        kprint("malloc() failed");
        exit(1);
    }
    // Init draw_group
    game->draw_group = (struct draw_group){
        .draws = draws,
        .count = w * h,
    };
    // Create tiles and draws
    kprint("Create tiles and draws")
    for (GLint x = 0; x < w; x++) {
        for (GLint y = 0; y < h; y++) {
            struct tile *t = tile_at(game, x, y);
            struct draw *d = draw_at(game, x, y);
            *t = (struct tile){
                .draw = d,
                .x = x,
                .y = y,
            };
            *d = (struct draw){
                .vao = vaos[VAO_QUAD],
                .tex = texture_hidden,
                .pos = (struct vec3){ x, y, 0 },
            };
        }
    }
    kprint("Done with %s()", __func__);
}

void game_restart(struct game *game)
{
    game_start(game, game->w, game->h, game->bombs);
}

void game_reset(struct game *game)
{
    if (game->tiles != NULL)
        free(game->tiles);
    if (game->draw_group.draws != NULL)
        free(game->draw_group.draws);
    *game = (struct game){ 0 };
}

void game_press(struct game *game, GLint x, GLint y)
{
    game_unpress(game);
    game->pressed = tile_at(game, x, y);
    if (game->pressed != NULL) {
        game->pressed->pressed = true;
        tile_update(game, game->pressed);
    }
}

void game_unpress(struct game *game)
{
    if (game->pressed != NULL) {
        game->pressed->pressed = false;
        tile_update(game, game->pressed);
        GLint x = game->pressed->x;
        GLint y = game->pressed->y;
        game->pressed = NULL;
    }
}

void game_reveal(struct game *game, GLint x, GLint y)
{
    struct tile *t = tile_at(game, x, y);
    if (t == NULL)
        return;
    if (t->state == TILE_STATE_REVEAL) {
        overclick(game, x, y);
        return;
    }
    reveal(game, t);
}

void game_flag(struct game *game, GLint x, GLint y)
{
    struct tile *t = tile_at(game, x, y);
    if (t != NULL)
        tile_flag(game, t);
}