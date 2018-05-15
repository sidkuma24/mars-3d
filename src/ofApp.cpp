//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Mars HiRise Project - startup scene
// 
//  This is an openFrameworks 3D scene that includes an EasyCam
//  and example 3D geometry which I have reconstructed from Mars
//  HiRis photographs taken the Mars Reconnaisance Orbiter
//
//  You will use this source file (and include file) as a starting point
//  to implement assignment 5  (Parts I and II)
//
//  Please do not modify any of the mappings.  I would like 
//  the input interface to be the same for each student's 
//  work.  Please also add your name/date below.

//  Please document/comment all of your work !
//  Have Fun !!
//
//  Student Name:  Siddharth Kumar
//  Date:          2018-05-12

/*******************************************************************************************
 * Key Bindings:
 *	'p' - Generate the paths and animate the rover.
 *	'd' - Alternate between cameras.
 *	'h' - first person camera.
 *	'z' - Toggle GUI.
 *	'r' - reset camera.
 *	'del' - delete a point.
 *	'g' - clear paths and points.
 ********************************************************************************************/

#include "ofApp.h"
#include "string"
#include "Util.h"
#include <ctime>    // For time()#include <cstdlib>  // For srand() and rand()#include <vector>//#include "Octree.h"using namespace std;
#include <ctime>
#include <chrono>

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup() {

	string version = ".4.1.";
	chrono::system_clock::time_point time = chrono::system_clock::now();
	time_t tt;
	tt = chrono::system_clock::to_time_t(time);
	string sttime = ctime(&tt);
	string logfilename = "mars3D.log" + version + sttime;
	logfile.open(logfilename);
	logfile << "Started log file:\n";

	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bRoverLoaded = false;
	bTerrainSelected = true;
	//	ofSetWindowShape(1024, 768);

	// Camera setup
	ofVec3f camAxis = ofPoint(0, 0, 1) - ofPoint(0, 0, 0);
	cam.setDistance(20);
	cam.setPosition(0, 0, 10);
	cam.lookAt(ofPoint(0, 0, 0));
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	cam.disableMouseInput();
	cameraList.push_back(cam);

	cam1.setNearClip(.1);
	cam1.setFov(65.5);
	cam1.disableMouseInput();
	cameraList.push_back(cam1);
	logfile << "INFO: Camera 1 Setup complete\n";

	cam2.setDistance(20);
	cam2.setNearClip(.1);
	cam2.setFov(65.5);
	cam2.disableMouseInput();
	cameraList.push_back(cam2);
	logfile << "INFO: Camera 2 Setup complete\n";

	cam3.setDistance(30);
	cam3.setNearClip(.1);
	cam3.setFov(65.5);
	cam3.disableMouseInput();
	cameraList.push_back(cam3);
	cameraNo = 0;
	logfile << "INFO: Camera 3 Setup complete\n";

	//god-cam setup

	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();

	// setup rudimentary lighting 
	//
	initLightingAndMaterials();

	mars.loadModel("geo/mars-low-v2.obj");
	mars.setScaleNormalization(false);
	logfile << "INFO: Mars model loaded \n";

	boundingBox = meshBounds(mars.getMesh(0));

	createOctree(root, mars.getMesh(0), 8);
//	octree = Octree();
//	octree.create(mars.getMesh(0),9);
	logfile << "INFO: Octree created for the mesh \n";
	//myTextFile = ofFile(filePath);
	gui.setup();
	gui.add(speed.setup("Speed", 0.02, 0.01, 0.1));
	logfile << "INFO: GUI added to screen \n";

	// set filepath, to save the control points
	char cwd[1024];
	getcwd(cwd, 1024);
	filepath = cwd + '/' + filepath;

	logfile << "INFO: ofApp setup complete\n";

}

//--------------------------------------------------------------
/*
 * adding animation to the rover and the rover cam
 *
 */

