// NONMATCHING: different op / idiom (div=48). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int Fix12;
struct BCA_File;
struct Anim;
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, struct BCA_File* f, int i, Fix12 spd, unsigned int g);
extern int _ZN9Animation8FinishedEv(void* self);
extern int _ZN6Player18HasFinishedTalkingEv(void);
extern void func_ov034_021125b8(char* c, int i);

struct Ent { int pad; struct BCA_File* f; };
extern struct Ent* data_ov034_02113888[];
extern struct Ent* data_ov034_02113874[];
extern char* CAMERA;

void func_ov034_02111e68(char* c) {
    int i1, i2, i3, n;
    char* a;
    char* mo;
    char* p;
    int finished;
    unsigned st;

    a = c + 0x110;
    a = a + 0xc8;
    n = 6;
    for (i1 = 2; i1 < 5; i1++) {
        if (*(unsigned char*)(c+0x8da) != n - 3) break;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(a, data_ov034_02113888[i1]->f, 0x40000000, 0x1000, 0);
        n += 3;
        a += 0x64;
    }

    mo = c + 0x160;
    a = c + 0x110;
    for (i2 = 0; i2 < 5; i2++) {
        if (_ZN9Animation8FinishedEv(mo)) break;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(a, data_ov034_02113874[i2]->f, 0, 0x1000, 0);
        mo += 0x64;
        a += 0x64;
    }

    *(int*)(c+0x8c8);
    finished = 1;
    p = c;
    for (i3 = 0; i3 < 5; i3++) {
        if (*(int*)(p+0x170) != *(int*)((char*)data_ov034_02113874[i3] + 4)) finished = 0;
        p += 0x64;
    }
    if (finished != 1) goto done;
    if (!_ZN6Player18HasFinishedTalkingEv()) goto done;
    st = *(unsigned char*)(c+0x8db);
    if (st < 4 && st > 1) {
        char* g;
        *(int*)(c+0xa8) = 0x41000 - ((st - 2) * 0xa << 0xc);
        *(int*)(c+0x9c) = -0x4000;
        g = (char*)CAMERA + 0x154;
        *(int*)g &= ~8;
        *(unsigned char*)(c+0x8e1) = 0;
        func_ov034_021125b8(c, 4);
    }
done:
    (*(unsigned char*)(c+0x8da))++;
    if (*(unsigned char*)(c+0x8da) > 0xc) *(unsigned char*)(c+0x8da) = 0xd;
}
