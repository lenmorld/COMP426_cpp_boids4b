#pragma once
class PVector
{
public:
	PVector();
	PVector(double x, double y);
	~PVector();

	double x;
	double y;


	PVector operator+(const PVector& p) {
		PVector pv;
		pv.x = this->x + p.x;
		pv.y = this->y + p.y;
		return pv;
	}

	PVector operator-(const PVector& p) {
		PVector pv;
		pv.x = this->x - p.x;
		pv.y = this->y - p.y;
		return pv;
	}

	PVector operator*(float val) {
		PVector pv;
		pv.x = this->x * val;
		pv.y = this->y * val;
		return pv;
	}

	PVector operator/(float val) {
		PVector pv;
		pv.x = this->x / val;
		pv.y = this->y / val;
		return pv;
	}
};

