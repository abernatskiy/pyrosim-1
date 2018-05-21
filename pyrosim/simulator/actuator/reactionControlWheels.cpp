#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ode/ode.h>

#include "reactionControlWheels.h"
#include "datastruct.h"

extern Data* data;

void REACTION_CONTROL_WHEELS::Read_From_Python(void) {

	std::cin >> ID;

	std::cin >> firstObjectID;
	std::cin >> mb[0];
	std::cin >> mb[1];
	std::cin >> mb[2];
	std::cin >> maxTorque;
}

void REACTION_CONTROL_WHEELS::Actuate(void) {

	if ( motorNeurons[0] == NULL &&
	     motorNeurons[1] == NULL &&
	     motorNeurons[2] == NULL )
		return;

	dVector3 torque;
	double newAccumulatedMomentum = 0.;
	for(unsigned i=0; i<3; i++) {
		torque[i] = motorNeurons[i] != NULL ?
		            (motorNeurons[i]->Get_Value())*maxTorque :
		            0.;
//		double oldTorque = torque[i];
		newAccumulatedMomentum = cm[i] + (data->dt)*torque[i];
		if(newAccumulatedMomentum > mb[i] || newAccumulatedMomentum < -mb[i])
			torque[i] = 0.;
		else
			cm[i] = newAccumulatedMomentum;
//		if(i==0) std::cerr << "Applied torque: " << torque[i] << " torque requested: " << oldTorque << " cm: " << cm[i] << " mb: " << mb[i] << std::endl;
	}

	dBodyAddRelTorque(firstObject->Get_Body(), torque[0], torque[1], torque[2]);
}

void REACTION_CONTROL_WHEELS::Create_In_Simulator(dWorldID world, OBJECT** allObjects, int numObjects, ACTUATOR** allActuators, int numActuators) {

	if ( firstObjectID >= 0 )
		firstObject = allObjects[firstObjectID];
	else {
		std::cerr << "Cannot attach a reaction control wheel to an object with id " << firstObjectID << "\n";
		exit(EXIT_FAILURE);
	}
//	std::cerr << "RCW created in simulator. Object id: " << firstObjectID << " pointer: " << firstObject << "\n";
}

bool REACTION_CONTROL_WHEELS::Create_Proprioceptive_Sensor(int sensorID, int evalPeriod) {

	proprioceptiveSensor = new PROPRIOCEPTIVE_REACTION_CONTROL_WHEELS_SENSOR(sensorID, evalPeriod);
	return true;
}

void REACTION_CONTROL_WHEELS::Poll_Sensors(int currentTimestep) {

	if(proprioceptiveSensor)
		proprioceptiveSensor->Poll(cm[0]/mb[0], cm[1]/mb[1], cm[2]/mb[2], currentTimestep);
}

void REACTION_CONTROL_WHEELS::Update_Sensor_Neurons(int t) {

	if(proprioceptiveSensor)
		proprioceptiveSensor->Update_Sensor_Neurons(t);
}

void REACTION_CONTROL_WHEELS::Write_To_Python(int evalPeriod) const {

	if(proprioceptiveSensor)
		proprioceptiveSensor->Write_To_Python(evalPeriod);
}

bool REACTION_CONTROL_WHEELS::Connect_Sensor_To_Sensor_Neuron(int sensorID, int sensorValueIndex, NEURON* sNeuron) {

	if(proprioceptiveSensor && proprioceptiveSensor->Get_ID() == sensorID) {
		proprioceptiveSensor->Connect_To_Sensor_Neuron(sNeuron, sensorValueIndex);
		return true;
	}
	return false;
}
