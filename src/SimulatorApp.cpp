#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/params/Params.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"
#include "cinder/ip/Checkerboard.h"
#include "Hexapod.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class SimulatorApp : public App {
	public:
		void setup() override;	// Runs once setting up everything
		void resize() override;	// Used for when we are resizing the window
		void update() override;	// Updates every tick
		void draw() override;	// Draws every tick after the update

	private:
		CameraPersp cam;			// Used to represent the camera
		float baseLength, baseWidth;// Represents the base dimensions
		float baseHeight;			// Represents the base height (not that important here)
		float femurLen, tibiaLen;	// Represents the leg dimentions
		float middleLegPosExt;		// Represents the robot's leg configuration. 0 means all legs are in line "E".
		bool hexagonalConfig;		// Use to determine if legs are angled or in line in base position
		float desiredLevel;			// Represents the gait altitude
		float stepLocationDist;		// Represents the home location of the leg in terms of distance from the base
		Hexapod *robot;				// Represents the robot.
		float xAxisAddon, xAxisTilt;// Represents the addon and tilt in terms of the x axis for testing the kinematic model
		float zAxisAddon, zAxisTilt;// Represents the addon and tilt in terms of the z axis for testing the kinematic model
		float yAxisTilt;			// Represents the tilt for the y axis. The addon is the desiredLevel.

		gl::BatchRef floor;			// Used to represent the floor
		gl::TextureRef texture;		// Used to form the checkered pattern on the floor

		const ColorA GRAY = ColorA(0.9f, 0.9f, 0.9f);	// Constant representing the color GRAY
};

void SimulatorApp::setup() {
	gl::clear(Colorf::white());	// Resets
	// Turn on z-buffering (Depth Buffering)
	gl::enableDepthRead();
	gl::enableDepthWrite();

	// Initializing the GUI handler
	#if ! defined( CINDER_GL_ES )
		ImGui::Initialize();
	#endif

	// Setting the camera rotation and orientation
	cam.lookAt(vec3(-35, 30, -35), vec3(0, 1, -20));
	
	// Setting all the variable default values
	femurLen = 10.0;
	tibiaLen = 10.0;
	baseLength = 30.0f;
	baseWidth = 10.0f;
	baseHeight = 5.0f;
	middleLegPosExt = 0.0f;
	desiredLevel = 4.0f;
	stepLocationDist = 5.0f;
	hexagonalConfig = true;

	xAxisAddon = xAxisTilt = 0.0f;
	zAxisAddon = zAxisTilt = 0.0f;
	yAxisTilt = 0.0f;

	// Forming the checkered pattern
	auto fmt = gl::Texture::Format().mipmap().minFilter(GL_LINEAR_MIPMAP_LINEAR).magFilter(GL_LINEAR).wrap(GL_REPEAT);
	texture = gl::Texture::create(ip::checkerboard(1024, 1024, 16, Colorf::gray(0.3f), Colorf::gray(0.7f)), fmt);
	// Forming the shader for the floor
	auto floorShader = gl::getStockShader(gl::ShaderDef().color().texture(texture));
	floor = gl::Batch::create(geom::Plane().size(vec2(512)).normal(vec3(0, 1, 0)), floorShader);

	// Defining the instance of our hexapod
	robot = new Hexapod(baseLength, baseHeight, baseWidth, femurLen, tibiaLen, { 0.5, 0 });
	robot->setLegConfig(middleLegPosExt, hexagonalConfig);
	robot->setGaitSettings(desiredLevel, stepLocationDist);
}

