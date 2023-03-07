#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "utils.h"
#include "shader.h"

const int fontSize=32;

GLuint textVAO,textVBO,textEBO;
unsigned int fontTexture;

const float texCoordScale=0.125;

float posX=0.0,posY=0.0;

int width, height;

float quadVertices[] = {
    // positions          // colors           // texture coords
     1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   texCoordScale+posX, texCoordScale+posY,   // top right
     1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   texCoordScale+posX, 0.0f+posY,   // bottom right
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f+posX,          0.0f+posY,   // bottom left
    -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f+posX,          texCoordScale+posY    // top left 
};
unsigned int quadIndices[] = {  
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

void textCoordinates(char glyph) {
    switch(glyph) {
        default:
            Logger("Invalid glyph");
            break;
        case ' ':
            posX=0.0;
            posY=0.125*7;
            break;
        case '!':
            posX=0.125;
            posY=0.125*7;
            break;
        case '\"':
            posX=0.125*2;
            posY=0.125*7;
            break;
        case '#':
            posX=0.125*3;
            posY=0.125*7;
            break;
        case '$':
            posX=0.125*4;
            posY=0.125*7;
            break;
        case '%':
            posX=0.125*5;
            posY=0.125*7;
            break;
        case '&':
            posX=0.125*6;
            posY=0.125*7;
            break;
        case '\'':
            posX=0.125*7;
            posY=0.125*7;
            break;
        case '(':
            posX=0.0;
            posY=0.125*6;
            break;
        case ')':
            posX=0.125;
            posY=0.125*6;
            break;
        case '*':
            posX=0.125*2;
            posY=0.125*6;
            break;
        case '+':
            posX=0.125*3;
            posY=0.125*6;
            break;
        case ',':
            posX=0.125*4;
            posY=0.125*6;
            break;
        case '-':
            posX=0.125*5;
            posY=0.125*6;
            break;
        case '.':
            posX=0.125*6;
            posY=0.125*6;
            break;
        case '/':
            posX=0.125*7;
            posY=0.125*6;
            break;
        case '0':
            posX=0.0;
            posY=0.125*5;
            break;
        case '1':
            posX=0.125;
            posY=0.125*5;
            break;
        case '2':
            posX=0.125*2;
            posY=0.125*5;
            break;
        case '3':
            posX=0.125*3;
            posY=0.125*5;
            break;
        case '4':
            posX=0.125*4;
            posY=0.125*5;
            break;
        case '5':
            posX=0.125*5;
            posY=0.125*5;
            break;
        case '6':
            posX=0.125*6;
            posY=0.125*5;
            break;
        case '7':
            posX=0.125*7;
            posY=0.125*5;
            break;
        case '8':
            posX=0.0;
            posY=0.125*4;
            break;
        case '9':
            posX=0.125;
            posY=0.125*4;
            break;
        case ':':
            posX=0.125*2;
            posY=0.125*4;
            break;
        case ';':
            posX=0.125*3;
            posY=0.125*4;
            break;
        case '<':
            posX=0.125*4;
            posY=0.125*4;
            break;
        case '=':
            posX=0.125*5;
            posY=0.125*4;
            break;
        case '>':
            posX=0.125*6;
            posY=0.125*4;
            break;
        case '?':
            posX=0.125*7;
            posY=0.125*4;
            break;
        case '@':
            posX=0.0;
            posY=0.125*3;
            break;
        case 'a':case 'A':
            posX=0.125;
            posY=0.125*3;
            break;
        case 'b':case 'B':
            posX=0.125*2;
            posY=0.125*3;
            break;
        case 'c':case 'C':
            posX=0.125*3;
            posY=0.125*3;
            break;
        case 'd':case 'D':
            posX=0.125*4;
            posY=0.125*3;
            break;
        case 'e':case 'E':
            posX=0.125*5;
            posY=0.125*3;
            break;
        case 'f':case 'F':
            posX=0.125*6;
            posY=0.125*3;
            break;
        case 'g':case 'G':
            posX=0.125*7;
            posY=0.125*3;
            break;
        case 'h':case 'H':
            posX=0.0;
            posY=0.125*2;
            break;
        case 'i':case 'I':
            posX=0.125;
            posY=0.125*2;
            break;
        case 'j':case 'J':
            posX=0.125*2;
            posY=0.125*2;
            break;
        case 'k':case 'K':
            posX=0.125*3;
            posY=0.125*2;
            break;
        case 'l':case 'L':
            posX=0.125*4;
            posY=0.125*2;
            break;
        case 'm':case 'M':
            posX=0.125*5;
            posY=0.125*2;
            break;
        case 'n':case 'N':
            posX=0.125*6;
            posY=0.125*2;
            break;
        case 'o':case 'O':
            posX=0.125*7;
            posY=0.125*2;
            break;
        case 'p':case 'P':
            posX=0.0;
            posY=0.125;
            break;
        case 'q':case 'Q':
            posX=0.125;
            posY=0.125;
            break;
        case 'r':case 'R':
            posX=0.125*2;
            posY=0.125;
            break;
        case 's':case 'S':
            posX=0.125*3;
            posY=0.125;
            break;
        case 't':case 'T':
            posX=0.125*4;
            posY=0.125;
            break;
        case 'u':case 'U':
            posX=0.125*5;
            posY=0.125;
            break;
        case 'v':case 'V':
            posX=0.125*6;
            posY=0.125;
            break;
        case 'w':case 'W':
            posX=0.125*7;
            posY=0.125;
            break;
        case 'x':case 'X':
            posX=0.0;
            posY=0.0;
            break;
        case 'y':case 'Y':
            posX=0.125;
            posY=0.0;
            break;
        case 'z':case 'Z':
            posX=0.125*2;
            posY=0.0;
            break;
        case '[':
            posX=0.125*3;
            posY=0.0;
            break;
        case '\\':
            posX=0.125*4;
            posY=0.0;
            break;
        case ']':
            posX=0.125*5;
            posY=0.0;
            break;
        case '^':
            posX=0.125*6;
            posY=0.0;
            break;
        case '_':
            posX=0.125*7;
            posY=0.0;
            break;
    }
}

inline static void textRenderer(const char* text, float x, float y, GLFWwindow* window){   
    Shader program("2dtext.vert","2dtext.frag");

    glfwGetFramebufferSize(window, &width, &height);

    float fwidth=width,fheight=height;Logger(std::to_string(fheight/fwidth));

    for(int i=0;i<strlen(text);i++){
        //set glyph texcoords
        char glyph=text[i];
        textCoordinates(glyph);
        quadVertices[6]=texCoordScale+posX;
        quadVertices[7]=texCoordScale+posY;
        quadVertices[14]=texCoordScale+posX;
        quadVertices[15]=0.0f+posY;
        quadVertices[22]=0.0f+posX;
        quadVertices[23]=0.0f+posY;
        quadVertices[30]=0.0f+posX;
        quadVertices[31]=texCoordScale+posY;

        float ratio=fheight/fwidth;
        float scale=fontSize/fheight;
        float dist=2*(fontSize*i)/fwidth;

        quadVertices[0]=ratio*scale+dist;
        quadVertices[1]=1.0*scale;
        quadVertices[8]=ratio*scale+dist;
        quadVertices[9]=-1.0*scale;
        quadVertices[16]=-ratio*scale+dist;
        quadVertices[17]=-1.0*scale;
        quadVertices[24]=-ratio*scale+dist;
        quadVertices[25]=1.0*scale;
    
        //update quadVertices in the VBO
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        
        program.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fontTexture);
        glBindVertexArray(textVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

#endif /* TEXTRENDERER_H */