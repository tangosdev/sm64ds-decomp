typedef struct { int x; int y; int z; } Vector3;

extern void Math_Function_0203b0fc(void *dest, int src, int step, int max);

void func_02007d0c(Vector3 *self, const Vector3 *src, int stepX, int stepY, int stepZ)
{
    Math_Function_0203b0fc((void *)self, src->x, stepX, 0x7fffffff);
    Math_Function_0203b0fc((void *)((char *)self + 4), src->y, stepY, 0x7fffffff);
    Math_Function_0203b0fc((void *)((char *)self + 8), src->z, stepZ, 0x7fffffff);
}
