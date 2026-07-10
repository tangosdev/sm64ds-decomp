typedef short s16;

struct Vector3 { int x, y, z; };

extern s16 data_ov081_021289a4;
extern s16 data_ov081_021289a6;
extern s16 data_ov081_021289a8;
extern char data_ov081_02128e94;

extern char* _ZN5Actor22ClosestNonVanishPlayerEv(char* self);
extern s16 Vec3_HorzAngle(const struct Vector3* v0, const struct Vector3* v1);
extern void ApproachAngle(s16* p, int target, int a, int b, int c);
extern int AngleDiff(int a, int b);
extern void func_ov081_02125488(char* self, char* p);

void func_ov081_0212423c(char* self, int idx)
{
    char* player;
    struct Vector3 v;
    int ang;
    int off;
    s16* p8;
    s16* p6;
    s16* p4;
    s16* p414;
    int lim;

    player = _ZN5Actor22ClosestNonVanishPlayerEv(self);
    if (player == 0)
        return;

    {
        int* pp = (int*)(((int)(player + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        int py = pp[1];
        int pz = pp[2];
        v.x = pp[0];
        v.y = py;
        v.z = pz;
    }
    ang = Vec3_HorzAngle((struct Vector3*)(self + 0x5c), &v);

    off = idx * 6;
    p8 = (s16*)((char*)&data_ov081_021289a8 + off);
    p6 = (s16*)((char*)&data_ov081_021289a6 + off);
    p4 = (s16*)((char*)&data_ov081_021289a4 + off);

    ApproachAngle((s16*)(self + 0x94), ang, *p4, *p6, *p8);

    if (idx != 1) {
        *(s16*)(self + 0x414) = 0;
        *(int*)(self + 0x410) = 0;
        goto second;
    }

    if (AngleDiff(ang, *(s16*)(self + 0x8e)) > 0x200) {
        p414 = (s16*)(((int)self + 0x414) & 0xFFFFFFFFFFFFFFFFLL);
        lim = -0x2000;
        *p414 = *p414 - 0x100;
        if (*(s16*)(self + 0x400 + 0x14) >= lim)
            goto second;
        *(s16*)(self + 0x400 + 0x14) = (s16)lim;
    } else {
        goto full_tail;
    }

    {
        int* p410 = (int*)(((int)self + 0x410) & 0xFFFFFFFFFFFFFFFFLL);
        *p410 = *p410 + 1;
    }
    if (*(int*)(self + 0x410) > 0x28) {
        *(int*)(self + 0x408) = *(s16*)(self + 0x8e) - ang;
        func_ov081_02125488(self, &data_ov081_02128e94);
        return;
    }
    goto second;

full_tail:
    *(int*)(self + 0x410) = 0;
    ApproachAngle((s16*)(((int)self + 0x414) & 0xFFFFFFFFFFFFFFFFLL), 0, 1, 0x500, 0x500);

second:
    ApproachAngle((s16*)(self + 0x8c), *(s16*)(self + 0x414), *p4, *p6, *p8);
    *(s16*)(self + 0x8e) = *(s16*)(self + 0x94);
}
