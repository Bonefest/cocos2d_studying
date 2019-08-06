#include <glad/glad.h>ſ
#include <GLFW/glfw3.h>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stbimage.h"

#include "Shader.h"
#include <vector>

float offset = 1.0f;
float mod = 2.0f;

struct vec3 {
    float x;
    float y;
    float z;
    vec3(float _x=0,float _y=0,float _z=0):x(_x),y(_y),z(_z) { }
};

vec3 getPoint(double c,int N);

void framebuffer_size_callback(GLFWwindow* window,int width,int height) {
    std::cout << width << " " << height << std::endl;
    glViewport(0,0,width,height);
}

void generateCircle(float radius,int segments,std::vector<vec3>& output);
float toRad(float deg);

void processInput(GLFWwindow* window) {
    if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window,true);
    } else if(glfwGetKey(window,GLFW_KEY_1) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    } else if(glfwGetKey(window,GLFW_KEY_2) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    } else if(glfwGetKey(window,GLFW_KEY_3) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
    } else if(glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS) {
        mod += 0.01f;
    } else if(glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS) {
        mod -= 0.01f;
    }
}


int main() {

    srand(time(0));

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);

    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640,480,"Intro",NULL,NULL);
    if(window == NULL) {
        std::cout << "Something is wrong!\n" << std::endl;
        glfwTerminate();

        return -1;
    }


    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD " << std::endl;
        return -1;
    }

    glViewport(0,0,640,480);
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    Shader shader("vertex.glsl","fragment.glsl");

    float segments = 3;
    std::vector<vec3> vv;
    float radius = 0.9f;
    while(radius > 0.0f) {
        generateCircle(radius,segments,vv);
        radius -= 0.05f;
    }

    size_t vertCount = vv.size();
    float verticies[vertCount*3]{};

    for(int i = 0;i<vertCount;++i) {
        verticies[i*3] = vv[i].x;
        verticies[i*3 + 1] = vv[i].y;
    }

    vec3 point;


    unsigned int VBO,VAO;

    glGenBuffers(1,&VBO);
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*vertCount*3,verticies,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,(void*)nullptr);
    glEnableVertexAttribArray(0);

    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP,0,vertCount);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);

    glfwTerminate();
    return 0;
}


void generateCircle(float radius,int segments,std::vector<vec3>& output) {
    float angleSize = 360.0f/segments;
    vec3 vert;
    for(int i = 0;i< segments;i++) {
        vert.x = radius*std::cos(toRad(90 + i*angleSize));
        vert.y = radius*std::sin(toRad(90 + i*angleSize));

        output.push_back(vert);
    }
}
float toRad(float deg) {
    return (deg*3.1415926)/180.0f;
}
