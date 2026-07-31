#pragma opt_strength_reduction off

typedef struct { int x, y; } V2;

typedef struct C {
    char pad0[4];
    int f4;             /* 0x04 */
    int f8;             /* 0x08 */
    int fc;             /* 0x0c */
    int f10;            /* 0x10 */
    int f14;            /* 0x14 */
    char pad18[0x10];   /* 0x18 */
    int f28;            /* 0x28 */
    char pad2c[8];      /* 0x2c */
    int f34;            /* 0x34 */
    unsigned char f38;  /* 0x38 */
    char pad39[3];
    int f3c;            /* 0x3c */
} C;

extern void func_ov006_02115598(void* c, int* src, int v2, int v3, int v5);
extern void func_ov006_02115008(void* p);
extern void func_02012718(void* a, int b);
extern void func_ov006_0211470c(int* a, int* b);
extern void Vec2_Sub(int* o, int* a, int* b);
extern int Vec2_Len(const void* v);

void func_ov006_0211192c(C* c)
{
    V2 pair;
    V2 bufA;
    V2 bufB;
    char* obj;
    int i;

    c->f10 = c->f8;
    c->f14 = c->fc;

    if (c->f38 == 1) {
        if (c->f3c == 0) {
            pair.x = c->f8;
            pair.y = c->fc;
            func_ov006_02115598((void*)c->f4, (int*)&pair, 0x12c, 0, 1);
            func_ov006_02115008((void*)c->f4);
            func_02012718((void*)0x19f, c->f8);
        }
        c->f38 = 0;
        if (c->f34 == 1)
            c->f3c = 0x100;
        else
            c->f3c = 4;
    } else if (c->f3c > 0) {
        {
            int* p = (int*)((long long)(int)&c->f3c);
            *p = *p - 1;
        }
        if (c->f3c < 4) {
            obj = (char*)c->f4;
            i = 0;
            if (*(int*)(obj + 0x4668) > 0) {
                do {
                    void* n = (i >= 0xd) ? (void*)0 : *(void**)(obj + i * 4 + 0x4688);
                    if (*(unsigned char*)((int)n + 0x30) != 0) {
                        void* m = (i >= 0xd) ? (void*)0 : *(void**)(obj + i * 4 + 0x4688);
                        int hit;
                        func_ov006_0211470c((int*)&bufA, (int*)m);
                        Vec2_Sub((int*)&bufB, &c->f8, (int*)&bufA);
                        hit = Vec2_Len((void*)&bufB) < 0x11000;
                        if (hit != 0) {
                            c->f3c = 4;
                            goto done;
                        }
                    }
                    obj = (char*)c->f4;
                    i++;
                } while (i < *(int*)(obj + 0x4668));
            }
        }
    }

done:
    if (c->f34 == 1) {
        if (c->f3c <= 0) {
            int* p = (int*)((long long)(int)&c->f28);
            *p += 0x1000;
            if (c->f28 > 0x7000)
                c->f28 = 0x7000;
        } else {
            int* p = (int*)((long long)(int)&c->f28);
            *p -= 0x1000;
            if (c->f28 < 0)
                c->f28 = 0;
        }
    }
}
