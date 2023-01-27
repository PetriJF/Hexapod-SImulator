#include "LegSim.h"

LegSim::LegSim() {
	shoulderAngle = 0.0f;
	hipAngle = 0.0f;
	kneeAngle = 0.0f;
	femurLen = 10.0f;
	tibiaLen = 10.0f;
	orientation = 0.0f;
	offset = { 0.5f, 0.0f };
	origin = { 0.0f, 0.0f, 0.0f };
}

LegSim::LegSim(Point origin, float orientation, float femurLen, float tibiaLen, ShoulderOffset offset) {
	shoulderAngle = 0.0f;
	hipAngle = 0.0f;
	kneeAngle = 0.0f;

	this->origin = origin;
	this->orientation = orientation;
	this->femurLen = femurLen;
	this->tibiaLen = tibiaLen;
	this->offset = offset;
}

void LegSim::setLegOrientation(float orientation) {
	this->orientation = orientation;
}

float LegSim::getLegOrientation() {
	return orientation;
}

void LegSim::setFemurLen(float femurLen) {
	this->femurLen = femurLen;
}

float LegSim::getFemurLen() {
	return femurLen;
}

void LegSim::setTibiaLen(float tibiaLen) {
	this->tibiaLen = tibiaLen;
}

float LegSim::getTibiaLen() {
	return tibiaLen;
}

void LegSim::setOffset(ShoulderOffset offset) {
	this->offset = offset;
}

ShoulderOffset LegSim::getOffset() {
	return offset;
}

void LegSim::setAngles(float shoulderAngle, float hipAngle, float kneeAngle){
	this->shoulderAngle = shoulderAngle;
	this->hipAngle = hipAngle;
	this->kneeAngle = kneeAngle;
}

void LegSim::setPosition(float stepLocationDist, float bodyAltitude) {
	/*float L = sqrt(bodyAltitude * bodyAltitude + stepLocationDist * stepLocationDist);
	float alpha = asin(bodyAltitude / L);
	float beta = acos((tibiaLen * tibiaLen - femurLen * femurLen - L * L) / (-2.0 * femurLen * L));
	float gamma = acos((femurLen * femurLen - tibiaLen * tibiaLen - L * L) / (-2.0 * tibiaLen * L));

	float tetha1 = beta - alpha - 3.1415f / 2.0f;
	float tetha2 = alpha + gamma + 3.1415f / 2.0f;

	setAngles(0.0f, -tetha1, tetha2);
*/}

void LegSim::setPosition(Point p) {
	legPosition = p;
	float stepDist = sqrt((p.x - origin.x) * (p.x - origin.x) + (p.z - origin.z) * (p.z - origin.z));
	float L = sqrt(origin.y * origin.y + stepDist * stepDist);
	
	float alpha = asin(origin.y / L);
	float beta = acos((tibiaLen * tibiaLen - femurLen * femurLen - L * L) / (-2.0 * femurLen * L));
	float gamma = acos((femurLen * femurLen - tibiaLen * tibiaLen - L * L) / (-2.0 * tibiaLen * L));

	//float tetha = atan((p.x - origin.x) / (p.z - origin.z));
	//float tetha = (((int)(orientation / 3.1415f) % 2 == 0) ? 1 : -1) * 3.1415f + atan(abs(p.x - origin.x) / abs(p.z - origin.z)) - orientation;
	float tetha = ((orientation > 0) ? 1 : -1 ) * 3.1415f / 2.0f - atan((p.x - origin.x) / (p.z - origin.z));
	float tetha1 = beta - alpha - 3.1415f / 2.0f;
	float tetha2 = alpha + gamma + 3.1415f / 2.0f;

	setAngles(tetha, -tetha1, tetha2);
}

Point LegSim::getOrigin() {
	return origin;
}

void LegSim::setOrigin(Point origin) {
	this->origin = origin;
}

