#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "time.h"

#include "vectors.h"
#include "draw.h"

struct draw_group {
    struct draw *draws;
    GLuint count;
};

extern int engine_go();

#endif