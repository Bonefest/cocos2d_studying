#include "Program.h"
#include <cstring>


void generateTriangles(int depth,std::list<vec2>& verticies,vec2 A,vec2 B,vec2 C) {
    if(depth > 0) {
        vec2 midAB = (A+B)/2;
        vec2 midBC = (B+C)/2;
        vec2 midAC = (A+C)/2;

        generateTriangles(depth-1,verticies,A,midAB,midAC);
        generateTriangles(depth-1,verticies,midAB,B,midBC);
        generateTriangles(depth-1,verticies,midAC,midBC,C);

    } else {
        verticies.push_back(A);
        verticies.push_back(B);
        verticies.push_back(C);
    }

}

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


    std::list<vec2> tvert;
    generateTriangles(4,tvert,vec2(-1.0f,-1.0f),vec2(-1.0f,1.0f),vec2(1.0f,-1.0f));

    vertSize = tvert.size();
    verticies = new float[vertSize*2];
    int i = 0;
    for(vec2 vert: tvert) {
        verticies[i*2] = vert.x;
        verticies[i*2+ 1] = vert.y;
        ++i;
    }

    initGL();
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
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*vertSize*2,verticies,GL_STATIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float)*2,(void*)nullptr);
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
    glClear(GL_COLOR_BUFFER_BIT);

    float time = glfwGetTime();
    shader.use();

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,vertSize);

    glfwSwapBuffers(mainWindow);
    glfwPollEvents();
}
