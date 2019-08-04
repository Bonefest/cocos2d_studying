#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stbimage.h"

#include "Shader.h"

float offset = 1.0f;
float opacity = 0.0f;


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
        if(opacity < 1.0f) opacity += 0.01f;
    } else if(glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS) {
        if(opacity > 0.0f) opacity -= 0.01f;
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

    float r1=0.2f,r2=0.5f;
    int N = 15;

    float verticies[3*N];

    for(int i = 0;i<N;++i) {
        verticies[i*3] = r1*std::cos(3.1415926*2*i/N);
        verticies[i*3 + 1] = r2*std::sin(3.1415926*2*i/N);
        verticies[i*3 + 2] = 0;
    }

    unsigned int VBO,VAO;

//    glGenBuffers(1,&VBO);
//    glGenVertexArrays(1,&VAO);
//    glBindVertexArray(VAO);
//    glBindBuffer(GL_ARRAY_BUFFER,VBO);
//    glBufferData(GL_ARRAY_BUFFER,sizeof(verticies),verticies,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,(void*)nullptr);
    glEnableVertexAttribArray(0);
//

    float arrowVert[6] ={
        0.0f,0.0f,0.0f,
        0.5f,0.5f,0.0f
    };

    unsigned int VBO_ARROW,VAO_ARROW;
    glGenBuffers(1,&VBO_ARROW);
    glGenVertexArrays(1,&VAO_ARROW);

    glBindVertexArray(VAO_ARROW);
    glBindBuffer(GL_ARRAY_BUFFER,VBO_ARROW);

    //glBufferData(GL_ARRAY_BUFFER,sizeof(arrowVert),arrowVert,GL_DYNAMIC_DRAW);
    float time;

    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f,0.0f,0.5f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        time = glfwGetTime();

        shader.use();

        //glBindVertexArray(VAO);
//        //glDrawArrays(GL_LINE_LOOP,0,N);
//
//        arrowVert[3] = r1*std::cos(time);
//        arrowVert[4] = r2*std::sin(time);

        glBindVertexArray(VAO_ARROW);

        //glBufferData(GL_ARRAY_BUFFER,sizeof(arrowVert),arrowVert,GL_STATIC_DRAW);
        glDrawArrays(GL_LINE_LOOP,0,2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);

    glfwTerminate();
    return 0;
}
