#ifndef _ACTUATOR_THRUSTER_CPP
#define _ACTUATOR_THRUSTER_CPP

#include <drawstuff/drawstuff.h>
#include <iostream>
#include <cmath>
#include <cstdlib>

#include "thruster.h"
#include "datastruct.h"

#ifdef dDOUBLE
#define dsDrawLine dsDrawLineD
#define dsDrawBox dsDrawBoxD
#define dsDrawSphere dsDrawSphereD
#define dsDrawCylinder dsDrawCylinderD
#define dsDrawCapsule dsDrawCapsuleD
#endif

extern Data* data;

void THRUSTER::Read_From_Python(void) {

	std::cin >> ID;

	std::cin >> firstObject;
	std::cin >> x;
	std::cin >> y;
	std::cin >> z;
	std::cin >> lowStop;
	std::cin >> highStop;
	std::cin >> shutoffThreshold;
	std::cin >> momentumBudget;
}

void THRUSTER::Actuate(void) {

	if ( motorNeurons[0] == NULL )
		return;

	double motorNeuronValue = motorNeurons[0]->Get_Value();
	if ( motorNeuronValue < shutoffThreshold )
		return;

	double zeroToOne = (motorNeuronValue - shutoffThreshold)/(1. - shutoffThreshold);
	double desiredTarget = zeroToOne * ( highStop - lowStop ) + lowStop;

	if(momentumBudget > 0) { // negative budget means infinite budget
		double newMomentumUsed = momentumUsed + (data->dt)*desiredTarget;
		if(newMomentumUsed > momentumBudget) {
			lastDesired = 0.;
			return;
		}
		else {
			momentumUsed = newMomentumUsed;
		}
	}

	const dReal *R = dBodyGetRotation(first->Get_Body());
	dReal xDir, yDir, zDir;

	xDir = R[0]*x + R[1]*y + R[2]*z;
	yDir = R[4]*x + R[5]*y + R[6]*z;
	zDir = R[8]*x + R[9]*y + R[10]*z;

	dBodyAddForce(first->Get_Body(), -xDir*desiredTarget, -yDir*desiredTarget, -zDir*desiredTarget);
	lastDesired = desiredTarget;
}

void THRUSTER::Create_In_Simulator(dWorldID world, OBJECT ** allObjects, int numObjects, ACTUATOR** allActuators, int numActuators) {

	if ( firstObject >= 0 )
		first = allObjects[firstObject];

	dReal mag = sqrt(x*x + y*y + z*z);
	x = x/mag; y = y/mag; z = z/mag;

	const dReal *R = dBodyGetRotation(first->Get_Body());
	dReal xDir, yDir, zDir;

	xDir = R[0]*x + R[4]*y + R[8]*z;
	yDir = R[1]*x + R[5]*y + R[9]*z;
	zDir = R[2]*x + R[6]*y + R[10]*z;

	x = xDir;	y = yDir;	z = zDir;
}

void THRUSTER::Draw() const {

//	std::cerr << lastDesired << std::endl;

	dVector3 jointPosition;
	dVector3 jointAxis;
	dMatrix3 rotation;
	dReal jointActuation;
	dReal radius = .025;
	dReal length = .3;

	const dReal *pos = dBodyGetPosition(first->Get_Body());
	const dReal *R = dBodyGetRotation(first->Get_Body());
	dReal xDir, yDir, zDir;

	xDir = R[0]*x + R[1]*y + R[2]*z;
	yDir = R[4]*x + R[5]*y + R[6]*z;
	zDir = R[8]*x + R[9]*y + R[10]*z;

	dRFromZAxis(rotation, xDir, yDir, zDir);

	float param = 1.0;
	float index = 0;

	// while(param < std::abs(lastDesired)){
	//   index ++;
	//   param *= 10.0;
	// }
	if (std::abs(lastDesired)>=1.0){
		index = log(std::abs(lastDesired));
		for(int j=1; j<5; j++){

			for(int i=0; i<3; i++){
				jointPosition[i] = pos[i];
			}

			if (lastDesired >0)
				length = .04*pow(j,1.3)*index;
			else
				length = -.04*pow(j,1.3)*index;

			radius = .005*(5-j)*index;

			jointPosition[0] += xDir*length/2.0;
			jointPosition[1] += yDir*length/2.0;
			jointPosition[2] += zDir*length/2.0;

			dsSetColorAlpha((j+10.)/15.0, (j+1)/6.0, .2, 1.0);
			dsDrawCylinder(jointPosition, rotation, length, radius);
		}
	}
}

bool THRUSTER::Create_Proprioceptive_Sensor(int sensorID, int evalPeriod) {

	proprioceptiveSensor = new PROPRIOCEPTIVE_THRUSTER_SENSOR(sensorID, evalPeriod);
	return true;
}

void THRUSTER::Poll_Sensors(int currentTimestep) {

	if(proprioceptiveSensor) {
		double reading = momentumBudget>0 ? 1. - 2.*momentumUsed/momentumBudget : 1.;
		proprioceptiveSensor->Poll(reading, currentTimestep);
	}
}

void THRUSTER::Update_Sensor_Neurons(int t) {

	if(proprioceptiveSensor)
		proprioceptiveSensor->Update_Sensor_Neurons(t);
}

void THRUSTER::Write_To_Python(int evalPeriod) const {

	if(proprioceptiveSensor)
		proprioceptiveSensor->Write_To_Python(evalPeriod);
}

bool THRUSTER::Connect_Sensor_To_Sensor_Neuron(int sensorID, int sensorValueIndex, NEURON* sNeuron) {

  if(proprioceptiveSensor && proprioceptiveSensor->Get_ID() == sensorID) {
      proprioceptiveSensor->Connect_To_Sensor_Neuron(sNeuron, sensorValueIndex);
      return true;
	}
  return false;
}

#endif // _ACTUATOR_THRUSTER_CPP
