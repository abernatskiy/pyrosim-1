#ifndef _CURRENT_CONTROLLER_SENSOR
#define _CURRENT_CONTROLLER_SENSOR

#include "sensor.h"

class CURRENT_CONTROLLER_SENSOR : public SENSOR {

private:
	double* controllerHistory;

public:
	CURRENT_CONTROLLER_SENSOR(int myID, int evalPeriod) : SENSOR(myID, evalPeriod, 1) {

		controllerHistory = values[0];
		for(int tt=0; tt<evalPeriod; tt++)
			controllerHistory[tt] = -1.;
	};

	void Set(int currentControllerID, int timeStep) {
		controllerHistory[timeStep] = (double) currentControllerID;
	};
};

#endif // _CURRENT_CONTROLLER_SENSOR

