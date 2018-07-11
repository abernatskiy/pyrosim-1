#ifndef _SENSOR_GLOBAL_COLLISION_H
#define _SENSOR_GLOBAL_COLLISION_H

#include <ode/ode.h>
#include "sensor.h"

class OBJECT;

class GLOBAL_COLLISION_SENSOR : public SENSOR {

private:
	double* mcs; // current maximum collision speed

	double scale;
	bool isLogarithmic;

public:
	GLOBAL_COLLISION_SENSOR(double sc, bool islog, int myID, int evalPeriod) : SENSOR(myID, evalPeriod, 1),
	                                                                scale(sc),
	                                                                isLogarithmic(islog) {
		mcs = values[0];
		for(int tt=0; tt<evalPeriod; tt++)
			mcs[tt] = 0.;
	};

	void Set(OBJECT* fco, OBJECT* sco, int tt); // first, second colliding objects and time

private:
	void Update_Last_Reading(double relativeVelosity, int tt);
	double Get_Sensor_Reading(double relativeVelocity);
};

#endif // _SENSOR_GLOBAL_COLLISION_H