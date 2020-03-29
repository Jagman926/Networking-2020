#ifndef A3_PHYSICS_MANAGER_H
#define A3_PHYSICS_MANAGER_H

struct PhysicsCircleObject
{
public:
	// position of object on screen
	float xPos = 0.0f, yPos = 0.0f;
	// current velocity of object in 2D space
	float xVel = 0.0f, yVel = 0.0f;
	// object radius
	float radius = 0.0f;

	// functions
	//ctor
	PhysicsCircleObject();
	PhysicsCircleObject(float x_pos, float y_pos, float x_vel, float y_vel, float rad);
	//update
	void UpdatePosition();
	// collision checks
	void CheckEdgeScreenCollision(float screenSize_x, float screenSize_y);
	void CheckPhysicsObjectCollision(PhysicsCircleObject physicsObjs);
	float DistanceToPoint(float x, float y);
	// sets
	void SetPosition(float x, float y) { xPos = x, yPos = y; };
	void SetVelocity(float x, float y) { xVel = x, yVel = y; };
};

// Const
const int OBJ_MAX = 100;
const int PLYR_MAX = 16;

class PhysicsManager
{
public:
	// master 2D array of all user arrays
	PhysicsCircleObject* physicsCircleManager[PLYR_MAX][OBJ_MAX];
	// current number of physics objects locally
	int numOfLocalObjs = 0;

	// Remote array tracking
	int numRemoteArrays = 0;

	// functions
	//ctor
	PhysicsManager();
	// init
	void InitAllLocalsToZero();
	void InitAllToZero();
	// add
	bool AddLocalCircleObject(PhysicsCircleObject obj);
	// delete last
	bool DeleteLastLocalCircleObject();
	// clear remotes
	bool ClearAllRemoteArrays();
	// clear all arrays
	bool ClearAllArrays();
	// copy circle physics object array to master
	bool CopyPhysicsCircleObjectArray(PhysicsCircleObject objArray[OBJ_MAX]);
	// Update objects
	void UpdateObjects(float windowWidth, float windowHeight);
};


#endif // !A3_PHYSICS_MANAGER_H

