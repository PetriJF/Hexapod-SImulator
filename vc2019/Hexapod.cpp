#include "Hexapod.h"

Hexapod::Hexapod() {
	baseLength = 30.0f;
	baseHeight = 5.0f;
	baseWidth = 10.0f;
	legConfig = 0.0f;

	offset = { 0.5, 0 };
	femurLen = 10;
	tibiaLen = 15;

	bodyAltitude = 5.0f;
	xAxisAddon = zAxisAddon = 0.0f;
	xAxisTilt = yAxisTilt = zAxisTilt = 0.0f;
	stepLocationDist = 5.0f;
	legConfig = 0.0f;

	setGaitSettings(bodyAltitude, stepLocationDist);
	updateOrigins();
	updateLegPos(legHomePosition);
}

Hexapod::Hexapod(float baseLength, float baseHeight, float baseWidth, float femurLen, float tibiaLen, ShoulderOffset offset) 
	:	legLB({ -baseLength / 2, tibiaLen + offset.offsetShoulderHipY + baseHeight / 2, -baseWidth / 2 }, -1.57075f, femurLen, tibiaLen, offset),
		legLM({ 0.0f, tibiaLen + offset.offsetShoulderHipY + baseHeight / 2, -baseWidth / 2 }, -1.57075f, femurLen, tibiaLen, offset),
		legLF({ baseLength / 2, tibiaLen + offset.offsetShoulderHipY + baseHeight / 2, -baseWidth / 2 }, -1.57075f, femurLen, tibiaLen, offset),
		legRB({ -baseLength / 2, tibiaLen + offset.offsetShoulderHipY + baseHeight / 2, baseWidth / 2 }, 1.57075f, femurLen, tibiaLen, offset),
		legRM({ 0.0f, tibiaLen + offset.offsetShoulderHipY + baseHeight / 2, baseWidth / 2 }, 1.57075f, femurLen, tibiaLen, offset),
		legRF({ baseLength / 2, tibiaLen + offset.offsetShoulderHipY + baseHeight / 2, baseWidth / 2 }, 1.57075f, femurLen, tibiaLen, offset)
{
	this->baseLength = baseLength;
	this->baseHeight = baseHeight;
	this->baseWidth = baseWidth;

	this->offset = offset;
	this->femurLen = femurLen;
	this->tibiaLen = tibiaLen;

	bodyAltitude = 5.0f;

	xAxisAddon = zAxisAddon = 0.0f;
	xAxisTilt = yAxisTilt = zAxisTilt = 0.0f;
	stepLocationDist = 5.0f;
	legConfig = 0.0f;

	setGaitSettings(bodyAltitude, stepLocationDist);
	updateOrigins();
	updateLegPos(legHomePosition);
}

