#ifndef _SENSOR_LIGHT_CPP
#define _SENSOR_LIGHT_CPP

#include <iostream>
#include <cmath>
#include "light.h"

LIGHT_SENSOR::LIGHT_SENSOR(int myID, int evalPeriod) : SENSOR(myID, evalPeriod),
                                                       sensitivityKind(0),
                                                       logarithmic(false),
                                                       initialLuminousity(1.),
                                                       initialLuminousityMeasured(false) {};

void LIGHT_SENSOR::Read_From_Python(void) {

	std::cin >> sensitivityKind;
	std::cin >> logarithmic;
}

void LIGHT_SENSOR::Poll(dReal luminousity, int t) {

	if(!initialLuminousityMeasured) {
		initialLuminousity = luminousity;
		initialLuminousityMeasured = true;
	}

	values[0][t] = logarithmic ? log(luminousity/initialLuminousity) + 1.0 : luminousity;
}

#endif // _SENSOR_LIGHT_CPP
