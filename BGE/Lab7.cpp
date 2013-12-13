#include "Lab7.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "LazerBeam.h"
#include "FountainEffect.h"

using namespace BGE;

Lab7::Lab7(void)
{
	elapsed = 10000;
}


Lab7::~Lab7(void)
{
}

bool Lab7::Initialise()
{
	std::shared_ptr<GameComponent> ground = make_shared<Ground>();
	Attach(ground);	

	ship1 = make_shared<GameComponent>();
	ship1->Attach(Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	ship1->position = glm::vec3(-10, 2, -10);
	ship1->Attach(make_shared<VectorDrawer>());
	Attach(ship1);

	ship2 = make_shared<GameComponent>();
	ship2->Attach(Content::LoadModel("ferdelance", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	ship2->Attach(make_shared<VectorDrawer>());
	ship2->diffuse= glm::vec3(1.0f,0.0f,0.0f);
	ship2->specular = glm::vec3(1.2f, 1.2f, 1.2f);

	ship2->position = glm::vec3(10, 2, -10);
	Attach(ship2);

	riftEnabled = false;
	fullscreen = false;
	width = 800;
	height = 600;

	slerping = false;
	t = 0;

	Game::Initialise();

	camera->GetController()->position = glm::vec3(0, 4, 20);
	return true;
}

void Lab7::Update(float timeDelta)
{	
	// Movement of ship2
	if (keyState[SDL_SCANCODE_UP])
	{
		ship2->Walk(speed * timeDelta);
	}
	if (keyState[SDL_SCANCODE_DOWN])
	{
		ship2->Walk(-speed * timeDelta);
	}
	if (keyState[SDL_SCANCODE_LEFT])
	{
		ship2->Yaw(timeDelta * speed * speed);
	}
	if (keyState[SDL_SCANCODE_RIGHT])
	{
		ship2->Yaw(-timeDelta * speed * speed);
	}

	if (keyState[SDL_SCANCODE_O])
	{
		ship2->Fly(timeDelta * speed);
	}

	if (keyState[SDL_SCANCODE_L])
	{
		ship2->Fly(-timeDelta * speed);
	}

	// Put code for ship1 here!!!

	glm::vec3 toShip = ship2->position - ship1->position;
	toShip = glm::normalize(toShip);
	float theta = glm::dot(GameComponent::basisLook, toShip);
	glm::vec3 crossP = glm::normalize(glm::cross(GameComponent::basisLook, toShip));
	fromQuaternion = ship1->orientation;

	if(slerping)
	{
		t += timeDelta;
		if(t >= 1)
		{
			t = 0;
			slerping = false;
		}
		ship1->orientation = glm::mix(fromQuaternion, toQuaternion, t);
	}
	else
		t = 0;
	
	if(keyState[SDL_SCANCODE_SPACE])
	{
		slerping = true;
		toQuaternion = glm::angleAxis(glm::degrees(glm::acos(theta)), crossP);
	}
	// End code for ship 1
	Game::Update(timeDelta);

}
