#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "utils.h"
#include "shader.h"

enum originPos{bottomLeft,topLeft,bottomRight,topRight,center};

const int fontSize=0;

GLuint textVAO,textVBO,textEBO;
unsigned int fontTexture;

const float texCoordScale=0.125;

float texposX=0.0,texposY=0.0;

int width, height;

float textQuadVertices[] = {
    // positions          // colors           // texture coords
     1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   texCoordScale+texposX, texCoordScale+texposY,   // top right
     1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   texCoordScale+texposX, 0.0f+texposY,   // bottom right
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f+texposX,          0.0f+texposY,   // bottom left
    -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f+texposX,          texCoordScale+texposY    // top left 
};

void textCoordinates(char glyph) {
    switch(glyph) {
        default:
            Logger("Invalid glyph");
            break;
        case ' ':
            texposX=0.0;
            texposY=0.125*7;
            break;
        case '!':
            texposX=0.125;
            texposY=0.125*7;
            break;
        case '\"':
            texposX=0.125*2;
            texposY=0.125*7;
            break;
        case '#':
            texposX=0.125*3;
            texposY=0.125*7;
            break;
        case '$':
            texposX=0.125*4;
            texposY=0.125*7;
            break;
        case '%':
            texposX=0.125*5;
            texposY=0.125*7;
            break;
        case '&':
            texposX=0.125*6;
            texposY=0.125*7;
            break;
        case '\'':
            texposX=0.125*7;
            texposY=0.125*7;
            break;
        case '(':
            texposX=0.0;
            texposY=0.125*6;
            break;
        case ')':
            texposX=0.125;
            texposY=0.125*6;
            break;
        case '*':
            texposX=0.125*2;
            texposY=0.125*6;
            break;
        case '+':
            texposX=0.125*3;
            texposY=0.125*6;
            break;
        case ',':
            texposX=0.125*4;
            texposY=0.125*6;
            break;
        case '-':
            texposX=0.125*5;
            texposY=0.125*6;
            break;
        case '.':
            texposX=0.125*6;
            texposY=0.125*6;
            break;
        case '/':
            texposX=0.125*7;
            texposY=0.125*6;
            break;
        case '0':
            texposX=0.0;
            texposY=0.125*5;
            break;
        case '1':
            texposX=0.125;
            texposY=0.125*5;
            break;
        case '2':
            texposX=0.125*2;
            texposY=0.125*5;
            break;
        case '3':
            texposX=0.125*3;
            texposY=0.125*5;
            break;
        case '4':
            texposX=0.125*4;
            texposY=0.125*5;
            break;
        case '5':
            texposX=0.125*5;
            texposY=0.125*5;
            break;
        case '6':
            texposX=0.125*6;
            texposY=0.125*5;
            break;
        case '7':
            texposX=0.125*7;
            texposY=0.125*5;
            break;
        case '8':
            texposX=0.0;
            texposY=0.125*4;
            break;
        case '9':
            texposX=0.125;
            texposY=0.125*4;
            break;
        case ':':
            texposX=0.125*2;
            texposY=0.125*4;
            break;
        case ';':
            texposX=0.125*3;
            texposY=0.125*4;
            break;
        case '<':
            texposX=0.125*4;
            texposY=0.125*4;
            break;
        case '=':
            texposX=0.125*5;
            texposY=0.125*4;
            break;
        case '>':
            texposX=0.125*6;
            texposY=0.125*4;
            break;
        case '?':
            texposX=0.125*7;
            texposY=0.125*4;
            break;
        case '@':
            texposX=0.0;
            texposY=0.125*3;
            break;
        case 'a':case 'A':
            texposX=0.125;
            texposY=0.125*3;
            break;
        case 'b':case 'B':
            texposX=0.125*2;
            texposY=0.125*3;
            break;
        case 'c':case 'C':
            texposX=0.125*3;
            texposY=0.125*3;
            break;
        case 'd':case 'D':
            texposX=0.125*4;
            texposY=0.125*3;
            break;
        case 'e':case 'E':
            texposX=0.125*5;
            texposY=0.125*3;
            break;
        case 'f':case 'F':
            texposX=0.125*6;
            texposY=0.125*3;
            break;
        case 'g':case 'G':
            texposX=0.125*7;
            texposY=0.125*3;
            break;
        case 'h':case 'H':
            texposX=0.0;
            texposY=0.125*2;
            break;
        case 'i':case 'I':
            texposX=0.125;
            texposY=0.125*2;
            break;
        case 'j':case 'J':
            texposX=0.125*2;
            texposY=0.125*2;
            break;
        case 'k':case 'K':
            texposX=0.125*3;
            texposY=0.125*2;
            break;
        case 'l':case 'L':
            texposX=0.125*4;
            texposY=0.125*2;
            break;
        case 'm':case 'M':
            texposX=0.125*5;
            texposY=0.125*2;
            break;
        case 'n':case 'N':
            texposX=0.125*6;
            texposY=0.125*2;
            break;
        case 'o':case 'O':
            texposX=0.125*7;
            texposY=0.125*2;
            break;
        case 'p':case 'P':
            texposX=0.0;
            texposY=0.125;
            break;
        case 'q':case 'Q':
            texposX=0.125;
            texposY=0.125;
            break;
        case 'r':case 'R':
            texposX=0.125*2;
            texposY=0.125;
            break;
        case 's':case 'S':
            texposX=0.125*3;
            texposY=0.125;
            break;
        case 't':case 'T':
            texposX=0.125*4;
            texposY=0.125;
            break;
        case 'u':case 'U':
            texposX=0.125*5;
            texposY=0.125;
            break;
        case 'v':case 'V':
            texposX=0.125*6;
            texposY=0.125;
            break;
        case 'w':case 'W':
            texposX=0.125*7;
            texposY=0.125;
            break;
        case 'x':case 'X':
            texposX=0.0;
            texposY=0.0;
            break;
        case 'y':case 'Y':
            texposX=0.125;
            texposY=0.0;
            break;
        case 'z':case 'Z':
            texposX=0.125*2;
            texposY=0.0;
            break;
        case '[':
            texposX=0.125*3;
            texposY=0.0;
            break;
        case '\\':
            texposX=0.125*4;
            texposY=0.0;
            break;
        case ']':
            texposX=0.125*5;
            texposY=0.0;
            break;
        case '^':
            texposX=0.125*6;
            texposY=0.0;
            break;
        case '_':
            texposX=0.125*7;
            texposY=0.0;
            break;
    }
}

