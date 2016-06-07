#ifndef Window_hpp
#define Window_hpp

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "NPC.hpp"
#include "shader.h"
#include "Track.hpp"
#include "skybox.hpp"
#include "OBJObject.hpp"
#include "Cube.h"
#include "Audio.hpp"

class Window
{
public:
    static int width;
    static int height;
    static glm::mat4 P;
    static glm::mat4 V;
    
    static void initialize_objects();
    static void clean_up();
    static GLFWwindow* create_window(int width, int height);
    static void resize_callback(GLFWwindow* window, int width, int height);
    static void idle_callback();
    static void display_callback(GLFWwindow*);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
    static glm::vec3 trackball_mapping(double x, double y);
    static void check_collision();
    
private:
    static void createNPCs();
    static void moveNpcs();
};

#endif /* Window_hpp */
