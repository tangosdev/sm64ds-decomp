typedef struct Vector3 Vector3;

extern int Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern short Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
extern short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);

void func_02007c9c(const Vector3 *v0, const Vector3 *v1, int *outDist, short *outVertAng, short *outHorzAng)
{
    *outDist = Vec3_Dist(v0, v1);
    *outVertAng = Vec3_VertAngle(v1, v0);
    *outHorzAng = Vec3_HorzAngle(v0, v1);
}
