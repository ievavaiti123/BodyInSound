#include "ofApp.h"

//--------------------------------------------------------------
/*
Body In Sound visualisation - made by Ieva Vaiti

This code needs the OSC addon to work.

The kinect data is transmitted from Touch Designer. I've provided the Touch Designer and Max MSP files, however you need kinect data to see anything happen. To do this you can download Kinect Visual Studio and play a pre-recorded example file. You can also replace the x and y values with mouseX and mouseY, and uncomment the keypressed  code to activate the shape.

*/


void ofApp::setup(){
	//osc receive
	receiver.setup(7320);

	//canvas settings (for mapping to projected shape aka floor)
	canvas.allocate(canvasWidth, canvasHeight, GL_RGBA);

	//set up the different points to map (square, first point top left, next top right etc
	//this is already a mapped shape. to map a new one reset the values to commented ones
	points[0] = glm::vec2(280, 0); //0, 0
	points[1] = glm::vec2(1640, 90); //1920, 0
	points[2] = glm::vec2(1790, 990); // 1920, 1080
	points[3] = glm::vec2(150, 930); // 0, 1080

	//set background colour and frame rate
	//frfame rate needed to be set for the custom shape to function properly
	ofSetBackgroundColor(0);
	ofSetFrameRate(60);

	//path booleans
    triggerOccurred = false;
    functionExecuted = false;
	monitoredVariable = 0;
	idTracker = 0;
	previousId = 0;
	lastActiveId = 0;

}

//--------------------------------------------------------------
void ofApp::update(){
	//constantly update the canvas texture - aka the mapped texture
	canvasTexture = canvas.getTexture();

	//for receiving OSC messages
	while (receiver.hasWaitingMessages()) {	//VERY IMPORTANT LINE!! without this, the receiver overfills with messages and becomes out of sync with the data that's coming in
		ofxOscMessage m;
		receiver.getNextMessage(&m);
		if (m.getAddress() == "/tx") {
			tx = m.getArgAsFloat(0); 
			//cout << "Received TX: " << tx << endl;
		
		}
		else if (m.getAddress() == "/ty") {
			ty = m.getArgAsFloat(0); 
			//cout << "Received TY: " << ty << endl;
		}
		else if (m.getAddress() == "/tz") {
			tz = m.getArgAsFloat(0); 
			//cout << "Received TZ: " << tz << endl;
		}
		else if (m.getAddress() == "/p1") {	//person 1 id number
			currentId = m.getArgAsFloat(0); 
			//cout << "Received p1: " << currentId << endl;
		}

	}

	//update customShape
	customShape.update();

	if (!triggerOccurred) {
		// check if trigger occured
		triggerFunction();
	}
	else {
		//retrack the line function
		retractLine();
			
	}

	//move about the vertice points of the path 
	//the circles get dispersed over time if not cleared - blurring the path
	for (int i = 0; i < lineVector.size(); ++i) {
			for (auto& vert : lineVector[i].getVertices()) {
				vert.x += ofRandom(-1, 1);
				vert.y += ofRandom(-1, 1);
			}
	}

	//is person 1 active/inactive
	if (currentId > 0) {
		p1_active = true;
	}
	else {
		p1_active = false;
	}

	//track id without 0s and previous id
	if (currentId > 0) {
		idTracker = currentId;
		if (idTracker > 0) {
			previousId = idTracker - 1;
		}
	}
	
	//logic to track which path to retract
	if (idTracker > previousId && !p1_active || idTracker == previousId && !p1_active && idTracker > 0 || idTracker > previousId && idTracker > 0) {
		lastActiveId = idTracker;
		// Check if lastActiveId is already in pathsToRetract
		auto it = find(pathsToRetract.begin(), pathsToRetract.end(), lastActiveId);
		// If it's not in pathsToRetract, add it
		if (it == pathsToRetract.end()) {
			pathsToRetract.push_back(lastActiveId);
		}
	}
	

}

