#pragma once
//#include <vector>
#include "Boid.h"
#include "PVector.h"
#include <vector>
using namespace std;

class Flock
{
public:
	Flock();
	Flock(int _id, PVector _location, PVector _velocity, PVector _center, vector<Boid>& _boids);

	int id;
	PVector center;
	PVector location;
	PVector velocity;
	//int num_boids;
	vector<Boid> boids;

	~Flock();
};

