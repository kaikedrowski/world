#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <array>

#include <stdio.h>
#include <unistd.h>

#include "textrenderer.h"
#include "logger.h"
#include "controls.h"
#include "utils.h"
#include "shader.h"

// config
bool guitext = true;
bool hdr = true;
float exposure = 2.2f;
int screenWidth = 960, screenHeight = 540;
bool faceculling = true;
bool wireframe = false;
int verticesiter = 128;

/* ---------- */

unsigned int colorBuffer, rboDepth;

void renderWorld(const Shader &mainShader, const Shader &lightShader, const Shader &skyboxShader);
void renderScene(const Shader &shader);
void renderQuad();

glm::vec3 cubePositions[0];

static void error_callback(int error, const char *description)
{
    Logger("Error: " + (std::string)description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    keycallback(window, key, scancode, action, mods);
}

static void cursor_position_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    cursorpositioncallback(window, xposIn, yposIn);
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    // focus on the window when clicked on, esc to re-enable cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);

    // color buffer
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    // render buffer
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

void rendertext(GLFWwindow *window)
{
    if (guitext)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_CULL_FACE);
        std::string fpstring = "FPS " + std::to_string(fps).substr(0, 4);
        textRenderer(fpstring.c_str(), 32.0, -32.0, window, 32, 1.0, 1.0, 1.0, topLeft);
        std::string pos = std::to_string(cameraPos[0]).substr(0, 5) + ", " + std::to_string(cameraPos[1]).substr(0, 5) + ", " + std::to_string(cameraPos[2]).substr(0, 5);
        textRenderer(pos.c_str(), 32.0, -96.0, window, 32, 1.0, 1.0, 1.0, topLeft);
        std::string fr = "front: " + std::to_string(cameraFront[0]).substr(0, 5) + ", " + std::to_string(cameraFront[1]).substr(0, 5) + ", " + std::to_string(cameraFront[2]).substr(0, 5);
        textRenderer(fr.c_str(), 32.0, -160.0, window, 32, 1.0, 1.0, 1.0, topLeft);
        std::string up = "up: " + std::to_string(cameraUp[0]).substr(0, 5) + ", " + std::to_string(cameraUp[1]).substr(0, 5) + ", " + std::to_string(cameraUp[2]).substr(0, 5);
        textRenderer(up.c_str(), 32.0, -224.0, window, 32, 1.0, 1.0, 1.0, topLeft);
        if (faceculling)
            glEnable(GL_CULL_FACE);
        if (wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}

double gameTime;

float ratio;
glm::mat4 m, v, p, mvp;
GLuint texcubeVAO, texCubeIndexBuffer;

glm::mat4 lightProjection, lightView, lightSpaceMatrix;
glm::vec3 lightPos = glm::vec3(0.0, 0.0, 100.0);

unsigned int diffuseMap, specularMap, normalMap, depthMap, skybox;

GLuint lightVBO, lightEBO, lightVAO;
unsigned int depthMapFBO;

GLuint skyboxVAO, skyboxVBO;

std::vector<float> vertices;
std::vector<int> indices;

int main(void)
{
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    Logger("Program Initialized");

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        Logger("glfwInit Failed");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "Graphics Simulation", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        Logger("glfwCreateWindow Failed");
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        Logger("Failed to initialize GLAD");
        return -1;
    }
    glfwSwapInterval(1);

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (faceculling)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_MULTISAMPLE);

    Shader mainShader("main.vert", "main.frag", nullptr, "tessellation.tesc", "tessellation.tese");
    Shader lightShader("light.vert", "light.frag");
    Shader hdrShader("hdr.vert", "hdr.frag");
    Shader skyboxShader("skybox.vert", "skybox.frag");

    // configure floating point framebuffer
    // ------------------------------------
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    // create floating point color buffer
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // create depth buffer (renderbuffer)
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    // attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        Logger("Framebuffer not complete: " + std::to_string(fboStatus));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // set number of vertices per patch (tessellation)
    glPatchParameteri(GL_PATCH_VERTICES, 3);

    // load height map texture
    stbi_set_flip_vertically_on_load(true);
    int hwidth, hheight, nChannels;
#if defined TARGET_WIN32 || defined TARGET_WIN64
    const char *heightmappath = "..\\resources\\textures\\earth-21600x10800-heightmap.png";
#elif defined TARGET_MAC
    const char *heightmappath = "../../../../resources/textures/earth-21600x10800-heightmap.png";
