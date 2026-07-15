// NONMATCHING: register allocation (div=14). Size-exact; pure regperm on five
// immediates (ROM r2/lr/ip/r3 for 0x600/0x2000/0xa000/0x1000). Logic verified.
// Counts as decompiled, not matched. mwccarm 1.2/sp2p3.
void SetAnim(void *self, void *f, int a, int b, unsigned int j);
int RandomIntInternal(int *seed);
void func_ov006_020c3754(int *elem, int *p0, int *p3);
extern int data_0209e650;
typedef struct { int a, b; } W2;
extern W2 data_ov006_0213aec8;

void func_ov006_020c3990(char *c)
{
    int i;
    char *elem;
    struct { int a[6]; int b[6]; } s;

    SetAnim(c + 0xd18, *(void **)(c + 0xd98), 0x40000000, 0x800, 0);
    i = 0;
    elem = c;
    *(int *)(c + 0xd70) = 0;
    for (; i < 0x16; i++) {
        if (*(int *)(elem + 0x48) == 0) {
            s.a[3] = 0x600;
            s.a[0] = 0x2000;
            s.a[1] = 0xa000;
            s.a[2] = 0x1000;
            s.a[4] = 0xa00;
            s.a[5] = 0xa00;
            s.a[3] += ((int)((unsigned)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13)) >> 2;
            s.a[4] += ((int)((unsigned)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13)) >> 2;
            {
                unsigned rv = (unsigned)RandomIntInternal(&data_0209e650) & 0x7fffffff;
                s.b[0] = s.a[0];
                s.b[1] = s.a[1];
                s.b[2] = s.a[2];
                s.b[3] = s.a[3];
                s.b[4] = s.a[4];
                s.a[5] += ((int)(rv >> 0x13)) >> 2;
                s.b[5] = s.a[5];
                func_ov006_020c3754((int *)(c + 8 + i * 0x98), &s.b[0], &s.b[3]);
            }
            break;
        }
        elem += 0x98;
    }
    *(W2 *)c = data_ov006_0213aec8;
}
