
#include <string>
#include <iostream>
#include <vector>
using namespace std;
#include <SDL2/SDL.h>

#include "sdlViewport.h"


sdlViewport::sdlViewport(int w, int h, const string &s,bool useGL) {

	height = h; width = w;
	gl_active = useGL;

	// Initialize SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		throw new sdlViewportEx("SDL Init Failed");

	// Create the application window. The size chosen is logical, not the exact
	// number of pixels of the window (mobiles will override it).
	int args = SDL_WINDOW_SHOWN | (useGL?SDL_WINDOW_OPENGL:0);
	window = SDL_CreateWindow(s.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			width, height, args
			);
	if(useGL) {
		glcontext = SDL_GL_CreateContext(window);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-width/2,width/2,height/2,-height/2,0,1);
		glMatrixMode(GL_MODELVIEW);
	} else {
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	}
}

sdlViewport::~sdlViewport() {
	if(gl_active)
		SDL_GL_DeleteContext(glcontext);  
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void sdlViewport::info() {
	int num = SDL_GetNumRenderDrivers();
	vector<SDL_RendererInfo> info(num + 1);

	for(int i = 0; i < num; i++) 
		SDL_GetRenderDriverInfo(i,&info[i]);

	SDL_GetRendererInfo(renderer,&info[num]);
	

	for(int i = 0; i < num + 1; i++) {
		SDL_RendererInfo *it = &info[i];

		cout << "Name "<< it->name << endl;
		cout << "Flags "<<it->flags << " ( ";
		if(it->flags & SDL_RENDERER_SOFTWARE)
			cout << "SW ";
		if(it->flags & SDL_RENDERER_ACCELERATED)
			cout << "ACCEL ";
		if(it->flags & SDL_RENDERER_PRESENTVSYNC)
			cout << "VSYNC ";
		if(it->flags * SDL_RENDERER_TARGETTEXTURE)
			cout << "TOTEXT ";
		cout << ")" << endl;

		cout << "# Texture Formats" << it->num_texture_formats << endl;

		for(i = 0; i < it->num_texture_formats; i++)
			cout << it->texture_formats[i] << endl;


		cout << "Max Texture Width "<<it->max_texture_width<<endl;

		cout << "Max Texture Height "<<it->max_texture_height<<endl;
	}

}



sdlViewportEx::sdlViewportEx(const char* f, ... ) { 
	string t;
	va_list va;
	va_start(va, f);
	t.resize(strlen(f) * 2);
	vsnprintf((char *)t.c_str(),t.length(),f,va);
	va_end(va);
	msg = t;
};


struct hLine {
	int xStart;
	int xEnd;
	hLine() {xStart = 0; xEnd = 0; };
};

struct hLineList {
	int yStart;
	vector<hLine> lines;
	hLineList() {yStart = 0;};
};

void ScanEdge(int X1, int Y1, int X2, int Y2, int SetXStart, int SkipFirst, hLineList &hList, int &listIdx) {
	int Y, DeltaX, DeltaY, i;
	float InvSlope;


	DeltaX = X2 - X1;
	if((DeltaY = Y2 - Y1) <= 0) 
		return; // 0 length!
	InvSlope = (float)DeltaX / (float)DeltaY;
	i = listIdx;
	for(Y = Y1 + SkipFirst; Y < Y2; Y++, i++) {
		if(SetXStart == 1)
			hList.lines[i].xStart = X1 + (int)(ceilf((Y - Y1) * InvSlope));
		else
			hList.lines[i].xEnd = X1 + (int)(ceilf((Y - Y1) * InvSlope));
	}
	listIdx = i;
}


int sdlViewport::DrawPolygon(const vector<SDL_Point *> &verts,const  int XOffset,const  int YOffset) {

#define INDEX_FORWARD(Index) Index = (Index + 1) % verts.size()
#define INDEX_BACKWARD(Index) Index = (Index - 1 + verts.size()) % verts.size()
#define INDEX_MOVE(Index,Direction) if (Direction > 0) INDEX_FORWARD(Index); else INDEX_BACKWARD(Index);

	int LeftEdgeDir,
		MaxPoint_Y,MinPoint_Y,
		MinIndexL,MaxIndex,MinIndexR, listIdx,
		DeltaXN,DeltaYN,DeltaXP,DeltaYP,
		NextIndex, CurrentIndex, PreviousIndex,
		TopIsFlat, SkipFirst, listLen;

	hLineList workingList;

	// reject empty vert list
	if(verts.size() == 0) return 1;

	// find the top and bottom
	MaxPoint_Y = MinPoint_Y = verts[MaxIndex = MinIndexL = 0]->y;
	for(int i = 1; i < verts.size(); i++) {
		if(verts[i]->y < MinPoint_Y)
			MinPoint_Y = verts[MinIndexL = i]->y;
		if(verts[i]->y > MaxPoint_Y)
			MaxPoint_Y = verts[MaxIndex = i]->y;
	}

	// reject 0-height polys
	if(MinPoint_Y == MaxPoint_Y) return 2;

	// find the first not-top
	MinIndexR = MinIndexL;
	while(verts[MinIndexR]->y == MinPoint_Y) {
		INDEX_FORWARD(MinIndexR);
	}
	INDEX_BACKWARD(MinIndexR);

	// find the last not-top
	while(verts[MinIndexL]->y == MinPoint_Y) {
		INDEX_BACKWARD(MinIndexL);
	}
	INDEX_FORWARD(MinIndexL);

	// if top is flat, then make some assumptions
	LeftEdgeDir = -1;
	if((TopIsFlat = (verts[MinIndexL]->x != verts[MinIndexR]->x)?1:0) == 1) {
		if(verts[MinIndexL]->x > verts[MinIndexR]->x) {
			LeftEdgeDir = 1;
			int t = MinIndexL; MinIndexL = MinIndexR; MinIndexR = t;
		}
	} else {
		// else figure out it's slope
		NextIndex = MinIndexR;
		INDEX_FORWARD(NextIndex);
		PreviousIndex = MinIndexL;
		INDEX_BACKWARD(PreviousIndex);
		DeltaXN = verts[NextIndex]->x - verts[MinIndexL]->x;
		DeltaYN = verts[NextIndex]->y - verts[MinIndexL]->y;
		DeltaXP = verts[PreviousIndex]->x - verts[MinIndexL]->x;
		DeltaYP = verts[PreviousIndex]->y - verts[MinIndexL]->y;
		if((DeltaXN * DeltaYP - DeltaYN * DeltaXP) < 0) {
			LeftEdgeDir = 1;
			int t = MinIndexL; MinIndexL = MinIndexR; MinIndexR = t;
		}
	}

	listLen = MaxPoint_Y - MinPoint_Y - 1 + TopIsFlat;

	// reject anything that's too thin still
	if(listLen <= 0) return 3;
	for(int i = 0; i < listLen; i++)
		workingList.lines.push_back(hLine());
	workingList.yStart = YOffset + MinPoint_Y + 1 - TopIsFlat;

	// scan edges
	PreviousIndex = CurrentIndex = MinIndexL;
	SkipFirst = TopIsFlat?0:1;
	listIdx = 0;
	do {
		INDEX_MOVE(CurrentIndex,LeftEdgeDir);
		ScanEdge(verts[PreviousIndex]->x + XOffset, verts[PreviousIndex]->y,
				verts[CurrentIndex]->x + XOffset, verts[CurrentIndex]->y,
				1, SkipFirst, workingList, listIdx);
		PreviousIndex = CurrentIndex;
		SkipFirst = 0;
	} while(CurrentIndex != MaxIndex);

	PreviousIndex = CurrentIndex = MinIndexR;
	SkipFirst = TopIsFlat?0:1;
	listIdx = 0;
	do {
		INDEX_MOVE(CurrentIndex,-LeftEdgeDir);
		ScanEdge(verts[PreviousIndex]->x + XOffset - 1, verts[PreviousIndex]->y,
				verts[CurrentIndex]->x + XOffset - 1, verts[CurrentIndex]->y,
				0, SkipFirst, workingList, listIdx);
		PreviousIndex = CurrentIndex;
		SkipFirst = 0;
	} while(CurrentIndex != MaxIndex);

	for(int i = 0; i < workingList.lines.size(); i++) {
		if(workingList.lines[i].xStart == workingList.lines[i].xEnd)
			DrawPoint(workingList.lines[i].xStart,workingList.yStart+i);
		else
			DrawLine(workingList.lines[i].xStart,workingList.yStart+i,workingList.lines[i].xEnd,workingList.yStart+i);
	}

	return 0;

}