void ofApp::update() {
	if (pathSet && bRoverLoaded) {
		translateRover();
	}

	if (bCamPos)
		cameraList[0].setPosition(rover.getPosition());
}
//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(ofColor::black);

	//cam.begin();
	cameraList[cameraNo].begin();

	ofPushMatrix();
	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		mars.drawWireframe();
		if (bRoverLoaded) {
			rover.drawWireframe();
			if (!bTerrainSelected)
				drawAxis(rover.getPosition());
		}
		if (bTerrainSelected)
			drawAxis(ofVec3f(0, 0, 0));
	} else {
		ofEnableLighting();              // shaded mode
		mars.drawFaces();

		// Change to Rover selected instead of loaded.
		if (bRoverLoaded) {
			rover.drawFaces();

			if (bRoverSelected) {
				ofNoFill();
				ofSetColor(ofColor::white);
				drawBox(boxRover);
				ofSetColor(ofColor::white);
				drawBox(boundingBox);

				/*
				 ofNoFill();
				 ofSetColor(ofColor::white);
				 drawBox(boundingBox);
				 */
			}
		}
		if (bTerrainSelected)
			drawAxis(ofVec3f(0, 0, 0));
	}

	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
		mars.drawVertices();
	}

	// highlight selected point (draw sphere around selected point)
	if (bPointSelected) {
		ofSetColor(ofColor::blue);
		for (int i = 0; i < pointsArray.size(); i++)
			ofDrawSphere(pointsArray[i], .1);
	}

	// Sphere box not drawn for easy view
	// Display coordinates upon selection of a sphere

	if (bDragPoint && !bRoverSelected) {
		getDragCoordinates();
		ofSetColor(ofColor::blue);
		ofDrawSphere(selectedPoint, .1);

	}

	// Generate Path upon pressing 'p' and render said path.
	if (pathSet) {
		renderPath();

	}

	ofPopMatrix();
	//cam.end();
	cameraList[cameraNo].end();

	// GUI display
	if (bShowGUI) {
//		 ofNoFill();
		// ofDisableDepthTest();
		gui.draw();
	}

}

void ofApp::getDragCoordinates() {
	ofNoFill();
	ofSetColor(ofColor::white);
	drawBox(pointBoxes[dragPoint]);
	string position = to_string(pointsArray[dragPoint].x) + ","
			+ to_string(pointsArray[dragPoint].y) + ","
			+ to_string(pointsArray[dragPoint].z);
	ofDrawBitmapString(position, pointsArray[dragPoint].x,
			pointsArray[dragPoint].y);
}

void ofApp::renderPath() {
	path.clear();
	if (pointsArray.size() < 1)
		return;
	path.addVertex(pointsArray[0]);
	path.curveTo(pointsArray[0]);
	for (int i = 1; i < pointsArray.size(); i++) {
		path.addVertex(pointsArray[i]);
		path.curveTo(pointsArray[i].x, pointsArray[i].y, pointsArray[i].z);
	}
	path.curveTo(pointsArray.back());
	path.draw();
	pathTotalLength = path.getLengthAtIndex(pointsArray.size() - 1);
}

// Move the rover. Controlled in update()
//void ofApp::translateRover() {
//
//	bRoverSelected = false;
//	currentPosition = currentPosition + speed;		// Current position along the path
//
//	ofVec3f direction;								//If current position exceeds path length, reset the current postion to starting point of path
//	if (currentPosition >= pathTotalLength) {
//		currentPosition = 0;
//	}
//
//	int index = path.getIndexAtLength(currentPosition);
//	if (index != prevIndex) {
//
//		ofVec3f roverPosition = rover.getPosition();
//		nextPoint = path.getPointAtLength(currentPosition);
//		rover.setPosition(nextPoint.x, nextPoint.y, nextPoint.z);
//		direction = roverPosition - nextPoint;
//		ofPoint axis(ofPoint(0.0, 1.0, 0.0));
//		int rotations = rover.getNumRotations();
//
//		// Calculate angle for orientation.
//		float m = ofVec3f(0, 0, -1).normalize().dot(direction.normalize());
//		float angle = acos(m) * 180.0 / PI;
//		float rotateAngle = path.getAngleAtIndex(index);
//		rover.setRotation(0, angle, 0, 1, 0);
//		prevIndex = index;
//	}
//	// Get the next point on the path based on current position. Increments subsequently
//	//nextPoint = path.getPointAtLength(currentPosition);
//	//rover.setPosition(nextPoint.x, nextPoint.y, nextPoint.z);
//}

