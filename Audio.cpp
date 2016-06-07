#include "Audio.hpp"

Audio::Audio(std::vector<NPC*> npcs) {
    
    // start the sound engine with default parameters
    engine = createIrrKlangDevice();
    
    if (!engine)
        return; // error starting up the engine
    
    // background music
    background = engine->addSoundSourceFromFile("background.wav");
    background->setDefaultVolume(0.5f);
    
    // play background music
    engine->play2D(background, true);
    
    /* load all sound effects */
    for (int i = 0; i < npcs.size(); i++) {
        if (npcs[i]->gender) {
            soundeffect.push_back(engine->play3D("femalescream.wav", vec3df(npcs[i]->npc->toWorld[3][0],npcs[i]->npc->toWorld[3][1],npcs[i]->npc->toWorld[3][2]), false, true));
        }
        else {
            soundeffect.push_back(engine->play3D("malescream.wav", vec3df(npcs[i]->npc->toWorld[3][0],npcs[i]->npc->toWorld[3][1],npcs[i]->npc->toWorld[3][2]), false, true));
        }
    }
    
    for (int i = 0; i < soundeffect.size(); i++) {
        // set bound and unpause
        if (soundeffect[i]) {
            soundeffect[i]->setVolume(0.5f);
            soundeffect[i]->setMinDistance(3.0f);
            soundeffect[i]->setIsLooped(true);
            if (npcs[i]->dead) {
                soundeffect[i]->setIsPaused(true);
            }
            else {
                soundeffect[i]->setIsPaused(false);
            }
        }
    }
    
    irrklang::vec3df position(0,0,0);        // position of the listener
    irrklang::vec3df lookDirection(0,0,0); // the direction the listener looks into
    irrklang::vec3df velPerSecond(0,0,0);    // only relevant for doppler effects
    irrklang::vec3df upVector(0,1,0);        // where 'up' is in your 3D scene

    engine->setListenerPosition(position, lookDirection, velPerSecond, upVector);
    
}

Audio::~Audio() {
    engine->removeAllSoundSources();
    engine->drop();// delete engine
}

void Audio::update(std::vector<NPC*> npcs, glm::vec3 cam_pos, glm::vec3 cam_look_at, glm::vec3 cam_up) {
    
    irrklang::vec3df position(cam_pos.x,cam_pos.y,cam_pos.z);        // position of the listener
    irrklang::vec3df lookDirection(cam_look_at.x,cam_look_at.y,cam_look_at.z); // the direction the listener looks into
    irrklang::vec3df velPerSecond(0,0,0);    // only relevant for doppler effects
    irrklang::vec3df upVector(cam_up.x,cam_up.y,cam_up.z);        // where 'up' is in your 3D scene
    
    engine->setListenerPosition(position, lookDirection, velPerSecond, upVector);
    
    for (int i = 0; i < npcs.size(); i++) {
        
        // check if on counter
        if (npcs[i]->deathcounter > 0) {
            npcs[i]->dead = true;
        }
        
        // check if dead
        if (npcs[i]->dead) {
            soundeffect[i]->setIsPaused(true);
            // engine->play2D("dead.wav");
        }
        else {
            soundeffect[i]->setIsPaused(false);
        }
        // move sound position
        soundeffect[i]->setPosition(vec3df(npcs[i]->npc->toWorld[3][0],npcs[i]->npc->toWorld[3][1],npcs[i]->npc->toWorld[3][2]));
    }
}

void Audio::dead() {
    engine->play2D("dead.wav");
}