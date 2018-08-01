#ifndef _SENSOR_PROPRIOCEPTIVE_TETHER_H
#define _SENSOR_PROPRIOCEPTIVE_TETHER_H

#include <ode/ode.h>
#include "sensor.h"
#include "../object.h"

class PROPRIOCEPTIVE_TETHER_SENSOR : public SENSOR {

public:
	PROPRIOCEPTIVE_TETHER_SENSOR(int myID, int evalPeriod) : SENSOR(myID, evalPeriod, 2) {};
	void Poll(double tension, int t) {
		values[0][t] = tension;
		values[1][t] = tension;
	};

	 void Connect_To_Sensor_Neuron(NEURON* sensorNeuron, int sensorValueIndex = 0) {
//		std::cerr << "Connecting neuron " << sensorNeuron->Get_ID() << " to tension sensor " << ID << " (index " << sensorValueIndex << ")\n";
		if(sensorValueIndex < numChannels)
			mySensorNeurons[sensorValueIndex] = sensorNeuron;
		else {
			std::cerr << "Cannot connect sensor neuron to sensor at value index " << sensorValueIndex << ": there are only " << numChannels << " channels\n";
			exit(EXIT_FAILURE);
		}
	};
};

#endif // _SENSOR_PROPRIOCEPTIVE_TETHER_H