void SimulatorApp::update() {	
	// GUI representation and information gathering
	#if ! defined( CINDER_GL_ES )
		ImGui::Begin("App Parameters ");	// Starts a menu for the app named app parameters
		bool changed = false;
		ImGui::Text("Base Settings");		// Se settings for the base of the robot and leg configuration
		changed |= ImGui::DragFloat("Base Length", &baseLength, 0.1f, 8.0f, 30.0f);
		changed |= ImGui::DragFloat("Base Width", &baseWidth, 0.1f, 4.0f, 20.0f);
		changed |= ImGui::DragFloat("Middle legs config", &middleLegPosExt, 0.1f, 0.0f, (sqrt(3.0f) * baseLength) / 6.0f);
		changed |= ImGui::Checkbox("Hexagonal Body", &hexagonalConfig);
		// ////////////////////////////////////////////////
		
		ImGui::Text("Leg Settings");		// Setting the leg settings
		changed |= ImGui::DragFloat("Femur Length", &femurLen, 0.1f, 0.0f, 30.0f);
		changed |= ImGui::DragFloat("Tibia Length", &tibiaLen, 0.1f, 0.0f, 30.0f);
		// ///////////////////////////////////////////////
		ImGui::Text("Gait Settings");		// Setting how tall the robot walks and how far the legs are stepping
		changed |= ImGui::DragFloat("Gait alt. level", &desiredLevel, 0.1f, 3.0f, 3.0f * (tibiaLen + femurLen) / 4.0f);
		changed |= ImGui::DragFloat("Step Home Location", &stepLocationDist, 0.1f, 5.0f, 15.0f);
		/////////////////////////////////////////////////
		ImGui::Text("Testing kinematic model for moving the center of gravity");	
		changed |= ImGui::DragFloat("Back to Front", &xAxisAddon, 0.1f, -3.0f, 3.0f);
		changed |= ImGui::DragFloat("Side to side", &zAxisAddon, 0.1f, -3.0f, 3.0f);
		ImGui::Text("Testing kinematic model for tilt around the center of gravity");
		changed |= ImGui::DragFloat("X-Axis tilt", &xAxisTilt, 0.05f, -3.1415f / 7.0f, 3.1415f / 7.0f);
		changed |= ImGui::DragFloat("Y-Axis tilt", &yAxisTilt, 0.1f, -3.1415f / 8.0f, 3.1415f / 8.0f);
		changed |= ImGui::DragFloat("Z-Axis tilt", &zAxisTilt, 0.05f, -3.1415f / 7.0f, 3.1415f / 7.0f);
		ImGui::Text("**Note: X axis is front to back and Z axis is left to right in terms of the robot");
		if (changed) {
			if ((sqrt(3.0f) * baseLength) / 6.0f < middleLegPosExt)
				middleLegPosExt = (sqrt(3.0f) * baseLength) / 6.0f;

			robot->setBaseLength(baseLength);
			robot->setBaseWidth(baseWidth);
			robot->setLegConfig(middleLegPosExt, hexagonalConfig);

			robot->setTibiaLen(tibiaLen);
			robot->setFemurLen(femurLen);
			robot->setGaitSettings(desiredLevel, stepLocationDist);

			robot->setGCenterAndTilt(xAxisAddon, zAxisAddon, xAxisTilt, yAxisTilt, zAxisTilt);
		}

		ImGui::End();
	#endif
}

void SimulatorApp::draw() {
	gl::clear(Colorf::white());

	{
		gl::ScopedTextureBind tex0{ texture, 0 };
		gl::ScopedDepth depth{ true };
		gl::ScopedColor color{ Colorf::white() };
		floor->draw();

		gl::drawCoordinateFrame(10.0f);
	}
	// Turn on z-buffering (Depth Buffering)
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::enableFaceCulling();
	
	gl::setMatrices(cam);
	
	// Forming the shader for the robot's body			
	auto lambert = gl::ShaderDef().lambert().color();
	auto shader = gl::getStockShader(lambert);
	shader->bind();

	// Drawing the robot
	robot->draw();
}

void SimulatorApp::resize() {
	cam.setPerspective(60, getWindowAspectRatio(), 1, 1000);
	gl::setMatrices(cam);
}

void prepareSettings(App::Settings* settings) {
	settings->setWindowSize(1280, 720);
}

CINDER_APP( SimulatorApp, RendererGl, prepareSettings)
