#include "Window.hpp"

double m_xpos,m_ypos;   // cursor position
double p_xpos,p_ypos;

const char* window_title = "GLFW Starter Project";

Skybox * skybox;
OBJObject * landscape;
OBJObject * alduin;
OBJObject * water;
Cube * cube;

BoundingBox * alduinbox;
BoundingBox * testbox;

Audio * audio;

// npcs
std::vector<NPC*> npcs;

// shaders
GLint skyboxShaderProgram;
GLint trackShaderProgram;
GLint objectShaderProgram;
GLint waterShaderProgram;

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 30.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is
glm::vec3 ball_pos(0.0f,0.0f,0.0f);         // cursor position on logical sphere

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

void Window::initialize_objects()
{
    // Load the shader program. Similar to the .obj objects, different platforms expect a different directory for files
#ifdef _WIN32 // Windows (both 32 and 64 bit versions)
    skyboxShaderProgram = LoadShaders("../skyboxShader.vert", "../skyboxShader.frag");
#else // Not windows
    skyboxShaderProgram = LoadShaders("skyboxShader.vert", "skyboxShader.frag");
#endif
    
#ifdef _WIN32 // Windows (both 32 and 64 bit versions)
    trackShaderProgram = LoadShaders("../trackShader.vert", "../trackShader.frag");
#else // Not windows
    trackShaderProgram = LoadShaders("trackShader.vert", "trackShader.frag");
#endif
    
#ifdef _WIN32 // Windows (both 32 and 64 bit versions)
    objectShaderProgram = LoadShaders("../objectShader.vert", "../objectShader.frag");
#else // Not windows
    objectShaderProgram = LoadShaders("objectShader.vert", "objectShader.frag");
#endif
    
#ifdef _WIN32 // Windows (both 32 and 64 bit versions)
    waterShaderProgram = LoadShaders("../shaderX.vert", "../shaderX.frag");
#else // Not windows
    waterShaderProgram = LoadShaders("shaderX.vert", "shaderX.frag");
#endif   
    
    
    skybox = new Skybox();
    alduin = new OBJObject("alduin.obj", "alduin.ppm");
    landscape = new OBJObject("landscape.obj", "alps_dn.ppm");
    water = new OBJObject("cube.obj", "waterdudv.ppm");
    
    // npcs
    createNPCs();
    
    alduinbox = new BoundingBox(alduin->boundingbox,alduin->boxVertices);
    
    testbox = alduinbox;
    
    audio = new Audio(npcs);
    
    // transform the dragon to its initial position
    alduin->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(10.0f,10.0f,10.0f));
    alduinbox->toWorld = alduin->toWorld;
    
    landscape->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,-4.0f,0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(150.0f,150.0f,150.0f));
    
    //creating water
    cube = new Cube();
    
    cube->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,-7.0f,0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(70.0f,1.0f,70.0f)) * glm::rotate(glm::mat4(1.0f), 45.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f,1.0f,0.0f));
}

void Window::clean_up()
{
    delete(skybox);
    delete(alduin);
    delete(cube);
    glDeleteProgram(skyboxShaderProgram);
    glDeleteProgram(objectShaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }
    
    // 4x antialiasing
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // Create the GLFW window
    GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);
    
    // Check if the window could not be created
    if (!window)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return NULL;
    }
    
    // Make the context of the window
    glfwMakeContextCurrent(window);
    
    // Set swap interval to 1
    glfwSwapInterval(1);
    
    // Get the width and height of the framebuffer to properly resize the window
    glfwGetFramebufferSize(window, &width, &height);
    // Call the resize callback to make sure things get drawn immediately
    Window::resize_callback(window, width, height);
    
    return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
    Window::width = width;
    Window::height = height;
    // Set the viewport size
    glViewport(0, 0, width, height);
    
    if (height > 0)
    {
        P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
        V = glm::lookAt(cam_pos, cam_look_at, cam_up);
    }
}

void Window::idle_callback()
{
    skybox->update();
    
    // move npcs on their track
    moveNpcs();
    check_collision();

    // change the view to dragon view
    glm::mat4 campos = alduin->toWorld * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.2f,0.1f));
    cam_pos = glm::vec3(campos[3][0],campos[3][1],campos[3][2]);
    
    glm::mat4 camlookat = alduin->toWorld * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,-0.6f));
    cam_look_at = glm::vec3(camlookat[3][0],camlookat[3][1],camlookat[3][2]);
    
    // V = glm::lookAt(cam_pos, cam_look_at, cam_up);
    
    audio->update(npcs, cam_pos,cam_look_at, cam_up);
}