void ofApp::translateRover() {

	bRoverSelected = false;
	currentPosition = currentPosition + speed;// Current position along the path

	//If current position exceeds path length, reset the current postion to starting point of path
	if (currentPosition >= pathTotalLength) {
		currentPosition = 0;
	}

	int index = path.getIndexAtLength(currentPosition);

	if (index != prevIndex) {
		ofPoint axis(ofPoint(0.0, 1.0, 0.0));
		int rotations = rover.getNumRotations();

		// Calculate angle for orientation.
		float rotateAngle = path.getAngleAtIndex(index);
		rover.setRotation(rotations, rotateAngle, axis.x, axis.y, axis.z);
		prevIndex = index;
	}
	// Get the next point on the path based on current position. Increments subsequently
	nextPoint = path.getPointAtLength(currentPosition);
	rover.setPosition(nextPoint.x, nextPoint.y, nextPoint.z);

	ofPoint roverPoint = rover.getPosition();
	cameraList[1].setTarget(roverPoint);
	cameraList[1].rotate(90, 0, 1, 0);
	cameraList[2].setPosition(roverPoint.x, roverPoint.y + 2, roverPoint.z - 4);
	//cameraList[2].rotate(roverRotationAngle, 0, 1, 0);
	cameraList[2].lookAt(roverPoint);
	cameraList[3].setPosition(roverPoint.x - 4, roverPoint.y + 2, roverPoint.z);
	//cameraList[3].rotate(roverRotationAngle, 0, 1, 0);
	cameraList[3].lookAt(roverPoint);
}

// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}

void ofApp::keyPressed(int key) {
	switch (key) {
	case 'C':
	case 'c':
		if (cameraList[0].getMouseInputEnabled())
			cameraList[0].disableMouseInput();
		else
			cameraList[0].enableMouseInput();
		break;
	case 'd':
		cameraNo = (cameraNo + 1) % cameraList.size();
		cout << "cameraNo: " << cameraNo << "\n";
		break;
	case 'f':
		ofToggleFullscreen();
		break;
	case 'F':
		break;
	case 'z':
		ofToggleGUI();
		break;
	case 'H':
	case 'h': {
		bCamPos = true;
		Vector3 center = boxRover.center();
		cameraList[0].setPosition(rover.getPosition());
		cameraList[0].setTarget(rover.getPosition());
	}
		break;
	case 'r':
		cameraList[cameraNo].reset();
		break;
	case 's':
		if (!pointsArray.empty()) {
			saveEditPoints();
			cout << "Points saved" << endl;
		} else
			savePicture();
		break;
	case 'l':
		if (pointsArray.size() > 0) {
			pointsArray.clear();
			cout << "Existing points cleared";
			loadEditPoints();
			cout << "Points loaded" << endl;
		} else if (pointsArray.size() == 0)
			loadEditPoints();
		break;
	case 't':
		setCameraTarget();
		break;
	case 'p':
		if (isTerrainSelected())
			bTerrainSelected = true;
		else
			bTerrainSelected = false;
		if (pointsArray.size() > 1) {
			pathSet = true;

		}
		break;
	case 'g':
		pointsArray.clear();
		path.clear();
		pointBoxes.clear();
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'w':
		toggleWireframeMode();
		break;
	case OF_KEY_ALT:
		cameraList[0].enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		if (bDragPoint)
			deletePoint();
		break;
	default:
		break;
	}
}

