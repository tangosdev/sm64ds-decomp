//cpp
typedef short s16;
typedef int Fix12;
struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };
struct CylinderClsn;
struct WithMeshClsn;

extern "C" {
int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void* self, void* wm, void* anim, unsigned int n);
unsigned short DecIfAbove0_Short(unsigned short* p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, CylinderClsn* cc);
int _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(void* self, WithMeshClsn* wm, Fix12 a, s16 b, int c, int d, void* e);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void* self, WithMeshClsn* wm, unsigned int j);
void func_ov062_02116010(void* self);
void _ZN12CylinderClsn5ClearEv(CylinderClsn* self);
void _ZN12CylinderClsn6UpdateEv(CylinderClsn* self);
void _ZN9Animation7AdvanceEv(void* self);
void func_ov062_02116d28(char* c);
void func_ov062_02116e80(void* c);
void func_ov062_02116dbc(char* c);
extern signed char LEVEL_ID;
extern char data_ov062_0211dea0[];
extern char data_ov062_0211dec0[];
extern char data_ov062_0211de70[];
extern char data_ov062_0211ded0[];
extern char data_ov062_0211dee0[];
extern char data_ov062_0211de90[];
extern char data_ov062_0211df00[];
}

extern "C" int func_ov062_0211707c(char* c)
{
    if (_ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(c, c + 0x144, c + 0x300, 3))
        return 1;

    DecIfAbove0_Short((unsigned short*)(c + 0x100));
    DecIfAbove0_Short((unsigned short*)(c + 0x3e6));
    DecIfAbove0_Short((unsigned short*)(c + 0x3e8));

    {
        M* m = *(M**)(c + 0x364);
        if (m->pmf != 0)
            (((Klass*)c)->*(m->pmf))();
    }

    *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
    *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
    *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, (CylinderClsn*)(c + 0x110));

    if (*(void**)(c + 0x364) != (void*)data_ov062_0211dea0) {
        if (*(void**)(c + 0x364) != (void*)data_ov062_0211dec0
            && *(void**)(c + 0x364) != (void*)data_ov062_0211de70) {
            int r2 = 0;
            signed char t = LEVEL_ID;
            if (t == 0x16) {
                if (*(int*)(c + 0x5c) > (int)0xff95c000)
                    r2 = 1;
            } else if (t == 0x15) {
                if (*(int*)(c + 0x64) < (int)0xff2f4000)
                    r2 = 1;
            }
            if (r2 != 0
                || (*(int*)(c + 0x98) != 0
                    && _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(
                           c, (WithMeshClsn*)(c + 0x144), 0x3c000, (s16)0x2888, 0, 1, (void*)0x32000))) {
                *(int*)(c + 0x98) = 0;
                *(char*)(c + 0x3e4) = 1;
                *(int*)(c + 0x5c) = *(int*)(c + 0x3d8);
                *(int*)(c + 0x60) = *(int*)(c + 0x3dc);
                *(int*)(c + 0x64) = *(int*)(c + 0x3e0);
            } else {
                if (*(unsigned char*)(c + 0x3e4) == 1)
                    *(char*)(c + 0x3e4) = 0;
            }
        }
        *(int*)(c + 0x3d8) = *(int*)(c + 0x5c);
        *(int*)(c + 0x3dc) = *(int*)(c + 0x60);
        *(int*)(c + 0x3e0) = *(int*)(c + 0x64);
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, (WithMeshClsn*)(c + 0x144), 3);
    }

    if (*(void**)(c + 0x364) == (void*)data_ov062_0211ded0
        || *(void**)(c + 0x364) == (void*)data_ov062_0211dee0
        || *(void**)(c + 0x364) == (void*)data_ov062_0211de90
        || *(void**)(c + 0x364) == (void*)data_ov062_0211df00) {
        func_ov062_02116010(c);
    }

    _ZN12CylinderClsn5ClearEv((CylinderClsn*)(c + 0x110));
    _ZN9Animation7AdvanceEv(c + 0x350);

    {
        char* o = (char*)(((int)c + 0x300) & 0xFFFFFFFFFFFFFFFF);
        (*(void(**)(void*))(*(int*)o + 0xc))(o);
    }

    {
        char* p3f8 = *(char**)(c + 0x3f8);
        if (p3f8 != 0) {
            int flag = (*(int*)(c + 0xb0) & 0x4000) != 0;
            if (flag) {
                if (*(int*)(p3f8 + 0xc8) != 0) {
                    func_ov062_02116d28(c);
                    goto ret;
                }
            }
        }
    }

    _ZN12CylinderClsn6UpdateEv((CylinderClsn*)(c + 0x110));
    *(int*)(c + 0x42c) = 0;
    *(int*)(c + 0x430) = 0;
    *(int*)(c + 0x434) = 0;
    func_ov062_02116e80(c);
    func_ov062_02116dbc(c);

ret:
    return 1;
}
