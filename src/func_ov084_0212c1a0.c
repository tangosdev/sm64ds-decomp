typedef short s16;

struct Vec3 { int x, y, z; };

extern s16 Vec3_HorzAngle(struct Vec3* v0, struct Vec3* v1);
extern int _ZN6Player12GetTalkStateEv(void* p);
extern int _Z14ApproachLinearRsss(s16* p, s16 target, s16 step);
extern void func_ov084_0212c9f0(char* c, int arg1, unsigned int arg2);
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern void OpenCannonInCurLevel(void);
extern void func_ov084_0212c960(char* c, int a);
extern int func_ov084_0212cae0(char* c);
extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_ov002_020bc990(void* actor);
extern int func_ov084_0212ccb4(char* c);

extern char* data_0209f318;
extern signed char data_0209f2f8;

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))

void func_ov084_0212c1a0(char* c)
{
    char* r5 = *(char**)(c + 0x194);
    char* r4 = data_0209f318;
    struct Vec3 v;
    s16 ang;
    int* src = (int*)AT(r5, 0x5c);

    v.x = src[0];
    v.y = src[1];
    v.z = src[2];
    ang = Vec3_HorzAngle((struct Vec3*)(c + 0x5c), &v);

    switch (*(unsigned char*)(c + 0x1e8)) {
    case 0:
        switch (_ZN6Player12GetTalkStateEv(r5)) {
        case 0: {
            int a1;
            if (_Z14ApproachLinearRsss((s16*)(c + 0x8e), ang, 0x800) == 0)
                return;
            a1 = data_0209f2f8 == 6 ? 0x8f : 0x14a;
            if (*(int*)(c + 0x198) != 0)
                func_ov084_0212c9f0(c, a1, 1);
            else
                func_ov084_0212c9f0(c, a1, 0);
            return;
        }
        case 2: {
            int* s1 = (int*)AT(r4, 0x80);
            int* s2 = (int*)AT(r4, 0x8c);
            *(int*)(c + 0x1cc) = s1[0];
            *(int*)(c + 0x1d0) = s1[1];
            *(int*)(c + 0x1d4) = s1[2];
            *(int*)(c + 0x1d8) = s2[0];
            *(int*)(c + 0x1dc) = s2[1];
            *(int*)(c + 0x1e0) = s2[2];
            _ZN6Camera9SetFlag_3Ev(r4);
            *(int*)AT(c, 0xb0) &= ~1;
            *(unsigned char*)AT(c, 0x1e8) += 1;
            return;
        }
        case -1:
            OpenCannonInCurLevel();
            *(unsigned char*)(c + 0x1ea) = 1;
            func_ov084_0212c960(c, 0);
            return;
        }
        return;
    case 1: {
        char* actor;
        if (*(int*)(c + 0x198) == 0)
            return;
        if (func_ov084_0212cae0(c) == 0)
            return;
        actor = _ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0x198));
        if (actor == 0)
            return;
        func_ov002_020bc990(actor);
        *(unsigned char*)AT(c, 0x1e8) += 1;
        return;
    }
    case 2: {
        unsigned int id = *(unsigned int*)(c + 0x198);
        if (id == 0)
            return;
        if (*(unsigned char*)(_ZN5Actor10FindWithIDEj(id) + 0x32e) == 1)
            *(unsigned char*)AT(c, 0x1e8) += 1;
        return;
    }
    case 3:
        if (func_ov084_0212ccb4(c) != 0)
            *(unsigned char*)AT(c, 0x1e8) += 1;
        return;
    case 4: {
        int s = _ZN6Player12GetTalkStateEv(r5);
        if (s == 1)
            return;
        if (s == 2) {
            int a1 = data_0209f2f8 == 6 ? 0x90 : 0x14b;
            func_ov084_0212c9f0(c, a1, 0);
            return;
        }
        *(int*)AT(c, 0xb0) |= 1;
        OpenCannonInCurLevel();
        *(unsigned char*)(c + 0x1ea) = 1;
        *(int*)AT(r4, 0x154) &= ~8;
        func_ov084_0212c960(c, 0);
        return;
    }
    }
}
