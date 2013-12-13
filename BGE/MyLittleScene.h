#include "Game.h"
#include "GameComponent.h"
#include "PhysicsController.h"
#include "PhysicsFactory.h"
#include <btBulletDynamicsCommon.h>

using namespace std;

namespace BGE
{
        class MyLittleScene :
                public Game
        {
		private:
			btBroadphaseInterface* broadphase;
 
			// Set up the collision configuration and dispatcher
			btDefaultCollisionConfiguration * collisionConfiguration;
			btCollisionDispatcher * dispatcher;
 
			// The actual physics solver
			btSequentialImpulseConstraintSolver * solver;

        public:
                MyLittleScene(void);
                ~MyLittleScene(void);
				
				shared_ptr<PhysicsFactory> physicsFactory;
				btDiscreteDynamicsWorld * dynamicsWorld;
                shared_ptr<GameComponent> ship1;
                float elapsed;
				float lastSpawn;
				int objHit;
				std::list<shared_ptr<GameComponent>> objects;
				glm::vec3 force;
				glm::vec3 newLook;
				float mass;
                bool Initialise();
                void Update(float timeDelta);
        };
}