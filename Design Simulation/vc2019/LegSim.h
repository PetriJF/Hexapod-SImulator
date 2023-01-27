/* Author: James Florin Petri, jamespetri28@gmail.com
 * Usage: Used to simulate one leg of spider type robot (quadpod, hexapod, etc)
 * Project: FYP
 */


#pragma once
#include "dataTypes.h"
#include "cinder/PolyLine.h"


using namespace ci;
using namespace ci::app;

class LegSim {
	private:
		float shoulderAngle, hipAngle, kneeAngle;	// Leg motors
		float femurLen, tibiaLen;					// Leg Dimensions
		float orientation;							// Orientation of the leg
		Point origin;								// Position of the leg
		Point legPosition;							// The position of the leg on the ground
		ShoulderOffset offset;						// Offset between the shoulder and the hip
		geom::Cylinder cylinder;					// Used for drawing cylinders
		const float CYLINDER_RADIUS = 0.5f;			// Radius of cylinder
		const float CYLINDER_HEIGHT = 1.5f;			// Base height of cylinder. Half of it used in some contexts
		const ColorA RED = ColorA(1.0f, 0.0f, 0.0f);// |v|v| Constants representing colors |v|v|
		const ColorA GREEN = ColorA(0.0f, 1.0f, 0.0f);
		const ColorA BLUE = ColorA(0.0f, 0.0f, 1.0f);
	public:
		LegSim();
		LegSim(Point origin, float orientation, float femurLen, float tibiaLen, ShoulderOffset offset);

		void setLegOrientation(float orientation);
		float getLegOrientation();
		void setFemurLen(float femurLen);
		float getFemurLen();
		void setTibiaLen(float tibiaLen);
		float getTibiaLen();
		void setOffset(ShoulderOffset offset);
		Point getOrigin();
		void setOrigin(Point origin);
		ShoulderOffset getOffset();
		void setAngles(float shoulderAngle, float hipAngle, float kneeAngle);
		void setPosition(float stepLocationDistance, float bodyAltitude);
		void setPosition(Point p);
		void draw();
};