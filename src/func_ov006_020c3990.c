typedef struct { int x, y, z; } Vec3;
typedef struct { int a, b; } S8;

extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *m, void *f, int a, int fix, unsigned int u);
extern int RandomIntInternal(int *seed);
extern void func_ov006_020c3754(void *a, Vec3 *b, Vec3 *d);

extern int data_0209e650;
extern int data_ov006_0213aec8[2];

void func_ov006_020c3990(char *c)
{
    int i;
    char *p;
    struct { Vec3 pos, vel, pos2, vel2; } b;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd18, *(void **)(c + 0xd98), 0x40000000, 0x800, 0);

    *(int *)(c + 0xd70) = 0;

    p = c;
    for (i = 0; i < 0x16; i++, p += 0x98) {
        if (*(int *)(p + 0x48) == 0) {
            b.pos.x = 0x2000;
            b.pos.y = 0xa000;
            b.pos.z = 0x1000;
            b.vel.x = 0x600;
            b.vel.y = 0xa00;
            b.vel.z = 0xa00;
            b.vel.x += (int)((unsigned int)(RandomIntInternal(&data_0209e650) & ~0x80000000) >> 19) >> 2;
            b.vel.y += (int)((unsigned int)(RandomIntInternal(&data_0209e650) & ~0x80000000) >> 19) >> 2;
            b.vel.z += (int)((unsigned int)(RandomIntInternal(&data_0209e650) & ~0x80000000) >> 19) >> 2;
            b.pos2.x = b.pos.x;
            b.pos2.y = b.pos.y;
            b.pos2.z = b.pos.z;
            b.vel2.x = b.vel.x;
            b.vel2.y = b.vel.y;
            b.vel2.z = b.vel.z;
            func_ov006_020c3754(c + 8 + i * 0x98, &b.pos2, &b.vel2);
            break;
        }
    }

    *(S8 *)c = *(S8 *)&data_ov006_0213aec8[0];
}
