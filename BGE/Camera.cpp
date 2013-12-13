#include "Camera.h"
#include "Game.h"

#include <iostream>
#include <string>
#include <sstream>

using namespace BGE;
using namespace std;
#include <SDL.h>

Camera::Camera(void):GameComponent()
{
	projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000000.0f);		
	cameraType = camera_types::normal;
	worldMode = world_modes::from_child;
}

Camera::~Camera(void)
{
}

bool Camera::Initialise()
{
	SDL_WarpMouseInWindow(
		Game::Instance()->GetMainWindow()
		,Game::Instance()->GetWidth() / 2
		,Game::Instance()->GetHeight() / 2);
	SDL_ShowCursor(SDL_DISABLE); 
	camPosFromObj = glm::vec3(0);
	centerAt = glm::vec3(0);
	return GameComponent::Initialise();
}

void Camera::Draw()
{
	GameComponent::Draw();
}

void Camera::FollowObject(shared_ptr<GameComponent> followObject, float distance, float height, float distanceInFront)
{
	this->followObject = followObject;
	this->position = followObject->position;
	followVariables.x = distance;
	followVariables.y = height;
	followVariables.z = distanceInFront;
	cameraType = camera_types::follow_behind;
}
//Accepts a radians angle, the higher the interpolation the smoother the transition
void Camera::PFollowObject(shared_ptr<GameComponent> followObject, glm::vec3 distance, glm::vec3 degreesAngle, float lookMultiplier, float interpolation)
{
	this->followObject = followObject;
	this->position = followObject->position;
	pfollowVariables.distance = distance; //width,height,length
	pfollowVariables.angles = degreesAngle;
	pfollowVariables.lookMultiplier = lookMultiplier;
	pfollowVariables.interpolation = interpolation;
	cameraType = camera_types::follow_perspective;
}
//Stops the following
void Camera::UnFollow()
{
	if(followObject)
	{
		this->position = position + glm::vec3(-10,5,0) * this->followObject->look;
		this->followObject = nullptr;
	}
	cameraType = camera_types::normal;
}
//Gets the distance from the object with angle encorporated
glm::vec3 Camera::GetDistanceWithAngle()
{
	glm::vec3 result = glm::vec3(0);
	
	result.x = pfollowVariables.distance.x * glm::cos(pfollowVariables.angles.x);
	result.y = pfollowVariables.distance.y;
	result.z = pfollowVariables.distance.z * glm::sin(pfollowVariables.angles.z);

	return result;
}

void Camera::Update(float timeDelta) {
	
	GameComponent::Update(timeDelta);

	if(!followObject)
	{
		cameraType = camera_types::normal;
	}

	float interp = 0.85f;
	glm::vec3 fakeCamPos = glm::vec3(0);
	switch(cameraType)
	{
	case camera_types::normal:
		centerAt = position + look;
		break;
	case camera_types::follow_behind:
		fakeCamPos = followObject->look * followVariables.x;
		fakeCamPos.y = followVariables.y;

		if(glm::distance(fakeCamPos, camPosFromObj) > glm::epsilon<float>())
		{
			camPosFromObj = interp * camPosFromObj + (1 - interp) * fakeCamPos;
		}

		position = followObject->position + camPosFromObj;

		interp = 0.75f;
		newCenterAt = followObject->position + (followObject->look * followVariables.z);
		if(glm::distance(newCenterAt, centerAt) > glm::epsilon<float>())
		{
			centerAt = interp * centerAt + (1 - interp) * newCenterAt;
		}
		break;
	case camera_types::follow_perspective:
		position = followObject->position + this->GetDistanceWithAngle();
		newCenterAt = followObject->position + followObject->look * glm::vec3(pfollowVariables.lookMultiplier);
		interp = pfollowVariables.interpolation * (1 - glm::distance(position, centerAt) / glm::length(this->GetDistanceWithAngle()));
		if(glm::distance(newCenterAt, centerAt) > glm::epsilon<float>())
		{
			centerAt = interp * centerAt + (1 - interp) * newCenterAt;
		}
		break;
	}
	
	view = glm::lookAt(
		position
		, centerAt + look
		, basisUp
		);


	stringstream ss;
	ss << "Camera Pos: " << position.x << " " << position.y << " " << position.z;
	Game::Instance()->PrintText(ss.str());

	//ss.str("");
	//ss << "Camera Look: " << look.x << " " << look.y << " " << look.z;
	//Game::Instance()->PrintText(ss.str());

	//ss.str("");
	//ss << "Camera Up: " << up.x << " " << up.y << " " << up.z;
	//Game::Instance()->PrintText(ss.str());

	//ss.str("");
	//ss << "Camera Right: " << right.x << " " << right.y << " " << right.z;
	//Game::Instance()->PrintText(ss.str());

}