void Window::display_callback(GLFWwindow* window)
{
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Draw objects
    glUseProgram(skyboxShaderProgram);
    skybox->draw(skyboxShaderProgram);
    
    glUseProgram(objectShaderProgram);
    landscape->draw(objectShaderProgram);
    alduin->draw(objectShaderProgram);
    // water->drawWater(objectShaderProgram, skybox->cubemapTexture);
    
    glUseProgram(trackShaderProgram);
    // alduinbox->draw(trackShaderProgram, false);
    
    testbox->draw(trackShaderProgram,false);

    
    glUseProgram(waterShaderProgram);
    cube->draw(waterShaderProgram, skybox->cubemapTexture);
    // water->drawWater(waterShaderProgram, skybox->cubemapTexture);

    
    for (int i = 0; i < npcs.size(); i++) {
        /*
        // check if on counter
        if (npcs[i]->deathcounter > 0) {
            npcs[i]->dead = true;
        }
         */
        
        if (!npcs[i]->dead) {
            // Draw npc
            glUseProgram(objectShaderProgram);
            npcs[i]->npc->draw(objectShaderProgram);
            // Draw track
            glUseProgram(trackShaderProgram);
            npcs[i]->box->draw(trackShaderProgram, npcs[i]->dead);
            // npcs[i]->track->draw(trackShaderProgram);
        }
        else {
            // Draw npc
            glUseProgram(objectShaderProgram);
            npcs[i]->npc->draw(objectShaderProgram);
            // Draw track
            glUseProgram(trackShaderProgram);
            npcs[i]->box->draw(trackShaderProgram, npcs[i]->dead);
            // npcs[i]->track->draw(trackShaderProgram);
        }
    }
    
    // Swap buffers
    glfwSwapBuffers(window);
    
    // Gets events, including input such as keyboard and mouse or window resizing
    glfwPollEvents();
    
}


void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Check for a key press
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        // Check if escape was pressed
        if (key == GLFW_KEY_ESCAPE)
        {
            // Close the window. This causes the program to also terminate.
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        
        // make dragon move forward
        if (key == GLFW_KEY_UP) {
            alduin->toWorld = alduin->toWorld * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,-0.1f));
            alduinbox->toWorld = alduin->toWorld;
        }
        
        // make dragon move backward
        if (key == GLFW_KEY_DOWN) {
            alduin->toWorld = alduin->toWorld * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,0.1f));
            alduinbox->toWorld = alduin->toWorld;
        }
        
        // make dragon turn left
        if (key == GLFW_KEY_LEFT) {
            alduin->toWorld = alduin->toWorld * glm::rotate(glm::mat4(1.0f), -10.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, -1.0f, 0.0f));
            alduinbox->toWorld = alduin->toWorld;
        }
        
        // make dragon turn right
        if (key == GLFW_KEY_RIGHT) {
            alduin->toWorld = alduin->toWorld * glm::rotate(glm::mat4(1.0f), -10.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
            alduinbox->toWorld = alduin->toWorld;
        }
    }
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    int left = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    
    if (left == GLFW_PRESS) {
        // calculate current location on trackball
        glm::vec3 curr = trackball_mapping(xpos, ypos);
        // get the changed movement
        glm::vec3 mov = curr - ball_pos;
        // movement speed
        float velocity = glm::length(mov);
        
        // if little movement, do nothing
        if (velocity > 0.0001) {
            // create rotation vector
            glm::vec3 rot = glm::cross(ball_pos, curr);
            rot = glm::normalize(rot);
            
            ball_pos = curr;
            
            velocity = velocity * 500;
            
            if (velocity > 360.0f || velocity < -360.0f) {
                velocity = 0.0f;
            }
            
            V = V * glm::rotate(glm::mat4(1.0f), velocity / 180.0f * glm::pi<float>(), rot);
        }
    }
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            // get cursor position to a logical sphere location
            glfwGetCursorPos(window, &m_xpos, &m_ypos);
            ball_pos = trackball_mapping(m_xpos, m_ypos);
        }
    }
}

void Window::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if (yoffset > 0) {
        // z += 0.01;
        V[3][0] *= 1.1;
        V[3][1] *= 1.1;
        V[3][2] *= 1.1;
    }
    else {
        // z -= 0.01;
        V[3][0] *= 0.9;
        V[3][1] *= 0.9;
        V[3][2] *= 0.9;
    }
}

