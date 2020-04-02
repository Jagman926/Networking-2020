#include "a3_PhysicsManager.h"
#include <math.h>
#include <stdlib.h>
#include <string>

PhysicsCircleObject::PhysicsCircleObject()
{
	SetPosition(0.0f, 0.0f);
	SetVelocity(0.0f, 0.0f);
	radius = 0;
	
}

PhysicsCircleObject::PhysicsCircleObject(float x_pos, float y_pos, float x_vel, float y_vel, float x_acc, float y_acc, float rad)
{
	SetPosition(xPos, yPos);
	SetVelocity(xVel, yVel);
	SetAcceleration(xAcc, yAcc);
	radius = rad;
	
}

void PhysicsCircleObject::UpdatePosition(float dt)
{
	SetPosition(xPos + xVel * dt, yPos + yVel * dt);
}

void PhysicsCircleObject::CheckEdgeScreenCollision(float screenSize_x, float screenSize_y)
{
	// Screen coordinate notes:
	/*

	(0,0)
	V
	------------- < (screenX,0)
	|	     	|
	|           |
	|           |
	------------- < (screenX,screenY)
	^
	(0,screenY)

	*/

	// Check horizontal edges
	if ((xPos + 2 * radius) > screenSize_x || (xPos - radius) < 0) // (going off right || going off left)
		// flip velocity in x direction
		SetVelocity(xVel * -1.0f, yVel);
	// Check vertical edges
	if ((yPos + 2 * radius) > screenSize_y || (yPos - radius) < 0) // (going off bottom || going off top)
		// flip velocity in y direction
		SetVelocity(xVel, yVel * -1.0f);
}

void PhysicsCircleObject::CheckPhysicsObjectCollision(PhysicsCircleObject physicsObjs)
{
	// If distance between object is less than combined radius
	if (DistanceToPoint(physicsObjs.xPos, physicsObjs.yPos) < (radius + physicsObjs.radius))
	{
		// flip velocity of both objects
		SetVelocity(xVel * -1.0f, yVel * -1.0f);
		physicsObjs.SetVelocity(xVel * -1.0f, yVel * -1.0f);
	}
}

float PhysicsCircleObject::DistanceToPoint(float x, float y)
{
	// distance formula: d(P,Q) = √(x2 − x1)^2 + (y2 − y1)^2
	return sqrtf((x - xPos) * (x - xPos) + (y - yPos) + (y - yPos));
}

PhysicsManager::PhysicsManager()
{
	numOfLocalObjs = 0;
	numRemoteArrays = 0;
}

void PhysicsManager::InitAllLocalsToZero()
{
	numOfLocalObjs = 0;
	numRemoteArrays = 0;

	for (int i = 0; i < OBJ_MAX; i++)
	{
		PhysicsCircleObject* circleObj = new PhysicsCircleObject;
		circleObj->radius = 0;
		circleObj->SetPosition(0, 0);
		circleObj->SetVelocity(0, 0);
		physicsCircleManager[0][i] = circleObj;
	
	}
}

void PhysicsManager::InitAllToZero()
{
	
	numOfLocalObjs = 0;
	numRemoteArrays = 0;
	int i, j;
	for (i = 0; i < PLYR_MAX; i++)
	{
		memset(systemAddressOrder[i], 0, sizeof (char[512]));
		systemAddressUpdated[i] = false;
		for (j = 0; j < OBJ_MAX; j++)
		{
			PhysicsCircleObject* circleObj = new PhysicsCircleObject;
			circleObj->radius = 0;
			circleObj->SetPosition(0, 0);
			circleObj->SetVelocity(0, 0);
			physicsCircleManager[i][j] = circleObj;

		}
	}
}

bool PhysicsManager::AddLocalCircleObject(PhysicsCircleObject obj)
{
	if (numOfLocalObjs < OBJ_MAX)
	{
		PhysicsCircleObject *circleObj = new PhysicsCircleObject;
		circleObj->radius = obj.radius;
		circleObj->SetPosition(obj.xPos, obj.yPos);
		circleObj->SetVelocity(obj.xVel, obj.yVel);
		physicsCircleManager[0][numOfLocalObjs] = circleObj;
		numOfLocalObjs++;
		return true;
	}
	return false;
}

