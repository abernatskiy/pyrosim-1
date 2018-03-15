#ifndef _LIGHT_SOURCE_H
#define _LIGHT_SOURCE_H

#include <iostream>
#include <vector>
#include <array>
#include <ode/ode.h>
#include <drawstuff/drawstuff.h>

#include "myODEMath.h"

#ifdef dDOUBLE
#define dsDrawSphere dsDrawSphereD
#endif // dDOUBLE

#define LIGHT_SOURCE_RADIUS 0.05

// Rainbow colors from https://www.webnots.com/vibgyor-rainbow-color-codes/
// Python one-liner converter: '{' + ', '.join(["{0:.2f}".format(float(x)/255.) for x in [255, 127, 0]]) + '}'
static const unsigned numColors = 7;
static const dReal black[3] = {0.00, 0.00, 0.00};
static const dReal colors[numColors][3] = {{0.58, 0.00, 0.83}, // violet
                                           {0.29, 0.00, 0.51}, // indigo
                                           {0.00, 0.00, 1.00}, // blue
                                           {0.00, 1.00, 0.00}, // green
                                           {1.00, 1.00, 0.00}, // yellow
                                           {1.00, 0.50, 0.00}, // orange
                                           {1.00, 0.00, 0.00}}; // red

class LIGHT_SOURCE {

private:
	int ID;
	dVector3 offset; // offset of the light source in the body's coordinate system
	dVector3 absPos; // absolute coordinates of the light source
	int kind;
	dBodyID body;
	const dReal brightness;

	int positionSensorID;
	std::array<std::vector<dReal>,3> position;

public:
	LIGHT_SOURCE(int myID, dBodyID myBody) : ID(myID),
	                                         kind(0),
	                                         body(myBody),
	                                         brightness(1.0),
	                                         positionSensorID(-1) {};

	void Read_From_Python(void) {
		std::cin >> offset[0];
		std::cin >> offset[1];
		std::cin >> offset[2];
		std::cin >> kind;
		std::cin >> positionSensorID;
	};

	dReal Luminousity_At(dReal x, dReal y, dReal z, int kindOfLight) {
		// FIXME: ideally, luminousity should be independent of external queries, i.e. something like vector<pair<int,dReal>> containing sensor-agnostic luminousities for all light kinds
		if(kind == 0 || kindOfLight == 0 || kind == kindOfLight) { // sensors of zeroth kind see all light sources; light sources of zeroth kind are visible to all sensors
			dReal dist = Square_Distance_To(x, y, z);
			// Light decays as inverse square distance, but saturates in close proximity to source
			// std::cerr << "dbg: " << dist << " " << brightness/dist << "\n";
			return dist>LIGHT_SOURCE_RADIUS ? brightness/dist : brightness/LIGHT_SOURCE_RADIUS;
		}
		else
			return 0.;
	};

	void Draw(void) {
		const dReal* col = getSensorColor();
		dsSetColor(col[0], col[1], col[2]);
		Update_Absolute_Position();
		const dReal* rot = dBodyGetRotation(body);
		dsDrawSphere(absPos, rot, LIGHT_SOURCE_RADIUS);
	};

	void Write_To_Python(int evalPeriod) {
		if(positionSensorID >= 0) {
			std::ostringstream oss;
			oss << positionSensorID << ' ' << 3 << ' ';
			for(int t=0; t<evalPeriod; t++)
				for(int i=0; i<3; i++)
					oss << position[i][t] << ' ';
			oss << '\n';
			std::cout << oss.str();
		}
	};

	void Poll_Sensors(void) {
		if(positionSensorID >= 0) {
			Update_Absolute_Position();
			for(int i=0; i<3; i++)
				position[i].push_back(absPos[i]);
		}
	};

private:
	dReal Square_Distance_To(dReal x, dReal y, dReal z) {
		Update_Absolute_Position();
		double dx, dy, dz;
		dx = absPos[0] - x; dy = absPos[1] - y; dz = absPos[2] - z;
		return dx*dx + dy*dy + dz*dz;
	};

	void Update_Absolute_Position(void) {
		const dReal* rot = dBodyGetRotation(body);
		dVector3 absOffset;
		myDMultiplyMat3Vec3(rot, offset, absOffset);
		const dReal* pos = dBodyGetPosition(body);
		myDVector3Add(pos, absOffset, absPos);
	};

	const dReal* getSensorColor(void) {
		if(kind == 0)
			return black;
		else
			return colors[(kind-1) % numColors];
	};
};

#endif // _LIGHT_SOURCE_H
