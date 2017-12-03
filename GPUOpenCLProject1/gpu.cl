__kernel void compute_rules(__global const float *X, __global const float *Y, __global const float *X_V, __global const float *Y_V,
					__global float *XR, __global float *YR, __global float *X_V_R, __global float *Y_V_R, 
					__global const int *INT_ARGS, __global const float *FLOAT_ARGS) {

    // Get the index of the current element to be processed
    int id = get_global_id(0);

	int COUNT = INT_ARGS[0], BOID_PER_FLOCK = INT_ARGS[1], FLOCKS = INT_ARGS[2], STEP_TO_CENTER_DIVISOR = INT_ARGS[3];
	float SCREEN_COHESION_RADIUS = FLOAT_ARGS[0], DESIRED_DISTANCE = FLOAT_ARGS[1], STEP_VELOCITY = FLOAT_ARGS[2], NEIGHBOR_RADIUS = FLOAT_ARGS[3];

	//XR[i] = X[i] + 0.01;
	//YR[i] = Y[i] + 0.01;

		float boid_x = X[id];
		float boid_y = Y[id];
		float boid_X_V = X_V[id];
		float boid_Y_V = Y_V[id];

		// TESTING
		//x[id] += 0.001;
		//y[id] += 0.001;

		// get boids within the same flock			// e.g. 139 (flock 6, boid 19)  last boid
		int flock_id = id / BOID_PER_FLOCK;				// 139 / 20 = 6
		int boid_id_in_flock = id % BOID_PER_FLOCK;		// 139 mod 20 = 19

		int flock_range_start = flock_id * BOID_PER_FLOCK;		// flock 6 * 20 -> range [120 -> 139]
		int flock_range_end = flock_range_start + BOID_PER_FLOCK;

		// iterate through flock's boids
		//for (int i = flock_range_start; i < flock_range_end; i++) {}

		// rule 1 ====================================================

		float sum_x = 0.0, sum_y = 0.0;

		// iterate through flock's boids e.g 0->19
		for (int i = 0; i < BOID_PER_FLOCK; i++) {
			if (boid_id_in_flock != i) {					// skip this boid in determining center	

				// boid_id -> actual id e.g. 0 of flock 5 -> boid 100
				int index = (flock_id * BOID_PER_FLOCK) + i;

				// ### NEARSIGHTED: if distance of this boid to our boid is greater than radius, don't consider this ####
				float distanceNeighbor = sqrt(pow((X[index] - boid_x), 2) + pow((Y[index] - boid_y), 2));
				if (distanceNeighbor > NEIGHBOR_RADIUS) {
					continue;
				}
				// ############################ END NEARSIGHTEDNESS ######################################################

				sum_x += X[index];
				sum_y += Y[index];
				//sum = sum + boids[i].location;

			}
		}

		// distance of centerFlock from this boid
		float center_flock_x = sum_x / (BOID_PER_FLOCK - 1);
		float center_flock_y = sum_y / (BOID_PER_FLOCK - 1);
		//PVector centerFlock(sum.x / (BOID_PER_FLOCK - 1), sum.y / (BOID_PER_FLOCK - 1));

		// step at a time to get to center
		float r1_step_x = (center_flock_x - boid_x) / STEP_TO_CENTER_DIVISOR;
		float r1_step_y = (center_flock_y - boid_y) / STEP_TO_CENTER_DIVISOR;
		//PVector r1_step = (centerFlock - boids[b.id].location) / 100;

		// rule 2 ========================================================
		float distance_x = 0.0, distance_y = 0.0;
		float desired_distance = DESIRED_DISTANCE;

		for (int i = 0; i < BOID_PER_FLOCK; i++) {
			if (boid_id_in_flock != i) {	// skip this boid in determining distance

				// boid_id -> actual id e.g. 0 of flock 5 -> boid 100
				int index = (flock_id * BOID_PER_FLOCK) + i;

				// ### NEARSIGHTED: if distance of this boid to our boid is greater than radius, don't consider this ####
				float distanceNeighbor = sqrt(pow((X[index] - boid_x), 2) + pow((Y[index] - boid_y), 2));
				if (distanceNeighbor > NEIGHBOR_RADIUS) {
					continue;
				}
				// ############################ END NEARSIGHTEDNESS ######################################################

				float distance2vectors = sqrt(pow((X[index] - boid_x), 2) + pow((Y[index] - boid_y), 2));
				if (distance2vectors < desired_distance) {
					distance_x = distance_x - (X[index] - boid_x);
					distance_y = distance_y - (Y[index] - boid_y);
				}
				//if (distance2Vectors(boids[i].location, boids[b.id].location) < desired_distance) {
				//	distance = distance - (boids[i].location - boids[b.id].location);
				//}

			}
		}

		float r2_step_x = distance_x;
		float r2_step_y = distance_y;

		// rule 3 ============================================
		float velocity_x = 0.0, velocity_y = 0.0;

		// get magnitude of the difference
		for (int i = 0; i < BOID_PER_FLOCK; i++) {
			if (boid_id_in_flock != i) {					// skip this boid in determining center	

				// boid_id -> actual id e.g. 0 of flock 5 -> boid 100
				int index = (flock_id * BOID_PER_FLOCK) + i;

				// ### NEARSIGHTED: if distance of this boid to our boid is greater than radius, don't consider this ####
				float distanceNeighbor = sqrt(pow((X[index] - boid_x), 2) + pow((Y[index] - boid_y), 2));
				if (distanceNeighbor > NEIGHBOR_RADIUS) {
					continue;
				}
				// ############################ END NEARSIGHTEDNESS ######################################################

				velocity_x += X_V[index];
				velocity_y += Y_V[index];
			}
		}

		velocity_x = velocity_x / (BOID_PER_FLOCK - 1);
		velocity_y = velocity_y / (BOID_PER_FLOCK - 1);

		float r3_step_x = (velocity_x - boid_X_V) / STEP_VELOCITY;
		float r3_step_y = (velocity_y - boid_Y_V) / STEP_VELOCITY;

		// rule 4 ==============================
		float scale = SCREEN_COHESION_RADIUS;
		float Xmin = -scale, Xmax = scale, Ymin = -scale, Ymax = scale;
		float step = 0.05;

		float vvX_V = 0.0, vvY_V = 0.0;

		if (boid_x < Xmin) {
			vvX_V = step;
		}

		if (boid_x > Xmax) {
			vvX_V = -step;
		}

		if (boid_y < Ymin) {
			vvY_V = step;
		}

		if (boid_y > Ymax) {
			vvY_V = -step;
		}

		float r4_step_x = vvX_V;
		float r4_step_y = vvY_V;


		// location and velocity read only
		// only update new velocity
		//vX_V[id] += (r1_step_x + r2_step_x + r3_step_x + r4_step_x);
		//vY_V[id] += (r1_step_y + r2_step_y + r3_step_y + r4_step_y);

		X_V_R[id] = (r1_step_x + r2_step_x + r3_step_x + r4_step_x);
		Y_V_R[id] = (r1_step_y + r2_step_y + r3_step_y + r4_step_y);

		//X_V_R[id] = 0.01;
		//Y_V_R[id] = 0.01;

	//XR[i] = X[i] + 0.01;
	//YR[i] = Y[i] + 0.01;


}