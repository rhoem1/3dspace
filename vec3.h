#ifndef __VEC3D_H__
#define __VEC3D_H__

#include <iostream>
#include <vector>
using namespace std;
#include <math.h>

#define kEQUALS 0.001f
#define kPi 3.141596254f
#define k2Pi (2.0f * kPi)
#define kRADIAN (kPi / 180.0f)
#define kDEGREE (1.0f / kRADIAN)
#define k90Rad (90.0f * kRADIAN)

struct vec3;
struct matrix4;
struct plane3;

typedef vec3 euler;
typedef vec3 color3;

class vecMath;
class matrixMath;
class planeMath;
class eulerMath;

typedef int vec3Idx;

/*
 * vec3, also doubles as a rgba so the vecmath stuff works
 *
 *
 */
struct vec3 {
	union {
		struct { float x,y,z,w; }; // named
		struct { float r,g,b,a; }; // colors
		struct { float Pitch, Yaw, Roll, Mag;}; // eulers
		float n[4]; // numbered
	};
	vec3() { x = 0.0f; y = 0.0f; z = 0.0f; w = 0.0f; };
	vec3(const float _x, const float _y, const float _z) {x = _x; y = _y; z = _z; w = 0.0f; };
	vec3(const float _x, const float _y, const float _z, const float _w) {x = _x; y = _y; z = _z; w = _w; };
	void set(const float _x, const float _y, const float _z) {x = _x; y = _y; z = _z; w = 0.0f; };
	void set(const float _x, const float _y, const float _z, const float _w) {x = _x; y = _y; z = _z; w = _w; };

	vec3(const vec3 &v) { x = v.x; y = v.y; z = v.z; w = v.w; };
	vec3 &operator =(const vec3 &v) { x = v.x; y = v.y; z = v.z; w = v.w;  };
	void dump();
};



// planes 
struct plane3 {
	vec3 n;
	float d;
	plane3() {};
	plane3(const vec3 &_n, float _d) { n = _n; d = _d; };
	plane3 &operator =(const plane3 &p) { n = p.n; d = p.d; };
};

typedef enum _planeSide {
	kFront,
	kBack,
	kCoplanar,
	kSplit
} planeSide;




struct matrix4 {
	union {
		struct {
		float m11,m12,m13,m14,
			  m21,m22,m23,m24,
			  m31,m32,m33,m34,
			  m41,m42,m43,m44;
		};
		float m[16];
		float t[4][4];
	};
	matrix4() {
		for(int i = 0; i < 16; i++)
			m[i] = 0.0f;
		t[0][0] = t[1][1] = t[2][2] = t[3][3] = 1.0f;
	};
	matrix4(const matrix4 &a) {
		for(int i = 0; i < 16; i++)
			m[i] = a.m[i];
	};
	matrix4 &operator =(const matrix4 &a) {
		for(int i = 0; i < 16; i++)
			m[i] = a.m[i];
	};
};


// use a class to bulk inline this stuff
class vecMath {
	public:
		// no div fast inverted sqrt
		static float InvSqrt(float x) {
			float xhalf = 0.5f*x;
			int i = *(int*)&x; // get bits for floating value
			i = 0x5f375a86 - (i>>1); // gives initial guess
			x = *(float*)&i; // convert bits back to float
			x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
			return x;
		};

		static float DotProduct(const vec3 &a, const vec3 &b) {
			return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
		};

		static vec3 CrossProduct(const vec3 &a, const vec3 &b) {
			vec3 r;
			r.x = a.y * b.z - a.z * b.y;
			r.y = a.z * b.x - a.x * b.z;
			r.z = a.x * b.y - a.y * b.x;
			return r;
		};

		// returns true distance
		static float Distance(const vec3 &a, const vec3 &b) {
			return 1.0f / InvMagnitude(MinusVec(a,b));
		};


