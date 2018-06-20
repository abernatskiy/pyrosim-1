#ifndef _SENSOR_GLOBAL_COLLISION_CPP
#define _SENSOR_GLOBAL_COLLISION_CPP

#include <iostream>
#include <cmath>
#include "globalCollision.h"
#include "../myODEMath.h"
#include "object.h"

void GLOBAL_COLLISION_SENSOR::Set(OBJECT* firstCollidingObject, OBJECT* secondCollidingObject, int tt) {

	const dReal* firstObjectSpeed;
	firstObjectSpeed = dBodyGetLinearVel(firstCollidingObject->Get_Body());
	const dReal* secondObjectSpeed;
	secondObjectSpeed = dBodyGetLinearVel(secondCollidingObject->Get_Body());
	dVector3 difference;
	myDVector3Subtract(firstObjectSpeed, secondObjectSpeed, difference);

	Update_Last_Reading(myDVector3Length(difference), tt);
}

void GLOBAL_COLLISION_SENSOR::Update_Last_Reading(double relativeVelocity, int tt) {

	double sensorReading = Get_Sensor_Reading(relativeVelocity);
	if(mcs[tt]<sensorReading)
	{
		mcs[tt] = sensorReading;
//		lastReading = sensorReading;
	}
}

double GLOBAL_COLLISION_SENSOR::Get_Sensor_Reading(double relativeVelocity) {

	if(relativeVelocity<=0.)
		return 0.;

	if(isLogarithmic)
		return log1p(relativeVelocity/scale);
	else
		return relativeVelocity/scale;
}

#endif // _SENSOR_GLOBAL_COLLISION_CPP
