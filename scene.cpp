#include <algorithm>
#include <cstring>
#include <cmath>

#include "scene.h"

Scene::Scene(u32 w, u32 h, u32 d) : width{w}, height{h}, depth{d}{
	chunk = new u8[w*h*d] {0};

	srand(0);
	for(u32 z = 0; z < 10; z++){
		for(u32 y = 0; y < height; y++)
		for(u32 x = 0; x < width; x++){
			if(rand()%5 == 0)
				Set(x, y, z, /*x*y*z + */rand());
		}
	}
}

struct IntersectResult {
	f32 t;
	bool intersect;
};

static IntersectResult IntersectAABB(const vec3& o, const vec3& inv, const vec3& lb, const vec3& ub){
	f32 tx1 = (lb.x - o.x) * inv.x;
	f32 tx2 = (ub.x - o.x) * inv.x;
	f32 ty1 = (lb.y - o.y) * inv.y;
	f32 ty2 = (ub.y - o.y) * inv.y;
	f32 tz1 = (lb.z - o.z) * inv.z;
	f32 tz2 = (ub.z - o.z) * inv.z;

	f32 tmin = std::max(std::max(std::min(tx1, tx2), std::min(tz1, tz2)), std::min(ty1, ty2));
	f32 tmax = std::min(std::min(std::max(tx1, tx2), std::max(tz1, tz2)), std::max(ty1, ty2));

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behind us
	// if tmin > tmax, ray doesn't intersect AABB
	auto oob = (u32)(tmax < 0 || tmin > tmax);

	return {tmin * (1 - oob) + tmax * oob, !oob};
}

u8 Scene::Test(const vec3& p){
	auto greater = p.x > width || p.y > height || p.z > depth;
	auto lesser = p.x < 0.f || p.y < 0.f || p.z < 0.f;

	auto noob = 1 - (u32)(greater || lesser);

	u32 x = (u32)floor(p.x);
	u32 y = (u32)floor(p.y);
	u32 z = (u32)floor(p.z);
	u64 idx = x + y*width + z*width*height;

	return chunk[idx*noob]*noob;
}

u8 Scene::Cast(vec3 o, const vec3& d){
	vec3 inv; // TODO: Cache
	inv.x = 1.0f / d.x;
	inv.y = 1.0f / d.y;
	inv.z = 1.0f / d.z;
	
	auto inx = IntersectAABB(o, inv, vec3{0,0,0}, vec3{width, height, depth});
	if(!inx.intersect) return 0;

	o += d * inx.t;

	for(u32 i = 0; i < 500; i++){
		auto b = Test(o);
		if(b > 0) return b;

		o += d*0.1f;
	}

	return 0;
}

void Scene::Set(u32 x, u32 y, u32 z, u8 v){
	chunk[x + y*width + z*width*height] = v;
}
