#ifndef _OBJECT_CPP
#define _OBJECT_CPP

#include <iostream>
#include <drawstuff/drawstuff.h>
#include <cstdlib>
#include <vector>

#include "constants.h"
#include "object.h"
#include "texturepath.h"
#include "geomData.h"
#include "myODEMath.h"

#ifdef dDOUBLE
#define dsDrawLine dsDrawLineD
#define dsDrawBox dsDrawBoxD
#define dsDrawSphere dsDrawSphereD
#define dsDrawCylinder dsDrawCylinderD
#define dsDrawCapsule dsDrawCapsuleD
#endif

OBJECT::~OBJECT(){

	if(bodyCreated)
		delete static_cast<GeomData*>(dGeomGetData(geom));
}

void OBJECT::Add_External_Force(float x, float y, float z, int timeStep){

	forces[timeStep][0] = x;
	forces[timeStep][1] = y;
	forces[timeStep][2] = z;
}

void OBJECT::Read_In_External_Force(void){

    int xPos,yPos,zPos,time;
    std::cin >> x;
    std::cin >> y;
    std::cin >> z;
    std::cin >> time;
    Add_External_Force(x,y,z,time);
}

void OBJECT::Apply_Stored_Forces(int timeStep){

    if (forces.find(timeStep)!= forces.end()){
        dBodyAddForce(body, forces[timeStep][0],forces[timeStep][1], forces[timeStep][2]);
    }

}

int OBJECT::Connect_Sensor_To_Sensor_Neuron(int sensorID, int sensorValueIndex, NEURON *sensorNeuron) {

    if ( lightSensor )
        if ( lightSensor->Get_ID() == sensorID ) {
            lightSensor->Connect_To_Sensor_Neuron(sensorNeuron, sensorValueIndex);
            return true;
        }
    if ( positionSensor )
        if ( positionSensor->Get_ID() == sensorID ) {
            positionSensor->Connect_To_Sensor_Neuron(sensorNeuron, sensorValueIndex);
            return true;
        }
    if ( raySensor )
        if ( raySensor->Get_ID() == sensorID ) {
            raySensor->Connect_To_Sensor_Neuron(sensorNeuron, sensorValueIndex);
            return true;
        }
    if ( proximitySensor )
        if ( proximitySensor->Get_ID() == sensorID ) {
            proximitySensor->Connect_To_Sensor_Neuron(sensorNeuron, sensorValueIndex);
            return true;
        }
    if ( touchSensor )
        if ( touchSensor->Get_ID() == sensorID ) {
            touchSensor->Connect_To_Sensor_Neuron(sensorNeuron, sensorValueIndex);
            return true;
        }
    if ( vestibularSensor )
        if ( vestibularSensor->Get_ID() == sensorID ) {
            vestibularSensor->Connect_To_Sensor_Neuron(sensorNeuron, sensorValueIndex);
            return true;
        }
    if ( isSeenSensor )
        if ( isSeenSensor->Get_ID() == sensorID) {
            isSeenSensor->Connect_To_Sensor_Neuron(sensorNeuron, sensorValueIndex);
            return true;
        }
    return false;
 }

void OBJECT::Create_IsSeen_Sensor(int myID, int evalPeriod){
    isSeenSensor = new IS_SEEN_SENSOR(myID, evalPeriod);
}

void OBJECT::Create_Ray_Sensor(dSpaceID space, int myID, int evalPeriod) {
	raySensor = new RAY_SENSOR(space,this,myID,evalPeriod);
}

void OBJECT::Create_Proximity_Sensor(dSpaceID space, int myID, int evalPeriod) {
	proximitySensor = new PROXIMITY_SENSOR(space,this,myID,evalPeriod);
}

void OBJECT::Create_Light_Sensor(int myID, int evalPeriod) {
	lightSensor = new LIGHT_SENSOR(myID, evalPeriod);
	lightSensor->Read_From_Python();
}

void OBJECT::Create_Light_Source(int myID) {
	lightSources.push_back(LIGHT_SOURCE(myID, body));
	lightSources.back().Read_From_Python();
}

void OBJECT::Create_Position_Sensor(int myID, int evalPeriod) {
	positionSensor = new POSITION_SENSOR(myID,evalPeriod);
}

