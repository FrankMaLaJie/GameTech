#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"

#include "../CSC8503Common/PositionConstraint.h"
#include "time.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame() {
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
	physics = new PhysicsSystem(*world);

	forceMagnitude = 10.0f;
	useGravity = false;
	inSelectionMode = false;

	Debug::SetRenderer(renderer);

	InitialiseAssets();
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes,
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it

*/
void TutorialGame::InitialiseAssets() {
	cubeMesh = new OGLMesh("cube.msh");
	cubeMesh->SetPrimitiveType(GeometryPrimitive::Triangles);
	cubeMesh->UploadToGPU();

	sphereMesh = new OGLMesh("sphere.msh");
	sphereMesh->SetPrimitiveType(GeometryPrimitive::Triangles);
	sphereMesh->UploadToGPU();

	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("brick.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	wallTex = (OGLTexture*)TextureLoader::LoadAPITexture("mucai.jpg");

	ballTex = (OGLTexture*)TextureLoader::LoadAPITexture("gaoerfuqiu.jpg");

	FakeCamera();
	InitWorld2();
	InitWorld();

	curScene = 0;
	
}

TutorialGame::~TutorialGame() {
	delete cubeMesh;
	delete sphereMesh;
	delete basicTex;
	delete wallTex;
	delete ballTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void TutorialGame::UpdateGame(float dt) {
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}

	UpdateKeys();



	SelectObject();
	MoveSelectedObject();
	Line();



	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	Debug::FlushRenderables();
	renderer->Render();

	if (curScene == 0) {
		renderer->DrawString("Welcome! Big Fan! ", Vector2(360, 350));
		renderer->DrawString("Press 1 to Star! ", Vector2(360, 0));
	}


	if(curScene==1||curScene==2)
	{
		renderer->DrawString("Press R to Reset the Game", Vector2(20, 650));
		renderer->DrawString("Press B to Find the Ball1", Vector2(20, 630));
		renderer->DrawString("Press N to Find the Ball2", Vector2(20, 610));
		renderer->DrawString("Press 1 to Level 1", Vector2(20, 590));
		renderer->DrawString("Press 2 to Level 2", Vector2(20, 570));
		renderer->DrawString("Goal :" + std::to_string(ClickNum), Vector2(20, 550));
		
		ClickNum -= Window::GetMouse()->ButtonPressed(NCL::MouseButtons::MOUSE_RIGHT)*100;
		ClickNum--;
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_R) || Window::GetKeyboard()->KeyPressed(KEYBOARD_1) || Window::GetKeyboard()->KeyPressed(KEYBOARD_2)) {
			ClickNum = 10000;
			selectionObject = nullptr;
		}

		if (useGravity) {
			Debug::Print("(G)ravity on", Vector2(10, 40));
		}
		else {
			Debug::Print("(G)ravity off", Vector2(10, 40));
		}



	}

	if (MoveWall1) {

		Ball2Position = Ball2->GetTransform().GetWorldPosition();
		EnemyBallPosition = EnemyBall->GetTransform().GetWorldPosition();
		GoalBallPosition = GoalBall->GetTransform().GetWorldPosition();
		if (useGravity)
		{
			EnemyBall->GetPhysicsObject()->AddForce(Vector3((Ball2Position.x - EnemyBallPosition.x)/80, (Ball2Position.y - EnemyBallPosition.y)/80, (Ball2Position.z - EnemyBallPosition.z)/80));
			GoalBall->GetPhysicsObject()->AddForce(Vector3(-(Ball2Position.x - GoalBallPosition.x) / 80, -(Ball2Position.y - GoalBallPosition.y) / 80, (Ball2Position.z - GoalBallPosition.z) / 80));
		}

		float EnemyBallDistance = sqrt((Ball2Position.x - EnemyBallPosition.x)*(Ball2Position.x - EnemyBallPosition.x) + (Ball2Position.y - EnemyBallPosition.y)*(Ball2Position.y - EnemyBallPosition.y));
		
		float GoalBallDistance = sqrt((Ball2Position.x - GoalBallPosition.x)*(Ball2Position.x - GoalBallPosition.x) + (Ball2Position.y - GoalBallPosition.y)*(Ball2Position.y - GoalBallPosition.y));
		
		if (EnemyBallDistance <= 60) {
			InitWorld2();
			InitCamera();
			selectionObject = nullptr;
			Sleep(1);
		}

		if (GoalBallDistance <= 60) {
			Sleep(1000);
			renderer->DrawString("Good Job! That's All!", Vector2(360, 350));
		}

		//if (650 < Ball2Position.x < 750 && 650 < Ball2Position.z < 750) {
		//	InitWorld();
		//	InitCamera();
		//	selectionObject = nullptr;
		//	Sleep(1);
		//}
		MoveWall1->GetPhysicsObject()->SetLinearVelocity(velocity1);
		MoveWall2->GetPhysicsObject()->SetLinearVelocity(velocity2);
		MoveWall3->GetPhysicsObject()->SetLinearVelocity(velocity3);


		MoveWall1Position = MoveWall1->GetTransform().GetWorldPosition();
		MoveWall2Position = MoveWall2->GetTransform().GetWorldPosition();
		MoveWall3Position = MoveWall3->GetTransform().GetWorldPosition();
		if (MoveWall1Position.x >= 550) {
			velocity1.x = -100;
		}
		else if (MoveWall1Position.x <= -550) {
			velocity1.x = 100;
		}
		MoveWall1->GetPhysicsObject()->SetLinearVelocity(velocity1);

		if (MoveWall2Position.x <= -300) {
			velocity2.x = 300;
		}
		else if (MoveWall2Position.x >= 300) {
			velocity2.x = -300;
		}
		MoveWall2->GetPhysicsObject()->SetLinearVelocity(velocity2);

		if (MoveWall3Position.z <= -300) {
			velocity3.z = 300;
		}
		else if (MoveWall3Position.z >= 300) {
			velocity3.z = -300;
		}
		MoveWall3->GetPhysicsObject()->SetLinearVelocity(velocity3);
	}

	if (curScene == 1) {
		Ball1Position = Ball1->GetTransform().GetWorldPosition();
		if (-90 < Ball1Position.x < 205 && Ball1Position.y < -100 && Ball1Position.z < -1010) {
			InitWorld2();
			InitCamera();
			selectionObject = nullptr;
			Sleep(1);
		}
		else if (Ball1Position.y < -100)
		{
			InitWorld();
			InitCamera();
			selectionObject = nullptr;
			Sleep(1);
		}
	}

}


