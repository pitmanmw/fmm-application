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
	int p;	    // Number of polynomial coefficients in the multipole expansion
    int epb;    // Max elements per box
    int maxl;   // Maximum number of FMM levels
	
	void resizeTopBox(pnt2d& minPoint, pnt2d& maxPoint);
	
	// Methods for generating and accessing cached coefficients for the FMM
	double **binomialCoefficients;
	void cacheCoeffs();
	int binaryCoeff(int n, int k);
	
	// FMM operations
	void multipoleExpansion();
	void upwardPass();
	void downwardPass();
	void nearInteractions();
	
	void writeVTK();
	
	// Constructors
	fmmTree2d() {};
	fmmTree2d( mesh2d& mesh, int nCoeffs=13, int maxEPB=15, int maxLevel=50);
	// Destructors
	~fmmTree2d();
};

#endif
