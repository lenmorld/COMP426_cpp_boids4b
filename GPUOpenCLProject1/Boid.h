#pragma once
#include "PVector.h"

class Boid
{
public:
	Boid();
	Boid(int id, PVector location, PVector velocity, PVector new_velocity);

	int id;
	PVector location;
	PVector velocity;
	PVector new_velocity;

	~Boid();
};
