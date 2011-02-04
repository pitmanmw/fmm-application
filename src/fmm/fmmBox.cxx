#include "fmm.h"

//##########################

void fmmBox2d::initPointers()
{
	// Initialize child pointers to NULL
	for (int i=0;i<4;i++) { children[i] = NULL; }
	// Initialize parent pointer to NULL
	parent = NULL;
};

void fmmBox2d::split( vector<fmmBox2d*>& leafs )
{
	int i, N;
	int childIndices[4];
	
	// Add the children to this parent
	for (i=0; i<4;i++) { addChild(i); }
	
	// Get the children that each element touches
	for (i=elements.size()-1; i>-1; i--)
	{
		assignToChildren(elements[i]);
		elements.pop_back();		// Delete this element from the parent
	}

	// Look through the children and if any of them meet the criteria
	// then split them too (recursive call)
	// OR: if they don't have any elements then point them to NULL
	for (i=0; i<4; i++)
	{
		if ((children[i]->elements.size() > 15) and ( children[i]->level < 50 ))
		{
			children[i]->split( leafs );
		}
		else if (children[i]->elements.size() == 0) 
		{
			delete children[i]; 
			children[i]=NULL; 
		}
		else
		{
			leafs.push_back( children[i] );		// This child box is a leaf
		}
	}
};

void fmmBox2d::writeToMesh(mesh2d& mesh, bool children=true)
{
	int i,j,c,s;
	double x, y;
	int pIndex[4];
	
	mesh.points.reserve( mesh.points.size() + 4 );
	s = mesh.points.size();
	c=0;
	// Write this box
	for (j=-1; j<2; j+=2)
	{
		for (i=-1; i<2; i+=2)
		{
			x = this->center.co[0] + ((double)i * this->length)/2.0;
			y = this->center.co[1] + ((double)j * this->length)/2.0;
			mesh.points.push_back( pnt2d(x, y) );
			pIndex[c]=s+c; c++;
		}
	}
	mesh.elements.push_back( meshElement( 4, &pIndex[0], mesh.points) );
	
	// Call to write children boxes
	if (children == true)
	{ 
		for (i=0;i<4;i++)
		{
			if (this->children[i] != NULL)
			{
				this->children[i]->writeToMesh(mesh, true);
			}
		}
	}
};

void fmmBox2d::assignToChildren(const meshElement* element)
{
	int nPoints = element->points.size();
	int nChilds = 0;
	int childIndices[4] = {-1};
		
	if ( nPoints == 1)
	{
		childIndices[0] = this->getChildIndex( element->points[0]->co );
		nChilds = 1;
	}
	else if (nPoints == 2)
	{
		nChilds = this->getChildIndex( 
			element->points[0]->co, element->points[1]->co, childIndices);
	}
	else
	{
		cout << "WARNING: Found an element with " << nPoints << " points while splitting box" << endl;
	}
	
	// Copy this element to the children
	for (int i=0; i<nChilds; i++)
	{
		this->children[childIndices[i]]->elements.push_back( element );
	}
};

//------------------
// POINTS
int fmmBox2d::getChildIndex(const double* co) const
{
	int index = 0;
	if (co[1] - center.co[1] > 0.0) {index=2;}
	if (co[0] - center.co[0] > 0.0) {index+=1;}
	return index;
};

