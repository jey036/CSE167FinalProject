#ifndef OBJObject_hpp
#define OBJObject_hpp

#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class OBJObject {
private:
    std::vector<unsigned int> indices;
    GLuint objTexture;
    
public:
    GLuint shaderProgram;
    GLuint VBO, VAO, EBO, TBO;
    float cut, cutoff, e, m;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> boxVertices;
    std::vector<glm::vec2> textures;
    glm::vec3 light_pos;
    glm::vec3 cam_pos;
    glm::mat4 toWorld;
    
    std::vector<GLfloat> boundingbox;
    
    OBJObject(const char* filepath, const char* texturefile);
    ~OBJObject();
    
    void parse(const char* filepath);
    void draw(GLuint shader);
    void drawWater(GLuint shader, GLuint box);
    unsigned char* loadPPM(const char*, int&, int&);
};

#endif /* OBJObject_hpp */