// Delete a specific point
void ofApp::deletePoint() {
	if (pointsArray.size() == 0)
		return;

	pointsArray.erase(pointsArray.begin() + dragPoint);
	pointBoxes.erase(pointBoxes.begin() + dragPoint);

	logfile << "INFO: Removed a control point\n";
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::ofToggleGUI() {
	bShowGUI = !bShowGUI;
}

void ofApp::keyReleased(int key) {

	switch (key) {

	case 'd':
		break;
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	default:
		break;

	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	//Box Intersect 
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
			Vector3(rayDir.x, rayDir.y, rayDir.z));

	// Timer and Selection
	//cout << "Point selected : ";
	//float time1 = ofGetElapsedTimeMicros();

	// cout << "Point selected" << endl;
	//float time2 = ofGetElapsedTimeMicros();
	//cout << "Elapsed time is : " << time2 - time1 << endl;

	// Upon rover selection
	if (bRoverLoaded && boxRover.intersect(ray, -100, 100)) {
		bRoverSelected = true;
		bTerrainSelected = false;

		logfile << "INFO: Rover Selected \n";
	} else if (isTerrainSelected()) {
		bRoverSelected = false;
		bTerrainSelected = true;
		bDragPoint = false;

		// Selection for existing points. If the ray intersects with the box of a specific point, point is selected.
		for (int i = 0; i < pointBoxes.size(); i++) {
			if (pointBoxes[i].intersect(ray, -100, 100)) {
				bDragPoint = true;
				dragPoint = i;
				break;
			}
		}

//		// Select a new point on the terrain.
//
//		if (doPointSelection(root, mars.getMesh(0)) && bDragPoint == false
//				&& bAltKeyDown == false) {
//
//			logfile << "INFO: Point Selected. \n";
//			pointsArray.push_back(selectedPoint);
//
//			Box pointBox = Box(
//					Vector3(selectedPoint.x - .1, selectedPoint.y - 1,
//							selectedPoint.z - .1),
//					Vector3(selectedPoint.x + .1, selectedPoint.y + .1,
//							selectedPoint.z + .1));
//			pointBoxes.push_back(pointBox);
//			logfile << "INFO: Point BBox created\n";
//		}
	} else {
		bRoverSelected = false;
		bTerrainSelected = false;
	}
}

//draw a box from a "Box" class  

void ofApp::drawBox(const Box &box) {

	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
	float w = size.x();
	float h = size.y();
	float d = size.z();

	if (bRoverSelected && bRoverLoaded)
		ofDrawBox(p, w, h, d);
}

// return a Mesh Bounding Box for the entire Mesh
// find the maximum and minimum vertices for the mesh
// from all the vertices
// then we return a box
Box ofApp::meshBounds(const ofMesh & mesh) {
	int n = mesh.getNumVertices();
	ofVec3f v = mesh.getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	for (int i = 1; i < n; i++) {
		ofVec3f v = mesh.getVertex(i);

		if (v.x > max.x)
			max.x = v.x;
		else if (v.x < min.x)
			min.x = v.x;

		if (v.y > max.y)
			max.y = v.y;
		else if (v.y < min.y)
			min.y = v.y;

		if (v.z > max.z)
			max.z = v.z;
		else if (v.z < min.z)
			min.z = v.z;
	}
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

//  Subdivide a Box into eight(8) equal size boxes, return them in boxList;
//
void ofApp::subDivideBox8(const Box &box, vector<Box> & boxList) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	float xdist = (max.x() - min.x()) / 2;
	float ydist = (max.y() - min.y()) / 2;
	float zdist = (max.z() - min.z()) / 2;
	Vector3 h = Vector3(0, ydist, 0);

	//  generate ground floor
	//
	Box b[8];
	b[0] = Box(min, center);
	b[1] = Box(b[0].min() + Vector3(xdist, 0, 0),
			b[0].max() + Vector3(xdist, 0, 0));
	b[2] = Box(b[1].min() + Vector3(0, 0, zdist),
			b[1].max() + Vector3(0, 0, zdist));
	b[3] = Box(b[2].min() + Vector3(-xdist, 0, 0),
			b[2].max() + Vector3(-xdist, 0, 0));

	boxList.clear();
	for (int i = 0; i < 4; i++)
		boxList.push_back(b[i]);

	// generate second story
	//
	for (int i = 4; i < 8; i++) {
		b[i] = Box(b[i - 4].min() + h, b[i - 4].max() + h);
		boxList.push_back(b[i]);
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	logfile << "--------- Inside Mouse Dragged ------------ \n\n";

	if (bDragPoint) {

		bPointSelected = false;

		doPointSelection(root, mars.getMesh(0));

		logfile << "Dragging point at: " << pointsArray[dragPoint] << "\n";

		pointsArray[dragPoint] = selectedPoint;

		// Create a box for the modified point based on new position and push to box list.
		Box pointBox = Box(
				Vector3(selectedPoint.x - .1, selectedPoint.y - 1,
						selectedPoint.z - .1),
				Vector3(selectedPoint.x + .1, selectedPoint.y + .1,
						selectedPoint.z + .1));
		pointBoxes[dragPoint] = pointBox;

		for (int i = 0; i < pointsArray.size(); i++) {
			cout << pointsArray[i] << endl;
		}
	}
	bDragPoint = false;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	// Select a new point on the terrain.
	if (isTerrainSelected()) {
		if (doPointSelection(root, mars.getMesh(0)) && bDragPoint == false
				&& bAltKeyDown == false) {

			logfile << "INFO: Point Selected. \n";
			pointsArray.push_back(selectedPoint);

			Box pointBox = Box(
					Vector3(selectedPoint.x - .1, selectedPoint.y - 1,
							selectedPoint.z - .1),
					Vector3(selectedPoint.x + .1, selectedPoint.y + .1,
							selectedPoint.z + .1));
			pointBoxes.push_back(pointBox);
			logfile << "INFO: Point BBox created\n";
		}
	}
}

//-------------------------------------------------------------
// Check if terrain is selected.
bool ofApp::isTerrainSelected() {
	ofPoint point;
	if (mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point)) {
		cout << "Terrain selected" << endl;
		return true;
	} else
		return false;
}

//--------------------------------------------------------------
// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {
	if (bRoverSelected) {
		cameraList[0].setTarget(rover.getPosition());
	} else if (bPointSelected && !bRoverSelected) {
		cameraList[0].setTarget(selectedPoint);
	}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {
	ofVec3f point;
	mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point);

	if (rover.loadModel(dragInfo.files[0])) {
		//	if (mouseIntersectPlane(cam.getPosition() - 5, cam.getZAxis().normalize(), point)) {
		rover.setScaleNormalization(false);
		rover.setScale(.005, .005, .005);
		rover.setPosition(point.x, point.y, point.z);

		bRoverLoaded = true;
		bRoverSelected = true;
		bTerrainSelected = false;

		// Create a box for the rover and  initialize the different camera's.
		ofVec3f min = rover.getSceneMin();
		ofVec3f max = rover.getSceneMax();

		boxRover = Box(
				Vector3(min.x * 0.005 + point.x, min.y * 0.005 + point.y,
						min.z * 0.005 + point.z),
				Vector3(max.x * 0.005 + point.x, max.y * 0.005 + point.y,
						max.z * 0.005 + point.z));

		cout << "Rover loaded" << rover.getPosition() << endl;

		ofPoint roverPoint = rover.getPosition();
		Vector3 center = boxRover.center();

		cameraList[1].setPosition(
				ofVec3f(center.x(), center.y(), center.z()) + ofVec3f(0, 1, 0));
		cameraList[1].setTarget(roverPoint + (0, 0, -2));
		cameraList[1].rotate(90, 0, 1, 0);
		cameraList[2].setPosition(roverPoint.x, roverPoint.y + 2,
				roverPoint.z - 4);
		cameraList[2].lookAt(roverPoint);
		cameraList[3].setPosition(roverPoint.x - 4, roverPoint.y + 2,
				roverPoint.z);
		cameraList[3].lookAt(roverPoint);
	} else
		cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] = { .5f, .5f, .5, 1.0f };
	static float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] = { 5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] = { GL_TRUE };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);
	//	glEnable(GL_LIGHT1);
	glShadeModel (GL_SMOOTH);
}

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

