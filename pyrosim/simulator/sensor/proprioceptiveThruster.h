#ifndef _SENSOR_PROPRIOCEPTIVE_THRUSTER_H
#define _SENSOR_PROPRIOCEPTIVE_THRUSTER_H

#include <ode/ode.h>
#include "sensor.h"
#include "../object.h"

class PROPRIOCEPTIVE_THRUSTER_SENSOR : public SENSOR {

public:
	PROPRIOCEPTIVE_THRUSTER_SENSOR(int myID, int evalPeriod) : SENSOR(myID, evalPeriod, 1) {};
	void Poll(double fuelGauge, int t) {
		values[0][t] = fuelGauge;
	};
};

#endif // _SENSOR_PROPRIOCEPTIVE_THRUSTER_H
