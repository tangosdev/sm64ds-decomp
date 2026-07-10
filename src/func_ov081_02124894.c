typedef unsigned char u8;
typedef short s16;
typedef int Fix12i;
typedef struct { int x, y, z; } Vector3;

extern int ApproachAngle(s16* angle, int target, int a, int b, int max);
extern int _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void* a, Vector3* v, unsigned n, Fix12i f, short s);
extern void func_ov081_02124134(void* c);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void* c);

int func_ov081_02124894(char* c)
{
    Vector3 v;
    /* Source written as (>0 ? sub : add) so mwccarm inversion emits the
       ROM's le/add arm first, then the unconditional sub arm. */
    if (*(int*)(c + 0x408) > 0)
        *(s16*)(((int)c + 0x414) & 0xFFFFFFFFFFFFFFFF) =
            *(s16*)(((int)c + 0x414) & 0xFFFFFFFFFFFFFFFF) - 0x200;
    else
        *(s16*)(((int)c + 0x414) & 0xFFFFFFFFFFFFFFFF) =
            *(s16*)(((int)c + 0x414) & 0xFFFFFFFFFFFFFFFF) + 0x200;

    {
        s16 *ang = (s16*)(((int)c + 0x94) & 0xFFFFFFFFFFFFFFFF);
        *ang = *ang + *(s16*)((c + 0x400) + 0x14);
    }
    ApproachAngle((s16*)(c + 0x8c), -0x2800, 1, 0x500, 0x500);
    {
        int *p = (int*)(((int)c + 0x410) & 0xFFFFFFFFFFFFFFFF);
        *p = *p + 1;
    }
    if (*(int*)(c + 0x410) <= 0x28) goto done;
    if (*(u8*)(c + 0x469) == 0) {
        v.x = *(int*)(c + 0x5c);
        v.y = *(int*)(c + 0x60);
        v.z = *(int*)(c + 0x64);
        _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &v, *(u8*)(c + 0x10a) + 1, 0xa000, 0);
    }
    func_ov081_02124134(c);
    _ZN5Actor24KillAndTrackInDeathTableEv(c);
done:
    return 1;
}