// EDGES
int fmmBox2d::getChildIndex(const double* co1, const double* co2, int* childIndices) const
{
	// This assumes that the edge fits correctly within the topmost (level 0) box
	double xi, yi, X, Y;
	
	// Get the child boxes that each end-point lie within
	int i1 = this->getChildIndex(co1);
	int i2 = this->getChildIndex(co2);
	
	// Both points are in the same box
	if (i1 == i2)
		{ childIndices[0] = i1; return 1; }
		
	// Both points are in separate boxed above or below x-axis of box
	else if ((i1 >= 2) == (i2 >= 2))
		{ childIndices[0] = i1; childIndices[1] = i2; return 2; }
	// Both points are in separate boxed above or below y-axis of box
	else if ((i1==0 or i1==2) == (i2==0 or i2==2)) 
		{ childIndices[0] = i1; childIndices[1] = i2; return 2; }
	
	// The edge might pass through a few children, get the x and y axis intersections
	X = co2[0] - co1[0]; Y = co2[1] - co1[1];
	yi = ((center.co[0] - co1[0])/X)*Y - (center.co[1]-co1[1]);
	xi = ((center.co[1] - co1[1])/Y)*X - (center.co[0]-co1[0]);
	
	// Get the primary quadrant that this edge is within (0 ,1 ,2, 3)
	// Add this quadrant box to the list of children that edge passes through 
	// (must at least pass through this box)
	int Qx=0; int Qy=0;
	if (xi>0.0) {Qx=1;}
	if (yi>0.0) {Qy=1;}
	int N = 0; childIndices[N] = Qx+(Qy*2);
	
	// If the x/y intercept is within the box then add the quadrant 
	// above or below to the children that the edge passes through
	double L = length/2.0;
	if (fabs(xi) < L) { N++; childIndices[N] = childIndices[0] + 2 - Qy*4; }
	if (fabs(yi) < L) { N++; childIndices[N] = childIndices[0] + 1 - Qx*2; }
	N++;
	
	return N;
};

//----------------------

void fmmBox2d::expandMultipole( int& p )
{
	int nPoints = 0;
	int nElements = elements.size();
	double pints[2] = {0.0, 1.0};
	
	ak = new complex<double>[p];
	
	for (int i=0; i<nElements; i++)
	{
		nPoints = elements[i]->points.size();
		if (nPoints == 1)
		{
			elements[i]->expandMultipole( this->center.co, ak, p );
		}
		else if (nPoints == 2)
		{
			// Get limits of line-integral (line-box intersection points)
			lineIntersectionPoints( elements[i], pints );
			cout << "---- " << pints[0] << endl;
			
		}
		else
		{
			cout << "Higher order elements not supported in 2d" << endl;
		}
	}
};

void fmmBox2d::lineIntersectionPoints( const meshElement* element, double pints[] )
{	
	int nInts = 2;
	pints[0] = 0.0;
	pints[1] = 1.0;
	cout << "Working out Box-line intersection points" << endl;
	// If co[0] is inside of box then pint[0] = 0.0
	if (pointInBox( element->points[0]->co )) { nInts--; }
	if (pointInBox( element->points[1]->co )) { nInts--; }
	
	if (nInts == 0) { return; }
	else
	{
		double eLength = element->points[0]->distToPoint( element->points[1] );
		double p = 0.0;
		double D = 0.0;
		double boxEdge = 0.0;
		// Move get box intersection on vertical (i=0) and horizontal (i=1) sides of box
		for (int i=0;i<2;i++)
		{
			for (int x=-1;x<2;x+=2)
			{
				boxEdge = center.co[i] + x*length/2.0
				D = (element->points[1]->co[i] - element->points[0]->co[i]);
				p = (boxEdge - element->points[0]->co[i])/D;
			}
		}
	}
};

bool fmmBox2d::pointInBox( double co[] )
{
	for (int i=0;i<2;i++)
	{
		if ( abs(co[i]-center.co[i]) <= (length/2.0) ) { return false; }
	}
	return true;
};

//------------------

void fmmBox2d::addChild(const unsigned i)
{
	// Get binary 0 or 1 index of child position from integer index
	int x = (i&(1<<0))>>0;
	int y = (i&(1<<1))>>1;
	
	// Add the child at this index position
	children[i] = new fmmBox2d();
	children[i]->center.co[0] = center.co[0] + ((double)x-0.5)*length/2.0;
	children[i]->center.co[1] = center.co[1] + ((double)y-0.5)*length/2.0;
	children[i]->length = length/2.0;
	children[i]->level = this->level + 1;
	children[i]->parent = this;
};

fmmBox2d* fmmBox2d::getPointBox(const double* co)
{
	int child = getChildIndex(co);
	if (children[child] != NULL)
		return children[child]->getPointBox(co);
	return this;
};