/*******************************************************
 * To save the control points to a file
 */
void ofApp::saveEditPoints() {
	ofstream file;
	file.open(filepath);

	for (int i = 0; i < pointsArray.size(); i++) {
		string control_points = ofToString(pointsArray[i]);
		file << control_points << endl;
	}

	file.close();
}

/***************************************************
 * Read the control points from the file
 */
void ofApp::loadEditPoints() {
	ifstream buffer(filepath);
	string line;

	while (getline(buffer, line)) {
		ofVec3f editpoints = ofFromString<ofVec3f>(line);
		pointsArray.push_back(editpoints);
	}
}

/***********************************************************************
 * Check if the ray cast by the mouse pointer instersects with a plane.
 * if it does, return true, with 'point' of intersection.
 */
bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm,
		ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

/*************************************************************************
 * Create the octree of mesh points.
 * Each node consists of :
 *
 * 	1. Box - bbox for the mesh points.
 * 	2. points - list of indices for the mesh points.
 * 	3. childNode - list of child nodes for the current node.
 *
 */
void ofApp::createOctree(TreeNode& root, const ofMesh& mesh, int lvl) {

	// Set bounding box and points to root.
	root.setBox(boundingBox);
	vector<int> points;

	for (int i = 0; i < mesh.getNumVertices(); ++i) {
		points.push_back(i);
	}

	root.setPoints(points);
	// Call sub divide initally on the tree.
	subdivide(root, mesh, lvl);
}