void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_R)) {
		curScene = 1;
		InitWorld(); //We can reset the simulation at any time with F1
		InitCamera();
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_1)) {
		curScene = 1;
		InitWorld();
		InitCamera();
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_2)) {
		curScene = 2;
		InitWorld2();
		FindBall2();
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_B)) {
		FindBall1(); //B will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_N)) {
		FindBall2(); //B will reset the camera to a specific default place
	}


	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::KEYBOARD_G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F8)) {
		world->ShuffleObjects(false);
	}
	//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-100, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(100, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 100, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -100, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(100, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -100));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 100));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_J)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -50));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_K)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(-50, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_L)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 50));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_I)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(50, 0, 0));
		}
	}
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(3.0f);
	world->GetMainCamera()->SetFarPlane(4200.0f);
	world->GetMainCamera()->SetPitch(-35.0f);
	world->GetMainCamera()->SetYaw(320.0f);
	world->GetMainCamera()->SetPosition(Vector3(-400, 200, 200));
}

void TutorialGame::FindBall1() {
	Ball1Position = Ball1->GetTransform().GetWorldPosition();

	world->GetMainCamera()->SetNearPlane(3.0f);
	world->GetMainCamera()->SetFarPlane(4200.0f);
	world->GetMainCamera()->SetPitch(-35.0f);
	world->GetMainCamera()->SetYaw(320.0f);
	world->GetMainCamera()->SetPosition(Vector3(Ball1Position.x - 50, Ball1Position.y + 120, Ball1Position.z + 200));
}

void TutorialGame::FindBall2() {
	Ball2Position = Ball2->GetTransform().GetWorldPosition();

	world->GetMainCamera()->SetNearPlane(3.0f);
	world->GetMainCamera()->SetFarPlane(4200.0f);
	world->GetMainCamera()->SetPitch(-35.0f);
	world->GetMainCamera()->SetYaw(320.0f);
	world->GetMainCamera()->SetPosition(Vector3(Ball2Position.x - 50, Ball2Position.y + 120, Ball2Position.z + 200));
}