void OBJECT::Create_Touch_Sensor(int myID, int evalPeriod) {
	touchSensor = new TOUCH_SENSOR(myID,evalPeriod);
}

void OBJECT::Create_Vestibular_Sensor(int myID, int evalPeriod) {
    vestibularSensor = new VESTIBULAR_SENSOR(myID,evalPeriod);
}

void OBJECT::Set_Adhesion(int adhesionKind) {

	adhesionTypesExhibiting.insert(adhesionKind);

	// If there are already some joints that rely on this adhesion group, add one duplicate entry to each of their lists of adhesion kinds
	// That way they won't be destroyed when the previous JOINT of the same kind sends Unset_Adhesion()
	if (adhesionTypesExhibiting.count(adhesionKind) > 1) {
		for (std::map<dJointID,std::multiset<int>>::iterator jointRecordIt=adhesiveJointsToTypes.begin(); jointRecordIt!=adhesiveJointsToTypes.end(); jointRecordIt++) {
			if (jointRecordIt->second.find(adhesionKind) != jointRecordIt->second.end())
				jointRecordIt->second.insert(adhesionKind);
		}
	}

	// Turn blue as soon as the adhesion is enabled
	// Color chosen in honor of the wonderful January 2018 Vermont weather
	r = 0.; g = 0.; b = 1.;
}

void OBJECT::Unset_Adhesion(int adhesionKind) {

	// Remove the adhesion kind from those exhibited by this object
	std::multiset<int>::iterator kpos = adhesionTypesExhibiting.find(adhesionKind);
	if (kpos!=adhesionTypesExhibiting.end())
		adhesionTypesExhibiting.erase(kpos);
	else {
		std::cerr << "Object " << this << " does not have adhesion of type " << adhesionKind << ", exiting" << std::endl;
		exit(1);
	}

	// Find out which joints are not supported by any kinds of adhesion anymore
	std::vector<dJointID> unstuckJoints;

	for (std::map<dJointID,std::multiset<int>>::iterator jointRecordIt=adhesiveJointsToTypes.begin(); jointRecordIt!=adhesiveJointsToTypes.end(); jointRecordIt++) {
		kpos = jointRecordIt->second.find(adhesionKind);
		if (kpos!=jointRecordIt->second.end())
			jointRecordIt->second.erase(kpos);
		if (jointRecordIt->second.empty())
			unstuckJoints.push_back(jointRecordIt->first);
	}

	// Destroy the obsolete joints and burn the records
	for (std::vector<dJointID>::iterator itUnJoint=unstuckJoints.begin(); itUnJoint!=unstuckJoints.end(); itUnJoint++) {
		dJointDestroy(*itUnJoint);
		adhesiveJointsToTypes.erase(*itUnJoint);
	}

	// Restore the color to normal once the adhesion is disabled
	r = tr; g = tg; b = tb;
}

void OBJECT::Process_Adhesive_Touch(dWorldID world, OBJECT* other) {


//	std::cerr << "Processing adhesive touch..." << std::endl;
	// Find all adhesion groups to which *other is susceptible and which *this exhibits, keeping in mind that there may be some duplicates among the latter.
	std::multiset<int> ats; // adhesion types set

/*
	std::cerr << "Others susceptible adhesion types:";
	for(auto susIt = other->adhesionTypesSusceptible.begin(); susIt!= other->adhesionTypesSusceptible.end(); susIt++)
		std::cerr << " " << *susIt;
	std::cerr << "\nSelf exhibiting adhesion types:";
	for(auto exhIt = adhesionTypesExhibiting.begin(); exhIt!=  adhesionTypesExhibiting.end(); exhIt++)
		std::cerr << " " << *exhIt;
	std::cerr << "\n";
*/

	std::set<int>::iterator susIt = other->adhesionTypesSusceptible.begin();
	std::multiset<int>::iterator exhIt = adhesionTypesExhibiting.begin();
	while (susIt!=other->adhesionTypesSusceptible.end() && exhIt!=adhesionTypesExhibiting.end()) {
		if(*susIt < *exhIt)
			susIt++;
		else if(*susIt > *exhIt)
			exhIt++;
		else {
			ats.insert(*exhIt);
			exhIt++;
		}
	}

	// If there are any valid adhesion groups, add a joint and make a record about why it was added
	if(!ats.empty()) {

//		std::cerr << "Set of adhesion groups nonempty! Creating a joint" << std::endl;
		dJointID j = dJointCreateFixed(world, 0);
		dJointAttach(j, Get_Body(), other->Get_Body());
		dJointSetFixed(j);

		adhesiveJointsToTypes[j] = ats;
	}
}