inline static void textRenderer(const char* text, float x, float y, GLFWwindow* window, int fontSize, float r, float g, float b, originPos e){
    fontSize=fontSize;

    glDisable(GL_DEPTH_TEST);//disabling the depth buffer allows transparent pixels to be written overlapping the previous glyph without erasing it 
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);

    Shader program("2dtext.vert","2dtext.frag");

    program.use();

    program.setVec3("textColor",r,g,b);

    glfwGetFramebufferSize(window, &width, &height);

    float fwidth=width,fheight=height;

    float ratio=fheight/fwidth;
    float scale=fontSize/fheight;
    float posX,posY;

    switch(e) {
        default: case bottomLeft:
            posX=(x/fwidth)-1;
            posY=(y/fheight)-1;
            break;
        case bottomRight:
            posX=((fwidth+x)/fwidth);
            posY=(y/fheight)-1;
            break;
        case topLeft:
            posX=(x/fwidth)-1;
            posY=((fheight+y)/fheight);
            break;
        case topRight:
            posX=((fwidth+x)/fwidth);
            posY=((fheight+y)/fheight);
            break;
        case center:
            posX=0;
            posY=0;
            break;
    }

    for(int i=0;i<strlen(text);i++){
        //set glyph texcoords
        char glyph=text[i];
        textCoordinates(glyph);
        textQuadVertices[6]=texCoordScale+texposX;
        textQuadVertices[7]=texCoordScale+texposY;
        textQuadVertices[14]=texCoordScale+texposX;
        textQuadVertices[15]=0.0f+texposY;
        textQuadVertices[22]=0.0f+texposX;
        textQuadVertices[23]=0.0f+texposY;
        textQuadVertices[30]=0.0f+texposX;
        textQuadVertices[31]=texCoordScale+texposY;

        float dist=1.25*(fontSize*i)/fwidth;

        textQuadVertices[0]=ratio*scale+dist+posX;
        textQuadVertices[1]=1.0*scale+posY;
        textQuadVertices[8]=ratio*scale+dist+posX;
        textQuadVertices[9]=-1.0*scale+posY;
        textQuadVertices[16]=-ratio*scale+dist+posX;
        textQuadVertices[17]=-1.0*scale+posY;
        textQuadVertices[24]=-ratio*scale+dist+posX;
        textQuadVertices[25]=1.0*scale+posY;
    
        //update textQuadVertices in the VBO
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(textQuadVertices), textQuadVertices, GL_STATIC_DRAW);
        
        program.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fontTexture);
        glBindVertexArray(textVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glDisable(GL_BLEND);
}

#endif /* TEXTRENDERER_H */