/* Map the mouse position to a logical sphere location */
glm::vec3 Window::trackball_mapping(double x, double y) {
    float d;
    glm::vec3 v;
    
    x = x * 2;
    y = y * 2;
    
    v.x = (2.0 * x - width) / width;
    v.y = (height - 2.0 * y) / height;
    v.z = 0.0f;
    d = glm::length(v);
    
    d = (d<1.0) ? d : 1.0;
    
    v.z = glm::sqrt(1.001 - d * d);
    v = glm::normalize(v);
    
    return v;
}

void Window::createNPCs() {
    std::vector<glm::mat4x3> track_1;
    std::vector<glm::mat4x3> track_2;
    std::vector<glm::mat4x3> track_3;
    std::vector<glm::mat4x3> track_4;
    std::vector<glm::mat4x3> track_5;
    std::vector<glm::mat4x3> track_6;
    std::vector<glm::mat4x3> track_7;
    
    // point 1
    glm::vec3 t1p1p0 = glm::vec3(-10.0f, 0.0f, 5.0f);
    glm::vec3 t1p1p1 = glm::vec3(-10.0f, 0.0f, -5.0f);
    glm::vec3 t1p1p2 = glm::vec3(-10.0f, 0.0f, -15.0f);
    glm::vec3 t1p1p3 = glm::vec3(-20.0f, 0.0f, -10.0f);
    
    glm::mat4x3 t1p1;
    t1p1[0] = t1p1p0;
    t1p1[1] = t1p1p1;
    t1p1[2] = t1p1p2;
    t1p1[3] = t1p1p3;
    
    // point 2
    glm::vec3 t1p2p2 = glm::vec3(-5.0f, 0.0f, -15.0f);
    glm::vec3 t1p2p3 = glm::vec3(0.0f, 0.0f, -20.0f);
    
    glm::mat4x3 t1p2;
    t1p2[0] = t1p1p3;
    t1p2[1] = t1p1p3 + (t1p1p3 - t1p1p2);
    t1p2[2] = t1p2p2;
    t1p2[3] = t1p2p3;
    
    // point 3
    glm::vec3 t1p3p2 = glm::vec3(20.0f, 0.0f, 10.0f);
    glm::vec3 t1p3p3 = glm::vec3(10.0f, 0.0f, -10.0f);
    
    glm::mat4x3 t1p3;
    t1p3[0] = t1p2p3;
    t1p3[1] = t1p2p3 + (t1p2p3 - t1p2p2);
    t1p3[2] = t1p3p2;
    t1p3[3] = t1p3p3;
    
    // point 4
    glm::vec3 t1p4p2 = glm::vec3(5.0f, 0.0f, 10.0f);
    glm::vec3 t1p4p3 = glm::vec3(10.0f, 0.0f, 15.0f);
    
    glm::mat4x3 t1p4;
    t1p4[0] = t1p3p3;
    t1p4[1] = t1p3p3 + (t1p3p3 - t1p3p2);
    t1p4[2] = t1p4p2;
    t1p4[3] = t1p4p3;
    
    // point 5
    glm::mat4x3 t1p5;
    t1p5[0] = t1p4p3;
    t1p5[1] = t1p4p3 + (t1p4p3 - t1p4p2);
    t1p5[2] = t1p1p0 + (t1p1p0 - t1p1p1);
    t1p5[3] = t1p1p0;

    track_1.push_back(t1p1);
    track_1.push_back(t1p2);
    track_1.push_back(t1p3);
    track_1.push_back(t1p4);
    track_1.push_back(t1p5);
    
    // point 1
    glm::vec3 t2p1p0 = glm::vec3(15.0f, 0.0f, 5.0f);
    glm::vec3 t2p1p1 = glm::vec3(-10.0f, 0.0f, -5.0f);
    glm::vec3 t2p1p2 = glm::vec3(0.0f, 0.0f, -5.0f);
    glm::vec3 t2p1p3 = glm::vec3(10.0f, 0.0f, 10.0f);
    
    glm::mat4x3 t2p1;
    t2p1[0] = t2p1p0;
    t2p1[1] = t2p1p1;
    t2p1[2] = t2p1p2;
    t2p1[3] = t2p1p3;
    
    // point 2
    glm::vec3 t2p2p2 = glm::vec3(15.0f, 0.0f, 15.0f);
    glm::vec3 t2p2p3 = glm::vec3(10.0f, 0.0f, -15.0f);
    
    glm::mat4x3 t2p2;
    t2p2[0] = t2p1p3;
    t2p2[1] = t2p1p3 + (t2p1p3 - t2p1p2);
    t2p2[2] = t2p2p2;
    t2p2[3] = t2p2p3;
    
    // point 3
    glm::vec3 t2p3p2 = glm::vec3(20.0f, 0.0f, -5.0f);
    glm::vec3 t2p3p3 = glm::vec3(-10.0f, 0.0f, 10.0f);
    
    glm::mat4x3 t2p3;
    t2p3[0] = t2p2p3;
    t2p3[1] = t2p2p3 + (t2p2p3 - t2p2p2);
    t2p3[2] = t2p3p2;
    t2p3[3] = t2p3p3;
    
    // point 4
    glm::vec3 t2p4p2 = glm::vec3(-15.0f, 0.0f, -20.0f);
    glm::vec3 t2p4p3 = glm::vec3(-20.0f, 0.0f, 5.0f);
    
    glm::mat4x3 t2p4;
    t2p4[0] = t2p3p3;
    t2p4[1] = t2p3p3 + (t2p3p3 - t2p3p2);
    t2p4[2] = t2p4p2;
    t2p4[3] = t2p4p3;
    
    // point 5
    glm::mat4x3 t2p5;
    t2p5[0] = t2p4p3;
    t2p5[1] = t2p4p3 + (t2p4p3 - t2p4p2);
    t2p5[2] = t2p1p0 + (t2p1p0 - t2p1p1);
    t2p5[3] = t2p1p0;
    
    track_2.push_back(t2p1);
    track_2.push_back(t2p2);
    track_2.push_back(t2p3);
    track_2.push_back(t2p4);
    track_2.push_back(t2p5);
    
    // point 1
    glm::vec3 t3p1p0 = glm::vec3(0.0f, 0.0f, 10.0f);
    glm::vec3 t3p1p1 = glm::vec3(-10.0f, 0.0f, 5.0f);
    glm::vec3 t3p1p2 = glm::vec3(0.0f, 0.0f, -5.0f);
    glm::vec3 t3p1p3 = glm::vec3(10.0f, 0.0f, 0.0f);
    
    glm::mat4x3 t3p1;
    t3p1[0] = t3p1p0;
    t3p1[1] = t3p1p1;
    t3p1[2] = t3p1p2;
    t3p1[3] = t3p1p3;
    
    // point 2
    glm::vec3 t3p2p2 = glm::vec3(5.0f, 0.0f, 10.0f);
    glm::vec3 t3p2p3 = glm::vec3(12.0f, 0.0f, -5.0f);
    
    glm::mat4x3 t3p2;
    t3p2[0] = t3p1p3;
    t3p2[1] = t3p1p3 + (t3p1p3 - t3p1p2);
    t3p2[2] = t3p2p2;
    t3p2[3] = t3p2p3;
    
    // point 3
    glm::vec3 t3p3p2 = glm::vec3(10.0f, 0.0f, 15.0f);
    glm::vec3 t3p3p3 = glm::vec3(-5.0f, 0.0f, -5.0f);
    
    glm::mat4x3 t3p3;
    t3p3[0] = t3p2p3;
    t3p3[1] = t3p2p3 + (t3p2p3 - t3p2p2);
    t3p3[2] = t3p3p2;
    t3p3[3] = t3p3p3;
    
    // point 4
    glm::vec3 t3p4p2 = glm::vec3(10.0f, 0.0f, -15.0f);
    glm::vec3 t3p4p3 = glm::vec3(8.0f, 0.0f, 10.0f);
    
    glm::mat4x3 t3p4;
    t3p4[0] = t3p3p3;
    t3p4[1] = t3p3p3 + (t3p3p3 - t3p3p2);
    t3p4[2] = t3p4p2;
    t3p4[3] = t3p4p3;
    
    // point 5
    glm::mat4x3 t3p5;
    t3p5[0] = t3p4p3;
    t3p5[1] = t3p4p3 + (t3p4p3 - t3p4p2);
    t3p5[2] = t3p1p0 + (t3p1p0 - t3p1p1);
    t3p5[3] = t3p1p0;
    
    track_3.push_back(t3p1);
    track_3.push_back(t3p2);
    track_3.push_back(t3p3);
    track_3.push_back(t3p4);
    track_3.push_back(t3p5);
    
    // point 1
    glm::vec3 t4p1p0 = glm::vec3(5.0f, 0.0f, 0.0f);
    glm::vec3 t4p1p1 = glm::vec3(-10.0f, 0.0f, -2.0f);
    glm::vec3 t4p1p2 = glm::vec3(0.0f, 0.0f, 10.0f);
    glm::vec3 t4p1p3 = glm::vec3(10.0f, 0.0f, 10.0f);
    
    glm::mat4x3 t4p1;
    t4p1[0] = t4p1p0;
    t4p1[1] = t4p1p1;
    t4p1[2] = t4p1p2;
    t4p1[3] = t4p1p3;
    
    // point 2
    glm::vec3 t4p2p2 = glm::vec3(0.0f, 0.0f, 20.0f);
    glm::vec3 t4p2p3 = glm::vec3(-10.0f, 0.0f, -20.0f);
    
    glm::mat4x3 t4p2;
    t4p2[0] = t4p1p3;
    t4p2[1] = t4p1p3 + (t4p1p3 - t4p1p2);
    t4p2[2] = t4p2p2;
    t4p2[3] = t4p2p3;
    
    // point 3
    glm::vec3 t4p3p2 = glm::vec3(18.0f, 0.0f, 15.0f);
    glm::vec3 t4p3p3 = glm::vec3(-12.0f, 0.0f, -15.0f);
    
    glm::mat4x3 t4p3;
    t4p3[0] = t4p2p3;
    t4p3[1] = t4p2p3 + (t4p2p3 - t4p2p2);
    t4p3[2] = t4p3p2;
    t4p3[3] = t4p3p3;
    
    // point 4
    glm::vec3 t4p4p2 = glm::vec3(0.0f, 0.0f, 8.0f);
    glm::vec3 t4p4p3 = glm::vec3(-10.0f, 0.0f, 5.0f);
    
    glm::mat4x3 t4p4;
    t4p4[0] = t4p3p3;
    t4p4[1] = t4p3p3 + (t4p3p3 - t4p3p2);
    t4p4[2] = t4p4p2;
    t4p4[3] = t4p4p3;
    
    // point 5
    glm::mat4x3 t4p5;
    t4p5[0] = t4p4p3;
    t4p5[1] = t4p4p3 + (t4p4p3 - t4p4p2);
    t4p5[2] = t4p1p0 + (t4p1p0 - t4p1p1);
    t4p5[3] = t4p1p0;
    
    track_4.push_back(t4p1);
    track_4.push_back(t4p2);
    track_4.push_back(t4p3);
    track_4.push_back(t4p4);
    track_4.push_back(t4p5);

    
    // create npcs
    npcs.push_back(new NPC(new OBJObject("daniel.obj","daniel.ppm"), new Track(track_1), false, true));
    npcs.push_back(new NPC(new OBJObject("morgan.obj", "morgan.ppm"), new Track(track_2), false, false));
    npcs.push_back(new NPC(new OBJObject("will.obj", "will.ppm"), new Track(track_3), false, false));
    npcs.push_back(new NPC(new OBJObject("petra.obj", "petra.ppm"), new Track(track_4), false, true));
    
    /*
    npcs.push_back(new NPC(new OBJObject("pblck.obj", "pblck.ppm"), new Track(track_5), false));
    npcs.push_back(new NPC(new OBJObject("baynes.obj", "baynes.ppm"), new Track(track_6), false));
    npcs.push_back(new NPC(new OBJObject("liza.obj", "liza.ppm"), new Track(track_7), false));
     */
    
}

