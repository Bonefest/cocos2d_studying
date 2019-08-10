#include "Program.h"
#include <cstring>


void generateTriangles(int depth,std::list<vec3>& verticies,vec3 A,vec3 B,vec3 C,vec3 D,vec3 E) {
    if(depth > 0) {
        vec3 midAB = (A+B)/2;
        vec3 midAD = (A+D)/2;
        vec3 midBC = (B+C)/2;
        vec3 midCD = (C+D)/2;

        vec3 midAE = (A+E)/2;
        vec3 midBE = (B+E)/2;
        vec3 midEC = (E+C)/2;
        vec3 midED = (E+D)/2;

        vec3 midAC = (A+C)/2;

        generateTriangles(depth-1,verticies,A,midAB,midAC,midAD,midAE);
        generateTriangles(depth-1,verticies,midAD,midAC,midCD,D,midED);
        generateTriangles(depth-1,verticies,midAB,B,midBC,midAC,midBE);
        generateTriangles(depth-1,verticies,midAC,midBC,C,midCD,midEC);
        generateTriangles(depth-1,verticies,midAE,midBE,midEC,midED,E);

    } else {
        verticies.push_back(A);
        verticies.push_back(B);
        verticies.push_back(C);

        verticies.push_back(A);
        verticies.push_back(C);
        verticies.push_back(D);

        verticies.push_back(A);
        verticies.push_back(B);
        verticies.push_back(E);

        verticies.push_back(B);
        verticies.push_back(E);
        verticies.push_back(C);

        verticies.push_back(C);
        verticies.push_back(E);
        verticies.push_back(D);

        verticies.push_back(A);
        verticies.push_back(E);
        verticies.push_back(D);
    }

}

Program::~Program() {
    delete [] verticies;

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);

    glfwTerminate();
}

void Program::init(Size screenSize,const char* programName,bool fullScreen) {
    angleX = 0.0f;
    angleY = 0.0f;

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


    std::list<vec3> tvert;

    generateTriangles(1,tvert,vec3(-1.0f,0.0f,-1.0f),vec3(-1.0f,0.0f,1.0f),vec3(1.0f,0.0f,1.0f),vec3(1.0f,0.0f,-1.0f),vec3(0.0f,1.0f,0.0f));

    vertSize = tvert.size();
    verticies = new float[vertSize*3];
    int i = 0;
    for(vec3 vert: tvert) {
        verticies[i*3] = vert.x;
        verticies[i*3+ 1] = vert.y;
        verticies[i*3+ 2] = vert.z;
        ++i;
    }

    initGL();
    glClearColor(0.0f,0.0f,0.0f,1.0f);

    for(int i = 0;i<3;++i) {
        for(int j=0;j<3;++j) {
            mat[i*3 + j] = (i==j)?1:0;
            mat2[i*3 + j] = mat[i*3 + j];
        }
    }

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
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*vertSize*3,verticies,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,(void*)nullptr);
    glEnableVertexAttribArray(0);
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
    } else if(glfwGetKey(mainWindow,GLFW_KEY_UP) == GLFW_PRESS) {
        setAngleX(mat2,std::fmod((angleX + 1),360));
    } else if(glfwGetKey(mainWindow,GLFW_KEY_DOWN) == GLFW_PRESS) {
        setAngleX(mat2,std::fmod((angleX - 1),360));
    } else if(glfwGetKey(mainWindow,GLFW_KEY_LEFT) == GLFW_PRESS) {
        setAngleY(mat,std::fmod((angleY - 1),360));
    } else if(glfwGetKey(mainWindow,GLFW_KEY_RIGHT) == GLFW_PRESS) {
        setAngleY(mat,std::fmod((angleY + 1),360));
    }
}
void Program::setAngleX(float* mat,float degree) {
    angleX = degree;

    mat[4] = std::cos(toRad*degree);
    mat[5] = std::sin(toRad*degree);
    mat[7] = -std::sin(toRad*degree);
    mat[8] = std::cos(toRad*degree);
}

void Program::setAngleY(float* mat,float degree) {
    angleY = degree;

    mat[0] = std::cos(toRad*degree);
    mat[2] = std::sin(toRad*degree);
    mat[6] = -std::sin(toRad*degree);
    mat[8] = std::cos(toRad*degree);
}

void Program::draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    float time = glfwGetTime();

    glUniformMatrix3fv(glGetUniformLocation(shader.ID,"anglesy"),1,GL_FALSE,mat);
    glUniformMatrix3fv(glGetUniformLocation(shader.ID,"anglesx"),1,GL_FALSE,mat2);

    shader.use();

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,vertSize);

    glfwSwapBuffers(mainWindow);
    glfwPollEvents();
}
