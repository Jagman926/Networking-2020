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

bool PhysicsManager::AddLocalCircleObject(PhysicsCircleObject obj)
{
	if (numOfLocalObjs < OBJ_MAX)
	{
		PhysicsCircleObject * circleObj = new PhysicsCircleObject;
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

bool PhysicsManager::CopyPhysicsCircleObjectArray(PhysicsCircleObject objArray[OBJ_MAX])
{
	if (numRemoteArrays < PLYR_MAX)
	{
		// increment arrays on the remote end
		numRemoteArrays++;
		int i;
		for (i = 0; i < OBJ_MAX; i++)
		{
			physicsCircleManager[numRemoteArrays][i]->radius = objArray[i].radius;
			physicsCircleManager[numRemoteArrays][numOfLocalObjs]->SetPosition(objArray[i].xPos, objArray[i].yPos);
			physicsCircleManager[numRemoteArrays][numOfLocalObjs]->SetVelocity(objArray[i].xVel, objArray[i].yVel);
		}

		return true;
	}
	return false;
}

void PhysicsManager::UpdateObjects(float windowWidth, float windowHeight)
{
	for (int i = 0; i < PLYR_MAX; i++)
		for (int j = 0; j < OBJ_MAX; j++)
		{
			if (physicsCircleManager[i][j])
			{
				physicsCircleManager[i][j]->CheckEdgeScreenCollision(windowWidth, windowHeight);
				physicsCircleManager[i][j]->UpdatePosition();
			}
		}
}
