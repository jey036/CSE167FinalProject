#ifndef NPC_hpp
#define NPC_hpp

#include "Track.hpp"
#include "OBJObject.hpp"
#include "BoundingBox.hpp"

#include <time.h> 
#include <iostream>

class NPC {
public:
    Track * track;
    OBJObject * npc;
    BoundingBox * box;
    int deathcounter;
    int position;
    bool dead;
    bool gender;
    
    NPC(OBJObject*,Track*,bool,bool);
    ~NPC();
    
    void setDeathCounter(int);
};

#endif /* NPC_hpp */