void Window::moveNpcs() {
    
    for(int i = 0; i < npcs.size(); i++) {
        // do nothing if dead
        if (npcs[i]->deathcounter > 0) {
            // decrease counter
            npcs[i]->deathcounter--;
            continue;
        }
        
        // orientation of npc
        int pos1 = (npcs[i]->position + 1) % npcs[i]->track->lineVertices.size();
        glm::vec3 z = glm::normalize(npcs[i]->track->lineVertices[npcs[i]->position] - npcs[i]->track->lineVertices[pos1]);
        glm::vec3 x = glm::normalize(glm::cross(glm::vec3(0.0f,1.0f,0.0f), z));
        glm::vec3 y = glm::normalize(glm::cross(z, x));
        npcs[i]->npc->toWorld[0] = glm::vec4(x, 0.0f);
        npcs[i]->npc->toWorld[1] = glm::vec4(y, 0.0f);
        npcs[i]->npc->toWorld[2] = glm::vec4(-z, 0.0f);
    
        // position of npc1
        npcs[i]->npc->toWorld[3][0] = npcs[i]->track->lineVertices[npcs[i]->position % npcs[i]->track->lineVertices.size()].x;
        npcs[i]->npc->toWorld[3][1] = npcs[i]->track->lineVertices[npcs[i]->position % npcs[i]->track->lineVertices.size()].y;
        npcs[i]->npc->toWorld[3][2] = npcs[i]->track->lineVertices[npcs[i]->position % npcs[i]->track->lineVertices.size()].z;
        
        npcs[i]->box->toWorld = npcs[i]->npc->toWorld;
    
        npcs[i]->position++;
    }
}

