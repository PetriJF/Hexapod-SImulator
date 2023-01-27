#pragma once

#include "LegSim.h"
#include "dataTypes.h"

class Hexapod {
	private:
		float baseLength, baseHeight, baseWidth;		// Setting the dimensions of the body
		float femurLen, tibiaLen;						// Setting the dimentions of the legs
		float legConfig;								// Setting the leg configuration (how far out are the middle legs to the side)
		ShoulderOffset offset;							// Offset between the shoulder and hip motors
		LegSim legRF, legRM, legRB;						// Right legs
		LegSim legLF, legLM, legLB;						// Left legs
		geom::Cylinder bodyElement;						// Used for drawing the lines representing the robots
		const ColorA CYAN = ColorA(0.0f, 1.0f, 1.0f);	// Constant representing the color gray 
		float bodyAltitude;								// Distance between the ground and the center of mass
		float stepLocationDist;							// Desired distance between origin and stand-by location of the leg
		bool hexagonalConfig;							// Represents if the legs are |_|_| or \_|_/
		float xAxisAddon, zAxisAddon;					// The amount the body of the robot translates on the X and Z axis (legs stay put)
		float xAxisTilt, yAxisTilt, zAxisTilt;			// Rotating the body on the 3 axis. Legs stay put
		Point legPos[6];								// Represents the current positions of the legs
		Point originLegPos[6];							// Represents the points in space for the shoulder motors (origins)
		Point legHomePosition[6];						// Represents the home location for the legs. Used for the kinematic model
		float legOrientation[6];						// Represents the direction of the leg

		void updateOrigins();							// Updates the origin of all the motors
		void updateLegPos(Point positions[]);			// Updates the leg positions
		void resetOrigins();							// Sets the origins
		Point getOriginPoint(float xTilt, float yTilt, float zTilt, float orientation, float L, float xAddon, float yAddon, float zAddon);
		void resetOrientations(bool hexagonalConfig);	// Resets the orientations of the legs in terms of the hexagonalConfig
	public:
		Hexapod();
		Hexapod(float baseLength, float baseHeight, float baseWidth, float femurLen, float tibiaLen, ShoulderOffset offset);
		void setTibiaLen(float tibiaLen);
		void setFemurLen(float femurLen);
		void setBaseLength(float baseLength);
		void setBaseWidth(float baseWidth);
		void setLegConfig(float legConfig, bool hexagonalConfig);
		void setGaitSettings(float bodyAltitude, float stepLocationDist);
		void setGCenterAndTilt(float xAxisAddon, float zAxisAddon, float xAxisTilt, float yAxisTilt,float zAxisTilt);
		void setBaseRotation();
		void draw();
};

