#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <stdio.h>
#include <thread>
#include <string>
#include <unistd.h>

#include "textrenderer.h"
#include "logger.h"
#include "controls.h"
#include "utils.h"
#include "shader.h"

//config
bool guitext = true;
bool hdr = false;
float exposure = 0.5f;
int screenWidth=960, screenHeight=540;
bool shadows = true;
const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
bool debugDepth=false;
float debug_near_plane = 50.0f, debug_far_plane = 200.0f;

/* ---------- */

unsigned int colorBuffer,rboDepth;

void renderWorld(const Shader &mainShader, const Shader &lightShader, const Shader &debugDepthQuad);
void renderScene(const Shader &shader);
void renderShadows(const Shader &simpleDepthShader);
void renderQuad();

glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -5.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -2.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, 2.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};

static void error_callback(int error, const char* description)
{
    Logger("Error: "+(std::string)description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    keycallback(window,key,scancode,action,mods);
}

static void cursor_position_callback(GLFWwindow* window, double xposIn, double yposIn){
    cursorpositioncallback(window,xposIn,yposIn);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    //focus on the window when clicked on, esc to re-enable cursor
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);

    //color buffer
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    //render buffer
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

void rendertext(GLFWwindow* window){
    if(guitext){
        glDisable(GL_CULL_FACE);
        std::string fpstring="FPS "+std::to_string(fps).substr(0,4);
        textRenderer(fpstring.c_str(),32.0,-32.0,window,32,1.0,1.0,1.0,topLeft);
        std::string pos=std::to_string(cameraPos[0]).substr(0,5)+", "+std::to_string(cameraPos[1]).substr(0,5)+", "+std::to_string(cameraPos[2]).substr(0,5);
        textRenderer(pos.c_str(),32.0,-96.0,window,32,1.0,1.0,1.0,topLeft);
        glEnable(GL_CULL_FACE);
    }
}

float ratio;
glm::mat4 m, v, p, mvp;
GLuint texcubeVAO,texCubeIndexBuffer;

glm::mat4 lightProjection, lightView, lightSpaceMatrix;
glm::vec3 lightPos;

unsigned int diffuseMap, specularMap, depthMap;

GLuint lightVBO, lightEBO, lightVAO;
unsigned int depthMapFBO;

int main(void)
{
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    Logger("Program Initialized");

    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);
 
    if (!glfwInit())
    {
        Logger("glfwInit Failed");
        exit(EXIT_FAILURE);
    }
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
 
    window = glfwCreateWindow(screenWidth, screenHeight, "MaxRockatasky Simulator", NULL, NULL);
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

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    Shader mainShader("main.vert","main.frag");
    Shader lightShader("main.vert","light.frag");
    Shader simpleDepthShader("shadow_mapping_depth.vert", "shadow_mapping_depth.frag");
    Shader debugDepthQuad("debug_quad.vert", "debug_quad_depth.frag");
    Shader hdrShader("hdr.vert", "hdr.frag");

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
        Logger("Framebuffer not complete: "+std::to_string(fboStatus));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //cubes
    GLuint VBO;
    glGenVertexArrays(1, &texcubeVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(texcubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubevertices), cubevertices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    //index buffer
    glGenBuffers(1, &texCubeIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texCubeIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(cubeIndices),cubeIndices,GL_STATIC_DRAW);
 
    //sun
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glGenBuffers(1, &lightEBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //textrenderer
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glGenBuffers(1, &textEBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // load textures
    diffuseMap=loadTexture("container2.png",GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR);
    specularMap=loadTexture("container2_specular.png",GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR);

    fontTexture=loadTexture("../font/bitmapfont.bmp",GL_NEAREST_MIPMAP_NEAREST,GL_NEAREST);

    // configure depth map FBO
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    //shader configuration
    mainShader.use();
    mainShader.setInt("material.diffuse", 0);
    mainShader.setInt("material.specular", 1);
    mainShader.setInt("shadowMap",2);
    debugDepthQuad.use();
    debugDepthQuad.setInt("depthMap", 0);
    hdrShader.use();
    hdrShader.setInt("hdrBuffer", 0);

    while (!glfwWindowShouldClose(window))
    {
        double gameTime=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()%100000000;
        float tx=sin(glm::radians(gameTime)/10000)*100+cameraPos[0];
        float ty=cos(glm::radians(gameTime)/10000)*100+cameraPos[1];
        float tz=cameraPos[2];

        loopTimer();

        keyActions();

        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);

        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        ratio = screenWidth / (float) screenHeight;

        lightPos=glm::vec3(tx,ty,tz);
        
        if(shadows)
        {
            renderShadows(simpleDepthShader);
        }

        // 1. render scene into floating point framebuffer
        // -----------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
            
        renderWorld(mainShader, lightShader, debugDepthQuad);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
        // --------------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        hdrShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        hdrShader.setInt("hdr", hdr);
        hdrShader.setFloat("exposure", exposure);
        renderQuad();

        rendertext(window);
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
 
    glfwTerminate();
    Logger("Window Closed");
    exit(EXIT_SUCCESS);
}

void renderWorld(const Shader &mainShader, const Shader &lightShader, const Shader &debugDepthQuad){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mainShader.use();
    mainShader.setVec3("viewPos", cameraPos);
    mainShader.setFloat("material.shininess", 32.0f);
    //light properties
    mainShader.setVec3("light.ambient",0.05,0.05,0.05);
    mainShader.setVec3("light.diffuse",0.6,0.55,0.5);
    mainShader.setVec3("light.specular",0.5,0.5,0.5);
    // light space matrix for shadows
    mainShader.setVec3("lightPos", lightPos);
    mainShader.setMat4("lightSpaceMatrix",lightSpaceMatrix);

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    // bind depth map
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    // pass projection matrix to shader (note that in this case it could change every frame)
    p=glm::perspective(1.57f, ratio, 0.001f, 100000.0f);
    mainShader.setMat4("projection",p);

    // camera/view transformation
    v=glm::lookAt(cameraPos,cameraPos+cameraFront,cameraUp);
    mainShader.setMat4("view",v);

    renderScene(mainShader);

    //draw sun
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    lightShader.use();
    lightShader.setMat4("projection", p);
    lightShader.setMat4("view", v);
    lightShader.setVec3("circleCenter",lightPos);
    float circleRadius=7.0f;
    lightShader.setFloat("circleRadius",circleRadius);
    glBindVertexArray(lightVAO);
    glm::mat4 model=glm::mat4(1.0f);
    model=glm::translate(model, lightPos);
    model=glm::rotate(model,glm::radians(90.0f),glm::vec3(0.0,1.0,0.0));
    model=glm::rotate(model,-atan(lightPos[1]/lightPos[0]),glm::vec3(1.0,0.0,0.0));
    model=glm::scale(model,glm::vec3(circleRadius));
    lightShader.setMat4("model", model);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    // render Depth map to quad for visual debugging
    // ---------------------------------------------
    if(debugDepth){
        debugDepthQuad.use();
        debugDepthQuad.setFloat("near_plane", debug_near_plane);
        debugDepthQuad.setFloat("far_plane", debug_far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderQuad();
    }
}

void renderScene(const Shader &shader){
    // bind vertex array
    glBindVertexArray(texcubeVAO);

    // bind index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,texCubeIndexBuffer);

    for (unsigned int i = 0; i < 10; i++)
    {
        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model=glm::mat4(1.0f);
        model=glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model=glm::rotate(model,glm::radians(angle),glm::vec3(1.0f,0.3f,0.5f));
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    }
}

void renderShadows(const Shader &simpleDepthShader)
{
    glm::vec3 center(0.0f,0.0f,0.0f);
    glm::vec3 up(0.0f,1.0f,0.0f);
    // render depth of scene to texture from the light's perspective
    lightProjection=glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, debug_near_plane, debug_far_plane);
    lightView=glm::lookAt(lightPos, center, up);
    lightSpaceMatrix=lightProjection*lightView;
    // render scene from light's point of view
    simpleDepthShader.use();
    simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    renderScene(simpleDepthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // reset viewport
    glViewport(0, 0, screenWidth, screenHeight);
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
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}