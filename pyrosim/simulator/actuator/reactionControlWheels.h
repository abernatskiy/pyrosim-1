#ifndef _ACTUATOR_REACTION_CONTROL_WHEELS_H
#define _ACTUATOR_REACTION_CONTROL_WHEELS_H

#include "../object.h"
#include "actuator.h"
#include "../sensor/proprioceptiveReactionControlWheels.h"

class REACTION_CONTROL_WHEELS : public ACTUATOR {

private:
	int	firstObjectID;
	dVector3 mb, cm; // momentum budget & current accumulated momentum
	double maxTorque;
	OBJECT* firstObject;
	PROPRIOCEPTIVE_REACTION_CONTROL_WHEELS_SENSOR* proprioceptiveSensor;

public:
	REACTION_CONTROL_WHEELS (void) : ACTUATOR(3),
	                 mb{1.,1.,1.}, cm{0.,0.,0.},
	                 maxTorque(1.),
	                 firstObject(NULL), proprioceptiveSensor(NULL) {};

	void Read_From_Python(void);

	void Actuate(void);
	void Create_In_Simulator(dWorldID world, OBJECT** allObjects, int numObjects, ACTUATOR** allActuators, int numActuators);
	void Draw(void) const {};

	bool Create_Proprioceptive_Sensor(int sensorID, int evalPeriod);
	void Poll_Sensors(int currentTimestep);
	void Update_Sensor_Neurons(int t);
	void Write_To_Python(int evalPeriod) const;
	bool Connect_Sensor_To_Sensor_Neuron(int sensorID, int sensorValueIndex, NEURON* sNeuron);
};

#endif // _ACTUATOR_REACTION_CONTROL_WHEELS_H
