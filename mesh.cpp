#include <vector>
#include <list>
using namespace std;
#include <SDL2/SDL.h>
#include "sdlViewport.h"

#include "vec3.h"
#include "mesh.h"

extern sdlViewport *SDL;


extern vec3 CameraDirection;
extern bool USING_OPENGL;


void mesh3::addTri(const vec3Idx a, const vec3Idx b, const vec3Idx c) {
	triangle t(a,b,c);
	t.norm = planeMath::PlaneFromVecs(verts[a], verts[b], verts[c]);
	tris.push_back(t);
}
void mesh3::recalcNorms() {
	for(int t=0; t < tris.size(); t++) 
		tris[t].norm = planeMath::PlaneFromVecs(verts[tris[t].a], verts[tris[t].b], verts[tris[t].c]);
}

void mesh3::translate(const matrix4 &m) {
	float plusZ = 0.0f;
	float negZ = 0.0f;
	for(int i=0;i<verts.size();i++) {
		workverts[i] = matrixMath::MultiplyVectorByMatrix(verts[i],m,1.0f);
		if(workverts[i].z > plusZ) plusZ = workverts[i].z;
		if(workverts[i].z < negZ) negZ = workverts[i].z;
	}

	sortedTris.clear();
	for(int i=0;i<tris.size(); i++) {
		tris[i].closeZ = plusZ; // not sure if this is right
		// find closest z
		for(int v = 1; v < 3; v++) 
			if(tris[i].closeZ > workverts[tris[i].v[v]].z)
				tris[i].closeZ = workverts[tris[i].v[v]].z;
		// rotate normal
		tris[i].normRot = vecMath::Normalize(matrixMath::MultiplyVectorByMatrix(tris[i].norm.n,m,0.0f));
		tris[i].d = vecMath::DotProduct(tris[i].normRot,CameraDirection); // camera direction
	}

	for(int i=0;i<tris.size(); i++) {
		// backface cull
		if(wireframe || tris[i].d >= 0.0) {
			// insert into sorted triangle list
			sortedTrisList::iterator lit;
			for(
					lit = sortedTris.begin();
					lit != sortedTris.end() &&
					(*lit)->closeZ >= tris[i].closeZ;
					lit++
			);
			sortedTris.insert(lit,&tris[i]);
		}
	}
}

float ProjectionScale;

void project(vec3 &v, SDL_Point &pt) {
	pt.x = ((int) (v.x / v.w * ProjectionScale + 0.5f));
	pt.y = ((int) (-v.y / v.w * ProjectionScale + 0.5f));
	if(!USING_OPENGL) {
		pt.x += SDL->width >> 1;
		pt.y += SDL->height >> 1;
	}
}

void mesh3::Paint(const matrix4 workMat, const obj3 *Obj, LightList &Lights) {
	vector<SDL_Point> ScreenVerts;
	translate(workMat);

	for(int i = 0; i < workverts.size(); i++) {
		SDL_Point s;
		project(workverts[i],s);
		ScreenVerts.push_back(s);
	}

	int c = 0;
	for(sortedTrisList::iterator lit = sortedTris.begin();lit != sortedTris.end(); lit++) {
		vec3 shade;
		triangle *t = (*lit);
		shade = vecMath::MultiplyScalar(Obj->ambientColor,Obj->ambientIntensity);
		if(t->matId > -1)
			shade = vecMath::MultiplyVec(Obj->materials[t->matId].color,shade);

		for(
				LightList::iterator light = Lights.begin();
				light != Lights.end();
				light++
			) {
			if(light->on) {
				float diff;
				if((diff = vecMath::DotProduct(eulerMath::EulerToUnit(light->orient),t->normRot)) > 0.0f) {
					shade = vecMath::AddVec(vecMath::MultiplyScalar(light->color,diff),shade);
				}
			}
		}
		shade = vecMath::Clamp(shade, 1.0f);

		if(USING_OPENGL) {
			glBegin(GL_TRIANGLES); // drawing a multicolored triangle
			glColor3f(shade.r,shade.g,shade.b);
			for(int v = 0; v < 3; v++) 
				glVertex2f(ScreenVerts[t->v[v]].x,ScreenVerts[t->v[v]].y);
			glEnd();
		} else {
			SDL->SetDrawColor(255 * shade.r,255 * shade.g,255 * shade.b,255);
			vector<SDL_Point *>draw_points(3);
			for(int v = 0; v < 3; v++)
				draw_points[v] = &ScreenVerts[t->v[v]];
			SDL->DrawPolygon(draw_points,0,0);
		}

	}

}
void MeshNfo::UpdateLocation(const matrix4 workMat) {
	// reorient origin and orientation of mesh
	o = vecMath::Normalize(
		matrixMath::MultiplyVectorByMatrix(
			eulerMath::EulerToUnit(dir)
			,workMat,0.0f
		)
	);
	v = matrixMath::MultiplyVectorByMatrix(pos,workMat,1.0f);
	// update matrix for mesh
	ori = matrixMath::concat(
		workMat,
		matrixMath::concat(
			matrixMath::Translation(v),
			eulerMath::UnitToMatrix(o)
		)
	);

}

void obj3::translate(const matrix4 &worldMat) {
	// set up matrix
	// based on location of object
	matrix4 workMat = 
		matrixMath::concat(
			matrixMath::concat(
				worldMat,
				matrixMath::Translation(origin)
			),
			eulerMath::EulerToMatrix(orient)
		);

	// update origins of meshes
	for(int c = 0; c < nfo.size(); c++ ) {
		nfo[c].UpdateLocation(workMat);
	}

	// update the meshes
	for( int i = 0; i < meshList.size(); i++) 
		meshList[i].translate(nfo[i].ori);

	sortedNfo.clear();
	// sort meshes based on z
	for( int i = 0; i < nfo.size(); i++) {
		MeshNfoList::iterator lit = sortedNfo.begin();
		for(;
				lit != sortedNfo.end() &&
				(*lit)->z >= nfo[i].z;
				lit++
		   );
		sortedNfo.insert(lit,&nfo[i]);
	}
}

void obj3::Paint(const matrix4 &viewMat, LightList &Lights) {

	int i = 0;
	for(
			MeshNfoList::iterator mit = sortedNfo.begin();
			mit != sortedNfo.end();
			mit++,i++
	   )
		meshList[(*mit)->idx].Paint(
				matrixMath::concat( viewMat, nfo[i].ori),
				this, Lights);
}


