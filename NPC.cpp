#include "NPC.hpp"

NPC::NPC(OBJObject* obj, Track* path, bool isDead, bool isFemale) {
    npc = obj;
    track = path;
    dead = isDead;
    deathcounter = 0;
    gender = isFemale;
    box = new BoundingBox(obj->boundingbox,obj->boxVertices);
    
    srand (time(NULL));
    int offset = rand();
    position = offset  % track->lineVertices.size();
}

NPC::~NPC() {
}

void NPC::setDeathCounter(int counter) {
    deathcounter = counter;
}