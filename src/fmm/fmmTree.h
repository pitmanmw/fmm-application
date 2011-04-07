#ifndef FMMTREE_H_
#define FMMTREE_H_

#include "fmmBox.h"
#include "primitives.h"
#include "mesh.h"
#include <list>
#include <vector>

#include <iostream>
#include <stdlib.h>
using namespace std;

class fmmTree2d
{
public:
	fmmBox2d topBox;
	vector<fmmBox2d*> leafBoxes;
	int p;	// Number of polynomial coefficiency in the multipole expansion
	
	void resizeTopBox(pnt2d& minPoint, pnt2d& maxPoint);
	
	void multipoleExpansion();
	void upwardPass();
	void downwardPass();
	void nearInteractions();
	
	void writeVTK();
	
	// Constructors
	fmmTree2d() {};
	fmmTree2d(const mesh2d& mesh, int nCoeffs);
	// Destructors
	~fmmTree2d() {};
};

#endif
