#include "MyLittleScene.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "LazerBeam.h"
#include "Steerable3DController.h"
#include "PhysicsController.h"
#include "PhysicsFactory.h"
#include <btBulletDynamicsCommon.h>
#include "Utils.h"

using namespace BGE;

MyLittleScene::MyLittleScene(void)
{
        elapsed = 0;
}


MyLittleScene::~MyLittleScene(void)
{
}

bool MyLittleScene::Initialise()
{
	
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);

		// The world.
		btVector3 worldMin(-1000,-1000,-1000);
		btVector3 worldMax(1000,1000,1000);
		broadphase = new btAxisSweep3(worldMin,worldMax);
		solver = new btSequentialImpulseConstraintSolver();
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
		dynamicsWorld->setGravity(btVector3(0,-9,0));
		
		physicsFactory = make_shared<PhysicsFactory>(dynamicsWorld);

		physicsFactory->CreateGroundPhysics();
		//physicsFactory->CreateCameraPhysics();

        std::shared_ptr<GameComponent> ground = make_shared<Ground>();
        Attach(ground);        

        ship1 = make_shared<GameComponent>();
		shared_ptr<Model> model = Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0)));
        ship1->Attach(model);
        ship1->position = glm::vec3(20, 5, 20); 
        //ship1->Attach(make_shared<VectorDrawer>());
		//shared_ptr<PhysicsController> pcontroller = physicsFactory->CreateFromModel("cobramk3", glm::vec3(), glm::quat(), glm::vec3(1.5f,1.5f,1.5f));
		//ship1->Attach(pcontroller);
        Attach(ship1);
		
		//This sets the camera to follow from behind
		//gamecomponent to follow, distance from model to camera, height from model to camera, distance how far from the object the camera looks.
		camera->FollowObject(ship1, -12.0f, 5.0f, 2.0f);
		//This sets the camera to follow in a perspective
		//camera->PFollowObject(ship1, glm::vec3(20,5,0), glm::vec3(glm::radians(20.0f), 0, glm::radians(60.0f)), 1.0f, 0.98f);

        riftEnabled = false;
        fullscreen = false;
        width = 800;
        height = 600;
		mass = 1;
        ship1->velocity = glm::vec3(0);
		newLook = glm::vec3(0);
        Game::Initialise();
		objHit = 0;
        //camera->GetController()->position = glm::vec3(0, 4, 20);

        return true;
}

void MyLittleScene::Update(float timeDelta)
{   
	if(keyState[SDL_SCANCODE_P])
	{
		camera->PFollowObject(ship1, glm::vec3(20,5,0), glm::vec3(glm::radians(20.0f), 0, glm::radians(60.0f)), 1.0f, 0.98f);
	}
	if(keyState[SDL_SCANCODE_B])
	{
		camera->FollowObject(ship1, -12.0f, 5.0f, 2.0f);
	}
	if(keyState[SDL_SCANCODE_F])
	{
		camera->UnFollow();
	}
	if ((keyState[SDL_SCANCODE_SPACE]) && (elapsed > 0.8f))
	{
		glm::vec3 pos = ship1->position + (ship1->look * 1.0f);
		glm::quat q(RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat());
		glm::normalize(q);
		shared_ptr<PhysicsController> physicsComponent = physicsFactory->CreateBox(1,1,1, pos, q);
		
		float force = 3000.0f;
		physicsComponent->rigidBody->applyCentralForce(GLToBtVector(ship1->look) * force);
		elapsed = 0.0f;
	}
	else
	{
		elapsed += timeDelta;
	}

	if(lastSpawn > 20.0f)
	{
		glm::vec3 pos = ship1->position + glm::vec3(RandomFloat() * 100.0f,RandomFloat() * 100.0f,RandomFloat() * 100.0f);
		shared_ptr<PhysicsController> physicsComponent = physicsFactory->CreateBox(5,5,5, pos, glm::quat());
		objects.push_back(physicsComponent);
		lastSpawn = 0.0f;
	}
	else
	{
		lastSpawn += timeDelta;
	}

	list<shared_ptr<GameComponent>>::iterator it = objects.begin();

	for(it; it != objects.end(); it++)
	{
		//checking if collision happened here...
	}

	float newtons = 15.0f;
    float epsilon = glm::epsilon<float>();
    if (keyState[SDL_SCANCODE_UP])
    {
            force += ship1->look * newtons;
    }
    if (keyState[SDL_SCANCODE_DOWN])
    {
            force -= ship1->look * newtons;
    }
    if (keyState[SDL_SCANCODE_LEFT])
    {
            force -= ship1->right * newtons;
    }
    if (keyState[SDL_SCANCODE_RIGHT])
    {
            force += ship1->right * newtons;
    }
    // Now calculate the acceleration, new velocity and new position
    glm::vec3 accel = force / mass;
	ship1->velocity += accel * timeDelta;
    ship1->position += ship1->velocity * timeDelta;
    // Check if the velocity length is > epsilon and if so create the look vector from the velocity
    if (glm::length(ship1->velocity) > epsilon)
    {
		ship1->look = glm::normalize(ship1->velocity);        
    }
    // Now check to see if the |look - basis| > epsilon
    // And if so calculate the quaternion
    if (glm::length(ship1->look - GameComponent::basisLook) > epsilon)
    {
            glm::vec3 axis = glm::cross(GameComponent::basisLook, ship1->look);
            axis = glm::normalize(axis);
            float theta = glm::acos(glm::dot(ship1->look, GameComponent::basisLook));
            ship1->orientation = glm::angleAxis(glm::degrees(theta), axis);
    }
    // Apply damping
    ship1->velocity *= 0.99f;
    // Reset the force accumulator
    force = glm::vec3(0,0,0);
	elapsed+=timeDelta;
	lastSpawn+=timeDelta;
	dynamicsWorld->stepSimulation(timeDelta,100);

	stringstream ss;
	ss << "Objects Hit: " << objHit;
	Game::Instance()->PrintText(ss.str());

	Game::Update(timeDelta);
}