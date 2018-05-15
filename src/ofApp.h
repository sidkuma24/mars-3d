#pragma once

#include "ofMain.h"
#include  "ofxAssimpModelLoader.h"
#include "box.h"
#include "ray.h"
#include <vector>
#include <ofxGui.h>
#include <iostream>
#include <fstream>

class TreeNode {

public:
	Box box;
	vector<int> points;
	vector<TreeNode> children;

public:
	TreeNode() {}
	void setBox(Box& b) {
		this->box = b;
	}

	void setPoints(vector<int>& points) {
		for (int i = 0; i< points.size(); ++i) {
			this->points.push_back(points[i]);
		}
	}

	bool setChild(TreeNode n) {
		int oldLength = this->children.size();
		this->children.push_back(n);
		int newLength = this->children.size();
		return newLength > oldLength;
	}

};

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void toggleMotion();
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
	void drawAxis(ofVec3f);
	void initLightingAndMaterials();
	void savePicture();
	void toggleWireframeMode();
	void togglePointsDisplay();
	void toggleSelectTerrain();
	void setCameraTarget();
	bool  doPointSelection();
	void drawBox(const Box &box);
	Box meshBounds(const ofMesh &);
	void subDivideBox8(const Box &b, vector<Box> & boxList);
	void subDivideBox8_2();

	bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);

	void getDragCoordinates();
	void renderPath();
	
	ofxAssimpModelLoader mars, rover;
	ofLight light;
	Box boundingBox,boxRover;

	bool bAltKeyDown;
	bool bCtrlKeyDown;
	bool bWireframe;
	bool bDisplayPoints;
	bool bPointSelected;

	bool bRoverLoaded;
	bool bTerrainSelected;

	ofVec3f selectedPoint;
	ofVec3f intersectPoint;
	const float selectionRange = 4.0;
	vector<Box> boxList;
	TreeNode root;
	Box returnBox;

	void getMeshPointsInBox(const ofMesh &mesh, const vector<int>& points, Box & box, vector<int>& pointsRtn);

	void createOctree(TreeNode& root, const ofMesh& mesh, int lvl);
	
	void subdivide(TreeNode& node, const ofMesh& mesh,  int lvl);
	
	// drawOcttree function not used. Another implementation performed.
	void drawOctree(TreeNode & node, int numLevels) {
		if (node.children.size() == 0){
			drawBox(node.box);
			return;
		}
		ofSetColor(ofColor::yellow);
		drawBox(node.box);
		numLevels--;
		for (int i = 0; i < node.children.size(); i++){
			drawOctree(node.children[i],numLevels);
		}
	}
	// log file
	ofstream logfile;

	bool doPointSelection(TreeNode& tree, const ofMesh& mesh);
	bool bRoverSelected = false;
	bool isTerrainSelected();
	vector<ofPoint> pointsArray;
	vector<Box> pointBoxes;
	
	// ofPath path;
	ofPolyline path;
	bool pathSet = false;
	ofVboMesh tessellation;
	void translateRover();
	ofVec3f nextPoint;
	int prevIndex = -1;
	int pathTotalLength;
	float currentPosition =0.0;

	bool bDragPoint = false;
	int dragPoint;
	void deletePoint();

	//Camera functionality
	ofVec3f lookDir; // already normalized
	ofVec3f camPos;
	ofVec3f targetPos;

	bool bCamPos;
	bool bTrackingCam;
	bool bFollowCam;
	bool bDriverCam;
	bool bRearCam;

	ofEasyCam cam,cam1,cam2,cam3;
	//ofEasyCam trackCam, followCam, driverCam, backupCam, cam1, cam2, cam3;
	vector<ofEasyCam> cameraList;
	int cameraNo = 0;

	ofxFloatSlider speed;
	ofxPanel gui;
	bool bShowGUI = true;

	void ofToggleGUI();
	void saveEditPoints();
	void loadEditPoints();

	string filepath = "control_points.txt";
};


