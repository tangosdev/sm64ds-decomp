extern void Vec3_Sub(void *out, void *a, void *b);
extern int LenVec3(void *v);
extern void *Actor_ClosestPlayer(void *self);
extern int Vec3_HorzDist(void *a, void *b);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern int RandomIntInternal(int *seed);
extern short Actor_HorzAngleToCPlayer(void *self);
extern int WithMeshClsn_IsOnWall(void *self);
extern short Vec3_HorzAngle(void *a, void *b);
extern void ApproachLinear(int *p, int target, int step);
extern void func_ov034_02112688(void *c);

extern int data_0209e650;
extern int data_ov034_02114488[];

void func_ov034_02111c48(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    int len;
    int r4;
    int v[3];

    Vec3_Sub(v, c + 0x5c, c + 0x8cc);
    len = LenVec3(v);

    if (*(unsigned char *)(c + 0x8e0) != 0 && *(unsigned char *)(c + 0x8df) == 0) {
        void *cp = Actor_ClosestPlayer(c);
        if (Vec3_HorzDist(c + 0x5c, (char *)cp + 0x5c) < 0xc8000) {
            *(unsigned char *)(c + 0x8df) = 1;
            *(int *)(c + 0x9c) = -0x1000;
            *(int *)(c + 0xa0) = -0x3c000;
            *(int *)(c + 0xa8) = 0;
        }
    }

    if (DecIfAbove0_Byte((unsigned char *)(c + 0x8da)) == 0) {
        r4 = RandomIntInternal(&data_0209e650);
        {
            unsigned char db = *(unsigned char *)(c + 0x8db);
            unsigned int hi;
            if (db <= 3 || (((hi = (unsigned int)r4 >> 0x1b)) & 1) == 0) {
                *(short *)(c + 0x800 + 0xd8) = Actor_HorzAngleToCPlayer(c);
            } else {
                *(short *)(c + 0x800 + 0xd8) = (short)((hi & 0xf) << 0xc);
            }
        }
        *(unsigned char *)(c + 0x8da) = (unsigned char)((unsigned int)(r4 + 0x1e) >> 0x1b);
        *(unsigned char *)(c + 0x8da) =
            (unsigned char)(*(unsigned char *)(c + 0x8da) - (4 - *(unsigned char *)(c + 0x8db)) * 0x1e);
        if (*(unsigned char *)(c + 0x8da) >= 0x40)
            *(unsigned char *)(c + 0x8da) = 0;
    }

    if (WithMeshClsn_IsOnWall(c + 0x708) != 0 || len > 0x5dc000) {
        r4 = RandomIntInternal(&data_0209e650);
        *(short *)(c + 0x800 + 0xd8) = Vec3_HorzAngle(c + 0x5c, c + 0x8cc);
        *(unsigned char *)(c + 0x8da) = (unsigned char)((unsigned int)(r4 + 0x1e) >> 0x1b);
        *(unsigned char *)(c + 0x8da) =
            (unsigned char)(*(unsigned char *)(c + 0x8da) - (4 - *(unsigned char *)(c + 0x8db)) * 0x1e);
        if (*(unsigned char *)(c + 0x8da) >= 0x40)
            *(unsigned char *)(c + 0x8da) = 0;
    }

    {
        unsigned int idx = (unsigned int)((4 - *(unsigned char *)(c + 0x8db)) & 0xff);
        if (idx >= 4)
            idx = 0;
        ApproachLinear((int *)(c + 0x98), data_ov034_02114488[idx], 0x1000);
    }

    if (*(unsigned char *)(c + 0x8e0) != 0)
        return;
    func_ov034_02112688(c);
}
