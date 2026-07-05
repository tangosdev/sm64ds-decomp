struct Vector3 { int x, y, z; };
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, struct Vector3 *out, unsigned int idx);
extern int Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
extern short Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
void func_ov019_021113b0(char *c)
{
    struct Vector3 v;
    int *cnt = (int *)(((int)c + 0x36c) & 0xFFFFFFFFFFFFFFFF);
    *cnt = *cnt + 1;
    _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x364, &v, *(int *)(c + 0x36c));
    *(int *)(c + 0x380) = Vec3_Dist((struct Vector3 *)(c + 0x5c), &v);
    *(short *)((char *)(((int)c + 0x300) & 0xFFFFFFFFFFFFFFFF) + 0x8c) = Vec3_HorzAngle((struct Vector3 *)(c + 0x5c), &v);
}