		// returns InvMagnitude, so if you want true mag, then 1.0 / m
		static float InvMagnitude(const vec3 &a) {
			float magSq = a.x * a.x + a.y * a.y + a.z * a.z;
			if(magSq != 0.0f) {
				return InvSqrt(magSq);
			}
			return 0.0f;
		};

		static vec3 MinusVec(const vec3 &a, const vec3 &b) {
			vec3 r;
			r.x = b.x - a.x;
			r.y = b.y - a.y;
			r.z = b.z - a.z;
			return r;
		};

		static vec3 Normalize(const vec3 &a) {
			float mag = vecMath::InvMagnitude(a);
			if(mag != 0.0f) {
				return vecMath::MultiplyScalar(a,mag);
			}
			return a;
		};

		static vec3 MultiplyScalar(const vec3 &a, float s) {
			vec3 r;
			r.x = a.x * s;
			r.y = a.y * s;
			r.z = a.z * s;
			return r;
		};

		static vec3 AddScalar(const vec3 &a, float s) {
			vec3 r;
			r.x = a.x + s;
			r.y = a.y + s;
			r.z = a.z + s;
			return r;
		};

		static vec3 SubtractScalar(const vec3 &a, float s) {
			vec3 r;
			r.x = a.x - s;
			r.y = a.y - s;
			r.z = a.z - s;
			return r;
		};

		static vec3 SubtractVec(const vec3 &a, const vec3 &b) {
			vec3 r;
			r.x = a.x - b.x;
			r.y = a.y - b.y;
			r.z = a.z - b.z;
			return r;
		};
		static vec3 AddVec(const vec3 &a, const vec3 &b) {
			vec3 r;
			r.x = a.x + b.x;
			r.y = a.y + b.y;
			r.z = a.z + b.z;
			return r;
		};
		static vec3 MultiplyVec(const vec3 &a, const vec3 &b) {
			vec3 r;
			r.x = a.x * b.x;
			r.y = a.y * b.y;
			r.z = a.z * b.z;
			return r;
		};

		static vec3 Project(const vec3 &orig, const vec3 &a, float dist) {
			return AddVec(orig,MultiplyScalar(a,dist));
		};

		static vec3 Clamp(const vec3 &v, const float lim) {
			vec3 n = v;
			if(lim > 0.0f) {
				if(v.x > lim) n.x = lim;
				if(v.y > lim) n.y = lim;
				if(v.z > lim) n.z = lim;
				if(v.w > lim) n.w = lim;
			} else {
				if(v.x < lim) n.x = lim;
				if(v.y < lim) n.y = lim;
				if(v.z < lim) n.z = lim;
				if(v.w < lim) n.w = lim;
			}
			return n;
		}

};


class planeMath {
	public:


		// given 3 points, create a plane struct
		// a, b, c AS Vec3D
		// returns Plane3d
		static plane3 PlaneFromVecs(const vec3 &a, const vec3 &b, const vec3 &c) {
			vec3 p = vecMath::Normalize(vecMath::CrossProduct(
						vecMath::SubtractVec(b, a),
						vecMath::SubtractVec(c, a)
						));
			float d = -(vecMath::DotProduct(p, a));

			return plane3(p, d);
		};

		static plane3 PlaneFromNormalAndVec(const vec3 &n,const vec3 &v) {
			return plane3(n, -(vecMath::DotProduct(n, v)));
		};

		// where is vec v in relation to plane p
		// v as vec3d, p as plane3d
		static planeSide testPlaneVec(const plane3 &p, const vec3 &v) {
			float dp = vecMath::DotProduct(v, p.n) + p.d;
			if(dp > kEQUALS)
				return kFront;
			if(dp < -kEQUALS)
				return kBack;
			return kCoplanar;
		};
/*
		static planeSide testPlaneMesh(const plane3 &p, mesh) {
			bool allfront = true;
			bool allback = true;
			for(int v = 0; v < mesh.vec.size(); v++) {
				switch(testPlaneVec(p, mesh.vec[v])) {
					case kFront:
						allback = false;
						break;
					case kBack:
						allfront = false;
						break;
					case kCoplanar:
						break;
				}
			}
			if(allfront && !allback)
				return kFront;
			if(!allfront && allback)
				return kBack;
			if(!allfront && !allback)
				return kSplit;
			return kCoplanar;
		};
		*/

};




