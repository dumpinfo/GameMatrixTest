#include "App.h"

using namespace Leadwerks;

App::App() : window(NULL), context(NULL), world(NULL), camera(NULL) {}

App::~App() { delete world; delete window; }

Vec3 camerarotation;
Entity* spectator;

bool App::Start()
{
	//Create a window
	window = Window::Create("Vegetation Sample",0,0,1024,768);

	//Create a rendering context
	context = Context::Create(window);
	
	//Create a world
	world = World::Create();

	//Create camera
	camera = Camera::Create();
	camera->Move(0, 0, -3);
	camera->SetSkybox("Materials/Sky/skybox_texture.tex");
	camera->SetFOV(70);

	//Spectator entity
	spectator = Pivot::Create();
	spectator->SetGravityMode(false);
	spectator->SetMass(1);
	Shape* shape = Shape::Sphere();
	spectator->SetShape(shape);
	shape->Release();
	spectator->SetPosition(224, 56, -500);

	//Load a terrain
	Map::Load("Maps/terrain.map");

	//Create the vegetation layers
	VegetationLayer* layer;
	
	//Trees
	layer = new VegetationLayerSample(world->terrain);
	layer->SetModel("Models/Pine-9m-Fresh/vegetation_pines_fresh_pine9m.mdl");
	layer->SetDensity(8);
	layer->SetSlopeConstraints(0,35);
	layer->SetBillboardDistance(30);
	layer->SetViewRange(300);
	
	//Bushes
	layer = new VegetationLayerSample(world->terrain);
	layer->SetModel("Models/bush-needles-c/bush-needles-c.mdl");
	layer->SetDensity(8);
	layer->SetBillboardDistance(20);
	layer->SetSlopeConstraints(0,35);
	layer->SetViewRange(100);

	//Grass
	layer = new VegetationLayerSample(world->terrain);
	layer->SetModel("Models/grass-and-grass-dry/grass.mdl");
	layer->SetDensity(2);
	layer->SetColor(1,1);
	layer->SetShadowMode(0);
	layer->SetSlopeConstraints(0,20);
	layer->SetBillboardDistance(10);
	layer->SetViewRange(50);
	
	//Hide the mouse cursor
	window->HideMouse();

	//Move the mouse to the center of the screen
	window->SetMousePosition(context->GetWidth() / 2, context->GetHeight() / 2);

	return true;
}

bool App::Loop()
{
	//Close the window and end the program
	if (window->Closed()) return false;
	if (window->KeyHit(Key::Escape)) return false;

	//Camera movement
	Vec3 movement;
	float force = 100 * Time::GetSpeed();
	if (window->KeyDown(Key::W)) movement.z += force;
	if (window->KeyDown(Key::S)) movement.z -= force;
	if (window->KeyDown(Key::A)) movement.x -= force;
	if (window->KeyDown(Key::D)) movement.x += force;
	movement = Transform::Vector(movement,camera,NULL);
	spectator->AddForce(movement);
	spectator->SetVelocity(spectator->GetVelocity()*0.98);

	//Get the mouse movement
	float sx = context->GetWidth() / 2;
	float sy = context->GetHeight() / 2;
	Vec3 mouseposition = window->GetMousePosition();
	float dx = mouseposition.x - sx;
	float dy = mouseposition.y - sy;

	//Adjust and set the camera rotation
	camerarotation.x += dy / 10.0;
	camerarotation.y += dx / 10.0;
	camera->SetRotation(camerarotation);
	camera->SetPosition(spectator->GetPosition());

	//Move the mouse to the center of the screen
	window->SetMousePosition(sx, sy);

	//Update world
	Time::Update();
	world->Update();

	//Render world
	world->Render();

	//Show FPS
	context->SetBlendMode(Blend::Alpha);
	context->DrawText(String(Time::UPS()), 0, 0);
	if (window->KeyDown(Key::F11))
	{
		context->DrawText(String(world->stats_vegetationculling)+" instances culled", 0, 20);
		context->DrawText(String(world->stats_vegetationinstances) + " instances drawn", 0, 40);
	}
	context->SetBlendMode(Blend::Solid);

	//Swap buffers
	context->Sync(false);

	return true;
}