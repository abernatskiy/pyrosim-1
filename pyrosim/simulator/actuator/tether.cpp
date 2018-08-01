#ifndef _ACTUATOR_TETHER_CPP
#define _ACTUATOR_TETHER_CPP

#include <drawstuff/drawstuff.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "tether.h"

#ifdef dDOUBLE
#define dsDrawLine dsDrawLineD
#endif

void TETHER::Read_From_Python(void) {

	std::cin >> ID;

	std::cin >> firstObject;
	std::cin >> secondObject;
	std::cin >> forceConstant;
	std::cin >> dampeningCoefficient;
}

void TETHER::Create_In_Simulator(dWorldID world, OBJECT** allObjects, int numObjects, ACTUATOR** allActuators, int numActuators) {

	if ( firstObject >= 0  && secondObject >=0 ) {
		first = allObjects[firstObject];
		second = allObjects[secondObject];
	}
	else {
		std::cerr << "Failed to create tether in simulator!\n";
		std::cerr << "Tether was intended to connect objects " << firstObject << " and " << secondObject << ", one of which does not exist\n";
		exit(EXIT_FAILURE);
	}

	pos1 = dBodyGetPosition(first->Get_Body());
	pos2 = dBodyGetPosition(second->Get_Body());
	currentLength = Get_Current_Length(); // this required addition to Update_Geometry() ensures that there is a sane value at currentLength when it is time to memorize it at previousLength

	Update_Geometry();
}

void TETHER::Actuate(void) {

	if(motorNeurons[0] == NULL || motorNeurons[1] == NULL)
		return;

	double minimumMotorNeuronOutput;
	minimumMotorNeuronOutput = std::min(motorNeurons[0]->Get_Value(), motorNeurons[1]->Get_Value());

	Update_Geometry();

	// std::cerr << "MMO: " << minimumMotorNeuronOutput << " Dampening force: " << (dampeningCoefficient * (currentLength-previousLength) / relaxedLength) << std::endl;

	if(minimumMotorNeuronOutput > 0.) {
		currentTension = forceConstant * minimumMotorNeuronOutput + dampeningCoefficient * (currentLength-previousLength) / previousLength;
		currentTension = currentTension>0 ? currentTension : 0.;

		// std::cerr << "N0: " << motorNeurons[0]->Get_Value() << " N1: " << motorNeurons[1]->Get_Value() << " Tension: " << currentTension << std::endl;

		double fx, fy, fz;
		fx = (pos2[0]-pos1[0]) * currentTension / currentLength;
		fy = (pos2[1]-pos1[1]) * currentTension / currentLength;
		fz = (pos2[2]-pos1[2]) * currentTension / currentLength;

		dBodyAddForce(first->Get_Body(), fx, fy, fz);
		dBodyAddForce(second->Get_Body(), -fx, -fy, -fz);
	}
}

void TETHER::Draw() const {

	dsSetColorAlpha(currentTension / forceConstant, 0., 0., 0.);
	dsDrawLine(pos1, pos2);
}

bool TETHER::Create_Proprioceptive_Sensor(int sensorID, int evalPeriod) {

//	std::cerr << "CREATING A PROPRIOECPTIVE SENSOR" << std::endl << std::flush;

	proprioceptiveSensor = new PROPRIOCEPTIVE_TETHER_SENSOR(sensorID, evalPeriod);
	return true;
}

void TETHER::Poll_Sensors(int currentTimestep) {

	if(proprioceptiveSensor)
		proprioceptiveSensor->Poll(currentTension / forceConstant, currentTimestep);
}

void TETHER::Update_Sensor_Neurons(int t) {

	if(proprioceptiveSensor)
		proprioceptiveSensor->Update_Sensor_Neurons(t);
}

void TETHER::Write_To_Python(int evalPeriod) const {

	if(proprioceptiveSensor)
		proprioceptiveSensor->Write_To_Python(evalPeriod);
}

bool TETHER::Connect_Sensor_To_Sensor_Neuron(int sensorID, int sensorValueIndex, NEURON *sNeuron) {

	if(proprioceptiveSensor)
		if (proprioceptiveSensor->Get_ID() == sensorID) {
			proprioceptiveSensor->Connect_To_Sensor_Neuron(sNeuron, sensorValueIndex);
			return true;
		}
	return false;
}

/***** Private functions *****/

void TETHER::Update_Geometry(void) {

	previousLength = currentLength;
	currentLength = Get_Current_Length();
}

#endif // _ACTUATOR_TETHER_CPP
