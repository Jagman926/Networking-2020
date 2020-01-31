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
	
	a3_DemoSceneObject.h
	Example of demo utility header file.
*/

#ifndef __ANIMAL3D_DEMOSCENEOBJECT_H
#define __ANIMAL3D_DEMOSCENEOBJECT_H


// math library
#include "animal3D-A3DM/animal3D-A3DM.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
	typedef struct a3_DemoSceneObject	a3_DemoSceneObject;
	typedef struct a3_DemoCamera		a3_DemoCamera;
	typedef struct a3_DemoPointLight	a3_DemoPointLight;
#endif	// __cplusplus

	
//-----------------------------------------------------------------------------


	// general scene objects
	struct a3_DemoSceneObject
	{
		a3mat4 modelMat;	// model matrix: transform relative to scene
		a3mat4 modelMatInv;	// inverse model matrix: scene relative to this
		a3vec3 euler;		// euler angles for direct rotation control
		a3vec3 position;	// scene position for direct control
		a3vec3 scale;		// scale (not accounted for in update)
		a3i32 scaleMode;		// 0 = off; 1 = uniform; other = non-uniform (nightmare)
	};

	// camera/viewer
	struct a3_DemoCamera
	{
		a3_DemoSceneObject *sceneObject;	// pointer to scene object
		a3mat4 projectionMat;				// projection matrix
		a3mat4 projectionMatInv;			// inverse projection matrix
		a3mat4 viewProjectionMat;			// concatenation of view-projection
		a3boolean perspective;				// perspective or orthographic
		a3real fovy;						// persp: vert field of view/ortho: vert size
		a3real aspect;						// aspect ratio
		a3real znear, zfar;					// near and far clipping planes
		a3real ctrlMoveSpeed;				// how fast controlled camera moves
		a3real ctrlRotateSpeed;				// control rotate speed (degrees)
		a3real ctrlZoomSpeed;				// control zoom speed (degrees)
	};

	// simple point light
	struct a3_DemoPointLight
	{
		a3vec4 worldPos;					// position in world space
		a3vec4 viewPos;						// position in viewer space
		a3vec4 color;						// RGB color with padding
		a3real radius;						// radius (distance of effect from center)
		a3real radiusInvSq;					// radius inverse squared (attenuation factor)
		a3real pad[2];						// padding
	};


//-----------------------------------------------------------------------------

	// scene object initializers and updates
	inline void a3demo_initSceneObject(a3_DemoSceneObject *sceneObject);
	inline void a3demo_updateSceneObject(a3_DemoSceneObject *sceneObject, const a3boolean useZYX);
	inline a3i32 a3demo_rotateSceneObject(a3_DemoSceneObject *sceneObject, const a3real speed, const a3real deltaX, const a3real deltaY, const a3real deltaZ);
	inline a3i32 a3demo_moveSceneObject(a3_DemoSceneObject *sceneObject, const a3real speed, const a3real deltaX, const a3real deltaY, const a3real deltaZ);

	inline void a3demo_setCameraSceneObject(a3_DemoCamera *camera, a3_DemoSceneObject *sceneObject);
	inline void a3demo_initCamera(a3_DemoCamera *camera);
	inline void a3demo_updateCameraProjection(a3_DemoCamera *camera);
	inline void a3demo_updateCameraViewProjection(a3_DemoCamera *camera);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_DEMOSCENEOBJECT_H