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

void generatePoints(float points[],int size);

void generator(int count,std::vector<vec3>& verts,vec3 A,vec3 B,vec3 C,int n);

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

    int N = 1000;
    int points = 40000;

    float verticies[points*3]{};
    vec3 point;
    float c = -0.13;
    for(int i = 0;i<points;++i) {
        point = getPoint(c,N);
        if(point.x == 0 && point.y == 0) break;

        verticies[i*3] = point.x*2;
        verticies[i*3 + 1] = point.y*2;


        c += 0.0001;
    }

    unsigned int VBO,VAO;

    glGenBuffers(1,&VBO);
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verticies),verticies,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,(void*)nullptr);
    glEnableVertexAttribArray(0);

    vec3 test = getPoint(0.251,1000);
    std::cout << test.x << " " << test.y << std::endl;

    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES,0,points);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);

    glfwTerminate();
    return 0;
}


vec3 getPoint(double c,int N) {
    vec3 result;
    double z = 0;
    for(int i = 0;i<N-1;++i) {
        z = z*z + c;
        if(z > 2) return result;
    }

    if(z*z + c <= 2) {
        result.x = z*z;
        result.y = c;
    }

    std::cout << result.x << " " << result.y << std::endl;
    return result;
}

