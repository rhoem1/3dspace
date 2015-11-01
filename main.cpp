#include <iostream>
using namespace std;
#include <SDL2/SDL.h>
#include "sdlViewport.h"
#include "vec3.h"
#include "mesh.h"
#include "unit_shapes.h"

#ifdef __ANDROID__
#define DIR_CUR "/sdcard/"
#define DIR_SEP     "/"
#else //UNIX
#define DIR_CUR ""
#define DIR_SEP "/"
#endif

//#define WIDTH  640
//#define HEIGHT 480

#define WIDTH 1024
#define HEIGHT 600

#define PROGRAM_NAME "3dtest"

bool USING_OPENGL;

sdlViewport *SDL;
matrix4 PolyWorldXform;
matrix4 WorldViewXform;
matrix4 PerspectiveXform;


euler Rotation;
vec3 Rotdir;
vec3 Origin;
vec3 CameraDirection;
extern float ProjectionScale;
int MoveScale;

int active_mesh;

LightList Lights;

obj3 Obj;

void setupLights() {
	Obj.ambientIntensity = 1.0f;
	Obj.ambientColor = vec3(0.2,0.2,0.2);
	light3 l;
	l.on = true;
	l.orient = vecMath::Normalize(vec3(1.0f,0.0f,0.2f));
	l.origin = vec3(0.0,1.0,0.0);
	l.color = vec3(1.0,0.0,0.0);
	Lights.push_back(l);
	
	l.on = true;
	l.orient = vecMath::Normalize(vec3(0.0f,1.0f,0.2f));
	l.origin = vec3(0.0,-1.333,0.0);
	l.color = vec3(0.0,1.0,0.0);
	Lights.push_back(l);

	l.on = true;
	l.orient = vecMath::Normalize(vec3(1.0f,1.0f,0.2f));
	l.origin = vec3(1.5,1.5,0.0);
	l.color = vec3(0.0,0.0,1.0);
	Lights.push_back(l);


}

void updateLights() {

	for(LightList::iterator light = Lights.begin(); light != Lights.end(); light++) {
		if(light->on) {
			if(fabsf(light->orient.Yaw   += (kPi/180.0f) * light->origin.Yaw  ) >= k2Pi) light->orient.Yaw   -= k2Pi;
			if(fabsf(light->orient.Pitch += (kPi/180.0f) * light->origin.Pitch) >= k2Pi) light->orient.Pitch -= k2Pi;
			if(fabsf(light->orient.Roll  += (kPi/180.0f) * light->origin.Roll ) >= k2Pi) light->orient.Roll  -= k2Pi;
		}
	}

}

void init() {

	// this is a zoom * width * 0.5
	ProjectionScale = 16 * (SDL->width * 0.5f);

	// basic no camera transform
	PerspectiveXform.t[0][0] = PerspectiveXform.t[1][1] = tanf(90.0 * 0.5 * kRADIAN);
	PerspectiveXform.t[2][2] = -1000.0 / (1000.0 - 10.0);
	PerspectiveXform.t[2][3] = -1;
	PerspectiveXform.t[3][2] = -1000.0 * 10.0 / (1000.0 - 10.0);
	PerspectiveXform.t[3][3] = 0;

	CameraDirection = vec3(0.0f,0.0f,1.0f);

	Origin.z = -40.0f;
	MoveScale = 1;

	Rotation.Yaw = 0.0f;
	Rotation.Pitch = 0.0f;
	Rotation.Roll = 0.0f;
	setupLights();

	Obj.AddMesh(genIcosahedronMesh(),Origin,Rotation);
	//meshes.push_back(genCubeMesh());

}

void cleanup() {
	//for(int i=0; i < meshes.size(); i++)
		//delete meshes[i];
}

void draw_frame() {


	matrix4 workMat = 
		matrixMath::concat(
			matrixMath::concat(
				PerspectiveXform,
				matrixMath::Translation( Origin)
			),
			eulerMath::EulerToMatrix(Rotation)
			
		);
	
	Obj.Paint(workMat,Lights);

}

// main function
// debugging/logging would also live here
//