class matrixMath {
	public:

		static matrix4 Translation(const vec3 &v) {
			matrix4 m;
			m.t[3][0] = v.x;
			m.t[3][1] = v.y;
			m.t[3][2] = v.z;
			return m;
		};

		static matrix4 rotPitch(const float ang) {
			matrix4 m;
			float c = cosf(ang);
			float s = sinf(ang);
			m.t[1][1] = c;
			m.t[1][2] = s;
			m.t[2][1] = -s;
			m.t[2][2] = c;
			return m;
		};

		static matrix4 rotYaw (const float t ) {
			matrix4 m;
			float c = cosf(t);
			float s = sinf(t);
			m.t[0][0] = c;
			m.t[0][2] = -s;
			m.t[2][0] = s;
			m.t[2][2] = c;
			return m;
		};

		static matrix4 rotRoll( const float t ) {
			matrix4 m;
			float c = cosf(t);
			float s = sinf(t);
			m.t[0][0] = c;
			m.t[0][1] = s;
			m.t[1][0] = -s;
			m.t[1][1] = c;
			return m;
		};

		static matrix4 rotAxis(const vec3 &a, const float d ) {
			matrix4 m;
			vec3 ax = vecMath::Normalize(a);
			float c = cosf(d);
			float s = sinf(d);
			m.t[0][0] = ax.x*ax.x*(1.0f-c)+c;
			m.t[0][1] = ax.x*ax.y*(1.0f-c)-(ax.z*s);
			m.t[0][2] = ax.x*ax.z*(1.0f-c)-(ax.y*s);
			m.t[1][0] = ax.y*ax.x*(1.0f-c)+(ax.z*s);
			m.t[1][1] = ax.y*ax.y*(1.0f-c)+c;
			m.t[1][2] = ax.y*ax.z*(1.0f-c)-(ax.x*s);
			m.t[2][0] = ax.z*ax.x*(1.0f-c)-(ax.y*s);
			m.t[2][1] = ax.z*ax.y*(1.0f-c)+(ax.x*s);
			m.t[2][2] = ax.z*ax.z*(1.0f-c)+c;
			return m;
		};


		// orient AS Vec3D (unit vector)
		// origin AS Vec3D (location)
		// returns a Matrix
		static matrix4 rotVectorAngle(const vec3 &orient, const vec3 &origin) {
			float sh = sin(orient.x);
			float sp = sin(orient.y);
			float sb = sin(orient.z);
			float ch = cos(orient.x);
			float cp = cos(orient.y);
			float cb = cos(orient.z);
			matrix4 m;
			m.t[0][0] = (ch * cp) + (sh * sp * sb);
			m.t[0][1] = sb * cp;
			m.t[0][2] = (-sh * cb) + (ch * sp * sb);
			m.t[0][3] = 0.0f;

			m.t[1][0] = (-ch * sb) + (sh * sp * cb);
			m.t[1][1] = cp * cb;
			m.t[1][2] = (sh * sb) + (ch * cb * sp);
			m.t[1][3] = 0.0f;

			m.t[2][0] = sh * cp;
			m.t[2][1] = -sp;
			m.t[2][2] = ch * cp;
			m.t[2][3] = 0.0f;

			m.t[3][0] = origin.x;
			m.t[3][1] = origin.y;
			m.t[3][2] = origin.z;
			m.t[3][3] = 1.0f;
			return m;
		};

