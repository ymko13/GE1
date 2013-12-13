#include "Steerable3DController.h"
#include "Content.h"
#include "Model.h"
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/matrix_transform.hpp>
#include <sstream>
#include <string>

using namespace BGE;
using namespace std;

Steerable3DController::Steerable3DController(shared_ptr<Model> model):GameComponent()
{
	mass = 10.0f;
	velocity = glm::vec3(0);
	force = glm::vec3(0);
	acceleration = glm::vec3(0);
	angularAcceleration = glm::vec3(0);
	angularVelocity = glm::vec3(0);
	torque = glm::vec3(0);
	worldMode = world_modes::to_parent;
	this->model = model;
}

Steerable3DController::~Steerable3DController(void)
{
}

bool Steerable3DController::Initialise()
{
	if (!model->initialised)
	{
		model->Initialise();
	}
	CalculateInertiaTensor();
	
	GameComponent::Initialise();
	return true;
}

void Steerable3DController::CalculateInertiaTensor() { 
	float width = model->boundingBox.max.x - model->boundingBox.min.x;
	float height = model->boundingBox.max.y - model->boundingBox.min.y;
	float depth = model->boundingBox.max.z - model->boundingBox.min.z;

	inertialTensor[0][0] = (float) (mass * (pow(height, 2) + pow(depth, 2))) / 12.0f;
    inertialTensor[1][1] = (float) (mass * (pow(width, 2) + pow(depth, 2))) / 12.0f;
    inertialTensor[2][2] = (float) (mass * (pow(width, 2) + pow(height, 2))) / 12.0f;
}

void Steerable3DController::AddForce(glm::vec3 force)
{
    this->force += force;
}

void Steerable3DController::AddTorque(glm::vec3 torque)
{
    this->torque += torque;
}

void Steerable3DController::AddForceAtPoint(glm::vec3 force, glm::vec3 point)
{
	glm::vec3 to = glm::cross(force, point);
    torque += to;

    force += force;
}

void Steerable3DController::Draw()
{
	GameComponent::Draw();
}


void Steerable3DController::Update(float timeDelta)
{
	const Uint8 * keyState = Game::Instance()->GetKeyState();

	float scale = 1000.0f;
	if (keyState[SDL_SCANCODE_SPACE])
    {
        AddForce(look * scale * timeDelta * 5.0f);
    }

    // Yaw
	if (keyState[SDL_SCANCODE_LEFT])
    {
		AddTorque(up * scale * timeDelta * 2.0f);
    }
    if (keyState[SDL_SCANCODE_RIGHT])
    {
        AddTorque(- up * scale * timeDelta * 2.0f);
    }
    // End of Yaw

    //Pitch
    if (keyState[SDL_SCANCODE_UP])
    {
        AddTorque(right * scale * timeDelta / 20.0f);
    }
    if (keyState[SDL_SCANCODE_DOWN])
    {
        AddTorque(- right * scale * timeDelta / 20.0f);
    }
    // End of Pitch

	// Roll
    if (keyState[SDL_SCANCODE_P])
    {
        AddTorque(look * scale * timeDelta);
    }

    if (keyState[SDL_SCANCODE_L])
    {
        AddTorque(- look * scale * timeDelta);
    }

    // Do the Newtonian integration
    acceleration = force / mass;
    velocity += acceleration * timeDelta;
    position += velocity * timeDelta;
	
    if (glm::length(velocity) > 0.0001f)
    {
		look = glm::normalize(velocity);
        right = glm::cross(look, up);
        velocity *= 0.99f;
    }

    // Do the Hamiltonian integration
	angularAcceleration = torque * glm::inverse(inertialTensor);
    angularVelocity = angularVelocity + angularAcceleration * timeDelta;
	angularVelocity *= 0.99f;

    glm::quat w = glm::quat(0, angularVelocity.x, angularVelocity.y, angularVelocity.z);

	orientation = orientation + ((w * (timeDelta / 2.0f)) * orientation);
	orientation = glm::normalize(orientation);
    torque = glm::vec3(0);
	force = glm::vec3(0);

	look = RotateVector(basisLook, orientation);
	up = RotateVector(basisUp, orientation);
	right = RotateVector(basisRight, orientation);

	GameComponent::Update(timeDelta);
}
