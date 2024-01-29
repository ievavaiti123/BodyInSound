#include "CustomShape.h"

//this shape class is for outlining person position on the floor
CustomShape::CustomShape() {
    // Set up the initial position and size of shape
    position = ofVec2f(ofGetWidth()/2, ofGetHeight()/2);
    
    angle = 0;
    
}

void CustomShape::update() {
    // Update the animation parameters
    angle += 0.001;

}

void CustomShape::draw(float height) {
    
    ofPath path;
    
    path.setStrokeColor(white);
    path.setStrokeWidth(1.5);

    // custom shape settings
    int span = 240;
    int radius_base = 200;
    int len = height;
    float hue = 0;

    // Position of blob
    float x = position.x;
    float y = position.y;

    vector<glm::vec2> in_vertices;
    for (int deg = 0; deg < 360; deg++) {
        
        auto noise_point = glm::vec2(x + radius_base * cos(deg * DEG_TO_RAD), y + radius_base * sin(deg * DEG_TO_RAD));
        
        float noise_len = ofMap(ofNoise(noise_point.x * 0.01, noise_point.y * 0.01, ofGetFrameNum() * 0.01), 0, 1, 5, len);

        float radius = radius_base + noise_len;

        radius = radius_base - noise_len;

        in_vertices.push_back(glm::vec2(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD)));
    }

    //begin path
    path.moveTo(in_vertices[0].x, in_vertices[0].y);
    for (size_t i = 1; i < in_vertices.size(); i++) {
        path.lineTo(in_vertices[i].x, in_vertices[i].y);
    }
    //end path
    path.close();

    ofColor grey(200, alpha);
    path.setFillColor(grey);
    path.draw();
}


//fading method
void CustomShape::appear(bool active) {
    if (active && white < 150) {
        white += 1;
        fadeDone = false;
    }
    else if (!active && white > 0) {
        white -= 1;
        fadeDone = false;
    }
   

    if (active && alpha < 20) {
        alpha += 1;
        fadeDone = false;
    }
    else if (!active && alpha > 0) {
        alpha -= 1;
        fadeDone = false;
    }
    else if (!active && alpha == 0 && white == 0) {
        //notify once finished fading
        fadeDone = true;
    }
    


}

//method to track if fade is finished
int CustomShape::isFinished() {
    if (fadeDone) {
        return true;
    }
    else {
        return false;
    }
}
