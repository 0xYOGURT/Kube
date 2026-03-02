#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
out vec3 ourColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos,1.0);
    ourColor = aColor;
})";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec3 ourColor;
uniform float time;
void main() {
    float glow = 0.5 + 0.5 * sin(time*5.0);
    FragColor = vec4(ourColor*glow,1.0);
})";

// Simple 4x4 matrix multiplication
void multiplyMatrix(float* result, float* a, float* b) {
    float temp[16];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temp[i * 4 + j] = 0;
            for (int k = 0; k < 4; k++) temp[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
        }
    }
    for (int i = 0; i < 16; i++) result[i] = temp[i];
}

// Rotation matrix around X axis
void rotationX(float* m, float angle) {
    float c = cos(angle), s = sin(angle);
    m[0] = 1; m[1] = 0; m[2] = 0; m[3] = 0;
    m[4] = 0; m[5] = c; m[6] = -s; m[7] = 0;
    m[8] = 0; m[9] = s; m[10] = c; m[11] = 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}

// Rotation matrix around Y axis
void rotationY(float* m, float angle) {
    float c = cos(angle), s = sin(angle);
    m[0] = c; m[1] = 0; m[2] = s; m[3] = 0;
    m[4] = 0; m[5] = 1; m[6] = 0; m[7] = 0;
    m[8] = -s; m[9] = 0; m[10] = c; m[11] = 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}

// Identity matrix
void identity(float* m) {
    for (int i = 0; i < 16; i++) m[i] = 0;
    m[0] = m[5] = m[10] = m[15] = 1;
}

// Simple perspective matrix
void perspective(float* m, float fov, float aspect, float znear, float zfar) {
    float tanHalfFov = tan(fov / 2);
    for (int i = 0; i < 16; i++) m[i] = 0;
    m[0] = 1 / (aspect * tanHalfFov);
    m[5] = 1 / tanHalfFov;
    m[10] = -(zfar + znear) / (zfar - znear);
    m[11] = -1;
    m[14] = -(2 * zfar * znear) / (zfar - znear);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW Cube", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    // Cube vertices: pos + color
    float vertices[] = {
       -0.5f,-0.5f,-0.5f, 1,0,0,
        0.5f,-0.5f,-0.5f, 0,1,0,
        0.5f, 0.5f,-0.5f, 0,0,1,
       -0.5f, 0.5f,-0.5f, 1,1,0,
       -0.5f,-0.5f, 0.5f, 1,0,1,
        0.5f,-0.5f, 0.5f, 0,1,1,
        0.5f, 0.5f, 0.5f, 1,1,1,
       -0.5f, 0.5f, 0.5f, 0,0,0
    };
    unsigned int indices[] = {
        0,1,2,2,3,0, 4,5,6,6,7,4,
        0,1,5,5,4,0, 2,3,7,7,6,2,
        0,3,7,7,4,0, 1,2,6,6,5,1
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);
    unsigned int shader = glCreateProgram();
    glAttachShader(shader, vs);
    glAttachShader(shader, fs);
    glLinkProgram(shader);
    glDeleteShader(vs);
    glDeleteShader(fs);

    float model[16], rotX[16], rotY[16], view[16], proj[16], temp[16];
    identity(view); view[14] = -3.0f; // move back z
    perspective(proj, 3.14159f / 4, 800.0f / 600.0f, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float t = (float)glfwGetTime();
        rotationX(rotX, t);
        rotationY(rotY, t);
        multiplyMatrix(model, rotY, rotX);

        glUseProgram(shader);
        glUniform1f(glGetUniformLocation(shader, "time"), t);
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, model);
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, view);
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, proj);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader);
    glfwTerminate();
}
