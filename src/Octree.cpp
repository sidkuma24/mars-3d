//
//  Kevin M. Smith - Basic Octree Class - CS134/CS 235 4/18/18
//
//   (c) Kevin M. Smith - 2018  - Not for Redistribution without Permission
//

#include "Octree.h"


// draw Octree (recursively)
//
void Octree::draw(TreeNode & node, int numLevels, int level) {
	if (level >= numLevels) return;
	drawBox(node.box);
	level++;
	for (int i = 0; i < node.children.size(); i++) {
		draw(node.children[i], numLevels, level);
	}
}


//draw a box from a "Box" class  
//
void Octree::drawBox(const Box &box) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
	float w = size.x();
	float h = size.y();
	float d = size.z();
	ofDrawBox(p, w, h, d);
}

// return a Mesh Bounding Box for the entire Mesh
//
Box Octree::meshBounds(const ofMesh & mesh) {
	int n = mesh.getNumVertices();
	ofVec3f v = mesh.getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	for (int i = 1; i < n; i++) {
		ofVec3f v = mesh.getVertex(i);

		if (v.x > max.x) max.x = v.x;
		else if (v.x < min.x) min.x = v.x;

		if (v.y > max.y) max.y = v.y;
		else if (v.y < min.y) min.y = v.y;

		if (v.z > max.z) max.z = v.z;
		else if (v.z < min.z) min.z = v.z;
	}
	cout << "vertices: " << n << endl;
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

// getMeshPointsInBox:  return an array of indices to points in mesh that are contained 
//                      inside the Box.  Return count of points found;
//
int Octree::getMeshPointsInBox(const vector<int>& points, Box & box, vector<int> & pointsRtn)
{
	int count = 0;
	for (int i = 0; i < points.size(); i++) {
		ofVec3f v = mesh.getVertex(points[i]);
		if (box.inside(Vector3(v.x, v.y, v.z))) {
			count++;
			pointsRtn.push_back(points[i]);
		}
	}
	return count;
}

//  Subdivide a Box into eight(8) equal size boxes, return them in boxList;
//
void Octree::subDivideBox8(const Box &box, vector<Box> & boxList) {
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
	b[1] = Box(b[0].min() + Vector3(xdist, 0, 0), b[0].max() + Vector3(xdist, 0, 0));
	b[2] = Box(b[1].min() + Vector3(0, 0, zdist), b[1].max() + Vector3(0, 0, zdist));
	b[3] = Box(b[2].min() + Vector3(-xdist, 0, 0), b[2].max() + Vector3(-xdist, 0, 0));

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

//  create the Octree "numLevels" deep from the mesh. Will return when
//  "numLevels" have been created or when all leave nodes contain one 
//  vertex, whichever comes first.
//
void Octree::create(const ofMesh & mesh, int numLevels) {
	// initialize octree structure
	//
	this->mesh = mesh;

    // initialize the firt root node (level 0)
	// it contains all vertex indices from the mesh
	// 
	int level = 0;
	root.box = meshBounds(mesh);
	for (int i = 0; i < mesh.getNumVertices(); i++) {
		root.points.push_back(i);
	}

	float t1 = ofGetElapsedTimeMillis();

	// recursively buid octree (starting at level 1)
	//
	level++;
	subdivide(root, numLevels, level);
	float t2 = ofGetElapsedTimeMillis();
	cout << "Time to Build Octree: " << t2 - t1 << " milliseconds" << endl;

}

//  primary recursive function to build octree.
//  algorithm:
//  1) return when the level is numLevels or greater
//  2) subdivide the current node box into 8 boxes
//  3) for each child box:
//      (a) distribute points in parent node to child node
//      (b) if there are points in child node add it to parent's children
//      (c) if a child has more than one point
//            recursively call subdivide on child
//
void Octree::subdivide(TreeNode & node, int numLevels, int level) {
	
}

//  primary recursive function to test for ray intersection, returns
//  first child leaf node intersected. returns true if a leaf node
//  with one point was found.
//  algorithm:
//  1) does ray intersect node's box ?
//         if yes and node contains, only one point, we are done done; return node and true
//         if yes and node contains more than one point, recursively call intersect()
//         on each child;
//     if no return false;
//
//
bool Octree::intersect(const Ray &ray, const TreeNode & node, TreeNode & nodeRtn) {

}



