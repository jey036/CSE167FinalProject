#ifndef Audio_hpp
#define Audio_hpp

#include <irrKlang/irrKlang.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include <iostream>
#include <vector>

#include "NPC.hpp"

using namespace irrklang;

class Audio {
public:
    Audio(std::vector<NPC*> npcs);
    ~Audio();
    
    ISoundEngine* engine;
    ISoundSource* background;
    std::vector<ISound*> soundeffect;
    
    void dead();
    void update(std::vector<NPC*> npcs, glm::vec3 cam_pos, glm::vec3 cam_look_at, glm::vec3 cam_up);
};

#endif /* Audio_hpp */