void LegSim::draw() {
	float x = origin.x, y = origin.y,  z = origin.z;
	float oX, oY, oZ;

	// Drawing the shoulder axis
	gl::color(RED);
	cylinder.height(CYLINDER_HEIGHT);
	cylinder.radius(CYLINDER_RADIUS);
	cylinder.origin(vec3(x, y - CYLINDER_HEIGHT / 2, z));
	cylinder.direction(vec3(0.0f, 1.0f, 0.0f));
	gl::draw(cylinder);

	// Coords for the hip
	x += offset.offsetShoulderHipX * cos(shoulderAngle);
	y += offset.offsetShoulderHipY;
	z += offset.offsetShoulderHipX * sin(shoulderAngle);

	// Drawing the hip axis
	gl::color(GREEN);
	cylinder.height(CYLINDER_HEIGHT / 2);
	cylinder.radius(CYLINDER_RADIUS);
	cylinder.origin(vec3(x, y, z));
	cylinder.direction(vec3(z, 0.0f, -x));
	gl::draw(cylinder);
	cylinder.direction(vec3(-z, 0.0f, x));
	gl::draw(cylinder);

	oX = x; oY = y; oZ = z;

	// Coords for the knee
	x += femurLen * cos(shoulderAngle) * sin(hipAngle);
	y += femurLen * cos(hipAngle);
	z += femurLen * sin(shoulderAngle) * sin(hipAngle);

	// Drawing the femur
	cylinder.height(femurLen);
	cylinder.set(vec3(oX, oY, oZ), vec3(x, y, z));
	gl::draw(cylinder);

	// Drawing the knee axis
	gl::color(BLUE);
	cylinder.height(CYLINDER_HEIGHT / 2);
	cylinder.radius(CYLINDER_RADIUS);
	cylinder.origin(vec3(x, y, z));
	cylinder.direction(vec3(z, 0.0f, -x));
	gl::draw(cylinder);
	cylinder.direction(vec3(-z, 0.0f, x));
	gl::draw(cylinder);

	oX = x; oY = y; oZ = z;

	// Coords for the end of the leg (tibia)
	x += tibiaLen * cos(shoulderAngle) * sin(kneeAngle);
	y += tibiaLen * cos(kneeAngle);
	z += tibiaLen * sin(shoulderAngle) * sin(kneeAngle);

	// Drawing the tibia
	cylinder.height(tibiaLen);
	cylinder.set(vec3(oX, oY, oZ), vec3(x, y, z));
	gl::draw(cylinder);
	gl::drawCube(vec3(x, y, z), vec3(1)); //< Marking end point (heel)

	
	//// Drawing the shoulder axis
	//gl::color(RED);
	//cylinder.height(CYLINDER_HEIGHT);
	//cylinder.radius(CYLINDER_RADIUS);
	//cylinder.origin(vec3(x, y - CYLINDER_HEIGHT / 2, z));
	//cylinder.direction(vec3(0.0f, 1.0f, 0.0f));
	//gl::draw(cylinder);

	//// Coords for the hip
	//x += offset.offsetShoulderHipX * cos(orientation + shoulderAngle);
	//y += offset.offsetShoulderHipY;
	//z += offset.offsetShoulderHipX * sin(orientation + shoulderAngle);

	//// Drawing the hip axis
	//gl::color(GREEN);
	//cylinder.height(CYLINDER_HEIGHT / 2);
	//cylinder.radius(CYLINDER_RADIUS);
	//cylinder.origin(vec3(x, y, z));
	//cylinder.direction(vec3(z, 0.0f, -x));
	//gl::draw(cylinder);
	//cylinder.direction(vec3(-z, 0.0f, x));
	//gl::draw(cylinder);

	//oX = x; oY = y; oZ = z;
	//
	//// Coords for the knee
	//x += femurLen * cos(orientation + shoulderAngle) * sin(hipAngle);
	//y += femurLen * cos(hipAngle);
	//z += femurLen * sin(orientation + shoulderAngle) * sin(hipAngle);
	//
	//// Drawing the femur
	//cylinder.height(femurLen);
	//cylinder.set(vec3(oX, oY, oZ), vec3(x, y, z));
	//gl::draw(cylinder);

	//// Drawing the knee axis
	//gl::color(BLUE);
	//cylinder.height(CYLINDER_HEIGHT / 2);
	//cylinder.radius(CYLINDER_RADIUS);
	//cylinder.origin(vec3(x, y, z));
	//cylinder.direction(vec3(z, 0.0f, -x));
	//gl::draw(cylinder);
	//cylinder.direction(vec3(-z, 0.0f, x));
	//gl::draw(cylinder);
	//
	//oX = x; oY = y; oZ = z;

	//// Coords for the end of the leg (tibia)
	//x += tibiaLen * cos(orientation + shoulderAngle) * sin(kneeAngle);
	//y += tibiaLen * cos(kneeAngle);
	//z += tibiaLen * sin(orientation + shoulderAngle) * sin(kneeAngle);
	//
	//// Drawing the tibia
	//cylinder.height(tibiaLen);
	//cylinder.set(vec3(oX, oY, oZ), vec3(x, y, z));
	//gl::draw(cylinder);
	//gl::drawCube(vec3(x, y, z), vec3(1)); //< Marking end point (heel)
}