int main(int argc, char *argv[]) {
	USING_OPENGL = false;
	try {
		SDL = new sdlViewport(WIDTH,HEIGHT,PROGRAM_NAME,USING_OPENGL);
		cout << "Initializing" <<endl;
		init();


		cout << "Running loop"<<endl;
		bool running = true;
		while(running) {


			if(USING_OPENGL) {
				glClearColor(0,0,0,1); // Use OpenGL commands, see the OpenGL reference.
				glClear(GL_COLOR_BUFFER_BIT); // clearing screen
			} else {
				SDL->SetDrawColor(2,2,2,255);
				SDL->Clear();
			}

			draw_frame();

			// delays to vsync, so this can make things sync to 60 fps
			SDL->Present();

			// this will wait till the close is clicked
			// or escape is pressed
			static SDL_Event event;
			while (SDL_PollEvent(&event)) {
				if(event.type == SDL_QUIT)
					running = false;
				if(event.type == SDL_KEYDOWN) {
					switch(event.key.keysym.sym) {
						case SDLK_ESCAPE:
						running = false;
						break;
						case SDLK_F1:
						MoveScale--; if (MoveScale < 1) MoveScale = 1;
						break;
						case SDLK_F2:
						MoveScale++;
						break;
						case SDLK_w:
						Origin.z += 0.1 * MoveScale;
						break;
						case SDLK_s:
						Origin.z -= 0.1 * MoveScale;
						break;
						case SDLK_a:
						Origin.x -= 0.1 * MoveScale;
						break;
						case SDLK_d:
						Origin.x += 0.1 * MoveScale;
						break;				
						case SDLK_q:
						Origin.y -= 0.1 * MoveScale;
						break;
						case SDLK_e:
						Origin.y += 0.1 * MoveScale;
						break;				
						case SDLK_i:
						Rotdir.x -= 0.1 * MoveScale;
						break;
						case SDLK_k:
						Rotdir.x += 0.1 * MoveScale;
						break;
						case SDLK_l:
						Rotdir.y -= 0.1 * MoveScale;
						break;
						case SDLK_j:
						Rotdir.y += 0.1 * MoveScale;
						break;
						case SDLK_u:
						Rotdir.z += 0.1 * MoveScale;
						break;
						case SDLK_o:
						Rotdir.z -= 0.1 * MoveScale;
						break;
						case SDLK_SPACE:
						Rotdir.x = Rotdir.y = Rotdir.z = 0;
						break;
						case SDLK_n:
						Rotation.x = Rotation.y = Rotation.z = 0;
						break;
						case SDLK_m:
						Origin.x = Origin.y = 0;
						Origin.z = -10.0;
						break;
						case SDLK_RETURN:
						Rotdir.x = Rotdir.y = Rotdir.z = 1.0;
						break;
						case SDLK_1:
						active_mesh--;
						//active_mesh %= meshes.size();
						break;
						case SDLK_2:
						active_mesh++;
						//active_mesh %= meshes.size();
						break;
						case SDLK_y:
						Obj.ambientIntensity += 0.1;
						if (Obj.ambientIntensity > 1.0) Obj.ambientIntensity = 1.0;
						break;
						case SDLK_h:
						Obj.ambientIntensity -= 0.1;
						if (Obj.ambientIntensity < 0.0) Obj.ambientIntensity = 0.0;
						break;
					}
				}
			}

			updateLights();
			if(fabsf(Rotation.x += (kPi/180.0f) * Rotdir.x) >= k2Pi) Rotation.x -= k2Pi;
			if(fabsf(Rotation.y += (kPi/180.0f) * Rotdir.y) >= k2Pi) Rotation.y -= k2Pi;
			if(fabsf(Rotation.z += (kPi/180.0f) * Rotdir.z) >= k2Pi) Rotation.z -= k2Pi;

			SDL_Delay(5); // 0.010 of a second
		}

		cout << "cleaning up" << endl;


		cleanup();
		delete SDL;
	} catch (sdlViewportEx e) {
		cout << e.getMsg() << endl;
		return 1;
	}

	return 0;
}



