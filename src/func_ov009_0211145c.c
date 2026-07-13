/* func_ov009_0211145c — spawn-count loop + free-bird velocity/sound setup.
 * Store/call tail uses mwccarm asm: pure C emits rsb r0 + wrong str/mov
 * interleave for -0x14000 / 0xff06a000 / func_0201267c (same wall as sibling
 * 021116ec avoids by not calling after the stores).
 */
struct V3 { int x, y, z; };
extern char* _ZN5Actor13ClosestPlayerEv(void* self);
extern int Vec3_Sub(struct V3* dst, void* a, void* b);
extern int Vec3_HorzLen(struct V3* v);
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int id, unsigned int p1, void* pos, void* rot, int a, int b);
extern void func_ov009_02111224(char* c, int r1);
extern int func_0201267c(unsigned int a, void* b);
extern int RandomIntInternal(void* rng);
extern int data_0209e650;

void func_ov009_0211145c(char* c) {
    if (*(unsigned char*)(c + 0x180) != 0) {
        char* p;
        struct V3 copy;
        struct V3 diff;
        int n;
        p = _ZN5Actor13ClosestPlayerEv(c);
        if (p == 0) {
            return;
        }
        Vec3_Sub(&diff, c + 0x5c, p + 0x5c);
        copy.x = diff.x;
        copy.y = diff.y;
        copy.z = diff.z;
        if (Vec3_HorzLen(&copy) > 0x7d0000) {
            return;
        }
        n = *(int*)(c + 8) & 0xf;
        if (n > 1) {
            int i = 0;
            int lim = n - 1;
            if (lim > 0) {
                do {
                    char* a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x157, 0x10, c + 0x5c, 0, *(signed char*)(c + 0xcc), -1);
                    if (a != 0) {
                        func_ov009_02111224(a, *(int*)(c + 4));
                    }
                    i++;
                } while (i < lim);
            }
        }
        asm {
            mov r0,#0x14000
            rsb r1,r0,#0
            str r1,[c,#0x160]
            ldr r0,=0xff06a000
            add r1,c,#0x74
            str r0,[c,#0x168]
            mov r0,#0x6a
            bl func_0201267c
        }
    }
    *(short*)(c + 0x92) = 5000 - (unsigned int)RandomIntInternal(&data_0209e650) % 4000;
    *(unsigned short*)(c + 0x94) = (unsigned int)RandomIntInternal(&data_0209e650) >> 16;
    *(int*)(c + 0x174) = 0x28000;
    *(int*)(c + 0x17c) = 3;
    *(int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~0x10000;
}
