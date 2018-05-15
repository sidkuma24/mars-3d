//
//  Kevin M. Smith - Basic Octree Class - CS134/CS235 4/18/18
//
//  (c) Kevin M. Smith - 2018 -  Not for Redistribution without Permsission
//
//  
//   Example use:
//
//   Octree octree;
//   octree.create(mesh, 20);
//
//   For rendering;
//
//   int level;
//   octree.draw(5, level);
//
//   For selection;
//   
//   TreeNode node;
//   bool hit = octree.intersect(ray, octree.root, node);
//
//
#pragma once
#include "ofMain.h"
#include "box.h"
#include "ray.h"
#include <vector>




class TreeNode {
public:
	Box box;
	vector<int> points;
	vector<TreeNode> children;
};

class Octree {
public:

	void create(const ofMesh & mesh, int numLevels);
	void subdivide(TreeNode & node, int numLevels, int level);
	bool intersect(const Ray &, const TreeNode & node, TreeNode & nodeRtn);
	void draw(TreeNode & node, int numLevels, int level);
	void draw(int numLevels, int level) {
		draw(root, numLevels, level);
	}
	void drawBox(const Box &box);
	Box meshBounds(const ofMesh &);
	int getMeshPointsInBox(const vector<int> & points, Box & box, vector<int> & pointsRtn);
	void subDivideBox8(const Box &b, vector<Box> & boxList);

	TreeNode root;
	ofMesh mesh;
};