bool PhysicsManager::DeleteLastLocalCircleObject()
{
	if (numOfLocalObjs > 0)
	{
		numOfLocalObjs--;
		memset(physicsCircleManager[0][numOfLocalObjs], 0, sizeof(physicsCircleManager[0][numOfLocalObjs]));
		return true;
	}
	return false;
}

bool PhysicsManager::ClearAllRemoteArrays()
{
	for (int i = 1; i < PLYR_MAX; i++)
	{
		for (int j = 0; j < OBJ_MAX; j++)
		{
			if (physicsCircleManager[i][j])
			{
				physicsCircleManager[i][j]->radius = 0;
				physicsCircleManager[i][j]->SetPosition(0, 0);
				physicsCircleManager[i][j]->SetVelocity(0, 0);
				physicsCircleManager[i][j]->SetAcceleration(0, 0);
			}
		}
	}
	// no more remote arrays
	numRemoteArrays = 0;
	return true;
}

bool PhysicsManager::ClearAllArrays()
{
	for (int i = 0; i < PLYR_MAX; i++)
	{
		for (int j = 0; j < OBJ_MAX; j++)
		{
			if (physicsCircleManager[i][j])
			{
				physicsCircleManager[i][j]->radius = 0;
				physicsCircleManager[i][j]->SetPosition(0, 0);
				physicsCircleManager[i][j]->SetVelocity(0, 0);
				physicsCircleManager[i][j]->SetAcceleration(0, 0);
			}
		}
	}
	// all arrays cleared
	numRemoteArrays = 0;
	return true;
}

bool PhysicsManager::CopyPhysicsCircleObjectArray(PhysicsCircleObject objArray[OBJ_MAX], char sysAddress[512])
{
	
	if (numRemoteArrays < PLYR_MAX)
	{

		strcpy(systemAddressOrder[numRemoteArrays], sysAddress);

		// increment arrays on the remote end
		numRemoteArrays += 1;

		int i;
		for (i = 0; i < OBJ_MAX; i++)
		{
			// check if radius isnt 0
			if (objArray[i].radius != 0)
			{
				physicsCircleManager[numRemoteArrays][i]->radius = objArray[i].radius;
				physicsCircleManager[numRemoteArrays][i]->SetPosition(objArray[i].xPos, objArray[i].yPos);
				physicsCircleManager[numRemoteArrays][i]->SetVelocity(objArray[i].xVel, objArray[i].yVel);
				physicsCircleManager[numRemoteArrays][i]->SetAcceleration(objArray[i].xAcc, objArray[i].yAcc);
			}
			else
			{
				return true;
			}
		}
		return true;
	}
	return false;
}

bool PhysicsManager::CheckPhysicsCircleObjectArray(PhysicsCircleObject objArray[OBJ_MAX],  int index)
{

	// increment arrays on the remote end
	numRemoteArrays += 1;
	int i;
	for (i = 0; i < OBJ_MAX; i++)
	{

		physicsCircleManager[index][i]->radius = objArray[i].radius;
		physicsCircleManager[index][i]->SetPosition(objArray[i].xPos, objArray[i].yPos);
		physicsCircleManager[index][i]->SetVelocity(objArray[i].xVel, objArray[i].yVel);
		physicsCircleManager[index][i]->SetAcceleration(objArray[i].xAcc, objArray[i].yAcc);
	}
	
	return true;
}

void PhysicsManager::UpdateObjects(int index, float windowWidth, float windowHeight, float dt)
{
	
	for (int j = 0; j < OBJ_MAX; j++)
	{
		if (physicsCircleManager[index][j])
		{
			physicsCircleManager[index][j]->CheckEdgeScreenCollision(windowWidth, windowHeight);
			physicsCircleManager[index][j]->UpdatePosition(dt);
		}
	}
}

int PhysicsManager::GetIndexFromSystemAddress(char sysAddres[512])
{
	for (int i = 0; i < PLYR_MAX; i++)
	{
		if (systemAddressOrder[i] == sysAddres)
		{
			return i;
		}
	}
	return -1;
}

void PhysicsManager::SetSytemAddressBool(bool recieved, int index)
{
	systemAddressUpdated[index] = recieved;
}

void PhysicsManager::ResetSytemAddressBools()
{
	for (int i = 0; i < PLYR_MAX; i++)
	{
		systemAddressUpdated[i] = false;
	}
}