void TutorialGame::FakeCamera() {
	world->GetMainCamera()->SetNearPlane(3.0f);
	world->GetMainCamera()->SetFarPlane(4200.0f);
	world->GetMainCamera()->SetPitch(-350.0f);
	world->GetMainCamera()->SetYaw(0.0f);
	world->GetMainCamera()->SetPosition(Vector3(100, 2800, 0));
}

void TutorialGame::MainMenu() {
	world->ClearAndErase();
	physics->Clear();

	AddFloorToWorld(Vector3(0, -100, 0));
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();

	

	MoveWall1 = nullptr;
	MoveWall2 = nullptr;
	MoveWall3 = nullptr;
	Ball2 = nullptr;
	EnemyBall = nullptr;
	GoalBall = nullptr;

	//InitCubeGridWorld(5, 5, 50.0f, 50.0f, Vector3(10, 10, 10));
	//InitSphereGridWorld(1, 1, 3.0f, 3.0f, 3.0f);
	Ball1 = AddSphereToWorld(Vector3(-300, 50, 100), 30.0f, 30.0f);
	//InitSphereGridWorld(1, 1, 50.0f, 50.0f, 10.0f);
	//InitCubeGridWorld(w,1, 1, 50.0f, 50.0f, Vector3(10, 10, 10));
	//InitCubeGridWorld(w, 1, 1, 50.0f, 50.0f, Vector3(8, 8, 8));

	InitSphereCollisionTorqueTest();
	//InitCubeCollisionTorqueTest();

	//InitSphereGridWorld(1, 1, 50.0f, 50.0f, 10.0f);
	//BridgeConstraintTest();
	//InitGJKWorld();

	//DodgyRaycastTest();
	//InitGJKWorld();
	//InitSphereAABBTest();
	//SimpleGJKTest();
	//SimpleAABBTest2();

	//InitSphereCollisionTorqueTest(w);


}

void TutorialGame::InitWorld2() {
	world->ClearAndErase();
	physics->Clear();

	

	Ball2 = AddSphereToWorld(Vector3(-300, 150, -400), 30.0f, 30.0f);

	EnemyBall = AddSphereToWorld(Vector3(-800, 150, -400), 30.0f, 30.0f);

	GoalBall = AddSphereToWorld(Vector3(100, 150, 500), 30.0f, 30.0f);

	MoveWall1 = AddCubeToWorld(Vector3(0, 0, 0), Vector3(400, 100, 10), 0);
	MoveWall2 = AddCubeToWorld(Vector3(0, 0, 400), Vector3(600, 100, 10), 0);
	MoveWall3 = AddCubeToWorld(Vector3(0, 0, 400), Vector3(10, 100, 600), 0);


	AddCubeToWorld(Vector3(0, 0, -1010), Vector3(1000, 100, 10), 0); // north 左
	AddCubeToWorld(Vector3(0, 0, 1010), Vector3(1000, 100, 10), 0);  // south右
	AddCubeToWorld(Vector3(-1010, 0, 0), Vector3(10, 100, 1000), 0); // west下
	AddCubeToWorld(Vector3(1010, 0, 0), Vector3(10, 100, 1000), 0); // east上



	AddFloorToWorld(Vector3(0, -100, 0));

}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(1000, 10, 1000);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetWorldScale(floorSize);
	floor->GetTransform().SetWorldPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple'
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject("Ball");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetWorldScale(sphereSize);
	sphere->GetTransform().SetWorldPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, ballTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform().SetWorldPosition(position);
	cube->GetTransform().SetWorldScale(dimensions);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, wallTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, radius, z * rowSpacing);
			AddSphereToWorld(position, radius);
		}
	}
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 10.0f;
	Vector3 cubeDims = Vector3(10, 10, 10);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, cubeDims.y, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
	AddFloorToWorld(Vector3(0, -100, 0));
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, cubeDims.y, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(10, -100, 1));
}


