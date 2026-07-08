struct Vec3 { int x, y, z; };
extern void Vec3_Sub(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern void Vec3_RotateYAndTranslate(struct Vec3* out, void* m, int ang, struct Vec3* in);

extern unsigned char CURR_PLAYER_ID;
extern char* PLAYER_ARR[];
extern char _ZN7Vector38IDENTITYE[];

char* func_ov100_02145f00(char* c)
{
    struct Vec3 tmp;
    char* p = PLAYER_ARR[CURR_PLAYER_ID];
    Vec3_Sub(&tmp, (struct Vec3*)(p + 0x5c), (struct Vec3*)(c + 0x5c));
    Vec3_RotateYAndTranslate((struct Vec3*)(c + 0x80), _ZN7Vector38IDENTITYE, (short)(-*(short*)(c + 0x8e)), &tmp);
    return p;
}