//--------------------------------------------------------------
void ofApp::subdivide(TreeNode& node, const ofMesh& mesh, int lvl) {
	if (lvl == 0) {
		return;
	}

	// Create a box vector and subdivide to 8 boxes
	int childCount = 0;
	vector<Box> boxList;
	subDivideBox8(node.box, boxList);

	for (int i = 0; i < boxList.size(); ++i) {
		vector<int> boxPoints;
		getMeshPointsInBox(mesh, node.points, boxList[i], boxPoints);

		// Only if a point exists, create a node
		if (boxPoints.size() > 0) {
			TreeNode childNode;
			childNode.setBox(boxList[i]); // Errors during rendering.
			// finalBoxList.push_back(boxList[i]); // Creating a global variable to render
			childNode.setPoints(boxPoints);
			node.setChild(childNode);
			childCount++; // Set for child nodes and increment count of childs. To find out number of children.
		}
	}
	lvl -= 1;

	// Exit condition
	if (node.children.size() == 0)
		return;

	// Call subdivide recursively passing a child as the root node
	for (int i = 0; i < childCount; ++i) {
		subdivide(node.children[i], mesh, lvl);
	}
}

//--------------------------------------------------------------
// Code provided in class. isInside() implementation similar to meshBounds()
void ofApp::getMeshPointsInBox(const ofMesh &mesh, const vector<int>& points,
		Box & box, vector<int>& pointsRtn) {

	int count = 0;

	for (int i = 0; i < points.size(); ++i) {
		ofVec3f v = mesh.getVertex(points[i]);
		if (box.inside(Vector3(v.x, v.y, v.z))) {
			count++;
			pointsRtn.push_back(points[i]);
		}
	}
}

//--------------------------------------------------------------
// Selection with the OctTree
bool ofApp::doPointSelection(TreeNode& node, const ofMesh& mesh) {

	// Initialize ray and get current mouse position.
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
			Vector3(rayDir.x, rayDir.y, rayDir.z));

	// If ray intersects with point and only one point is present in node. Exit condition not optimal
	if (node.box.intersect(ray, -100, 100) && node.points.size() == 1
			&& bTerrainSelected) {
		bPointSelected = true;
		selectedPoint = mesh.getVertex(node.points[0]);
	}

	// Iterate through the tree and call recursively for each child.
	for (int i = 0; i < node.children.size(); i++) {
		if (node.children[i].box.intersect(ray, -100, 100)) {
			doPointSelection(node.children[i], mesh);
		}
	}
	return bPointSelected;
}

//--------------------------------------------------------------
//Initial selection based on mesh points.
//
//  Select Target Point on Terrain by comparing distance of mouse to
//  vertice points projected onto screenspace.
//  if a point is selected, return true, else return false;
//

bool ofApp::doPointSelection() {
	ofMesh mesh = mars.getMesh(0);
	int n = mesh.getNumVertices();
	float nearestDistance = 0;
	int nearestIndex = 0;

	bPointSelected = false;

	ofVec2f mouse(mouseX, mouseY);
	vector<ofVec3f> selection;

	// We check through the mesh vertices to see which ones
	// are "close" to the mouse point in screen space.  If we find
	// points that are close, we store them in a vector (dynamic array)
	//
	for (int i = 0; i < n; i++) {
		ofVec3f vert = mesh.getVertex(i);
		ofVec3f posScreen = cam.worldToScreen(vert);
		float distance = posScreen.distance(mouse);
		if (distance < selectionRange) {
			selection.push_back(vert);
			bPointSelected = true;
		}

	}

	//  if we found selected points, we need to determine which
	//  one is closest to the eye (camera). That one is our selected target.
	//
	if (bPointSelected) {
		float distance = 0;
		for (int i = 0; i < selection.size(); i++) {
			ofVec3f point = cam.worldToCamera(selection[i]);
			// In camera space, the camera is at (0,0,0), so distance from
			// the camera is simply the length of the point vector
			//
			float curDist = point.length();

			if (i == 0 || curDist < distance) {
				distance = curDist;
				selectedPoint = selection[i];
			}
		}
	}
	return bPointSelected;
}
//bool ofApp::doPointSelectionOctree() {
//
////	Initialize ray and get current mouse position.
//	ofVec3f mouse(mouseX, mouseY);
//	ofVec3f rayPoint = cam.screenToWorld(mouse);
//	ofVec3f rayDir = rayPoint - cam.getPosition();
//	rayDir.normalize();
//	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
//			Vector3(rayDir.x, rayDir.y, rayDir.z));
//
//	TreeNode node;
//	bPointSelected = octree.intersect(ray, root, node);
//	ofMesh mesh = mars.getMesh(0);
//	selectedPoint = mesh.getVertex(node.points[0]);
//
//
//	return bPointSelected;
//
//}
