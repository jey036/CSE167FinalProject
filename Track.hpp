#ifndef Track_hpp
#define Track_hpp

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

class Track {
public:
    glm::mat4 toWorld;
    GLuint VBO, VAO;
    
    std::vector<glm::mat4x3> points;
    std::vector<glm::vec3> lineVertices; // store all points along the curve
    
    Track(std::vector<glm::mat4x3>& point);
    ~Track();
    
    void draw(GLuint);
    void calculateCurves();
};

#endif /* Track_hpp */
