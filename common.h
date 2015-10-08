#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

#ifndef PI
#define PI M_PI
#endif

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8  = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using f32 = float;
using f64 = double;

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat3 = glm::mat3;
using mat4 = glm::mat4;

struct Color {
	union {
		u8 v[4];
		u32 i;
		struct {
			u8 r, g, b, a;
		};
	};

	Color(u8 r, u8 g, u8 b, u8 a) : v{r,g,b,a} {}
	Color(u32 rgba) : i{rgba} {}
};

void PutPixel(u16 x, u16 y, Color p);
void PutPixel(u32 i, Color p);


#endif