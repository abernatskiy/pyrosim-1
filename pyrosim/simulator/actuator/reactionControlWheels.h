#ifndef _ACTUATOR_REACTION_CONTROL_WHEELS_H
#define _ACTUATOR_REACTION_CONTROL_WHEELS_H

#include "../object.h"
#include "actuator.h"

class REACTION_CONTROL_WHEELS : public ACTUATOR {

private:
	int	firstObjectID;
	dVector3 mb, cm; // momentum budget & current accumulated momentum
	double maxTorque;
	OBJECT* firstObject;

public:
	REACTION_CONTROL_WHEELS (void) : ACTUATOR(3),
	                 mb({1.,1.,1.}), cm({0.,0.,0.}),
	                 maxTorque(1.),
	                 firstObject(NULL) {};

	void Actuate(void);
	void Draw(void) const {};
	void Create_In_Simulator(dWorldID world, OBJECT** allObjects, int numObjects, ACTUATOR** allActuators, int numActuators);
	void Read_From_Python(void);
};

#endif // _ACTUATOR_REACTION_CONTROL_WHEELS_H
