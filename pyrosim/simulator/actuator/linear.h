#ifndef _ACTUATOR_LINEAR_H
#define _ACTUATOR_LINEAR_H

#include "../object.h"
#include "rotary.h"

// Linear actuator turns out to be very similar to rotary in terms of implementation, hence the inheritance

class LINEAR_ACTUATOR : public ROTARY_ACTUATOR {

public:
	void Actuate(void);

	void Create_In_Simulator(dWorldID world, OBJECT ** allObjects, int numObjects);

	void Draw() const;

	void Read_From_Python(void);

	void Poll_Sensors(int t);

};

#endif // _ACTUATOR_LINEAR_H
