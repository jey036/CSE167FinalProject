#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

class Skybox
{
public:
    Skybox();
    ~Skybox();
    
    glm::mat4 toWorld;
    GLuint VBO, VAO, EBO;
    GLuint cubemapTexture;
    
    void draw(GLuint);
    void update();
    void spin(float);
    
    unsigned char* loadPPM(const char*, int&, int&);
    GLuint loadTexture(std::vector<const GLchar*>);
};

#endif