		// this is probably, horribly wrong
		// loc = vec of where we are
		// lookAt = vec of where to aim
		// up = vec of world up
		static matrix4 objectLookAt(const vec3 &loc, const vec3 &lookAt, const vec3 &up) {
			matrix4 m;

			vec3 viewVec = vecMath::SubtractVec(lookAt, loc);
			float mag = vecMath::InvMagnitude(viewVec);
			viewVec = vecMath::MultiplyScalar(viewVec, 1.0 / mag); // could be just not invinv
			float fDot = vecMath::DotProduct(up,viewVec);
			vec3 upVec = vecMath::Normalize(
					vecMath::MultiplyVec(
						vecMath::SubtractScalar(up, fDot),
						viewVec
						)
					);
			vec3 rightVec = vecMath::CrossProduct(upVec, viewVec);

			m.t[0][0] = rightVec.x;
			m.t[0][1] = rightVec.y;
			m.t[0][2] = rightVec.z;
			m.t[1][0] = upVec.x;
			m.t[1][1] = upVec.y;
			m.t[1][2] = upVec.z;
			m.t[2][0] = viewVec.x;
			m.t[2][1] = viewVec.y;
			m.t[2][2] = viewVec.z;
			m.t[3][0] = loc.x;
			m.t[3][1] = loc.y;
			m.t[3][2] = loc.z;
			return m;
		};

		// loc = vec of where we are
		// lookAt = vec of where to aim
		// up = vec of world up
		static matrix4 cameraLookAt(const vec3 &loc, const vec3 &lookAt, const vec3 &up) {
			matrix4 m;
			vec3 viewVec = vecMath::SubtractVec(lookAt, loc);
			float mag = vecMath::InvMagnitude(viewVec);
			viewVec = vecMath::MultiplyScalar(viewVec, mag);
			float fDot = vecMath::DotProduct(up,viewVec);
			vec3 upVec = vecMath::Normalize(vecMath::MultiplyVec(
						vecMath::SubtractScalar(up, fDot),viewVec));
			vec3 rightVec = vecMath::CrossProduct(upVec, viewVec);
			m.t[0][0] = rightVec.x;
			m.t[1][0] = rightVec.y;
			m.t[2][0] = rightVec.z;
			m.t[0][1] = upVec.x;
			m.t[1][1] = upVec.y;
			m.t[2][1] = upVec.z;
			m.t[0][2] = viewVec.x;
			m.t[1][2] = viewVec.y;
			m.t[2][2] = viewVec.z;
			m.t[0][3] = -vecMath::DotProduct(loc, rightVec);
			m.t[1][3] = -vecMath::DotProduct(loc, upVec);
			m.t[2][3] = -vecMath::DotProduct(loc, viewVec);
			return m;
		};

		static matrix4 inverse(const matrix4 &a) {
			matrix4 m;
			m.t[0][0] = a.t[0][0];
			m.t[0][1] = a.t[1][0];
			m.t[0][2] = a.t[2][0];
			m.t[1][0] = a.t[0][1];
			m.t[1][1] = a.t[1][1];
			m.t[1][2] = a.t[2][1];
			m.t[2][0] = a.t[0][2];
			m.t[2][1] = a.t[1][2];
			m.t[2][2] = a.t[2][2];
			m.t[3][0] = -(a.t[3][0] * a.t[0][0] + a.t[3][1] * a.t[0][1] + a.t[3][2] * a.t[0][2]);
			m.t[3][1] = -(a.t[3][0] * a.t[1][0] + a.t[3][1] * a.t[1][1] + a.t[3][2] * a.t[1][2]);
			m.t[3][2] = -(a.t[3][0] * a.t[2][0] + a.t[3][1] * a.t[2][1] + a.t[3][2] * a.t[2][2]);
			return m;
		};


		// multiply matrix by matrix 
		static matrix4 concat(const matrix4 &a,const matrix4 &b) {
			matrix4 m;
			for(int j = 0; j < 4; j++)
				for(int i = 0; i < 4; i++) {
					m.t[i][j] = 0;
					for(int k = 0; k < 4; k++)
						m.t[i][j] += b.t[i][k] * a.t[k][j];
				}
			return m;
		};




