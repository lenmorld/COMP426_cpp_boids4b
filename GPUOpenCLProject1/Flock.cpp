#include "Flock.h"



Flock::Flock()
{
}

Flock::Flock(int _id, PVector _location, PVector _velocity, PVector _center, vector<Boid>& _boids)
{
	id = _id;
	location = _location;
	velocity = _velocity;
	center = _center;
	boids = _boids;
}



Flock::~Flock()
{
}
