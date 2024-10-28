#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#include <vector>
#include <string>
#include <numeric>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include "CUSTOM/shader.h"
#include <mutex>


std::tuple<float*, int, float, float> tilemapDecoder(std::vector<std::vector<int>> tilemap, int tileTextureSize, int windowWidth, int windowHeight, float blockSize) {
    int numOfTriangles = 0;

    float xScale = blockSize / windowWidth;
    float yScale = blockSize / windowHeight;
    float offset = (1.0f / tileTextureSize);
    float yOffset;

    size_t totalSize = 0;
    for (const auto& row : tilemap) {
        totalSize += row.size() * 30 * sizeof(float);
    }

    float* vertexData = new float[totalSize];
    int index = 0;
    int tileType = 0;

    for (int height = 0; height < tilemap.size(); height++) {
        for (int width = 0; width < tilemap[0].size(); width++) {
            tileType = tilemap[height][width];
            yOffset = offset * tileType;
            if (tileType != 0) {
                numOfTriangles += 2;

                //Bottom left
                vertexData[index++] = width * xScale;
                vertexData[index++] = height * yScale;
                vertexData[index++] = 0;
                vertexData[index++] = yOffset;
                vertexData[index++] = tileType;

                //Top left
                vertexData[index++] = width * xScale;
                vertexData[index++] = height * yScale + yScale;
                vertexData[index++] = 0;
                vertexData[index++] = yOffset - offset;
                vertexData[index++] = tileType;

                //Bottom left
                vertexData[index++] = width * xScale + xScale;
                vertexData[index++] = height * yScale;
                vertexData[index++] = 1;
                vertexData[index++] = yOffset;
                vertexData[index++] = tileType;

                //Top right
                vertexData[index++] = width * xScale + xScale;
                vertexData[index++] = height * yScale + yScale;
                vertexData[index++] = 1;
                vertexData[index++] = yOffset - offset;
                vertexData[index++] = tileType;

                //Top left
                vertexData[index++] = width * xScale;
                vertexData[index++] = height * yScale + yScale;
                vertexData[index++] = 0;
                vertexData[index++] = yOffset - offset;
                vertexData[index++] = tileType;

                //Bottom left
                vertexData[index++] = width * xScale + xScale;
                vertexData[index++] = height * yScale;
                vertexData[index++] = 1;
                vertexData[index++] = yOffset;
                vertexData[index++] = tileType;
            }
        }
    }
    return std::make_tuple(vertexData, numOfTriangles, xScale, yScale);
}

class RenderLayer {
public:
    RenderLayer();
    RenderLayer(std::vector<int> attributes, std::string shaderName) {
        //Create & bind buffers
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        //Set up vertex attributes
        int offset = 0;
        int count = 0;
        step = accumulate(attributes.begin(), attributes.end(), 0);
        for (int attribSize : attributes) {
            glVertexAttribPointer(count, attribSize, GL_FLOAT, GL_FALSE, step * sizeof(float), (void*)(offset * sizeof(float)));
            glEnableVertexAttribArray(count);
            count++;
            offset += attribSize;
        }
        //Create shader obj for this rendering layer
        std::string vertexPath = "assets/shaders/" + shaderName + "_vertex_shader.glsl";
        std::string fragmentPath = "assets/shaders/" + shaderName + "_fragment_shader.glsl";
        shader = Shader(vertexPath.c_str(), fragmentPath.c_str());
    }

    void setVertices(float* vertices, int numTriangles, int numFloatsPerTriangle, unsigned int mode) {
        std::lock_guard<std::mutex> lock(mtx);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, numTriangles * numFloatsPerTriangle * sizeof(float), vertices, mode);
    }
    void draw(int numTriangles) {
        std::lock_guard<std::mutex> lock(mtx);
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, texture);
        shader.use();
        glDrawArrays(GL_TRIANGLES, 0, numTriangles * 3);
    }
    void setFloat(std::string name, float value) {
        std::lock_guard<std::mutex> lock(mtx);
        shader.use();
        shader.setFloat(name, value);
    }
    void setBool(std::string name, bool value) {
        std::lock_guard<std::mutex> lock(mtx);
        shader.use();
        shader.setBool(name, value);
    }
    void setArray_64_vec4(std::string name, float (*value)[4], int elementCount) {
        std::lock_guard<std::mutex> lock(mtx);
        shader.use();
        float** arr = new float* [64];
        for (int i = 0; i < 64; ++i) {
            arr[i] = value[i];
        }
        shader.setArray_64_vec4(name, arr);
    }

private:
    unsigned int VBO;
    unsigned int VAO;
    unsigned int texture;
    int step;
    Shader shader;
    std::mutex mtx;
};

class Renderer {
public:
    float mouseX;
    float mouseY;
    int width;
    int height;
    Renderer(int width, int height, std::string windowName) : width(width), height(height) {
        mouseX = 0.0f;
        mouseY = 0.0f;
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, windowName.c_str(), NULL, NULL);
        glfwMakeContextCurrent(window);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        glfwSetWindowUserPointer(window, this);
        glfwSetCursorPosCallback(window, this->mouseCallback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        lastFrame = glfwGetTime();
    }
    void updateDisplay() {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    void fillScreen(int r, int g, int b) {
        glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    bool isRunning() {
        return !glfwWindowShouldClose(window);
    }
    float getBlockWidth() { return 100.0f / width; }
    float getBlockHeight() { return 100.0f / height; }
    bool getKeyDown(unsigned int keycode) { return glfwGetKey(window, keycode); }
    float getDeltaTime() {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        return deltaTime;
    }
    void setMousePosition(double x, double y) {
        mouseX = x;
        mouseY = y;
    }
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
        Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
        if (renderer) {
            renderer->mouseX = static_cast<float>((xpos/renderer->width) * 2.0 - 1.0);
            renderer->mouseY = static_cast<float>(((ypos / renderer->height) * 2.0 - 1.0) * -1.0f);
        }
    }

    bool mouseDown() {
        return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    }

    bool mouseWasJustClicked() {
        if (mouseDown()) {
            mouseCount++;
        }
        else {
            mouseCount = 0;
        }
        return mouseCount == 1;
    }

    inline float getMouseX() { return mouseX; }
    inline float getMouseY() { return mouseY; }

private:
    GLFWwindow* window;
    float lastFrame;
    int mouseCount = 0;
};