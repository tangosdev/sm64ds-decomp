typedef int Fix12i;
struct Vec3 { int x, y, z; };

extern void *_ZN5Actor13ClosestPlayerEv(void);
extern Fix12i Vec3_Dist(const struct Vec3 *a, const struct Vec3 *b);
extern short Vec3_HorzAngle(const struct Vec3 *a, const struct Vec3 *b);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *c);

void func_ov084_02129cf4(char *c, Fix12i distThresh)
{
    struct Vec3 ppos;

    *(void **)(c + 0x438) = _ZN5Actor13ClosestPlayerEv();

    if (*(void **)(c + 0x438) == 0
        || (Vec3_Dist((struct Vec3*)(c+0x5c), (struct Vec3*)(c+0x41c)) > distThresh
            && *(unsigned char*)(c+0x113) >= 6)) {
        *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vec3*)(c+0x5c), (struct Vec3*)(c+0x41c));
        *(int*)(c+0x440) = 0x61a8000;
        return;
    }

    {
        int *ppos_src = (int *)(int)((long long)(int)(*(char **)(c + 0x438) + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
        ppos.x = ppos_src[0];
        ppos.y = ppos_src[1];
        ppos.z = ppos_src[2];
    }

    if (*(unsigned char*)(c+0x113) < 6) {
        if (Vec3_Dist((struct Vec3*)(c+0x5c), (struct Vec3*)(c+0x41c)) > distThresh
            && !_ZNK12WithMeshClsn8IsOnWallEv(c+0x1b4)) {
            *(int*)(c+0x440) = 0x61a8000;
            *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vec3*)(c+0x5c), (struct Vec3*)(c+0x41c));
            return;
        }

        if (Vec3_Dist((struct Vec3*)(c+0x5c), &ppos) < *(int*)(c+0x448)) {
            *(int*)(c+0x440) = Vec3_Dist((struct Vec3*)(c+0x5c), &ppos);
            if (*(unsigned short*)(c+0x400+0x58) != 0) {
                *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vec3*)(c+0x5c), &ppos);
                return;
            }
            *(short*)(c+0x400+0x5a) = Vec3_HorzAngle(&ppos, (struct Vec3*)(c+0x5c));
            return;
        }
        *(int*)(c+0x440) = 0x61a8000;
        return;
    }

    if (Vec3_Dist((struct Vec3*)(c+0x41c), &ppos) > distThresh) {
        *(int*)(c+0x440) = 0x61a8000;
        return;
    }
    *(int*)(c+0x440) = Vec3_Dist((struct Vec3*)(c+0x5c), &ppos);
    *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vec3*)(c+0x5c), &ppos);
}
