/*
 * generates unit sized objects
 */
#include "vec3.h"
#include "mesh.h"
#include "unit_shapes.h"

mesh3 *genCubeMesh() {
	mesh3 *Mesh = new mesh3;

	Mesh->addVert(vecMath::Normalize(vec3( 1.0f, 1.0f,-1.0f)));
	Mesh->addVert(vecMath::Normalize(vec3( 1.0f,-1.0f,-1.0f)));
	Mesh->addVert(vecMath::Normalize(vec3(-1.0f,-1.0f,-1.0f)));
	Mesh->addVert(vecMath::Normalize(vec3(-1.0f, 1.0f,-1.0f)));
	Mesh->addVert(vecMath::Normalize(vec3( 1.0f, 1.0f, 1.0f)));
	Mesh->addVert(vecMath::Normalize(vec3( 1.0f,-1.0f, 1.0f)));
	Mesh->addVert(vecMath::Normalize(vec3(-1.0f,-1.0f, 1.0f)));
	Mesh->addVert(vecMath::Normalize(vec3(-1.0f, 1.0f, 1.0f)));


	Mesh->addTri(0,3,2);
	Mesh->addTri(2,1,0);

	Mesh->addTri(3,7,6);
	Mesh->addTri(6,2,3);

	Mesh->addTri(1,2,6);
	Mesh->addTri(6,5,1);

	Mesh->addTri(4,5,6);
	Mesh->addTri(6,7,4);

	Mesh->addTri(4,0,1);
	Mesh->addTri(1,5,4);

	Mesh->addTri(7,3,0);
	Mesh->addTri(0,4,7);


	return Mesh;
}

mesh3 *genIcosahedronMesh() {
	mesh3 *Mesh = new mesh3;

	// y slice
	
	Mesh->addVert(-0.5255731,0,0.850651); // 0

	Mesh->addVert(0.5255731,0,0.850651); // 1
	Mesh->addVert(0,-0.850651,0.5255731); // 2
	Mesh->addVert(-0.850651,-0.5255731,0); // 3
	Mesh->addVert(-0.850651,0.5255731,0); // 4
	Mesh->addVert(0,0.850651,0.5255731); // 5

	Mesh->addVert(0,0.850651,-0.5255731); // 6
	Mesh->addVert(0.850651,0.5255731,0); // 7
	Mesh->addVert(0.850651,-0.5255731,0); // 8
	Mesh->addVert(0,-0.850651,-0.5255731); // 9
	Mesh->addVert(-0.5255731,0,-0.850651); // 10

	Mesh->addVert(0.5255731,0,-0.850651); // 11

	// top
	Mesh->addTri( 0, 1, 2);
	Mesh->addTri( 0, 2, 3);
	Mesh->addTri( 0, 3, 4);
	Mesh->addTri( 0, 4, 5);
	Mesh->addTri( 0, 5, 1); 

	// bottom 
	Mesh->addTri(11, 7, 6);
	Mesh->addTri(11, 8, 7);
	Mesh->addTri(11, 9, 8);
	Mesh->addTri(11,10, 9);
	Mesh->addTri(11, 6,10);

	// middle    
	
	Mesh->addTri( 1, 5, 7);
	Mesh->addTri( 2, 1, 8);
	Mesh->addTri( 3, 2, 9);
	Mesh->addTri( 4, 3,10);
	Mesh->addTri( 5, 4, 6);
	Mesh->addTri( 1, 7, 8);
	Mesh->addTri( 2, 8, 9);
	Mesh->addTri( 3, 9,10);
	Mesh->addTri( 4,10, 6);
	Mesh->addTri( 5, 6, 7);
	

	return Mesh;
}

