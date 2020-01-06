#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "time.h"

#include "vectors.h"
#include "draw.h"

#define DOTS_PER_UNIT 32

struct kge_obj {
    struct vec3i pos;
    struct draw *draw;
};

#define OBJECTS_MAX 100000
struct object_group {
    struct draw draws[OBJECTS_MAX];
    struct kge_obj objs[OBJECTS_MAX];
    GLuint count;
};

extern struct object_group background_objs;
extern struct object_group foreground_objs;

extern GLuint tex_akko;
extern GLuint tex_ritsu;

extern int engine_go();

#endif