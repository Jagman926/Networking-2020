/*
	Copyright 2011-2020 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_DemoSceneObject.c
	Example of demo utility source file.
*/

#include "a3_DemoSceneObject.h"


//-----------------------------------------------------------------------------

// scene object setup and manipulation
extern inline void a3demo_initSceneObject(a3_DemoSceneObject *sceneObject)
{
	a3real4x4SetIdentity(sceneObject->modelMat.m);
	a3real4x4SetIdentity(sceneObject->modelMatInv.m);
	a3real3Set(sceneObject->euler.v, a3real_zero, a3real_zero, a3real_zero);
	a3real3Set(sceneObject->position.v, a3real_zero, a3real_zero, a3real_zero);
}

extern inline void a3demo_updateSceneObject(a3_DemoSceneObject *sceneObject, const a3boolean useZYX)
{
	if (useZYX)
		a3real4x4SetRotateZYX(sceneObject->modelMat.m, sceneObject->euler.x, sceneObject->euler.y, sceneObject->euler.z);
	else
		a3real4x4SetRotateXYZ(sceneObject->modelMat.m, sceneObject->euler.x, sceneObject->euler.y, sceneObject->euler.z);
	sceneObject->modelMat.v3.xyz = sceneObject->position;
	a3real4x4TransformInverseIgnoreScale(sceneObject->modelMatInv.m, sceneObject->modelMat.m);
}

extern inline a3i32 a3demo_rotateSceneObject(a3_DemoSceneObject *sceneObject, const a3real speed, const a3real deltaX, const a3real deltaY, const a3real deltaZ)
{
	if (speed && (deltaX || deltaY || deltaZ))
	{
		// validate angles so they don't get zero'd out (trig functions have a limit)
		sceneObject->euler.x = a3trigValid_sind(sceneObject->euler.x + speed * deltaX);
		sceneObject->euler.y = a3trigValid_sind(sceneObject->euler.y + speed * deltaY);
		sceneObject->euler.z = a3trigValid_sind(sceneObject->euler.z + speed * deltaZ);

		return 1;
	}
	return 0;
}

extern inline a3i32 a3demo_moveSceneObject(a3_DemoSceneObject *sceneObject, const a3real speed, const a3real deltaX, const a3real deltaY, const a3real deltaZ)
{
	if (speed && (deltaX || deltaY || deltaZ))
	{
		a3real3 delta[3];
		a3real3ProductS(delta[0], sceneObject->modelMat.m[0], deltaX);	// account for orientation of object
		a3real3ProductS(delta[1], sceneObject->modelMat.m[1], deltaY);
		a3real3ProductS(delta[2], sceneObject->modelMat.m[2], deltaZ);
		a3real3Add(delta[0], delta[1]);									// add the 3 deltas together
		a3real3Add(delta[0], delta[2]);
		a3real3MulS(delta[0], speed * a3real3LengthInverse(delta[0]));	// normalize and scale by speed
		a3real3Add(sceneObject->position.v, delta[0]);					// add delta to current

		return 1;
	}
	return 0;
}


extern inline void a3demo_setCameraSceneObject(a3_DemoCamera *camera, a3_DemoSceneObject *sceneObject)
{
	camera->sceneObject = sceneObject;
}

extern inline void a3demo_initCamera(a3_DemoCamera *camera)
{
	a3real4x4SetIdentity(camera->projectionMat.m);
	a3real4x4SetIdentity(camera->projectionMatInv.m);
	a3real4x4SetReal4x4(camera->viewProjectionMat.m, camera->sceneObject->modelMatInv.m);
	camera->perspective = a3false;
	camera->fovy = a3real_two;
	camera->aspect = a3real_one;
	camera->znear = -a3real_one;
	camera->zfar = +a3real_one;
	camera->ctrlMoveSpeed = a3real_zero;
	camera->ctrlRotateSpeed = a3real_zero;
	camera->ctrlZoomSpeed = a3real_zero;
}

extern inline void a3demo_updateCameraProjection(a3_DemoCamera *camera)
{
	if (camera->perspective)
		a3real4x4MakePerspectiveProjection(camera->projectionMat.m, camera->projectionMatInv.m,
			camera->fovy, camera->aspect, camera->znear, camera->zfar);
	else
		a3real4x4MakeOrthographicProjection(camera->projectionMat.m, camera->projectionMatInv.m,
			camera->fovy * camera->aspect, camera->fovy, camera->znear, camera->zfar);
}

extern inline void a3demo_updateCameraViewProjection(a3_DemoCamera *camera)
{
	a3real4x4Product(camera->viewProjectionMat.m, camera->projectionMat.m, camera->sceneObject->modelMatInv.m);
}


//-----------------------------------------------------------------------------
