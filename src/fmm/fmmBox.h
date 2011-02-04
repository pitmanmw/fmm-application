#ifndef FMMBOX_H_
#define FMMBOX_H_

#include "mesh.h"
#include "primitives.h"

#include <vector>
#include <complex>

#include <iostream>
#include <stdlib.h>
using namespace std;

class fmmBox2d
{
private:
	void initPointers();
	
public:
	fmmBox2d* parent;
	fmmBox2d* children[4];
	pnt2d center;
	double length;
	int level;
	
	// FMM Storage
	complex<double> * ak;
	
	// Vector of elements and info stored in this box
	vector<const pnt2d*> targets;
	vector<const meshElement*> elements;
	
	// Methods for getting the child boxes of points, edges and faces
	void assignToChildren(const meshElement* element);
	int getChildIndex(const double* co) const;
	int getChildIndex(const double* co1, const double* co2, int* childIndices) const;
	
	// Methods for adding children, creating and outputting the tree
	void addChild(const unsigned i);
	void split( vector<fmmBox2d*>& leafs );
	void writeToMesh(mesh2d& mesh, bool children);

	// Recursive call to get smallest box in which a point resides
	int lineIntersectionPoints( const meshElement* element, double pints[] );
	bool pointInBox( double co[] );
	fmmBox2d* getPointBox(const double* co);
	
	// FMM Methods
	void expandMultipole( int& p );
	
	// Constructors
	fmmBox2d() { initPointers(); center.co[0] = 0.0; center.co[1] = 0.0; length=0.0; level=0; }
	fmmBox2d( double a, double b, double c) { initPointers(); center.co[0]=a; center.co[1]=b; length=c; level=0; }
	// Destructors
	~fmmBox2d() {};
};

#endif
