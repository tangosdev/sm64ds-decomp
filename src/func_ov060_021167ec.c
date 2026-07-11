struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };

struct Actor {
    char pad[0x92];
    short f92;   /* 0x92 */
    short f94;   /* 0x94 */
    short f96;   /* 0x96 */
};

extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern short Vec3_HorzAngle(const struct Vector3* a, const struct Vector3* b);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, const struct Vector3* v, const struct Vector3_16* p, int e, int f);
extern void _ZN9ActorBase18MarkForDestructionEv(void* a);

void func_ov060_021167ec(char* c)
{
    char* p;
    if (*(unsigned short*)(c + 0x374) != 0) return;
    p = (char*)_ZN5Actor13ClosestPlayerEv(c);
    if (p == 0) return;
    *(short*)(c + 0x94) = Vec3_HorzAngle((struct Vector3*)(c + 0x5c), (struct Vector3*)(p + 0x5c));
    *(int*)(c + 0x360) = 0x5000;
    {
        int i = 0;
        int ang = 0;
        do {
            struct Actor* a = (struct Actor*)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x118, 4, (struct Vector3*)(c + 0x5c), (const struct Vector3_16*)0, *(signed char*)(c + 0xcc), -1);
            short v = *(short*)(c + 0x94) + ang;
            a->f92 = 0;
            a->f94 = v;
            a->f96 = 0;
            *(int*)((char*)a + 0x360) = *(int*)(c + 0x360);
            i++;
            ang += 0x5555;
        } while (i < 3);
    }
    _ZN9ActorBase18MarkForDestructionEv(c);
}