void OBJECT::Draw(void) {

	dsSetColor(r,g,b);
	const dReal *pos = dBodyGetPosition(body);
	const dReal *rot = dBodyGetRotation(body);

	// dsSetTexture (DS_WOOD);
	if (myShape == BOX){
		dReal sides[3] = {length,width,height};
		dsDrawBox (pos,rot,sides);
	}
	else if (myShape == CYLINDER)
		dsDrawCylinder(pos,rot,length,radius);
	else if (myShape == CAPSULE)
		dsDrawCapsule(pos,rot,length,radius);
	else if (myShape == SPHERE)
		dsDrawSphere(pos,rot,radius);

	if(proximitySensor)
		proximitySensor->Draw();

	for(std::vector<LIGHT_SOURCE>::iterator lsit=lightSources.begin(); lsit!=lightSources.end(); lsit++)
		lsit->Draw();
	// TODO: add light sensor drawing
}

void OBJECT::Draw_Ray_Sensor(double x, double y, double z, int t) {

	if ( raySensor )
		raySensor->Draw(x,y,z,t);
}

void OBJECT::Poll_Sensors(int numObjects, OBJECT** objects, int t) {

	if ( lightSensor ) {
		int lightSensitivityKind = lightSensor->Get_Sensitivity_Kind();
		dReal luminousity = 0.;
		const dReal* lightSensorPos = dBodyGetPosition(body);
		for(int i=0; i<numObjects; i++)
			luminousity += objects[i]->Get_Luminousity_Produced_At(lightSensorPos, lightSensitivityKind);
		lightSensor->Poll(luminousity, t);
	}

	if ( positionSensor )
		positionSensor->Poll(body,t);

	if ( vestibularSensor )
		vestibularSensor->Poll(body,t);

	for(auto it=lightSources.begin(); it!=lightSources.end(); it++)
		it->Poll_Sensors();
}

void OBJECT::Read_From_Python(dWorldID world, dSpaceID space, int shape) {

	myShape = shape;

	std::cin >> ID;

	std::cin >> x;
	std::cin >> y;
	std::cin >> z;
	std::cin >> r1;
	std::cin >> r2;
	std::cin >> r3;

	if ( myShape == BOX ) {
		std::cin >> length;
		std::cin >> width;
		std::cin >> height;
	}
	else if (myShape == CYLINDER or myShape == CAPSULE) { //cylinder specific
		std::cin >> length;
		std::cin >> radius;
	}
	else { //sphere specific
		std::cin >> radius;
	}
	std::cin >> mass;
	std::cin >> collisionGroup;
	std::cin >> r;
	std::cin >> g;
	std::cin >> b;

	tr = r; tg = g; tb = b;

	CreateBody(world, space);
}

void OBJECT::Set_Ray_Sensor(double distance, OBJECT *objectThatWasHit, int t) {
	if ( raySensor )
		raySensor->Set(distance,objectThatWasHit,t);
}

void OBJECT::Set_Proximity_Sensor(double distance, dVector3 contactPoint, OBJECT *objectThatWasHit, int t) {
	if ( proximitySensor )
		proximitySensor->Set(distance, contactPoint, objectThatWasHit, t);
}

void OBJECT::Touch_Sensor_Fires(int t) {
	if ( touchSensor )
		touchSensor->Fires(t);
}

void OBJECT::IsSeen_Sensor_Fires(int t){
	if ( isSeenSensor )
		isSeenSensor->Fires(t);
}

