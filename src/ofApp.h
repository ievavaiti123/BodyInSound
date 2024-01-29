#pragma once

#include "ofMain.h"
#include "CustomShape.h"
#include "ofxOsc.h"
#include <algorithm>
#include <vector>

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		//variables for receiving the last position after the person has left, so the shape doesn't jump to a random point
		int lastX;
		int lastZ;

		//receive osc messages
		ofxOscReceiver receiver;

		//shape to go over person
		CustomShape customShape;

		//line
		ofPolyline line;
		//a vector of lines
		vector<ofPolyline> lineVector;
		//points
		ofPoint pt;

		//for the path particles
		vector<int> radius;
		vector<int> alpha;
		vector <float> xOffsetArray;
		vector <float> yOffsetArray;
		
		//track person ids (each time person1 enters and leaves the space)
		float previousId;
		float currentId = 0.;
		//array of current or recent id's
		vector<int> ids;
		//current index of person
		float idTracker;
		//array of paths to retract
		vector<int>pathsToRetract;
		bool hasChanged = false;
		bool p1_active = false;
		int lastActiveId;

		//vector<unsigned long long> pathTriggerTimes;

		//track positions
		float tx;
		float tz;
		float ty;
		//set ranges once callibrated
		float range_left = -1.3;
		float range_right = 1.3;
		float range_up = 1.8;
		float range_down = 3.35;
		float range_yFrom = -1.;
		float range_yTo = 1.;

		// Trigger function
		void triggerFunction();

		// Variables to track time
		//int startTime;
		bool triggerOccurred;
		bool functionExecuted;

		//function for retracting the path
		void retractLine();
		//monitoring for line retraction
		int monitoredVariable;

		//testing
		bool lineStop = false;

		//texture mapping
		ofFbo canvas;
		ofFbo canvas2;
		int canvasWidth = 1920;
		int canvasHeight = 1080;
		ofTexture canvasTexture;

		ofPoint points[4];
		int cornerIndex = 0;
};


