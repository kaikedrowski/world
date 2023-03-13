#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "linmath.h"

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
bool guitext=true;

/* ---------- */

vec3 cubePositions[] = {
    { 0.0f,  0.0f,  0.0f},
    { 2.0f,  5.0f, -15.0f},
    {-1.5f, -2.2f, -2.5f},
    {-3.8f, -2.0f, -12.3f},
    { 2.4f, -0.4f, -3.5f},
    {-1.7f,  3.0f, -7.5f},
    { 1.3f, -2.0f, -2.5f},
    { 1.5f,  2.0f, -2.5f},
    { 1.5f,  0.2f, -1.5f},
    {-1.3f,  1.0f, -1.5f}
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
}

void rendertext(GLFWwindow* window){
    if(guitext){
        std::string fpstring="FPS "+std::to_string(fps).substr(0,4);
        textRenderer(fpstring.c_str(),32.0,-32.0,window,32,1.0,1.0,1.0,topLeft);
        std::string pos=std::to_string(cameraPos[0]).substr(0,5)+", "+std::to_string(cameraPos[1]).substr(0,5)+", "+std::to_string(cameraPos[2]).substr(0,5);
        textRenderer(pos.c_str(),32.0,-96.0,window,32,1.0,1.0,1.0,topLeft);
    }
}

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
 
    window = glfwCreateWindow(960, 540, "MaxRockatasky Simulator", NULL, NULL);
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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);  

    Shader mainShader("main.vert","main.frag");
    Shader lightShader("main.vert","light.frag");

    //cubes
    GLuint VBO, texcubeVAO;
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
 
    //sun
    GLuint lightVBO, lightEBO, lightVAO;
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
    unsigned int diffuseMap=loadTexture("container2.png",GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR);
    unsigned int specularMap=loadTexture("container2_specular.png",GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR);

    fontTexture=loadTexture("../font/bitmapfont.bmp",GL_NEAREST_MIPMAP_NEAREST,GL_NEAREST);

    // configure depth map FBO
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
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

    while (!glfwWindowShouldClose(window))
    {
        loopTimer();

        keyActions();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);

        mainShader.use();
        mainShader.setVec3("viewPos", cameraPos);
        mainShader.setFloat("material.shininess", 32.0f);

        double gameTime=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()%100000000;

        float tx=sin(radians(gameTime)/10000)*50000;
        float ty=cos(radians(gameTime)/10000)*50000;
        float tz=1.0f;

        //light properties
        // directional light
        mainShader.setVec3("dirLight.direction", -tx, -ty, -tz);
        mainShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
        mainShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
        mainShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
        // point light 1
        mainShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        mainShader.setVec3("pointLights[0].position", tx, ty, tz);
        mainShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        mainShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        mainShader.setFloat("pointLights[0].constant", 1.0f);
        mainShader.setFloat("pointLights[0].linear", 0.0014f);
        mainShader.setFloat("pointLights[0].quadratic", 0.000007f);
        // spotLight
        mainShader.setVec3("spotLight.position", cameraPos);
        mainShader.setVec3("spotLight.direction", cameraFront);
        mainShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        mainShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
        mainShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
        mainShader.setFloat("spotLight.constant", 1.0f);
        mainShader.setFloat("spotLight.linear", 0.09f);
        mainShader.setFloat("spotLight.quadratic", 0.032f);
        mainShader.setFloat("spotLight.cutOff", cos(radians(12.5f)));
        mainShader.setFloat("spotLight.outerCutOff", cos(radians(15.0f)));

        float ratio;
        int width, height;
        mat4x4 m, v, p, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        // pass projection matrix to shader (note that in this case it could change every frame)
        mat4x4_perspective(p, 1.57, ratio, 0.001, 100000.0);
        mainShader.setMat4("projection",p);

        // camera/view transformation
        vec3 sum;
        vec3_add(sum,cameraPos,cameraFront);
        mat4x4_look_at(v,cameraPos,sum,cameraUp);
        mainShader.setMat4("view",v);

        // world transformation
        mat4x4_identity(m);// make sure to initialize matrix to identity matrix first
        mainShader.setMat4("model",m);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glBindVertexArray(texcubeVAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            mat4x4 model;
            mat4x4_identity(model);
            mat4x4_translate(model, cubePositions[i][0],cubePositions[i][1],cubePositions[i][2]);
            float angle = 20.0f * i;
            mat4x4_rotate(model,model,1.0f,0.3f,0.5f,radians(angle));
            mainShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        lightShader.use();
        lightShader.setMat4("projection", p);
        lightShader.setMat4("view", v);
        lightShader.setVec3("circleCenter",tx,ty,tz);
        float circleRadius=3000.0f;
        lightShader.setFloat("circleRadius",circleRadius);
        glBindVertexArray(lightVAO);
        for (unsigned int i = 0; i < 1; i++)
        {
            mat4x4 model;
            mat4x4_identity(model);
            mat4x4_translate(model, tx,ty,tz);
            mat4x4_rotate_Y(model,model,radians(90));
            mat4x4_rotate_X(model,model,-atan(ty/tx));
            mat4x4_scale_aniso(model, model, circleRadius, circleRadius, circleRadius);
            lightShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        glDisable(GL_BLEND);

        rendertext(window);
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
 
    glfwTerminate();
    Logger("Window Closed");
    exit(EXIT_SUCCESS);
}