#ifndef _ACTUATOR_THRUSTER_H
#define _ACTUATOR_THRUSTER_H

#include "../object.h"
#include "actuator.h"
#include "../sensor/proprioceptiveThruster.h"

class THRUSTER : public ACTUATOR {

private:
	int	firstObject;

	double x, y, z;
	double lowStop;
	double highStop;
	double shutoffThreshold;

	double lastDesired;

	double momentumBudget, momentumUsed;

	OBJECT* first;

	PROPRIOCEPTIVE_THRUSTER_SENSOR* proprioceptiveSensor;

public:
	THRUSTER(void) : ACTUATOR(1),
	                 lastDesired(0.0),
	                 momentumBudget(-1.), momentumUsed(0.),
	                 first(NULL), proprioceptiveSensor(NULL) {};

	~THRUSTER(void) {

		if(proprioceptiveSensor)
			delete proprioceptiveSensor;
	};

	void Read_From_Python(void);

	void Actuate(void);
	void Create_In_Simulator(dWorldID world, OBJECT** allObjects, int numObjects, ACTUATOR** allActuators, int numActuators);
	void Draw(void) const;

	bool Create_Proprioceptive_Sensor(int sensorID, int evalPeriod);
	void Poll_Sensors(int currentTimestep);
	void Update_Sensor_Neurons(int t);
	void Write_To_Python(int evalPeriod) const;
	bool Connect_Sensor_To_Sensor_Neuron(int sensorID, int sensorValueIndex, NEURON* sNeuron);
};

#endif // _ACTUATOR_THRUSTER_H
