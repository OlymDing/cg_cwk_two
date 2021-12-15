#pragma once

#include "glad/glad.h"
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

struct Shader {
    unsigned int ID;
    float opacity = 0.0;
    
    Shader(const char * vertexPath, const char * fragmentPath)
    {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file’s buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch(std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }

        const char * vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();

        unsigned int vertex, fragment;
        int success;
        char infoLog[512];
        // vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        // print compile errors if any
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
            infoLog << std::endl;
        };

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        // print compile errors if any
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
            infoLog << std::endl;
        };

        // create program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        // print linking errors if any
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
            infoLog << std::endl;
        }
        // delete shaders; they’re linked into our program and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);

    };

    void use() const
    {
        glUseProgram(ID);
    };

    void setBool(const std::string& name, bool value) const
    {
        int location = glGetUniformLocation(ID, name.c_str());
        glUniform1i(location, (int)value);
    };

    void setInt(const std::string& name, int value) const
    {
        int location = glGetUniformLocation(ID, name.c_str());
        glUniform1i(location, value);
    };
    
    void setFloat(const std::string& name, float value) const
    {
        int location = glGetUniformLocation(ID, name.c_str());
        glUniform1f(location, value);
    };

    void setFloat(const std::string& name, float first, float second, float third, float fourth) const
    {
        int location = glGetUniformLocation(ID, name.c_str());
        glUniform4f(location, first, second, third, fourth);
    };

    void setMat4 (const std::string& name, glm::mat4 matrix) const
    {
        int location = glGetUniformLocation(ID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void setVec3 (const std::string & name, glm::vec3 vector) const
    {
        int location = glGetUniformLocation(ID, name.c_str());
        glUniform3fv(location, 1, &vector[0]);
    }

    void setVec3 (const std::string & name, float x, float y, float z) const
    {
        int location = glGetUniformLocation(ID, name.c_str());
        glUniform3f(location, x, y, z);
    }
};