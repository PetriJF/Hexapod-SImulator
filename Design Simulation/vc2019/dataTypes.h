#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/params/Params.h"
#include "cinder/gl/gl.h"

struct ShoulderOffset {
	float offsetShoulderHipX;
	float offsetShoulderHipY;
};

struct Point {
	float x, y, z;
};