void TutorialGame::InitSphereCollisionTorqueTest() {
	//AddSphereToWorld(Vector3(15, 0, 0), 10.0f);
	//AddSphereToWorld(Vector3(-25, 0, 0), 10.0f);
	//AddSphereToWorld(Vector3(-50, 0, 0), 10.0f);

	//位置 前+后-，上+下-，右+左-
	//大小 前后，上下，右+左-
	AddCubeToWorld(Vector3(600, 10, -1010), Vector3(400, 100, 10), 0); // north1 左1
	AddCubeToWorld(Vector3(-550, 10, -1010), Vector3(450, 100, 10), 0); // north2 左2
	AddCubeToWorld(Vector3(0, 10, 1010), Vector3(1000, 100, 10), 0);  // south右
	AddCubeToWorld(Vector3(-1010, 10, 0), Vector3(10, 100, 500), 0); // west下
	AddCubeToWorld(Vector3(1010, 10, -500), Vector3(10, 100, 500), 0); // east上

	AddCubeToWorld(Vector3(500, 10, -700), Vector3(10, 100, 300), 0);//1
	AddCubeToWorld(Vector3(500, 10, 300), Vector3(10, 100, 400), 0);//2
	AddCubeToWorld(Vector3(205, 10, -300), Vector3(10, 100, 700), 0);//3
	AddCubeToWorld(Vector3(0, 10, 710), Vector3(500, 100, 10), 0);//4
	AddCubeToWorld(Vector3(-90, 10, -150), Vector3(10, 100, 850), 0);//5
	AddCubeToWorld(Vector3(-300, 10, -600), Vector3(200, 100, 10), 0);//6
	AddCubeToWorld(Vector3(-510, 10, -690), Vector3(10, 100, 100), 0);//7
	AddCubeToWorld(Vector3(-510, 10, 200), Vector3(10, 100, 500), 0);//8
	AddCubeToWorld(Vector3(-400, 10, 200), Vector3(100, 100, 10), 0);//9
	AddCubeToWorld(Vector3(-450, 10, -310), Vector3(230, 100, 10), 0);//10
	AddCubeToWorld(Vector3(-230, 10, -200), Vector3(10, 100, 100), 0);//11
	AddCubeToWorld(Vector3(-690, 10, -550), Vector3(10, 100, 250), 0);//13
	AddCubeToWorld(Vector3(-840, 10, 200), Vector3(150, 100, 10), 0);//14
	AddCubeToWorld(Vector3(-700, 10, 310), Vector3(10, 100, 100), 0);//15
	AddCubeToWorld(Vector3(50, 10, -700), Vector3(10, 200, 10), 0);//Goal

	//AddCubeToWorld(Vector3(0, -80, 200), Vector3(1000, 20, 800), 0);//地板右
	//AddCubeToWorld(Vector3(0, -80, -850), Vector3(1000, 20, 150), 0);//地板左
	//AddCubeToWorld(Vector3(550, -80, -700), Vector3(450, 20, 100), 0);//地板上
	//AddCubeToWorld(Vector3(-500, -80, -700), Vector3(500, 20, 100), 0);//地板下

	AddFloorToWorld(Vector3(0, -100, 0));
}


void TutorialGame::InitCubeCollisionTorqueTest() {
	Vector3 cubeSize(10, 10, 10);
	AddCubeToWorld(Vector3(15, 0, 0), cubeSize, 10.0f);
	AddCubeToWorld(Vector3(-25, 0, 0), cubeSize, 10.0f);
	AddCubeToWorld(Vector3(-50, 0, 0), cubeSize, 10.0f);

	AddCubeToWorld(Vector3(-50, 0, -50), Vector3(60, 10, 10), 10.0f);

	AddFloorToWorld(Vector3(0, -100, 0));
}

void TutorialGame::InitSphereAABBTest() {
	Vector3 cubeSize(10, 10, 10);

	AddCubeToWorld(Vector3(0, 0, 0), cubeSize, 10.0f);
	AddSphereToWorld(Vector3(2, 0, 0), 5.0f, 10.0f);
}

void TutorialGame::InitGJKWorld() {
	Vector3 dimensions(20, 2, 10);
	float inverseMass = 10.0f;

	for (int i = 0; i < 2; ++i) {
		GameObject* cube = new GameObject();

		OBBVolume* volume = new OBBVolume(dimensions);

		cube->SetBoundingVolume((CollisionVolume*)volume);

		cube->GetTransform().SetWorldPosition(Vector3(0, 0, 0));
		cube->GetTransform().SetWorldScale(dimensions);

		if (i == 1) {
			cube->GetTransform().SetLocalOrientation(Quaternion::AxisAngleToQuaterion(Vector3(1, 0, 0), 90.0f));
		}

		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
		cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

		cube->GetPhysicsObject()->SetInverseMass(inverseMass);
		cube->GetPhysicsObject()->InitCubeInertia();

		world->AddGameObject(cube);
	}
}

