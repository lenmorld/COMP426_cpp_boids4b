#include "Boid.h"

Boid::Boid()
{
	location = PVector(0.0, 0.0);
	velocity = PVector(0.0, 0.0);
	new_velocity = PVector(0.0, 0.0);
}

Boid::Boid(int _id, PVector _location, PVector _velocity, PVector _new_velocity)
{
	id = _id;
	location = _location;
	velocity = _velocity;
	new_velocity = _new_velocity;
}

Boid::~Boid()
{
}
