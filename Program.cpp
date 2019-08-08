#include "Program.h"
#include <cstring>


void frameBufCallback(GLFWwindow* window,int width,int height) {
    glViewport(0,0,width,height);
}

Program::~Program() {
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
    glfwSetFramebufferSizeCallback(mainWindow,frameBufCallback);

    shader.loadShader("vertex.glsl","fragment.glsl");

    float tverticies[] = {
        -0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,
         0.5f, 0.5f, 0.0f, //FRONT

        -0.5f, 0.0f, 0.0f,
         0.5f, 0.5f, 0.0f,
         0.5f, 0.0f, 0.0f,

         0.5f, 0.5f, 0.0f,
         0.5f, 0.5f, 0.5f,
         0.5f, 0.0f, 0.0f, //RIGHT

         0.5f, 0.5f, 0.5f,
         0.5f, 0.0f, 0.5f,
         0.5f, 0.0f, 0.0f,

        -0.5f, 0.0f, 0.5f,
        -0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, //BACK

        -0.5f, 0.0f, 0.5f,
         0.5f, 0.5f, 0.5f,
         0.5f, 0.0f, 0.5f,

        -0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.0f, 0.0f, //LEFT

        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.0f, 0.5f,
        -0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.0f,
         0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.5f,

         0.5f, 0.5f, 0.0f,
         0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f, //UP

        -0.5f, 0.0f, 0.0f,
         0.5f, 0.0f, 0.0f,
        -0.5f, 0.0f, 0.5f,

         0.5f, 0.0f, 0.0f,
         0.5f, 0.0f, 0.5f,
        -0.5f, 0.0f, 0.5f, //DOWN


    };

    memcpy(verticies,tverticies,sizeof(float)*9*12);

    initGL();

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
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verticies),verticies,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,(void*)nullptr);
    glEnableVertexAttribArray(0);
}

void Program::updateWindow() {
    if(mainWindow)
        glfwDestroyWindow(mainWindow);

    mainWindow = glfwCreateWindow(_screenSize.x,_screenSize.y,_programName,NULL,NULL);
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
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float matAnglex[] = {
        1,0,0,
        0,1,0,
        0,0,1
    };
    float time = glfwGetTime();

    matAnglex[0] = std::cos(time);
    matAnglex[2] = std::sin(time);


    matAnglex[6] = -std::sin(time);
    matAnglex[8] = std::cos(time);

    int matAngleUniformx = glGetUniformLocation(shader.ID,"anglesx");
    //int matAngleUniformz = glGetUniformLocation(shader.ID,"anglesz");
    glUniformMatrix3fv(matAngleUniformx,1,GL_FALSE,matAnglex);
    //glUniformMatrix3fv(matAngleUniformz,1,GL_FALSE,matAnglez);

    shader.use();

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,36);

    glfwSwapBuffers(mainWindow);
    glfwPollEvents();
}
