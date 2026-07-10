typedef struct Vector3 { int x, y, z; } Vector3;
typedef struct Matrix4x3 { int m[12]; } Matrix4x3;

void MulVec3Mat4x3(Vector3* v, Matrix4x3* m, Vector3* dst)
{
    int y = v->y;
    int x = v->x;
    int z = v->z;
    int* py = (int*)(int)(((long long)(int)((char*)dst + 4)) & 0xFFFFFFFFFFFFFFFFLL);
    int* pz = (int*)(int)(((long long)(int)((char*)dst + 8)) & 0xFFFFFFFFFFFFFFFFLL);

    dst->x = (int)(((long long)x * m->m[0] + (long long)y * m->m[3] + (long long)z * m->m[6]) >> 12);
    dst->x += m->m[9];
    dst->y = (int)(((long long)x * m->m[1] + (long long)y * m->m[4] + (long long)z * m->m[7]) >> 12);
    *py += m->m[10];
    dst->z = (int)(((long long)x * m->m[2] + (long long)y * m->m[5] + (long long)z * m->m[8]) >> 12);
    *pz += m->m[11];
}