void OBJECT::Update_Sensor_Neurons(int t) {
	if ( raySensor )
		raySensor->Update_Sensor_Neurons(t);

	if ( lightSensor )
		lightSensor->Update_Sensor_Neurons(t);

	if ( positionSensor )
		positionSensor->Update_Sensor_Neurons(t);

	if ( touchSensor )
		touchSensor->Update_Sensor_Neurons(t);

	if ( vestibularSensor )
		vestibularSensor->Update_Sensor_Neurons(t);

	if ( proximitySensor )
		proximitySensor->Update_Sensor_Neurons(t);
}

void OBJECT::Write_To_Python(int evalPeriod) {
    std::cerr << "writing sensors in body " << this->Get_ID() << " to python " << std::endl;
	if ( raySensor ){
        std::cerr << "  writing ray sensor to python "  << std::endl;
		raySensor->Write_To_Python(evalPeriod);
    }

	if ( proximitySensor ){
        std::cerr << "  writing proximity sensor to python "  << std::endl;
		proximitySensor->Write_To_Python(evalPeriod);
    }

	if ( lightSensor ){
		lightSensor->Write_To_Python(evalPeriod);
        std::cerr << "  writing light sensor to python "  << std::endl;
    }
	if ( positionSensor ){
        std::cerr << "  writing position sensor to python "  << std::endl;
		positionSensor->Write_To_Python(evalPeriod);
    }
	if ( touchSensor ){
        std::cerr << "  writing touch sensor to python "  << std::endl;
		touchSensor->Write_To_Python(evalPeriod);
    }
    if ( vestibularSensor ){
        std::cerr << "  writing vestib sensor to python "  << std::endl;
        vestibularSensor->Write_To_Python(evalPeriod);
    }
    if ( isSeenSensor ){
        std::cerr << "  writing is seen sensor to python "  << std::endl;
        isSeenSensor->Write_To_Python(evalPeriod);
    }

	for(auto it=lightSources.begin(); it!=lightSources.end(); it++)
		it->Write_To_Python(evalPeriod);
}

// ------------------------------- Private methods ------------------------------

//int OBJECT::Contains_A_Light_Source(void) {
//	return containsLightSource;
//}

void OBJECT::CreateBody(dWorldID world, dSpaceID space){

	dMass m;

	body = dBodyCreate (world);
	dBodySetPosition (body,x,y,z);

	dMatrix3 R;
	dRFromZAxis(R,r1,r2,r3);
	dBodySetRotation(body,R);

	if(myShape == BOX){
		dMassSetBoxTotal (&m,mass,length,width,height);
		geom = dCreateBox(space,length,width,height);
	}
	else if(myShape == CAPSULE){
		dMassSetCapsuleTotal(&m,mass,3,radius,length);
		geom = dCreateCapsule(space,radius,length);
	}
	else if(myShape == CYLINDER){
		dMassSetCylinderTotal(&m,mass,3,radius,length);
		geom = dCreateCylinder(space,radius,length);
	}
	else if(myShape == SPHERE){
		dMassSetSphereTotal(&m,mass,radius);
		geom = dCreateSphere(space,radius);
	}
	dMassRotate(&m, R);
	dBodySetMass (body,&m);

	dGeomSetBody (geom,body);

	GeomData* gd = new GeomData();
	gd->geomType = DEFAULT;
	gd->objectPtr = this;
	dGeomSetData(geom, static_cast<void*>(gd));

	bodyCreated = true;
}

/*
double OBJECT::Distance_To(OBJECT *otherObject) {

	const dReal *myPos = dBodyGetPosition( body );
	const dReal *hisPos = dBodyGetPosition( otherObject->Get_Body() );

	double xDiff = myPos[0] - hisPos[0];
	double yDiff = myPos[1] - hisPos[1];
	double zDiff = myPos[2] - hisPos[2];

	return sqrt( pow(xDiff,2.0) + pow(yDiff,2.0) + pow(zDiff,2.0) );
}
*/

dReal OBJECT::Get_Luminousity_Produced_At(const dReal* pos, int kindOfLight) {

	dReal luminousity = 0.;
	for(std::vector<LIGHT_SOURCE>::iterator lsit=lightSources.begin(); lsit!=lightSources.end(); lsit++)
		luminousity += lsit->Luminousity_At(pos[0], pos[1], pos[2], kindOfLight);
	return luminousity;
}

#endif // _OBJECT_CPP