void Hexapod::updateOrigins() {
	/*originLegPos[0] = {
		(-baseLength / 2) * cos(zAxisTilt) + xAxisAddon ,
		bodyAltitude + (baseWidth / 2) * sin(xAxisTilt) + (-baseLength / 2) * sin(zAxisTilt),
		(-baseWidth / 2) * cos(-xAxisTilt) + zAxisAddon
	};*/
	originLegPos[0] = getOriginPoint(-xAxisTilt, -yAxisTilt, zAxisTilt, 
		-acos(-baseLength/(2* sqrt((baseLength / 2) * (baseLength / 2) + (baseWidth / 2) * (baseWidth / 2)))),
		sqrt((baseLength / 2) * (baseLength / 2) + (baseWidth / 2) * (baseWidth / 2)),
		xAxisAddon, bodyAltitude, zAxisAddon);
	/*originLegPos[0] = getOriginPoint(-xAxisTilt, -yAxisTilt, zAxisTilt, legLB.getLegOrientation(),
		sqrt((baseLength / 2) * (baseLength / 2) + (baseWidth / 2) * (baseWidth / 2)),
		xAxisAddon, bodyAltitude, zAxisAddon);*/
	originLegPos[1] = {
		xAxisAddon,
		bodyAltitude + (baseWidth / 2 + legConfig) * sin(xAxisTilt),
		(-baseWidth / 2 - legConfig) * cos(xAxisTilt) + zAxisAddon
	};
	/*originLegPos[1] = getOriginPoint(-xAxisTilt, -yAxisTilt, zAxisTilt, legLM.getLegOrientation(),
		sqrt(0.0f * 0.0f + (baseWidth / 2 + legConfig) * (baseWidth / 2 + legConfig)),
		xAxisAddon, bodyAltitude, zAxisAddon);*/
	originLegPos[2] = {
		(baseLength / 2) * cos(zAxisTilt) + xAxisAddon,
		bodyAltitude + (baseWidth / 2) * sin(xAxisTilt) + (baseLength / 2) * sin(zAxisTilt),
		(-baseWidth / 2) * cos(-xAxisTilt) + zAxisAddon
	};
	/*originLegPos[2] = getOriginPoint(-xAxisTilt, -yAxisTilt, zAxisTilt, legLF.getLegOrientation(),
		sqrt((baseLength / 2) * (baseLength / 2) + (baseWidth / 2) * (baseWidth / 2)),
		xAxisAddon, bodyAltitude, zAxisAddon);*/
	originLegPos[3] = { 
		(baseLength / 2) * cos(zAxisTilt) + xAxisAddon ,
		bodyAltitude - (baseWidth / 2) * sin(xAxisTilt) + (baseLength / 2) * sin(zAxisTilt),
		(baseWidth / 2) * cos(-xAxisTilt) + zAxisAddon
	};
	originLegPos[4] = {
		xAxisAddon,
		bodyAltitude - (baseWidth / 2 + legConfig) * sin(xAxisTilt),
		(baseWidth / 2 + legConfig) * cos(xAxisTilt) + zAxisAddon
	};
	originLegPos[5] = {
		(-baseLength / 2) * cos(zAxisTilt) + xAxisAddon ,
		bodyAltitude - (baseWidth / 2) * sin(xAxisTilt) + (-baseLength / 2) * sin(zAxisTilt),
		(baseWidth / 2) * cos(-xAxisTilt) + zAxisAddon
	};

	resetOrigins();
}

Point Hexapod::getOriginPoint(float xTilt, float yTilt, float zTilt, float orientation, float L, float xAddon, float yAddon, float zAddon) {
	return {
		cos(zTilt) * cos(orientation + yTilt) * L + xAddon,
		sin(xTilt) * sin(orientation + yTilt) * L + sin(zTilt) * cos(orientation + yTilt) * L + yAddon,
		cos(xTilt) * sin(orientation + yTilt) * L + zAddon
	};
}

void Hexapod::resetOrigins() {
	legLB.setOrigin(originLegPos[0]);
	legLM.setOrigin(originLegPos[1]);
	legLF.setOrigin(originLegPos[2]);
	legRF.setOrigin(originLegPos[3]);
	legRM.setOrigin(originLegPos[4]);
	legRB.setOrigin(originLegPos[5]);

	updateLegPos(legHomePosition);
}

void Hexapod::updateLegPos(Point positions[]) {
	legLB.setPosition(positions[0]);
	legLM.setPosition(positions[1]);
	legLF.setPosition(positions[2]);
	legRF.setPosition(positions[3]);
	legRM.setPosition(positions[4]);
	legRB.setPosition(positions[5]);
}

void Hexapod::setTibiaLen(float tibiaLen) {
	this->tibiaLen = tibiaLen;

	legLB.setTibiaLen(tibiaLen);
	legLM.setTibiaLen(tibiaLen);
	legLF.setTibiaLen(tibiaLen);
	legRB.setTibiaLen(tibiaLen);
	legRM.setTibiaLen(tibiaLen);
	legRF.setTibiaLen(tibiaLen);
}

void Hexapod::setFemurLen(float femurLen) {
	this->femurLen = femurLen;

	legLB.setFemurLen(femurLen);
	legLM.setFemurLen(femurLen);
	legLF.setFemurLen(femurLen);
	legRB.setFemurLen(femurLen);
	legRM.setFemurLen(femurLen);
	legRF.setFemurLen(femurLen);
}

void Hexapod::setBaseLength(float baseLength) {
	this->baseLength = baseLength;
	updateOrigins();
}

void Hexapod::setBaseWidth(float baseWidth) {
	this->baseWidth = baseWidth;
	updateOrigins();
}

void Hexapod::setLegConfig(float legConfig, bool hexagonalConfig) {
	this->legConfig = legConfig;
	this->hexagonalConfig = hexagonalConfig;
	resetOrientations(hexagonalConfig);
	updateOrigins();
	updateLegPos(legHomePosition);
}