#endif
    unsigned char *data = stbi_load(heightmappath,
                                    &hwidth, &hheight, &nChannels,
                                    0);
    if (!data)
    {
        Logger("Heightmap Texture failed to load");
        stbi_image_free(data);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hwidth, hheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        mainShader.setInt("heightMap", 0);
        Logger("Loaded heightmap of size " + std::to_string(hheight) + " x " + std::to_string(hwidth));
    }

    // world vertices
    Logger("Generating Vertices");
    int loops = 0;
    for (int i = 0; i < 6; i++)
    {
        for (float j = 0; j < verticesiter; j++)
        {
            for (float k = 0; k < verticesiter; k++)
            {
                int index = 4 * loops;
                for (int l = 0; l < 4; l++)
                {
                    float x, y, z;
                    float dx, dy, dz;
                    if (i < 2)
                    {
                        x = cubevertices[i * 32 + 8 * l] / verticesiter - (1.0f - 1.0f / verticesiter) + j * 2 / verticesiter;
                        y = cubevertices[i * 32 + 1 + 8 * l] / verticesiter - (1.0f - 1.0f / verticesiter) + k * 2 / verticesiter;
                        z = cubevertices[i * 32 + 2 + 8 * l];
                    }
                    else if (i < 4)
                    {
                        x = cubevertices[i * 32 + 8 * l];
                        y = cubevertices[i * 32 + 1 + 8 * l] / verticesiter - (1.0f - 1.0f / verticesiter) + j * 2 / verticesiter;
                        z = cubevertices[i * 32 + 2 + 8 * l] / verticesiter - (1.0f - 1.0f / verticesiter) + k * 2 / verticesiter;
                    }
                    else
                    {
                        x = cubevertices[i * 32 + 8 * l] / verticesiter - (1.0f - 1.0f / verticesiter) + j * 2 / verticesiter;
                        y = cubevertices[i * 32 + 1 + 8 * l];
                        z = cubevertices[i * 32 + 2 + 8 * l] / verticesiter - (1.0f - 1.0f / verticesiter) + k * 2 / verticesiter;
                    }
                    dx = x * sqrtf(1.0 - (y * y / 2.0) - (z * z / 2.0) + (y * y * z * z / 3.0));
                    dy = y * sqrtf(1.0 - (z * z / 2.0) - (x * x / 2.0) + (z * z * x * x / 3.0));
                    dz = z * sqrtf(1.0 - (x * x / 2.0) - (y * y / 2.0) + (x * x * y * y / 3.0));

                    glm::vec2 texcoords = glm::vec2((atan2f(dy, dx) / M_PI + 1.0) * 0.5, (asinf(dz) / M_PI + 0.5));

                    /*
                    int pix = (int)(((texcoords.x * hwidth)) + hwidth * ((int)(texcoords.y * (hheight - 1))));
                    pix *= nChannels;
                    unsigned char *texel = data + pix;
                    float disp = ((float)texel[0] / 184519.548) + 1.0;
                    dx *= disp;
                    dy *= disp;
                    dz *= disp;
                    */

                    vertices.insert(
                        vertices.end(),
                        {dx, dy, dz});

                    glm::vec3 norm = glm::normalize(glm::vec3(dx, dy, dz));
                    vertices.insert(
                        vertices.end(),
                        {norm.x,
                         norm.y,
                         norm.z,
                         texcoords.x,
                         texcoords.y});
                }
                // fix seam in texcoords when jumping from 0.0 to 1.0 at the end of texture
                int size = vertices.size();
                bool f1 = false;
                bool f2 = false;
                for (int l = 0; l < 4; l++)
                {
                    if (vertices[size - 8 * l - 2] < 0.1)
                        f1 = true;
                }
                for (int l = 0; l < 4; l++)
                {
                    if (vertices[size - 8 * l - 2] > 0.9)
                        f2 = true;
                }
                if (f1 && f2)
                {
                    for (int l = 0; l < 4; l++)
                    {
                        if (vertices[size - 8 * l - 2] < 0.1)
                            vertices[size - 8 * l - 2]++;
                    }
                }

                if (i == 0 || i == 3 || i == 4)
                {
                    indices.insert(indices.end(), {index, index + 2, index + 1, index, index + 3, index + 2});
                }
                else if (i == 1 || i == 2 || i == 5)
                {
                    indices.insert(indices.end(), {index, index + 1, index + 2, index, index + 2, index + 3});
                }
                loops++;
            }
        }
    }
    stbi_image_free(data);

    Logger("Loaded " + std::to_string(verticesiter * verticesiter) + " patches of 4 control points each");
    Logger("Processing " + std::to_string(verticesiter * verticesiter * 4) + " vertices in vertex shader");

    // cubes
    GLuint VBO;
    glGenVertexArrays(1, &texcubeVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(texcubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // multiply vector size by 4 in order to emulate sizeof c float array
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * 4, vertices.data(), GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(cubevertices), cubevertices, GL_STATIC_DRAW);
    //  position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // index buffer
    glGenBuffers(1, &texCubeIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texCubeIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, indices.data(), GL_STATIC_DRAW);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(cubeIndices),cubeIndices,GL_STATIC_DRAW);

    // sun
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glGenBuffers(1, &lightEBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // textrenderer
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glGenBuffers(1, &textEBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    // create cubemap skybox texture
    glGenTextures(1, &skybox);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
    std::vector<std::string> faces{
        "cubemaps/+x.png",
        "cubemaps/-x.png",
        "cubemaps/+y.png",
        "cubemaps/-y.png",
        "cubemaps/-z.png",
        "cubemaps/+z.png"};
    skybox = loadCubemap(faces);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // load textures
    diffuseMap = loadTexture("earth-8k.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    specularMap = loadTexture("earth-8k-spec.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    normalMap = loadTexture("earth-8k-norm.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    fontTexture = loadTexture("../font/bitmapfont.bmp", GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST);

    // shader configuration
    mainShader.use();
    mainShader.setInt("material.diffuse", 0);
    mainShader.setInt("material.specular", 1);
    mainShader.setInt("material.normal", 3);
    hdrShader.use();
    hdrShader.setInt("hdrBuffer", 0);

    cameraView = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    glfwFocusWindow(window);
    framebuffer_size_callback(window, screenWidth * 2, screenHeight * 2);

    while (!glfwWindowShouldClose(window))
    {
        gameTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 100000000 / 10000.0;

        loopTimer();

        keyActions();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);

        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        ratio = screenWidth / (float)screenHeight;

        // 1. render scene into floating point framebuffer
        // -----------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

        renderWorld(mainShader, lightShader, skyboxShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
        // --------------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        hdrShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        hdrShader.setInt("hdr", hdr);
        hdrShader.setFloat("exposure", exposure);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        renderQuad();
        if (wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        rendertext(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    Logger("Window Closed");
    exit(EXIT_SUCCESS);
}

void renderWorld(const Shader &mainShader, const Shader &lightShader, const Shader &skyboxShader)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mainShader.use();
    mainShader.setVec3("viewPos", cameraPos);
    mainShader.setFloat("material.shininess", 32.0f);
    // light properties
    mainShader.setVec3("light.ambient", 0.05, 0.05, 0.05);
    mainShader.setVec3("light.diffuse", 0.6, 0.55, 0.5);
    mainShader.setVec3("light.specular", 0.5, 0.5, 0.5);
    // light space for shadows
    mainShader.setVec3("lightPos", lightPos);

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    // bind normal map
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, normalMap);

    // pass projection matrix to shader (note that in this case it could change every frame)
    p = glm::perspective(1.57f, ratio, 0.001f, 100000.0f);
    mainShader.setMat4("projection", p);

    // camera/view transformation
    v = cameraView;
    mainShader.setMat4("view", v);

    renderScene(mainShader);

    // draw skybox
    glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
    skyboxShader.use();
    v = glm::mat4(glm::mat3(v)); // remove translation from the view matrix
    skyboxShader.setMat4("view", v);
    skyboxShader.setMat4("projection", p);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default

    // draw sun
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    lightShader.use();
    lightShader.setMat4("projection", p);
    lightShader.setMat4("view", v);
    lightShader.setVec3("circleCenter", lightPos);
    float circleRadius = 3.0f;
    lightShader.setFloat("circleRadius", circleRadius);
    glBindVertexArray(lightVAO);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, glm::vec3(circleRadius));
    lightShader.setMat4("model", model);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDisable(GL_BLEND);
    if (faceculling)
        glEnable(GL_CULL_FACE);
}

double prev_gameTime = 0.0;
void renderScene(const Shader &shader)
{
    // rotate cameraPos and cameraFront by earth's rotation
    glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), (float)glm::radians(gameTime - prev_gameTime), glm::vec3(0.0f, 1.0f, 0.0f));
    cameraPos = glm::vec3(glm::vec4(cameraPos, 1.0f) * rotation_matrix);
    cameraFront = glm::vec3(glm::vec4(cameraFront, 1.0f) * rotation_matrix);
    prev_gameTime = gameTime;

    // bind vertex array
    glBindVertexArray(texcubeVAO);

    // bind index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texCubeIndexBuffer);

    // calculate the model matrix for each object and pass it to shader before drawing
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, (float)glm::radians(gameTime), glm::vec3(0.0f, 0.0f, -1.0f));
    model = glm::scale(model, glm::vec3(10.0));
    shader.setMat4("model", model);
    glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, nullptr);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            0.0f,
            0.0f,
            1.0f,
            1.0f,
            0.0f,
            1.0f,
            1.0f,
            1.0f,
            -1.0f,
            0.0f,
            1.0f,
            0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}