//--------------------------------------------------------------
void ofApp::draw(){
	
	//begin texture map canvas
	canvas.begin();
	
	//draw my own brackground - texture canvas is transparent
	ofSetColor(0);
	ofFill();
	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

	//map xz values to screen size
	float p1_tx = ofMap(tx, range_left, range_right, 0, 1920, true);
	float p1_tz = ofMap(tz, range_up, range_down, 0, 1080,true);
	//y values for height - changing the noise amount
	float p1_ty = ofMap(ty, range_yFrom, range_yTo, 15, 50);


	//if person 1 is active
	if (currentId > 0.) {

		//draw silhouette
		ofPushMatrix();
		ofTranslate(p1_tx, p1_tz);
		customShape.appear(true);
		customShape.draw(p1_ty);
		ofPopMatrix();

		//add new paths and track their ids
		if (monitoredVariable != currentId) {
			// Add the new value to the vector
			ids.push_back(previousId);
			lineVector.push_back(line);
		
			// Update the monitored variable
			monitoredVariable = currentId;
		}
		//set position for points
		pt.set(p1_tx, p1_tz);
			if (p1_active) {
				if (pt.x > 0 && pt.y > 0 && !lineStop) {
					//control how many points are drawn at any one time
					for (int i = 0; i < lineVector.size(); i++) {
						if (lineVector[i].getVertices().empty() || (pt - lineVector[i].getVertices().back()).length() > 3) {
							lineVector[i].addVertex(pt);
						}
						
					}
				}
			}
			lastX = p1_tx;
			lastZ = p1_tz;
	}
	else {

		//fade out when person is inactive
		if (!customShape.isFinished()) {
			ofPushMatrix();
			ofTranslate(lastX, lastZ);
			customShape.draw(p1_ty);
			customShape.appear(false);
			ofPopMatrix();
		}
	}
	ofSetColor(255);
	
	//settings for drawing a particle path
	int maxRadius = 10;  
	int minRadius = 2;
	int radiusStepSize = 50; 
	int maxOffsetDistance = 80;

	for (int i = 0; i < lineVector.size(); i++) {
		const ofPolyline& polyline = lineVector[i];
		for (int j = 0; j < polyline.size(); ++j) {
			//create some variations in circle sizes, colour and offset
			radius.push_back(ofRandom(minRadius, maxRadius));
			alpha.push_back(ofRandom(20, 80));
			float angle = ofRandom(ofDegToRad(360.0));
			float distance = ofRandom(maxOffsetDistance);
			float xOffset = cos(angle) * distance;
			xOffsetArray.push_back(xOffset);
			float yOffset = sin(angle) * distance;
			yOffsetArray.push_back(yOffset);
			ofSetColor(255, alpha[j]);
			//draw the circles with the values
			ofDrawCircle(polyline[j].x + xOffsetArray[j], polyline[j].y + yOffsetArray[j], radius[j]);
		}
	}
	
	//the end of the drawing of canvas aka the texture mapping
	canvas.end();
	
	//draw the mapped texture
	canvasTexture.draw(points[0], points[1], points[2], points[3]); // Draw on a 3D object


	
}

void ofApp::triggerFunction() {
	//track trigger, track elapsed time 
	if (currentId == 0. && ids.size() > 0) {
		triggerOccurred = true;
	}
}

void ofApp::retractLine() {
	//function to retract line when person is out
	for (int i = 0; i < pathsToRetract.size(); i++) {

		if (lineVector[i].size() > 0) {
			functionExecuted = false;
			int index = lineVector[i].size();

			if (ofGetFrameNum() % 4 == 0) {
				//slow down retract time
				//erase the vertice points
				lineVector[i].getVertices().erase(
					lineVector[i].getVertices().begin() + index - 1);

			}
		}
		else if (lineVector[i].size() == 0) {
			//once empty mark as complete
			functionExecuted = true;
		}
		
	}
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	//for adjusting the projector mapping
	//this is for setting up only.
	if (key == '1') {
		cornerIndex = 0;
	}
	if (key == '2') {
		cornerIndex = 1;
	}
	if (key == '3') {
		cornerIndex = 2;
	}
	if (key == '4') {
		cornerIndex = 3;
	}



	if (key == OF_KEY_LEFT) {
		points[cornerIndex].x -= 10;
		//cout << "points" << cornerIndex << "x" << points[cornerIndex].x << endl;
	}
	if (key == OF_KEY_RIGHT) {
		points[cornerIndex].x += 10;
		//cout << "points" << cornerIndex << "x" << points[cornerIndex].x << endl;
	}
	if (key == OF_KEY_UP) {
		points[cornerIndex].y -= 10;
		//cout << "points" << cornerIndex << "y" << points[cornerIndex].y << endl;
	}
	if (key == OF_KEY_DOWN) {
		points[cornerIndex].y += 10;
		//cout << "points" << cornerIndex << "y" << points[cornerIndex].y << endl;
	}

	if (key == 'f') {
		//debugging of canvas position
		cout << points[0] << endl;
		cout << points[1] << endl;
		cout << points[2] << endl;
		cout << points[3] << endl;

		ofExit();
	}


	//this code was used for testing only with mouse
	/*if (key == 'a') {
		lineStop = true;
	}*/


	//testing only
	/*if (key == '0') {
		currentId = 0.;
	}

	if (key == '1') {
		currentId = 1.;
	}

	if (key == '2') {
		currentId = 2.;
	}*/
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}