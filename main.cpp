#include <iostream>
#include <sstream>
#include <SDL2/SDL.h>

#include "common.h"
#include "scene.h"

enum {
	// WIDTH = 800,
	// HEIGHT = 600,
	WIDTH = 640,
	HEIGHT = 480,

	SIZE = WIDTH*HEIGHT,
};

struct Clock {
	u64 frames = 0;
	u32 beg = 0;
	f32 runTime = 0.f;
	f32 dt = 0.f;
	f32 fps = 0.f;
	f32 avgfps = 0.f;

	Clock() {
		Start();
	}

	void Reset() {
		auto end = SDL_GetTicks();
		auto diff = end - beg;
		beg = end;
		frames++;

		dt = diff / 1000.f;
		runTime += dt;
		fps = 1.0 / dt;
		
		constexpr f32 weighting = 10.f;
		avgfps = (avgfps * (weighting - 1.f) + fps) / weighting;
	}

	void Start() {
		beg = SDL_GetTicks();
	}
};

SDL_Surface* screen;

void Render(Scene&, Clock&);

int main(){
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		std::cerr << "SDL init failed" << std::endl;
		return 1;
	}

	auto window = SDL_CreateWindow("Raycast test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
	if(!window) {
		std::cerr << "Window creation failed" << std::endl;
		return 1;
	}

	screen = SDL_GetWindowSurface(window);

	auto renderer = SDL_CreateSoftwareRenderer(screen);
	if(!renderer){
		std::cerr << "Renderer creation failed" << std::endl;
		return 1;
	}

	auto scene = Scene{100, 100, 100};

	std::cout << std::boolalpha;
	std::cout << "Initialisation complete" << std::endl;
	std::cout << "Window size: " << WIDTH << "x" << HEIGHT << std::endl;
	std::cout << "Num pixels: " << SIZE << std::endl;
	std::cout << "Must lock screen?: " << SDL_MUSTLOCK(screen) << std::endl;
	SDL_SetRenderDrawColor(renderer, 0x10, 0x10, 0x10, 0xff);

	auto clock = Clock{};
	auto rendclock = Clock{};

	bool running = true;
	while(running){
		SDL_Event e;
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_QUIT) running = false;
			switch(e.type){
				case SDL_QUIT: running = false; break;
				case SDL_KEYDOWN: {
					if(e.key.keysym.sym == SDLK_ESCAPE) running = false;
					break;
				}
			}
		}

		// Probably not necessary
		SDL_RenderClear(renderer);

		rendclock.Start();
		Render(scene, clock);
		rendclock.Reset();

		SDL_UpdateWindowSurface(window);
		SDL_RenderPresent(renderer);

		clock.Reset();
		if(clock.frames%5 == 0){
			static std::stringstream titleStream;
			titleStream.str("");
			titleStream.precision(3);
			titleStream << "Raytracing test\t\t";
			titleStream << rendclock.dt*1000.f << "ms\t\t";
			titleStream << clock.avgfps << "fps\t\t";
			titleStream.precision(6);
			titleStream << clock.avgfps*SIZE << "rays/s";
			SDL_SetWindowTitle(window, titleStream.str().data());
		}

		SDL_Delay(5);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

void PutPixel(u32 i, Color p){
	((Color*)screen->pixels)[i] = p;
}

void PutPixel(u16 x, u16 y, Color p){
	PutPixel(x + y * WIDTH, p);
}

vec3* rayDirs = nullptr;

void Render(Scene& scene, Clock& clk){
	auto t = clk.runTime;

	auto camPos = vec3{scene.width/2.f + cos(t), scene.height/2.f, sin(t)*20. -50.};

	auto yaw = PI/2. + sin(t*0.5)*PI/6.;
	auto camFwd   = vec3{cos(yaw),0,sin(yaw)};
	auto camRight = glm::cross(camFwd, vec3{0,1,0});
	auto camUp    = glm::cross(camRight, camFwd);

	if(!rayDirs){
		rayDirs = new vec3[WIDTH*HEIGHT];

		auto fov = 2.*PI/3.;
		auto aspect = WIDTH * 1.0/HEIGHT;

		for(u32 y = 0; y < HEIGHT; y++){
			f32 sy = (y-HEIGHT*0.5f)/HEIGHT*0.5f;
			f32 phi = -sy*fov + PI/2.0f;
			
			for(u32 x = 0; x < WIDTH; x++){
				f32 sx = (x-WIDTH*0.5f)/WIDTH*0.5f * aspect;
				f32 theta = -sx*fov + PI / 2.0;

				vec3 a;
				a.x = sin(phi)*cos(theta);
				a.y = cos(phi);
				a.z = sin(phi)*sin(theta);

				rayDirs[x+y*WIDTH] = a;
			}
		}
	}

	for(u32 y = 0; y < HEIGHT; y++){
		for(u32 x = 0; x < WIDTH; x++){
			auto a = rayDirs[x+y*WIDTH];
			auto dir = camRight * a.x + camUp * a.y + camFwd * a.z;

			auto b = scene.Cast(camPos, glm::normalize(dir));
			PutPixel(x, y, b*0x00f00fff);
		}
	}
}