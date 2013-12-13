#pragma once
#include "GameComponent.h"

using namespace std;

namespace BGE {
	class Camera:public GameComponent {
	public:
		Camera(void);
		~Camera(void);
		void Update(float timeDelta);
		void Draw();
		bool Initialise();
		void FollowObject(shared_ptr<GameComponent> followObject, float distance, float height, float distanceInFront);
		void PFollowObject(shared_ptr<GameComponent> followObject, glm::vec3 distance, glm::vec3 radiansAngle, float lookMultiplier, float interpolation);
		void UnFollow();
		
		enum camera_types {normal,follow_behind, follow_perspective};
		camera_types cameraType;
		
		glm::mat4 view;
		glm::mat4 projection;
		
		
		shared_ptr<GameComponent> followObject;
		glm::vec3 centerAt;
		glm::vec3 newCenterAt; 
		//Follow perspective camera
		struct pfollow_vars {
			glm::vec3 distance; //Distance from the object
			glm::vec3 angles; //Angles x,y,z axis
			float lookMultiplier; //Look multiplier to create a nice effect when object moves
			float interpolation;
		};
		pfollow_vars pfollowVariables;

		glm::vec3 followVariables; //Distance - x, Height - y, Distance from center to center the camera on(a multiple of look vector) - z
		glm::vec3 camPosFromObj;
	private:
		glm::vec3 GetDistanceWithAngle();
	};
}

