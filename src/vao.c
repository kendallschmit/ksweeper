#include "vao.h"

GLuint vaos[VAOS_MAX];

extern void vaos_init() {
    GLuint vao; // Name of VAO
    GLuint buf; // Name of vertex buffer


    /* NORMAL QUAD */

    glGenVertexArrays(1, &vao); // Generate one VAO
    glBindVertexArray(vao); // Make it active VAO

    // lower left quad
    /*GLfloat quad_buf[] = {
        0.0, 0.0, 0.0,   0.0, 1.0,   // BL
        1.0, 0.0, 0.0,   1.0, 1.0,   // BR
        0.0, 1.0, 0.0,   0.0, 0.0,   // TL

        1.0, 1.0, 0.0,   1.0, 0.0,   // TR
        0.0, 1.0, 0.0,   0.0, 0.0,   // TL
        1.0, 0.0, 0.0,   1.0, 1.0,   // BR
    }; */
    // Centered quad
    GLfloat quad_buf[] = {
        -0.5, -0.5, -0.5,   0.0, 1.0,   // BL
         0.5, -0.5, -0.5,   1.0, 1.0,   // BR
        -0.5,  0.5, -0.5,   0.0, 0.0,   // TL
         0.5,  0.5, -0.5,   1.0, 0.0,   // TR
        -0.5,  0.5, -0.5,   0.0, 0.0,   // TL
         0.5, -0.5, -0.5,   1.0, 1.0,   // BR
    };
    // VERTEX BUFFER
    glGenBuffers(1, &buf); // Generate buffer
    glBindBuffer(GL_ARRAY_BUFFER, buf); // Make it the active ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_buf), quad_buf, GL_STATIC_DRAW);
    // Set x, y positions for tris
    // index, size, type, normalized, stride, offset
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
            sizeof(GLfloat) * 5, 0);
    glEnableVertexAttribArray(0); // Enable VA at index 0
    // Set uv positions for tris
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
            sizeof(GLfloat) * 5, (GLvoid *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1); // Enable VA at index 1
    // DONE MAKING CHANGES
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // Store it
    vaos[VAO_QUAD] = vao;


    /* TALL QUAD */

    glGenVertexArrays(1, &vao); // Generate one VAO
    glBindVertexArray(vao); // Make it active VAO

    GLfloat tall_buf[] = {
        -0.25, -0.5, -0.5,   0.0, 1.0,   // BL
         0.25, -0.5, -0.5,   1.0, 1.0,   // BR
        -0.25,  0.5, -0.5,   0.0, 0.0,   // TL
         0.25,  0.5, -0.5,   1.0, 0.0,   // TR
        -0.25,  0.5, -0.5,   0.0, 0.0,   // TL
         0.25, -0.5, -0.5,   1.0, 1.0,   // BR
    };
    // VERTEX BUFFER
    glGenBuffers(1, &buf); // Generate buffer
    glBindBuffer(GL_ARRAY_BUFFER, buf); // Make it the active ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(tall_buf), tall_buf, GL_STATIC_DRAW);
    // Set x, y positions for tris
    // index, size, type, normalized, stride, offset
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
            sizeof(GLfloat) * 5, 0);
    glEnableVertexAttribArray(0); // Enable VA at index 0
    // Set uv positions for tris
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
            sizeof(GLfloat) * 5, (GLvoid *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1); // Enable VA at index 1
    // DONE MAKING CHANGES
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // Store it
    vaos[VAO_TALL] = vao;


    /* WIDE QUAD */

    glGenVertexArrays(1, &vao); // Generate one VAO
    glBindVertexArray(vao); // Make it active VAO

    GLfloat wide_buf[] = {
        -1.0, -0.5, -0.5,   0.0, 1.0,   // BL
         1.0, -0.5, -0.5,   1.0, 1.0,   // BR
        -1.0,  0.5, -0.5,   0.0, 0.0,   // TL
         1.0,  0.5, -0.5,   1.0, 0.0,   // TR
        -1.0,  0.5, -0.5,   0.0, 0.0,   // TL
         1.0, -0.5, -0.5,   1.0, 1.0,   // BR
    };
    // VERTEX BUFFER
    glGenBuffers(1, &buf); // Generate buffer
    glBindBuffer(GL_ARRAY_BUFFER, buf); // Make it the active ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(wide_buf), wide_buf, GL_STATIC_DRAW);
    // Set x, y positions for tris
    // index, size, type, normalized, stride, offset
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
            sizeof(GLfloat) * 5, 0);
    glEnableVertexAttribArray(0); // Enable VA at index 0
    // Set uv positions for tris
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
            sizeof(GLfloat) * 5, (GLvoid *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1); // Enable VA at index 1
    // DONE MAKING CHANGES
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // Store it
    vaos[VAO_WIDE] = vao;
}

extern void vaos_deinit() {
    glDeleteVertexArrays(VAOS_MAX, vaos);
}
