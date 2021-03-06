#ifndef _ACTUATOR_ACTUATOR_H
#define _ACTUATOR_ACTUATOR_H

#include "../neuron.h"
#include "../object.h"

class ACTUATOR {

protected:
	/********* WARNING: DO NOT REDECLARE OR REINITIALIZE THOSE VARIABLES IN DERIVATIVE CLASSES *********/
	int ID;
	NEURON* motorNeuron;

public:
	ACTUATOR(void) : ID(-1), motorNeuron(NULL) {};

	virtual void Read_From_Python(void) = 0;
	virtual void Write_To_Python(int evalPeriod) const {};

	virtual void Create_In_Simulator(dWorldID world, OBJECT** allObjects, int numObjects) = 0;
	virtual void Actuate(void) = 0;
	virtual void Draw(void) const = 0;

	virtual bool Connect_Sensor_To_Sensor_Neuron(int sensorID, NEURON *sNeuron) {return false;};
	bool Connect_To_Motor_Neuron(int actuatorID, NEURON *mNeuron)
	{
		if (actuatorID == ID)
		{
			motorNeuron = mNeuron;
			return true;
		}
		else
			return false;
	};

	virtual void Poll_Sensors(int currentTimestep) {};
	virtual void Update_Sensor_Neurons(int t) {};
	virtual bool Create_Proprioceptive_Sensor(int sensorID, int evalPeriod) {return false;};
};

#endif // _ACTUATOR_ACTUATOR_H
