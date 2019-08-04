#include <glad/glad.h>
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

bool gener=true;

struct vec3 {
    float x;
    float y;
    float z;
    vec3(float _x=0,float _y=0,float _z=0):x(_x),y(_y),z(_z) { }
};


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
        gener=true;
    } else if(glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS) {
        mod -= 0.01f;
        gener=true;
    }
}

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

    std::vector<vec3> vverticies;

    generator(12,vverticies,vec3(-1.0f,-1.0f,0.0f),vec3(0.0f,1.0f,0.0f),vec3(1.0f,-1.0f,0.0f),-1);
    int N = vverticies.size();

    float verticies[vverticies.size()*3];
    for(unsigned int i = 0;i<vverticies.size();++i) {
        verticies[i*3] = vverticies[i].x;
        verticies[i*3 + 1] = vverticies[i].y;
        verticies[i*3 + 2] = vverticies[i].z;
    }


    unsigned int VBO,VAO;

    glGenBuffers(1,&VBO);
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verticies),verticies,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,(void*)nullptr);
    glEnableVertexAttribArray(0);

    float time;

    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        time = glfwGetTime();

        shader.use();

        if(gener) {
            vverticies.clear();
            generator(12,vverticies,vec3(-1.0f,-1.0f,0.0f),vec3(0.0f,1.0f,0.0f),vec3(1.0f,-1.0f,0.0f),-1);

            for(unsigned int i = 0;i<vverticies.size();++i) {
                verticies[i*3] = vverticies[i].x;
                verticies[i*3 + 1] = vverticies[i].y;
                verticies[i*3 + 2] = vverticies[i].z;
            }

            glBufferData(GL_ARRAY_BUFFER,sizeof(verticies),verticies,GL_STATIC_DRAW);
            gener=false;
        }

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES,0,N);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);

    glfwTerminate();
    return 0;
}


void generator(int count,std::vector<vec3>& verts,vec3 A,vec3 B,vec3 C,int n) {
    if(count > 0) {
        if(n!=0) verts.push_back(A);
        if(n!=1) verts.push_back(B);
        if(n!=2) verts.push_back(C);


        vec3 midAB = vec3((A.x+B.x)/mod,(A.y+B.y)/mod,(A.z+B.z)/mod);
        vec3 midAC = vec3((A.x+C.x)/mod,(A.y+C.y)/mod,(A.z+C.z)/mod);
        vec3 midBC = vec3((B.x+C.x)/mod,(B.y+C.y)/mod,(B.z+C.z)/mod);

        generator(count-1,verts,A,midAB,midAC,0);
        generator(count-1,verts,midAC,midBC,C,2);
        generator(count-1,verts,midAB,B,midBC,1);
    }
}
