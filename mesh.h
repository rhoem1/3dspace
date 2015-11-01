#ifndef __MESH_H__
#define __MESH_H__
#include <vector>
#include <list>
using namespace std;
#include "vec3.h"

struct light3;
struct material;
struct triangle;
struct obj3;
struct mesh3;


struct light3 {
	bool on;
	euler orient;
	vec3 origin;
	vec3 color;
};
typedef list<light3> LightList ;



struct material {
	color3 color;
	// other material stuff
	// like texture, spec, etc. whatever
};

struct triangle {
	union {
		struct { vec3Idx a,b,c; }; // named
		vec3Idx v[3]; // numberd (vec3didx)
	};
	float d;
	plane3 norm;
	int matId;
	float closeZ;

	// temp values
	vec3 normRot;

	triangle() {matId = -1; closeZ = 0.0; d = 0.0;};
	triangle(const vec3Idx _a,const vec3Idx _b,const vec3Idx _c) { a = _a; b = _b; c = _c; };
	triangle(const vec3Idx _a,const vec3Idx _b,const vec3Idx _c, const int _m) { a = _a; b = _b; c = _c; matId = _m; };

	triangle &operator =(const triangle &t) { a = t.a; b = t.b; c = t.c; norm = t.norm; matId = t.matId; closeZ = t.closeZ; };

};
typedef int triangleIdx;

struct mesh3 {
	bool wireframe;
	bool showverts;

	// orig verts
	vector<vec3> verts;

	// triangle stuff
	vector<triangle> tris;
	
	// temp
	vector<vec3> workverts;
	typedef list<triangle *> sortedTrisList;
	sortedTrisList sortedTris;

	mesh3() { wireframe = false; };

	// translate my verts, norms, etc
	void translate(const matrix4 &m);

	void addVert(const vec3 &v) {
		verts.push_back(v);
		workverts.push_back(vec3());
	};
	void addVert(const float x, const float y, const float z) {
		vec3 v(x,y,z);
		addVert(v);
	}
	void addTri(const vec3Idx a, const vec3Idx b, const vec3Idx c);

	void Paint(const matrix4 workMat, const obj3 *Obj, LightList &Lights);

	void recalcNorms();

};
typedef vector<mesh3> MeshList ;

struct MeshNfo {
	int idx;
	float z; // dist from viewpoint

	vec3 pos;
	euler dir;

	// where, relative to obj 0,0,0
	matrix4 ori;

	// temps
	// rotated and located in world
	vec3 o;
	vec3 v;

	MeshNfo() { z = 0.0f; };
	MeshNfo &operator =(const MeshNfo &m) {
		ori = m.ori;
		pos = m.pos; dir = m.dir;
		v = m.v; o = m.o;
		z = m.z; idx = m.idx;
	};
	void UpdateLocation(const matrix4 workMat);
};
typedef list<MeshNfo *> MeshNfoList;

struct obj3 {

	// location and rotation in worldspace
	vec3 origin;
	euler orient;

	color3 ambientColor;
	float ambientIntensity;

	// meshes
	MeshList meshList;
	// info on the meshes
	vector<MeshNfo> nfo;
	MeshNfoList sortedNfo;

	// materials used by the meshes
	vector<material> materials;

	void translate(const matrix4 &worldMat);
	void Paint(const matrix4 &viewMat, LightList &Lights);

	// pos, dir are relative to parent object
	// and are stored as a matrix
	void AddMesh(const mesh3 *m, const vec3 &pos, const vec3 &dir) { 
		meshList.push_back(*m);
		MeshNfo n;
		n.pos = pos;
		n.dir = dir;
		n.idx = meshList.size();
		nfo.push_back(n);
	};
	void AddMaterial(const material &m) {materials.push_back(m); };
};





#endif