void Hexapod::setGaitSettings(float bodyAltitude, float stepLocationDist) {
	this->bodyAltitude = bodyAltitude;
	this->stepLocationDist = stepLocationDist;
	resetOrientations(hexagonalConfig);

	legHomePosition[0] = {
		cos(legOrientation[0]) * stepLocationDist - baseLength / 2,
		0.0f,
		sin(legOrientation[0]) * stepLocationDist - baseWidth / 2
	};
	legHomePosition[1] = {
		0.0f, 0.0f,
		-legConfig - stepLocationDist - baseWidth / 2
	};
	legHomePosition[2] = {
		cos(legOrientation[2]) * stepLocationDist + baseLength / 2,
		0.0f,
		sin(legOrientation[2]) * stepLocationDist - baseWidth / 2
	};
	legHomePosition[3] = {
		cos(legOrientation[3]) * stepLocationDist + baseLength / 2,
		0.0f,
		sin(legOrientation[3]) * stepLocationDist + baseWidth / 2
	};
	legHomePosition[4] = {
		0.0f, 0.0f,
		stepLocationDist + legConfig + baseWidth / 2
	};
	legHomePosition[5] = {
		cos(legOrientation[5]) * stepLocationDist - baseLength / 2,
		0.0f,
		sin(legOrientation[5]) * stepLocationDist + baseWidth / 2
	};

	updateLegPos(legHomePosition);
	updateOrigins();
}

void Hexapod::setGCenterAndTilt(float xAxisAddon, float zAxisAddon, float xAxisTilt, float yAxisTilt, float zAxisTilt) {
	this->xAxisAddon = xAxisAddon; 
	this->zAxisAddon = zAxisAddon;
	this->xAxisTilt = xAxisTilt;
	this->yAxisTilt = yAxisTilt;
	this->zAxisTilt = zAxisTilt;


	updateOrigins();
	updateLegPos(legHomePosition);
}

void Hexapod::resetOrientations(bool hexagonalConfiguration) {
	legOrientation[0] = (hexagonalConfiguration) ? -1.5708f - atan(1.0f * legConfig / (baseLength / 2.0f)) : -1.57075f;
	legOrientation[1] = -1.57075f;
	legOrientation[2] = (hexagonalConfiguration) ? -1.5708f + atan(1.0f * legConfig / (baseLength / 2.0f)) : -1.57075f;
	legOrientation[3] = (hexagonalConfiguration) ? 1.5708f - atan(1.0f * legConfig / (baseLength / 2.0f)) : 1.57075f;
	legOrientation[4] = 1.57075f;
	legOrientation[5] = (hexagonalConfiguration) ? 1.5708f + atan(1.0f * legConfig / (baseLength / 2.0f)) : 1.57075f;
	
	legLB.setLegOrientation(legOrientation[0]);
	legLM.setLegOrientation(legOrientation[1]);
	legLF.setLegOrientation(legOrientation[2]);
	legRF.setLegOrientation(legOrientation[3]);
	legRM.setLegOrientation(legOrientation[4]);
	legRB.setLegOrientation(legOrientation[5]);


	updateLegPos(legHomePosition);
}

void Hexapod::draw() {
	gl::color(CYAN);

	bodyElement.height(baseLength);
	bodyElement.radius(0.5f);
	for (int i = 1; i < 6; i++) {
		bodyElement.set(vec3(originLegPos[i - 1].x, originLegPos[i - 1].y, originLegPos[i - 1].z), vec3(originLegPos[i].x, originLegPos[i].y, originLegPos[i].z));
		gl::draw(bodyElement);
	}
	bodyElement.set(vec3(originLegPos[5].x, originLegPos[5].y, originLegPos[5].z), vec3(originLegPos[0].x, originLegPos[0].y, originLegPos[0].z));
	gl::draw(bodyElement);
	
	legLB.draw();
	legLM.draw();
	legLF.draw();
	legRB.draw();
	legRM.draw();
	legRF.draw();

	gl::color(1, 0, 0);
	for (int i = 0; i < 6; i++)
		gl::drawCube(vec3(legPos[i].x, legPos[i].y, legPos[i].z), vec3(0.5));
}