#ifndef CONTROLS_H
#define CONTROLS_H

#include "utils.h"
#include "linmath.h"

vec3 cameraPos={0.0,0.0,3.0};
vec3 cameraFront={0.0,0.0,-1.0};
vec3 cameraUp={0.0,1.0,0.0};
float cameraSpeed=0.05f;

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

bool key_forward=false;//w
bool key_left=false;//a
bool key_back=false;//s
bool key_right=false;//d
bool key_up=false;//space
bool key_down=false;//lshift

enum camera_mode {camera_mode_fps, camera_mode_three_dimensional};

int camera_mode=camera_mode_fps;

static inline void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(action==GLFW_PRESS)
        switch(key){
            case GLFW_KEY_W:
                key_forward=true;
                break;
            case GLFW_KEY_A:
                key_left=true;
                break;
            case GLFW_KEY_S:
                key_back=true;
                break;
            case GLFW_KEY_D:
                key_right=true;
                break;
            case GLFW_KEY_SPACE:
                key_up=true;
                break;
            case GLFW_KEY_LEFT_SHIFT:
                key_down=true;
                break;
            case GLFW_KEY_ESCAPE:
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    if(action==GLFW_RELEASE)
        switch(key){
            case GLFW_KEY_W:
                key_forward=false;
                break;
            case GLFW_KEY_A:
                key_left=false;
                break;
            case GLFW_KEY_S:
                key_back=false;
                break;
            case GLFW_KEY_D:
                key_right=false;
                break;
            case GLFW_KEY_SPACE:
                key_up=false;
                break;
            case GLFW_KEY_LEFT_SHIFT:
                key_down=false;
                break;
        }
}

static inline void keyActions() {
    switch(camera_mode){
        case camera_mode_fps:
            if(key_forward){
                vec3 product;
                //set cameraFront y value to 0 to restrict vertical movement
                float y=cameraFront[1];
                cameraFront[1]=0;
                vec3_scale(product,cameraFront,cameraSpeed);
                cameraFront[1]=y;
                vec3_add(cameraPos,cameraPos,product);
            }
            if(key_left){
                vec3 product;
                vec3_mul_cross(product,cameraFront,cameraUp);
                vec3_norm(product,product);
                vec3_scale(product,product,cameraSpeed);
                vec3_sub(cameraPos,cameraPos,product);
            }
            if(key_back){
                vec3 product;
                //set cameraFront y value to 0 to restrict vertical movement
                float y=cameraFront[1];
                cameraFront[1]=0;
                vec3_scale(product,cameraFront,cameraSpeed);
                cameraFront[1]=y;
                vec3_sub(cameraPos,cameraPos,product);
            }
            if(key_right){
                vec3 product;
                vec3_mul_cross(product,cameraFront,cameraUp);
                vec3_norm(product,product);
                vec3_scale(product,product,cameraSpeed);
                vec3_add(cameraPos,cameraPos,product);
            }
            if(key_up){
                cameraPos[1]+=cameraSpeed;
            }
            if(key_down){
                cameraPos[1]-=cameraSpeed;
            }
            break;
        case camera_mode_three_dimensional:
            if(key_forward){
                vec3 product;
                vec3_scale(product,cameraFront,cameraSpeed);
                vec3_add(cameraPos,cameraPos,product);
            }
            if(key_left){
                vec3 product;
                vec3_mul_cross(product,cameraFront,cameraUp);
                vec3_norm(product,product);
                vec3_scale(product,product,cameraSpeed);
                vec3_sub(cameraPos,cameraPos,product);
            }
            if(key_back){
                vec3 product;
                vec3_scale(product,cameraFront,cameraSpeed);
                vec3_sub(cameraPos,cameraPos,product);
            }
            if(key_right){
                vec3 product;
                vec3_mul_cross(product,cameraFront,cameraUp);
                vec3_norm(product,product);
                vec3_scale(product,product,cameraSpeed);
                vec3_add(cameraPos,cameraPos,product);
            }
            if(key_up){
                cameraPos[1]+=cameraSpeed;
            }
            if(key_down){
                cameraPos[1]-=cameraSpeed;
            }
            break;
    }
}

static inline void cursorpositioncallback(GLFWwindow* window, double xposIn, double yposIn){
    if(glfwGetInputMode(window,GLFW_CURSOR)==GLFW_CURSOR_NORMAL)return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    vec3 front;
    front[0] = cos(radians(yaw)) * cos(radians(pitch));
    front[1] = sin(radians(pitch));
    front[2] = sin(radians(yaw)) * cos(radians(pitch));
    vec3_norm(cameraFront,front);
}

#endif /* CONTROLS_H */