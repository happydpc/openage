#include "engine.h"

#include <stdio.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "texture.h"
#include "../log/log.h"

namespace openage {
namespace engine {

//global engine variables; partially accesible via engine.h
SDL_GLContext glcontext;
SDL_Window *window;

draw_method_ptr draw_method;
input_handler_ptr input_handler;

bool running;
unsigned window_x = 800;
unsigned window_y = 600;



void init(draw_method_ptr draw_method, input_handler_ptr input_handler) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		log::fatal("SDL initialization: %s\n", SDL_GetError());
		//TODO exception
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow("openage", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_x, window_y, SDL_WINDOW_OPENGL);

	if (window == nullptr) {
		log::fatal("SDL window creation: %s\n", SDL_GetError());
		//TODO exception
		return;
	}


	// load support for the JPG and PNG image formats
	int wanted_image_formats = IMG_INIT_JPG | IMG_INIT_PNG;
	int sdlimg_inited = IMG_Init(wanted_image_formats);
	if ((sdlimg_inited & wanted_image_formats) != wanted_image_formats) {
		log::fatal("IMG_Init: Failed to init required jpg and png support!\n");
		log::fatal("IMG_Init: %s\n", IMG_GetError());
		//TODO exception
		return;
	}

	glcontext = SDL_GL_CreateContext(window);

	//vsync on
	SDL_GL_SetSwapInterval(1);

	//enable alpha blending
	glEnable(GL_BLEND);

	//order of drawing relevant for depth
	//what gets drawn last is displayed on top.
	glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	engine::draw_method = draw_method;
	engine::input_handler = input_handler;
}

void destroy() {
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
}

void engine_window_resized(unsigned w, unsigned h) {

	//store the current window size
	window_x = w;
	window_y = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);

	// set orthographic projection: left, right, bottom, top, nearVal, farVal
	glOrtho(0, w, 0, h, 9001, -1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	log::dbg("engine window has been resized to %ux%u\n", w, h);
}

void engine_input_handler(SDL_Event *e) {
	switch(e->type) {
	case SDL_WINDOWEVENT:
		switch(e->window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			engine_window_resized(e->window.data1, e->window.data2);
			break;
		}
		break;
	}
}

void loop() {
	int glerrorstate;
	running = true;
	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			//first, handle the event ourselves (e.g.: detect window resize)
			engine_input_handler(&e);
			//now, give the event to the user of the engine
			input_handler(&e);
		}

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		draw_method();

		glerrorstate = glGetError();
		if (glerrorstate != 0) {
			log::fatal("opengl error %d", glerrorstate);
			return;
		}

		SDL_GL_SwapWindow(window);
	}
}

} //namespace engine
} //namespace openage
