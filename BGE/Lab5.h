#pragma once
#include "Game.h"
#include "GameComponent.h"
#include "FountainEffect.h"

using namespace std;

namespace BGE
{
	class Lab5 :
		public Game
	{
	public:
		Lab5(void);
		~Lab5(void);

		shared_ptr<GameComponent> ship1;
		shared_ptr<GameComponent> ship2;
		float elapsed;
		bool Initialise();
		void Update(float timeDelta);
		vector<shared_ptr<FountainEffect>> fountains;
		float fountainTheta;
	};
}

