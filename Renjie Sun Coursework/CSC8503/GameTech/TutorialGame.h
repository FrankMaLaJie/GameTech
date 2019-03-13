#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"


namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();
			void FindBall1();
			void FindBall2();

			void MainMenu();
			void FakeCamera();
			void InitWorld();//level 1
			void InitWorld2();//level 2
			int curScene = 0;
			

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitSphereCollisionTorqueTest();
			void InitCubeCollisionTorqueTest();
			void InitSphereAABBTest();
			void InitGJKWorld();
			void BridgeConstraintTest();
			void SimpleGJKTest();
			void SimpleAABBTest();
			void SimpleAABBTest2();

			bool SelectObject();
			void MoveSelectedObject();
			void Line();
			bool AddForce = false;
			Vector3 ThePoint;

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* Ball1 = nullptr;
			GameObject* Ball2 = nullptr;
			GameObject* GoalBall = nullptr;
			GameObject* EnemyBall = nullptr;
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			
			GameObject* MoveWall1 = nullptr;
			GameObject* MoveWall2 = nullptr;
			GameObject* MoveWall3 = nullptr;
			GameObject* line = nullptr;

			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;
			int			ClickNum = 100;
			float		BallDistance;

			GameObject* selectionObject = nullptr;

			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLTexture* wallTex		= nullptr;
			OGLTexture* ballTex		= nullptr;
			OGLShader*	basicShader = nullptr;

			Vector3 Ball1Position;
			Transform Ball1Transform;
			Vector3 Ball2Position;
			Transform Ball2Transform;
			Vector3 EnemyBallPosition;
			Transform EnemyBallTransform;
			Vector3 GoalBallPosition;
			Transform GoalBallTransform;

			Vector3 MoveWall1Position;
			Transform MoveWall1Transform;
			Vector3 MoveWall2Position;
			Transform MoveWall2Transform;
			Vector3 MoveWall3Position;
			Transform MoveWall3Transform;

			Vector3 velocity1 = Vector3(300,0,0);
			Vector3 velocity2 = Vector3(-300, 0, 0);
			Vector3 velocity3 = Vector3(0, 0, 300);
			Vector3 RayDirection;
			Vector3 RayPosition;


		};
	}
}

