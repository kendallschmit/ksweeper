#ifndef KGE_UTIL_H
#define KGE_UTIL_H

#include <stdio.h>
#include <stdbool.h>

#include <glad/glad.h>

// Define if compiling for Windows
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define PLATFORM_WINDOWS
#endif

// open() flags for reading binary files
#ifdef PLATFORM_WINDOWS
    #define OPEN_FLAGS_READ_BINARY "rb"
#else
    #define OPEN_FLAGS_READ_BINARY "r"
#endif

extern bool kge_util_kprint_needs_header(char const *file, char const *func);

#define kprint(...) do {\
    if (kge_util_kprint_needs_header(__FILE__, __func__)) {\
        printf("%s::%s():\n", __FILE__, __func__);\
    }\
    printf(" * ");\
    printf(__VA_ARGS__);\
    printf("\n");\
    fflush(stdout);\
} while (0);

extern GLfloat randf(GLfloat minimum, GLfloat maximum);
extern GLint randi(GLfloat minimum, GLfloat maximum);

#endif
