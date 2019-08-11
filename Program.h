#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <list>

#include "Shader.h"


struct vec2 {
    float x;
    float y;
    vec2 operator+(const vec2& rop) {
        return vec2(x+rop.x,y+rop.y);
    }

    vec2 operator/(float value) {
        return vec2(x/value,y/value);
    }

    vec2(float _x=0,float _y=0):x(_x),y(_y) { }
};

struct vec3 {
    float x;
    float y;
    float z;
    vec3 operator+(const vec3& rop) {
        return vec3(x+rop.x,y+rop.y,z+rop.z);
    }

    vec3 operator/(float value) {
        return vec3(x/value,y/value,z/value);
    }

    vec3(float _x=0,float _y=0,float _z=0):x(_x),y(_y),z(_z) { }
};

typedef vec2 Size;

const float toRad = 3.1415926/180.0f;

class Program {
public:
    ~Program();

    void init(Size screenSize,const char* programName,bool fullScreen=false);
    void start();

    void draw();
    void controll();

    void setScreenSize(Size screenSize);
    Size getScreenSize() const { return _screenSize; }

private:
    void initGLFW();
    void initGL();
    void updateWindow();
    unsigned int loadTexture(const char* path,unsigned int target=GL_TEXTURE0);

    Shader shader;
    unsigned VBO,VAO,EBO;
    unsigned VBO2,VAO2,EBO2;

    GLFWwindow* mainWindow;
    GLFWmonitor* monitor;
    Size _screenSize;
    const char* _programName;
    unsigned int texture;

    float* verticies;
    unsigned indecies[6];
};

#endif // PROGRAM_H_INCLUDED
