#ifndef SHADER_CPP_INCLUDED
#define SHADER_CPP_INCLUDED

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;

    Shader(const GLchar* vertexPath,const GLchar* fragmentPath) {
        std::ifstream fin(vertexPath);
        std::string resultVertexShader = "";
        std::string line = "";
        while(std::getline(fin,line)) {
            resultVertexShader += line;
        }

        std::cout << line << std::endl;
    }

    void use() {

    }

    void setBool(const std::string& name,bool value) const { }
    void setInt(const std::string& name,int value) const { }
    void setFloat(const std::string& name,float value) const { }
};


#endif // SHADER_CPP_INCLUDED
