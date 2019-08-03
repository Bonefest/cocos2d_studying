#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;

    Shader(const GLchar* vertexPath,const GLchar* fragmentPath) {
        std::string strShader;
        const char* chShader;

        strShader = getSourceFromFile(vertexPath);
        chShader = strShader.c_str();

        int vertexShader,fragmentShader;
        vertexShader = generateVertexShader(chShader);
        checkForCompileErrors(vertexShader);

        strShader = getSourceFromFile(fragmentPath);
        chShader = strShader.c_str();

        fragmentShader = generateFragmentShader(chShader);
        checkForCompileErrors(fragmentShader);

        ID = glCreateProgram();
        glAttachShader(ID,vertexShader);
        glAttachShader(ID,fragmentShader);
        glLinkProgram(ID);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void use() {
        glUseProgram(ID);
    }

    void setBool(const std::string& name,bool value) const { }
    void setInt(const std::string& name,int value) const { }
    void setFloat(const std::string& name,float value) const { }

private:
    void checkForCompileErrors(unsigned int shaderID) {
        int success;
        char buffer[512];
        glGetShaderiv(shaderID,GL_COMPILE_STATUS,&success);
        if(!success) {
            glGetShaderInfoLog(shaderID,512,NULL,buffer);
            std::cout << "Compile error: " << buffer << std::endl;
        }
    }

    int generateVertexShader(const char* source) {
        int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader,1,&source,nullptr);
        glCompileShader(vertexShader);
        return vertexShader;
    }

    int generateFragmentShader(const char* source) {
        int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader,1,&source,nullptr);
        glCompileShader(fragmentShader);

        return fragmentShader;
    }

    std::string getSourceFromFile(const std::string& fileName) {
        std::ifstream shaderFile(fileName);
        std::string result = "";
        std::string line = "";
        while(!shaderFile.eof()) {
            std::getline(shaderFile,line);
            result += line + '\n';
        }

        shaderFile.close();
        return result;
    }
};


#endif // SHADER_H_INCLUDED
