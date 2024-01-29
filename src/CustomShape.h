#pragma once

#include "ofMain.h"

class CustomShape {
public:
    CustomShape();

    void update();
    void draw(float height);
    void appear(bool active);
    int isFinished();

    ofVec2f position;
    float angle;
    bool fadeDone = false;
    
    
    int white = 0;
    int alpha = 0;
};


