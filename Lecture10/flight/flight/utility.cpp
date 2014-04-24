
#include "stdafx.h"
#include "Utility.h"


// 보간.
void utility::lerp(OUT sVector &out, const sVector &a, const sVector &b, float t)
{
	out.x = a.x + (b.x-a.x) * t;
	out.y = a.y + (b.y-a.y) * t;
	//out.z = a.z + (b.z-a.z) * t;
}


// 베지어 곡선을 리턴한다.
// point[ 0 ~ 3]
// t : 0 ~ 1
void utility::bezier(OUT sVector &out, const vector<sVector> &points, const float t)
{
	sVector ab,bc,cd,abbc,bccd;
	utility::lerp(ab, points[ 0], points[ 1], t);
	utility::lerp(bc, points[ 1], points[ 2], t);
	utility::lerp(cd, points[ 2], points[ 3], t);
	utility::lerp(abbc, ab, bc, t);
	utility::lerp(bccd, bc, cd, t);
	utility::lerp(out, abbc, bccd, t);
}
