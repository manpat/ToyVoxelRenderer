#ifndef SCENE_H
#define SCENE_H

#include "common.h"

struct Scene {
	u32 width, height, depth;

	u8* chunk;

	Scene(u32,u32,u32);
	u8 Cast(vec3 o, const vec3& d);
	u8 Test(const vec3& p);

	void Set(u32, u32, u32, u8);
};

#endif