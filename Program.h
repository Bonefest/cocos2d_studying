#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <cmath>


#include "Shader.h"

struct vec2 {
    float x;
    float y;
    vec2(float _x=0,float _y=0):x(_x),y(_y) { }
};

struct vec3 {
    float x;
    float y;
    float z;
    vec3(float _x=0,float _y=0,float _z=0):x(_x),y(_y),z(_z) { }
};

typedef vec2 Size;


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

    Shader shader;
    unsigned VBO,VAO;

    GLFWwindow* mainWindow;
    GLFWmonitor* monitor;
    Size _screenSize;
    const char* _programName;


    float verticies[9*12];
};


#endif // PROGRAM_H_INCLUDED