		// multiply vector by matrix, return vector
		// gets called a whoooole bunch
		// v AS Vec3D, w as optional single (would be v.w)
		// returns AS Vec3D
		static vec3 MultiplyVectorByMatrix(const vec3 &v,const matrix4 &a,const float w = 0.0) {
			vec3 m;
			m.x = v.x * a.t[0][0] + v.y * a.t[1][0] + v.z * a.t[2][0] + w * a.t[3][0];
			m.y = v.x * a.t[0][1] + v.y * a.t[1][1] + v.z * a.t[2][1] + w * a.t[3][1];
			m.z = v.x * a.t[0][2] + v.y * a.t[1][2] + v.z * a.t[2][2] + w * a.t[3][2];
			m.w = v.x * a.t[0][3] + v.y * a.t[1][3] + v.z * a.t[2][3] + w * a.t[3][3];
			return m;
		};

};

class eulerMath {
	public:
		// orientation functions

		// convert Euler to Radian (unit)
		// euler.X = heading
		// euler.Y = pitch
		// euler.Z = roll
		// euler AS Vec3D, returns AS Vec3D
		static vec3 EulerToUnit(const euler &e) {
			vec3 v;
			v.x = cosf(e.Yaw) * sin(e.Pitch);
			v.y = -sinf(e.Yaw);
			v.z = cosf(e.Yaw) * cos(e.Pitch);
			return vecMath::Normalize(v);
		};

		static matrix4 UnitToMatrix(const vec3 &u) {
			matrix4 m;
			m = eulerMath::EulerToMatrix(eulerMath::UnitToEuler(u));
			return m;
		};

		static matrix4 EulerToMatrix(const euler &e) {
			matrix4 m;
			//vec3 u = eulerMath::EulerToUnit(e);
			//return matrixMath::rotVectorAngle(u,e.Roll);
			m = matrixMath::concat(
				matrixMath::concat(
					matrixMath::rotRoll(e.Roll)
					,
					matrixMath::rotYaw(e.Yaw)
				),
				matrixMath::rotPitch(e.Pitch)
			);

			return m;
		};
		static matrix4 EulerToMatrix(const euler &e, const vec3 &o) {
			matrix4 m = eulerMath::EulerToMatrix(e);
			m = matrixMath::Translation(o);

			return m;
		};

		// converts a unit vector back to Euler angles
		// unit AS Vec3D, euler AS EulerVec (returns)
		static euler UnitToEuler(const vec3 &unit) {
			euler e;
			e.Pitch=0.0f;
			e.Yaw=0.0f;
			e.Mag=1.0f;

			e.Pitch = atan2f(unit.x, unit.y);
			e.Yaw = atan2f(-unit.y, 1.0f / vecMath::InvSqrt(unit.x * unit.x + unit.z * unit.z));
			e.Roll = 0.0f;
			return e;
		};

		// wraps around pi
		// x AS SINGLE
		// returns AS SINGLE
		static float WrapPi(const float &x) {
			float tx = x + kPi;
			tx = floorf(tx * (1 / k2Pi)) * k2Pi;
			return tx - kPi;
		};
		// trims values to canonical range,
		// heading +/- 180, pitch +/- 90, bank +/- 180
		// vec AS Vec3D
		static euler Canonize(const euler &eu) {
			euler e;
			e.Pitch = WrapPi(eu.Pitch);
			if(e.Pitch < k90Rad) {
				e.Yaw = eu.Yaw + kPi;
				e.Pitch = -kPi - eu.Pitch;
				e.Roll = eu.Roll + kPi;
			} else {
				if(e.Pitch > k90Rad) {
					e.Yaw = eu.Yaw + kPi;
					e.Pitch = kPi - eu.Pitch;
					e.Roll = eu.Roll + kPi;
				}
				if(fabsf(e.Pitch) > k90Rad) {
					// Gimbal lock, I fix!
					e.Yaw = eu.Yaw + eu.Roll;
					e.Roll = 0.0f;
				} else {
					e.Roll = WrapPi(eu.Roll);
				}
				e.Yaw = WrapPi(eu.Yaw);
			}
			return e;
		};


};

#endif