void Window::check_collision() {
    std::vector<float> dragonBound = alduinbox->getBoundary();
    std::vector<float> npcBound;
    // check collision for each npc
    for (int i = 0; i < npcs.size(); i++) {
        
        // check if on counter
        if (npcs[i]->deathcounter > 0) {
            npcs[i]->dead = true;
        }
        
        bool checkDeath = npcs[i]->dead;
        npcBound = npcs[i]->box->getBoundary();
        // check bound
        if (dragonBound[0] > npcBound[1] && dragonBound[2] > npcBound[3] && dragonBound[4] > npcBound[5] &&
            dragonBound[1] < npcBound[0] && dragonBound[3] < npcBound[2] && dragonBound[5] < npcBound[4]) {
            npcs[i]->dead = true;
            // std::cout << "DEAD" << std::endl;
        }
        else {
            npcs[i]->dead = false;
        }
        
        if (checkDeath == false && npcs[i]->dead == true) {
            audio->dead();
            npcs[i]->deathcounter = 1000;
        }
    }
    
    /* Debug */
    float minX = dragonBound[1], minY = dragonBound[3], minZ = dragonBound[5];
    float maxX = dragonBound[0], maxY = dragonBound[2], maxZ = dragonBound[4];
    
    std::vector<float> boundingbox;
    std::vector<glm::vec3> boxVertices;
    
    // bounding box
    boundingbox.push_back(minX);
    boundingbox.push_back(minY);
    boundingbox.push_back(maxZ);
    boxVertices.push_back(glm::vec3(minX,minY,maxZ));
    
    boundingbox.push_back(maxX);
    boundingbox.push_back(minY);
    boundingbox.push_back(maxZ);
    boxVertices.push_back(glm::vec3(maxX,minY,maxZ));
    
    boundingbox.push_back(maxX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(maxZ);
    boxVertices.push_back(glm::vec3(maxX,maxY,maxZ));
    
    boundingbox.push_back(minX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(maxZ);
    boxVertices.push_back(glm::vec3(minX,maxY,maxZ));
    
    /* */
    boundingbox.push_back(minX);
    boundingbox.push_back(minY);
    boundingbox.push_back(maxZ);
    
    boundingbox.push_back(minX);
    boundingbox.push_back(minY);
    boundingbox.push_back(minZ);
    boxVertices.push_back(glm::vec3(minX,minY,minZ));
    
    boundingbox.push_back(maxX);
    boundingbox.push_back(minY);
    boundingbox.push_back(minZ);
    boxVertices.push_back(glm::vec3(maxX,minY,minZ));
    
    boundingbox.push_back(maxX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(minZ);
    boxVertices.push_back(glm::vec3(maxX,maxY,minZ));
    
    boundingbox.push_back(minX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(minZ);
    boxVertices.push_back(glm::vec3(minX,maxY,minZ));
    
    /* */
    boundingbox.push_back(minX);
    boundingbox.push_back(minY);
    boundingbox.push_back(minZ);
    
    boundingbox.push_back(minX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(maxZ);
    boundingbox.push_back(minX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(minZ);
    
    boundingbox.push_back(maxX);
    boundingbox.push_back(minY);
    boundingbox.push_back(maxZ);
    boundingbox.push_back(maxX);
    boundingbox.push_back(minY);
    boundingbox.push_back(minZ);
    
    boundingbox.push_back(maxX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(maxZ);
    boundingbox.push_back(maxX);
    boundingbox.push_back(maxY);
    boundingbox.push_back(minZ);

    if (testbox != alduinbox) {
        delete testbox;
    }
    
    testbox = new BoundingBox(boundingbox,boxVertices);
    
}