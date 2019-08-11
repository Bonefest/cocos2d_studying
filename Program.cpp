#include "Program.h"
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define STB_IMAGE_IMPLEMENTATION
#include "stbimage.h"


Program::~Program() {
    delete [] verticies;

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);

    glfwTerminate();
}

void Program::init(Size screenSize,const char* programName,bool fullScreen) {
    _screenSize = screenSize;
    _programName = programName;
    mainWindow = nullptr;

    initGLFW();

    monitor = (fullScreen)?glfwGetPrimaryMonitor():NULL;
    updateWindow();

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD " << std::endl;
    }
    glViewport(0,0,screenSize.x,screenSize.y);

    shader.loadShader("vertex.glsl","fragment.glsl");

    verticies = new float[16] {
        -1.0f,-1.0f,0.0f,0.0f,
        -1.0f, 1.0f,0.0f,1.0f,
         1.0f, 1.0f,1.0f,1.0f,
         1.0f,-1.0f,1.0f,0.0f
    };

    indecies[0] = 0;
    indecies[1] = 1;
    indecies[2] = 2;

    indecies[3] = 2;
    indecies[4] = 3;
    indecies[5] = 0;

    initGL();
    texture = loadTexture("wall.jpg",GL_TEXTURE0);
    glClearColor(0.0f,0.0f,0.0f,1.0f);

}

void Program::initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
}

void Program::initGL() {
    glGenBuffers(1,&VBO);
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,16*sizeof(float),verticies,GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indecies),indecies,GL_STATIC_DRAW);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float)*4,(void*)nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(float)*4,(void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    /////////////////////////////////////////////////

}

unsigned int Program::loadTexture(const char* path,unsigned int target) {
    int width,height,comp;
    unsigned char* data = stbi_load(path,&width,&height,&comp,0);
    std::cout << width << " " << height << std::endl;
    unsigned int textureid;
    glGenTextures(1,&textureid);
    glBindTexture(GL_TEXTURE_2D,textureid);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
    glGenerateMipmap(GL_TEXTURE_2D);

    shader.use();
    glUniform1i(glGetUniformLocation(shader.ID,"texture1"),0);

    stbi_image_free(data);

    return textureid;
}

void Program::updateWindow() {
    if(mainWindow)
        glfwDestroyWindow(mainWindow);

    mainWindow = glfwCreateWindow(_screenSize.x,_screenSize.y,_programName,monitor,NULL);
    if(mainWindow == NULL) {
        std::cout << "Something is wrong!\n" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(mainWindow);
}


void Program::start() {

    while(!glfwWindowShouldClose(mainWindow)) {
        controll();
        draw();
    }

}

void Program::controll() {
    if(glfwGetKey(mainWindow,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(mainWindow,true);
    } else if(glfwGetKey(mainWindow,GLFW_KEY_1) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    } else if(glfwGetKey(mainWindow,GLFW_KEY_2) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    } else if(glfwGetKey(mainWindow,GLFW_KEY_3) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
    }
}
void Program::draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture);


    float time = glfwGetTime();
    glm::mat4 trans = glm::mat4(1.0f);

    trans = glm::translate(trans,glm::vec3(0.5f,-0.5f,0.0f));
    trans = glm::scale(trans,glm::vec3(0.3f,0.3f,0.3f));
    trans = glm::rotate(trans,time,glm::vec3(0.0f,0.0f,1.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader.ID,"transform"),1,GL_FALSE,glm::value_ptr(trans));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

    trans = glm::mat4(1.0f);
    trans = glm::translate(trans,glm::vec3(-0.5f,0.5f,0));
    trans = glm::scale(trans,glm::vec3(std::sin(time),std::sin(time),1.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader.ID,"transform"),1,GL_FALSE,glm::value_ptr(trans));

    glUniform1i(glGetUniformLocation(shader.ID,"text"),texture);
    shader.use();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

    glfwSwapBuffers(mainWindow);
    glfwPollEvents();
}
