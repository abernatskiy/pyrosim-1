#ifndef _SENSOR_PROPRIOCEPTIVE_REACTION_CONTROL_WHEELS_H
#define _SENSOR_PROPRIOCEPTIVE_REACTION_CONTROL_WHEELS_H

#include <ode/ode.h>
#include "sensor.h"
#include "../object.h"

class PROPRIOCEPTIVE_REACTION_CONTROL_WHEELS_SENSOR : public SENSOR {

public:
	PROPRIOCEPTIVE_REACTION_CONTROL_WHEELS_SENSOR(int myID, int evalPeriod) : SENSOR(myID, evalPeriod, 3) {};
	void Poll(double mbxGauge, double mbyGauge, double mbzGauge, int t) {
		values[0][t] = mbxGauge;
		values[1][t] = mbyGauge;
		values[2][t] = mbzGauge;
	};
};

#endif // _SENSOR_PROPRIOCEPTIVE_REACTION_CONTROL_WHEELS_H
