#ifndef _BOX_H_
#define _BOX_H_

#include <assert.h>
#include "vector3.h"
#include "ray.h"

/*
 * Axis-aligned bounding box class, for use with the optimized ray-box
 * intersection test described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

class Box {
  public:
    Box() { }
    Box(const Vector3 &min, const Vector3 &max) {
      assert(min < max);
      parameters[0] = min;
      parameters[1] = max;
    }
    // (t0, t1) is the interval for valid hits
    bool intersect(const Ray &, float t0, float t1) const;

    // corners
    Vector3 parameters[2];
	Vector3 min() { return parameters[0]; }
	Vector3 max() { return parameters[1]; }

	bool inside(const Vector3 point){
		Vector3 min = this->min();
		Vector3 max = this->max();

//		cout << "Min length:" << min.length() << endl;
//		cout << "Min:" << min.x() << "," << min.y() << "," << min.z()<< endl;
//		cout << "Min:" << max.x() << "," << max.y() << "," << max.z()<< endl;

		if(point.x() < max.x() && point.y() < max.y() && point.z() < max.z() &&
		   point.x() > min.x() && point.y() > min.y() && point.z() > min.z()){
			return true;
		}

	 return false;
	}

	Vector3 center(){
		float x,y,z;
		x = (this->max().x() + this->min().x())/2;
		y = (this->max().y() + this->min().y())/2;
		z = (this->max().z() + this->min().z())/2;
		Vector3 center(x,y,z);
		return center;
	}
};

#endif // _BOX_H_
