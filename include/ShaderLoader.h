#pragma once
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using std::cout;
GLuint loadShader(const GLchar* vertexPath, const GLchar* fragmentPath) {
    GLuint program;
    std::string vertexCode, fragmentCode;
    std::ifstream vertexShaderFile, fragmentShaderFile;
    //read code
    vertexShaderFile.exceptions(std::ifstream::badbit);
    fragmentShaderFile.exceptions(std::ifstream::badbit);
    try {
        vertexShaderFile.open(vertexPath);
        fragmentShaderFile.open(fragmentPath);
        std::stringstream vertexShaderStream, fragmentShaderStream;
        vertexShaderStream << vertexShaderFile.rdbuf();
        fragmentShaderStream << fragmentShaderFile.rdbuf();
        vertexShaderFile.close();
        fragmentShaderFile.close();
        vertexCode = vertexShaderStream.str();
        fragmentCode = fragmentShaderStream.str();
    }
    catch (std::ifstream::failure e) {
        cout << "Shader failed to read.\n";
    }
    const GLchar* vertexShaderCode = vertexCode.c_str();
    const GLchar* fragmentShaderCode = fragmentCode.c_str();

    GLuint vertexShader, fragmentShader;
    GLint success;
    GLchar log[512];

    //gl shader creation and compilation function calls
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, log);
        cout << "Vertex shader compilation failed.\n" << log << "\n";
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, log);
        cout << "Fragment shader compilation failed.\n" << log << "\n";
    }

    //program combines above shaders
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, log);
        cout << "Shader program linking failed for "<<vertexPath<<" and "<<fragmentPath<<" .\n"; 
        cout << log << "\n";
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

GLuint loadShader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath) {
    GLuint program;
    std::string vertexCode, fragmentCode, geometryCode;
    std::ifstream vertexShaderFile, fragmentShaderFile, geometryShaderFile;
    //read code
    vertexShaderFile.exceptions(std::ifstream::badbit);
    fragmentShaderFile.exceptions(std::ifstream::badbit);
    geometryShaderFile.exceptions(std::ifstream::badbit);
    try {
        vertexShaderFile.open(vertexPath);
        fragmentShaderFile.open(fragmentPath);
        geometryShaderFile.open(geometryPath);

        std::stringstream vertexShaderStream, fragmentShaderStream, geometryShaderStream;
        vertexShaderStream << vertexShaderFile.rdbuf();
        fragmentShaderStream << fragmentShaderFile.rdbuf();
        geometryShaderStream << geometryShaderFile.rdbuf();

        vertexShaderFile.close();
        fragmentShaderFile.close();
        geometryShaderFile.close();

        vertexCode = vertexShaderStream.str();
        fragmentCode = fragmentShaderStream.str();
        geometryCode = geometryShaderStream.str();
    }
    catch (std::ifstream::failure e) {
        //probably should spread this out for each shader, but whatever
        cout << "Shader failed to read\n";
    }
    const GLchar* vertexShaderCode = vertexCode.c_str();
    const GLchar* fragmentShaderCode = fragmentCode.c_str();
    const GLchar* geometryShaderCode = geometryCode.c_str();

    GLuint vertexShader, fragmentShader, geometryShader;
    GLint success;
    GLchar log[512];

    //gl shader creation/compilation functions
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, log);
        cout << "Vertex shader compilation failed\n" << log << "\n";
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, log);
        cout << "Fragment shader compilation failed\n" << log << "\n";
    }

    geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader, 1, &geometryShaderCode, NULL);
    glCompileShader(geometryShader);
    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, log);
        cout << "Geometry shader compilation failed\n" << log << "\n";
    }

    //program combines above shaders
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, geometryShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    //oh and apparantly glGenProgramPipelines() is also a thing
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, log);
        cout << "Shader program linking failed\n" << log << "\n";
    }

    //glDetachShader(program, vertexShader) should be called after linking, but I'll keep it simple
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);
    return program;
}