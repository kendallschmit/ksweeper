#include "shader.h"

#include <string.h>

#include "kge_util.h"

#include "resgen_shader.h"

GLuint shader_program_simple = 0;

static GLuint load_shader(char *source, GLenum shader_type) {
    GLchar *lines = source;
    const GLchar **lines_ptr = (const GLchar **)&lines;
    GLint success;
    GLuint shad = glCreateShader(shader_type);
    glShaderSource(shad, 1, lines_ptr, NULL);
    glCompileShader(shad);
    glGetShaderiv(shad, GL_COMPILE_STATUS, &success);
    if (success) {
        kprint("Compiled shader");
    }
    else {
        GLint log_size = 0;
        glGetShaderiv(shad, GL_INFO_LOG_LENGTH, &log_size);
        GLchar log[log_size];
        glGetShaderInfoLog(shad, log_size, NULL, log);
        kprint("Failed to compile shader: %s", log);
    }
    return shad;
}

static GLuint fshad = 0;
static GLuint vshad = 0;

extern void shader_init() {
    char sourcebuf[4096];
    // Simple vertex
    strncpy(sourcebuf, (char *)resgen_shader_simple_vertex, sizeof(resgen_shader_simple_vertex));
    sourcebuf[sizeof(resgen_shader_simple_vertex)] = '\0';
    vshad = load_shader(sourcebuf, GL_VERTEX_SHADER);
    // Simple fragment
    strncpy(sourcebuf, (char *)resgen_shader_simple_fragment, sizeof(resgen_shader_simple_fragment));
    sourcebuf[sizeof(resgen_shader_simple_fragment)] = '\0';
    fshad = load_shader(sourcebuf, GL_FRAGMENT_SHADER);

    // Make a basic program, attach shaders
    shader_program_simple = glCreateProgram();
    glAttachShader(shader_program_simple, vshad);
    glAttachShader(shader_program_simple, fshad);

    glLinkProgram(shader_program_simple);
}

extern void shader_deinit() {
    glDeleteShader(vshad);
    glDeleteShader(fshad);

    glDetachShader(shader_program_simple, vshad);
    glDetachShader(shader_program_simple, fshad);
    glDeleteProgram(shader_program_simple);
}
