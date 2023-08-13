#ifndef CONTROLS_H
#define CONTROLS_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utils.h"

glm::vec3 cameraPos(0.0,0.0,15.0);
glm::vec3 cameraFront(0.0,0.0,-1.0);
glm::vec3 cameraUp(0.0,1.0,0.0);
glm::vec3 cameraRight(1.0,0.0,0.0);
float cameraSpeed=0.05f;
glm::mat4 cameraView;

glm::vec3 pole(0.0,1.0,0.0);

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

enum camera_mode {camera_mode_fps, camera_mode_three_dimensional, camera_mode_orbital};

int camera_mode=camera_mode_orbital;

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
                glm::vec3 cameraFrontTemp=cameraFront;
                //set cameraFront y value to 0 to restrict vertical movement
                float y=cameraFront[1];
                cameraFront[1]=0;
                cameraFront=glm::normalize(cameraFront);
                cameraPos+=cameraFront*cameraSpeed;
                cameraFront[1]=y;
                cameraFront=cameraFrontTemp;
            }
            if(key_left){
                cameraPos-=cameraRight*cameraSpeed;
            }
            if(key_back){
                glm::vec3 cameraFrontTemp=cameraFront;
                glm::vec3 product;
                //set cameraFront y value to 0 to restrict vertical movement
                float y=cameraFront[1];
                cameraFront[1]=0;
                cameraFront=glm::normalize(cameraFront);
                cameraPos-=cameraFront*cameraSpeed;
                cameraFront[1]=y;
                cameraFront=cameraFrontTemp;
            }
            if(key_right){
                cameraPos+=cameraRight*cameraSpeed;
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
                cameraPos+=cameraFront*cameraSpeed;
            }
            if(key_left){
                cameraPos-=cameraRight*cameraSpeed;
            }
            if(key_back){
                cameraPos-=cameraFront*cameraSpeed;
            }
            if(key_right){
                cameraPos+=cameraRight*cameraSpeed;
            }
            if(key_up){
                cameraPos[1]+=cameraSpeed;
            }
            if(key_down){
                cameraPos[1]-=cameraSpeed;
            }
            break;
        case camera_mode_orbital:
            glm::vec3 dist=cameraPos-(cameraUp*10.0f);
            float distfloat=sqrtf(dist.x*dist.x+dist.y*dist.y+dist.z*dist.z);
            if(key_forward){
                glm::vec3 oldpos=cameraPos;
                cameraPos-=glm::normalize(glm::cross(cameraRight,cameraUp))*distfloat*cameraSpeed/10.0f;
                double acos_param=((double)oldpos.x*cameraPos.x+(double)oldpos.y*cameraPos.y+(double)oldpos.z*cameraPos.z)/(sqrt((double)oldpos.x*oldpos.x+(double)oldpos.y*oldpos.y+(double)oldpos.z*oldpos.z)*sqrt((double)cameraPos.x*cameraPos.x+(double)cameraPos.y*cameraPos.y+(double)cameraPos.z*cameraPos.z));
                //protect against precision error resulting in undefined number
                if(acos_param>1.0)
                    acos_param=1.0;
                else if(acos_param<-1.0)
                    acos_param=-1.0;
                float angle=acos(acos_param);
                glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f),-angle,glm::cross(oldpos,cameraPos));
                cameraFront=glm::vec3(glm::vec4(cameraFront,1.0f)*rotation_matrix);
            }
            if(key_left){
                glm::vec3 oldpos=cameraPos;
                cameraPos-=cameraRight*distfloat*cameraSpeed/10.0f;
                double acos_param=((double)oldpos.x*cameraPos.x+(double)oldpos.y*cameraPos.y+(double)oldpos.z*cameraPos.z)/(sqrt((double)oldpos.x*oldpos.x+(double)oldpos.y*oldpos.y+(double)oldpos.z*oldpos.z)*sqrt((double)cameraPos.x*cameraPos.x+(double)cameraPos.y*cameraPos.y+(double)cameraPos.z*cameraPos.z));
                //protect against precision error resulting in undefined number
                if(acos_param>1.0)
                    acos_param=1.0;
                else if(acos_param<-1.0)
                    acos_param=-1.0;
                float angle=acos(acos_param);
                glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f),-angle,glm::cross(oldpos,cameraPos));
                cameraFront=glm::vec3(glm::vec4(cameraFront,1.0f)*rotation_matrix);
            }
            if(key_back){
                glm::vec3 oldpos=cameraPos;
                cameraPos+=glm::normalize(glm::cross(cameraRight,cameraUp))*distfloat*cameraSpeed/10.0f;
                double acos_param=((double)oldpos.x*cameraPos.x+(double)oldpos.y*cameraPos.y+(double)oldpos.z*cameraPos.z)/(sqrt((double)oldpos.x*oldpos.x+(double)oldpos.y*oldpos.y+(double)oldpos.z*oldpos.z)*sqrt((double)cameraPos.x*cameraPos.x+(double)cameraPos.y*cameraPos.y+(double)cameraPos.z*cameraPos.z));
                //protect against precision error resulting in undefined number
                if(acos_param>1.0)
                    acos_param=1.0;
                else if(acos_param<-1.0)
                    acos_param=-1.0;
                float angle=acos(acos_param);
                glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f),-angle,glm::cross(oldpos,cameraPos));
                cameraFront=glm::vec3(glm::vec4(cameraFront,1.0f)*rotation_matrix);
            }
            if(key_right){
                glm::vec3 oldpos=cameraPos;
                cameraPos+=cameraRight*distfloat*cameraSpeed/10.0f;
                double acos_param=((double)oldpos.x*cameraPos.x+(double)oldpos.y*cameraPos.y+(double)oldpos.z*cameraPos.z)/(sqrt((double)oldpos.x*oldpos.x+(double)oldpos.y*oldpos.y+(double)oldpos.z*oldpos.z)*sqrt((double)cameraPos.x*cameraPos.x+(double)cameraPos.y*cameraPos.y+(double)cameraPos.z*cameraPos.z));
                //protect against precision error resulting in undefined number
                if(acos_param>1.0)
                    acos_param=1.0;
                else if(acos_param<-1.0)
                    acos_param=-1.0;
                float angle=acos(acos_param);
                glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f),-angle,glm::cross(oldpos,cameraPos));
                cameraFront=glm::vec3(glm::vec4(cameraFront,1.0f)*rotation_matrix);
            }
            if(key_up){
                if(abs(dist.x)+abs(dist.y)+abs(dist.z)<10000.0f)
                    cameraPos*=glm::vec3(1.0f)+distfloat*cameraSpeed/100.0f;
            }
            if(key_down){
                if(abs(dist.x)+abs(dist.y)+abs(dist.z)>0.0f)
                    cameraPos/=glm::vec3(1.0f)+distfloat*cameraSpeed/100.0f;
            }
            cameraUp=normalize(cameraPos);
            break;
    }
    cameraRight=glm::normalize(glm::cross(cameraFront,cameraUp));
    cameraView=glm::lookAt(cameraPos,cameraPos+cameraFront,cameraUp);
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

    if(yaw>360.0f)
        yaw-=360.0f;
    if(yaw<-360.0f)
        yaw+=360.0f;

    if(camera_mode!=camera_mode_orbital){
        glm::vec3 front;
        front[0] = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front[1] = sin(glm::radians(pitch));
        front[2] = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront=glm::normalize(front);
        cameraView=glm::lookAt(cameraPos,cameraPos+cameraFront,cameraUp);
    }else{
        glm::quat world_axes_rot=
        glm::normalize(
            glm::rotate(
                glm::normalize(glm::angleAxis(-glm::radians(yaw),glm::vec3(0.0f,1.0f,0.0f))),
                glm::radians(pitch),
                glm::vec3(1.0f,0.0f,0.0f)
            )
        );

        cameraUp=normalize(cameraPos);
        float dp=glm::dot(cameraUp,pole);
        glm::vec3 value=pole-dp*cameraUp;
        pole=glm::normalize(value);
        
        glm::mat4 local_transform;

        local_transform[0] = glm::vec4(pole.x, pole.y, pole.z, 0.0f);
        local_transform[1] = glm::vec4(cameraUp.x, cameraUp.y, cameraUp.z, 0.0f);
        glm::vec3 tmp = glm::cross(pole,cameraUp);
        local_transform[2] = glm::vec4(tmp.x, tmp.y, tmp.z, 0.0f);
        local_transform[3] = glm::vec4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);

        cameraView=local_transform*glm::mat4_cast(world_axes_rot);
        cameraFront=-1.0f*glm::vec3(cameraView[2]);
        cameraView=glm::inverse(cameraView);
    }
    cameraRight=glm::normalize(glm::cross(cameraFront,cameraUp));
}

#endif /* CONTROLS_H */