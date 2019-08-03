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


    float verticies[] = {


    };

    unsigned int indecies[] = {
        0,1,2,
        1,2,3
    };

    unsigned int VBO,VAO,EBO;

    glGenBuffers(1,&VBO);
    glGenVertexArrays(1,&VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indecies),indecies,GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBindVertexArray(VAO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verticies),verticies,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*8,(void*)nullptr);
    glEnableVertexAttribArray(0);


    //TEXTURES
    int width,height,comp;
    unsigned int textures[2];
    glGenTextures(2,textures);

    //ONE
    unsigned char* data = stbi_load("container.jpg",&width,&height,&comp,0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,textures[0]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);


    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);


    //TWO
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("awesomeface.png",&width,&height,&comp,0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,textures[1]);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);



    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(float)*8,(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(float)*8,(void*)(sizeof(float)*6));
    glEnableVertexAttribArray(2);

    float time;

    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f,0.0f,0.5f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        time = glfwGetTime();
        int offsetUniformLoc = glGetUniformLocation(shader.ID,"offset");
        int opacityUniformLoc = glGetUniformLocation(shader.ID,"opacity");
        glUniform1i(glGetUniformLocation(shader.ID,"texture1"),0);
        glUniform1i(glGetUniformLocation(shader.ID,"texture2"),1);
        glUniform1f(opacityUniformLoc,std::cos(time));

        shader.use();
        glUniform2f(offsetUniformLoc,std::sin(time),std::cos(time));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        //glDrawArrays(GL_TRIANGLES,0,3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);

    glfwTerminate();
    return 0;
}
