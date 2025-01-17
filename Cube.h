#ifndef _CUBE_H_
#define _CUBE_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

class Cube
{
public:
    Cube();
    ~Cube();
    
    glm::mat4 toWorld;
    GLuint VBO, VAO, EBO;
    GLuint dudv;
    float angle;
    
    void draw(GLuint shaderProgram, GLuint skyboxTexture);
    unsigned char* loadPPM(const char* filename, int& width, int& height);
    void update();
    void spin(float);
};

#endif