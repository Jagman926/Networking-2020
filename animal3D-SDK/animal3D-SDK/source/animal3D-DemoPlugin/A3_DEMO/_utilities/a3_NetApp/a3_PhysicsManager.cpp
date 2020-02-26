#include "a3_PhysicsManager.h"
#include <math.h>
#include <stdlib.h>
#include <string>

PhysicsCircleObject::PhysicsCircleObject(float x_pos, float y_pos, float x_vel, float y_vel, float rad)
{
	SetPosition(xPos, yPos);
	SetVelocity(xVel, yVel);
	radius = rad;
}

void PhysicsCircleObject::UpdatePosition()
{
	SetPosition(xPos + xVel, yPos + yVel);
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
	if ((xPos + radius) > screenSize_x || (xPos - radius) < 0) // (going off right || going off left)
		// flip velocity in x direction
		SetVelocity(xVel * -1.0f, yVel);
	// Check vertical edges
	if ((yPos + radius) > screenSize_y || (yPos - radius) < 0) // (going off bottom || going off top)
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
}

bool PhysicsManager::AddLocalCircleObject(PhysicsCircleObject obj)
{
	if (numOfLocalObjs < OBJ_MAX)
	{
		physicsCircleManager[0][numOfLocalObjs]->radius = obj.radius;
		physicsCircleManager[0][numOfLocalObjs]->SetPosition(obj.xPos, obj.yPos);
		physicsCircleManager[0][numOfLocalObjs]->SetVelocity(obj.xVel, obj.yVel);
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

bool PhysicsManager::CopyPhysicsCircleObjectArray(int userIndex, PhysicsCircleObject objArray[OBJ_MAX])
{
	if (userIndex < PLYR_MAX)
	{
		int i;
		for (i = 0; i < OBJ_MAX; i++)
		{
			physicsCircleManager[userIndex][i]->radius = objArray[i].radius;
			physicsCircleManager[userIndex][numOfLocalObjs]->SetPosition(objArray[i].xPos, objArray[i].yPos);
			physicsCircleManager[userIndex][numOfLocalObjs]->SetVelocity(objArray[i].xVel, objArray[i].yVel);
		}
		return true;
	}
	return false;
}
