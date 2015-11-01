#ifndef __SDLVIEWPORT_H__
#define __SDLVIEWPORT_H__

#include <string>
#include <vector>
using namespace std;
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// wrap up the window, renderer in a class

struct sdlViewport {

	// Window and Renderer.
	SDL_Window* window;
	SDL_Renderer* renderer;

	SDL_GLContext glcontext;

	int width;
	int height;

	bool gl_active;

	sdlViewport(int w, int h,const string &s,bool useGL);
	void info();
	~sdlViewport();

	SDL_Texture *CreateTexture( Uint32 format, int access, int w, int h) { return SDL_CreateTexture(renderer,format,access,w,h); };
	SDL_Texture *CreateTextureFromSurface(SDL_Surface * surface) { return SDL_CreateTextureFromSurface(renderer, surface); };

	SDL_bool TargetSupported() {return SDL_RenderTargetSupported(renderer);};
	int SetTarget(SDL_Texture *texture) {return SDL_SetRenderTarget(renderer,texture);};
	SDL_Texture * GetTarget() {return SDL_GetRenderTarget(renderer);};

	int SetLogicalSize(int w, int h) {return SDL_RenderSetLogicalSize(renderer,w,h);};
	void GetLogicalSize(int *w, int *h) {SDL_RenderGetLogicalSize(renderer,w,h);};
	int SetViewport(const SDL_Rect * rect) {return SDL_RenderSetViewport(renderer,rect);};
	void GetViewport(SDL_Rect * rect) {SDL_RenderGetViewport(renderer,rect);};
	int SetClipRect(const SDL_Rect * rect) {return SDL_RenderSetClipRect(renderer,rect);};
	void GetClipRect(SDL_Rect * rect) {SDL_RenderGetClipRect(renderer,rect);};
	int SetScale(float scaleX, float scaleY) {return SDL_RenderSetScale(renderer,scaleX,scaleY);};
	void GetScale(float *scaleX, float *scaleY) {SDL_RenderGetScale(renderer,scaleX,scaleY);};

	int SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {return SDL_SetRenderDrawColor(renderer,r,g,b,a);};
	int GetDrawColor(Uint8 * r, Uint8 * g, Uint8 * b, Uint8 * a) {return SDL_GetRenderDrawColor(renderer,r,g,b,a);};
	int SetDrawBlendMode(SDL_BlendMode blendMode) {return SDL_SetRenderDrawBlendMode(renderer,blendMode);};
	int GetDrawBlendMode(SDL_BlendMode *blendMode) {return SDL_GetRenderDrawBlendMode(renderer,blendMode);};

	int DrawPoint(int x, int y) {return SDL_RenderDrawPoint(renderer,x,y);};
	int DrawPoints(const SDL_Point * points, int count) {return SDL_RenderDrawPoints(renderer,points,count);};
	int DrawLine(int x1, int y1, int x2, int y2) {return SDL_RenderDrawLine(renderer,x1,y1,x2,y2);};
	int DrawLines(const SDL_Point * points, int count) {return SDL_RenderDrawLines(renderer,points,count);};
	int DrawRect(const SDL_Rect * rect) {return SDL_RenderDrawRect(renderer,rect);};
	int DrawRects(const SDL_Rect * rects, int count) {return SDL_RenderDrawRects(renderer,rects,count);};

	int DrawPolygon(const vector<SDL_Point *> &verts,const  int XOffset,const  int YOffset);

	int FillRect(const SDL_Rect * rect) {return SDL_RenderFillRect(renderer,rect);};
	int FillRects(const SDL_Rect * rects, int count) {return SDL_RenderFillRects(renderer,rects,count);};

	int Copy(SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect) { return SDL_RenderCopy(renderer,texture,srcrect,dstrect);};
	int CopyEx(SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect, const double angle, const SDL_Point *center, const SDL_RendererFlip flip) { return SDL_RenderCopyEx(renderer,texture,srcrect,dstrect,angle,center,flip); };

	int ReadPixels(const SDL_Rect * rect, Uint32 format, void *pixels, int pitch) {return SDL_RenderReadPixels(renderer,rect,format,pixels,pitch);};

	void Present() {
		if(gl_active)
			SDL_GL_SwapWindow(window);
		else
			SDL_RenderPresent(renderer);
	};
	void Clear() {SDL_RenderClear(renderer); };

};


class sdlViewportEx {
	private:
		string msg;
	public:
		sdlViewportEx(const char* f, ... );
		string getMsg() { return msg; }
};

#endif
