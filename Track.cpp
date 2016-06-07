#include "Track.hpp"
#include "Window.hpp"

Track::Track(std::vector<glm::mat4x3>& point) {
    this->toWorld = glm::mat4(1.0f);
    points = point;
    
    calculateCurves();
    
    // Create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * lineVertices.size(), &lineVertices[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

Track::~Track() {
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Track::draw(GLuint shaderProgram)
{
    // Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices
    glm::mat4 MVP = Window::P * Window::V * toWorld;
    // We need to calculate this because as of GLSL version 1.40 (OpenGL 3.1, released March 2009), gl_ModelViewProjectionMatrix has been
    // removed from the language. The user is expected to supply this matrix to the shader when using modern OpenGL.
    GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, (int)lineVertices.size());
    glBindVertexArray(0);
}

void Track::calculateCurves()
{
    GLfloat cubicFactorial = glm::factorial(3); // 3! constant throughout calculation
    glm::vec4 c;                                // result of c in vec4
    
    for (auto k = points.begin(); k != points.end(); k++) {
        // t = 0.01
        for (GLfloat t = 0.0f; t <= 1.0f; t += 0.001f) {
            // calculate cubic bezier curve ci(t) for each i, i = 0 - 3
            for (int i = 0; i < 4; i++) {
                c[i] = cubicFactorial / (glm::factorial(3 - i) * glm::factorial(i));
                c[i] = c[i] * glm::pow(t,i) * glm::pow(1 - t, 3 - i);
            }
            
            // push the result to vector
            lineVertices.push_back(*k * c);
        }
    }
}