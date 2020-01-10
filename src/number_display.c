#include "number_display.h"

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#include "kge_util.h"
#include "shader.h"
#include "texture.h"
#include "vao.h"
#include "draw.h"

GLuint tex_n[10];

void number_display_init()
{
    tex_n[0] = texture_n0;
    tex_n[1] = texture_n1;
    tex_n[2] = texture_n2;
    tex_n[3] = texture_n3;
    tex_n[4] = texture_n4;
    tex_n[5] = texture_n5;
    tex_n[6] = texture_n6;
    tex_n[7] = texture_n7;
    tex_n[8] = texture_n8;
    tex_n[9] = texture_n9;
}

void number_display_refresh(struct number_display *nd)
{
    nd->group.draws = nd->draws;
    nd->group.count = nd->digits;
    for (GLint i = 0; i < nd->digits; i++) {
        GLfloat offset = i / 2.0f;
        if (nd->origin_right)
            offset -= (nd->digits - 1) / 2.0f;
        nd->group.draws[i] = (struct draw){
            .vao = vaos[VAO_TALL],
            .tex = texture_nblank,
            .pos = (struct vec3){ nd->pos.x + offset, nd->pos.y, nd->pos.z },
        };
    }
    // Apply value textures
    GLint value = nd->value;
    for (GLint i = nd->digits - 1; i >= 0; i--) {
        if (value == 0 && i != nd->digits - 1)
            nd->group.draws[i].tex = texture_nblank;
        else
            nd->group.draws[i].tex = tex_n[value % 10];
        value /= 10;
    }
}