void TutorialGame::BridgeConstraintTest() {
	float sizeMultiplier = 1.0f;

	Vector3 cubeSize = Vector3(8, 8, 8) * sizeMultiplier;

	int numLinks = 5;

	GameObject* start = AddCubeToWorld(Vector3(0, 0, 0), cubeSize, 0);

	GameObject* end = AddCubeToWorld(Vector3((numLinks + 2) * 20 * sizeMultiplier, 0, 0), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(Vector3((i + 1) * 20 * sizeMultiplier, 0, 0), cubeSize, 10.0f);
		PositionConstraint* constraint = new PositionConstraint(previous, block, 30.0f);
		world->AddConstraint(constraint);
		previous = block;
	}

	PositionConstraint* constraint = new PositionConstraint(previous, end, 30.0f);
	world->AddConstraint(constraint);
}

void TutorialGame::SimpleGJKTest() {
	Vector3 dimensions = Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(100, 2, 100);

	GameObject* fallingCube = AddCubeToWorld(Vector3(0, 20, 0), dimensions, 10.0f);
	GameObject* newFloor = AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);

	delete fallingCube->GetBoundingVolume();
	delete newFloor->GetBoundingVolume();

	fallingCube->SetBoundingVolume((CollisionVolume*)new OBBVolume(dimensions));
	newFloor->SetBoundingVolume((CollisionVolume*)new OBBVolume(floorDimensions));

}

void TutorialGame::SimpleAABBTest() {
	Vector3 dimensions = Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(100, 2, 100);

	GameObject* newFloor = AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);
	GameObject* fallingCube = AddCubeToWorld(Vector3(10, 20, 0), dimensions, 10.0f);
}

void TutorialGame::SimpleAABBTest2() {
	Vector3 dimensions = Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(8, 2, 8);

	GameObject* newFloor = AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);
	GameObject* fallingCube = AddCubeToWorld(Vector3(8, 20, 0), dimensions, 10.0f);
}

/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around.

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		renderer->DrawString("Press Q to change to camera mode!", Vector2(10, 0));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::MOUSE_LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		if (curScene == 1 || curScene == 2) {
			renderer->DrawString("Press Q to change to select mode!", Vector2(10, 0));
		}
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	if (curScene == 1 || curScene == 2) {
		renderer->DrawString("Click Force:" + std::to_string(forceMagnitude), Vector2(10, 20));
		forceMagnitude += Window::GetMouse()->GetWheelMovement() * 10.0f;
	}


	Ball1Position = Ball1->GetTransform().GetWorldPosition();

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::MOUSE_RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
			ThePoint = closestCollision.collidedAt;
			AddForce = true;

		}
	}

}

void TutorialGame::Line() {
	//if (!selectionObject) {
	//	return;
	//}

	//RayDirection = selectionObject->GetConstTransform().GetWorldOrientation().ToMatrix3()*Vector3(0, 0, 1.1);
	//RayPosition = selectionObject->GetConstTransform().GetWorldMatrix().GetPositionVector() + RayDirection*(selectionObject->GetTransform().GetLocalScale()*1.5);

	//Ray ray = Ray(RayDirection, RayPosition);

	//if (line) {
	//	line->GetRenderObject()->SetColour(Vector4(1, 2, 3, 4));
	//		line = nullptr;
	//}
	//Debug::DrawLine(RayPosition, RayPosition + RayDirection * 100);

	//RayCollision closestCollision;
	//if (world->Raycast(ray, closestCollision, true)) {
	//	if (closestCollision.node != selectionObject) {
	//		line = (GameObject*)closestCollision.node;
	//		line->GetRenderObject()->SetColour(Vector4(4, 3, 2, 1));
	//	}

	//}
	if (AddForce) {
		if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::MOUSE_RIGHT))
		{
			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)){

				Vector3 NewPoint = closestCollision.collidedAt;
				renderer->DrawLine(ThePoint, NewPoint, Vector4(0, 1, 1, 1));
				Vector3 LineDistance = ThePoint - NewPoint;
				float force = (LineDistance.Length() / 10);

			}
		}
	}
	else { 
		AddForce = false;
	}
}