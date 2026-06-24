struct Vec3 { int x, y, z; };
extern void Vec3_Sub(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern void Vec3_RotateYAndTranslate(struct Vec3* out, void* m, int ang, struct Vec3* in);

extern unsigned char data_0209f250;
extern char* data_0209f394[];
extern char data_020a0ebc[];

char* func_ov100_02145f00(char* c)
{
    struct Vec3 tmp;
    char* p = data_0209f394[data_0209f250];
    Vec3_Sub(&tmp, (struct Vec3*)(p + 0x5c), (struct Vec3*)(c + 0x5c));
    Vec3_RotateYAndTranslate((struct Vec3*)(c + 0x80), data_020a0ebc, (short)(-*(short*)(c + 0x8e)), &tmp);
    return p;
}
