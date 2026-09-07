typedef unsigned int u32;
typedef unsigned short u16;
typedef signed char s8;

typedef struct Vector3 { int x, y, z; } Vector3;
typedef struct dBgCh_Lin { int _[0x1e]; } dBgCh_Lin;
typedef struct dBgCh_Gnd { int _[0x15]; } dBgCh_Gnd;

extern signed char data_0209f2f8;
extern char data_02086ff4;
extern char data_02087274;
extern char data_0208742c;
extern char data_020873dc;
extern char data_02087094;
extern char data_0208733c;
extern char data_020871ac;
extern char data_020871d4;
extern char data_02087404;

extern void _ZN9dBgCh_LinC1Ev(dBgCh_Lin* t);
extern void _ZN9dBgCh_LinD1Ev(dBgCh_Lin* t);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(dBgCh_Lin* t, Vector3* a, Vector3* b, void* actor);
extern int _ZN9dBgCh_Lin10DetectClsnEv(dBgCh_Lin* t);
extern void _ZN9dBgCh_Lin10GetClsnPosEv(Vector3* ret, dBgCh_Lin* t);
extern void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd* t);
extern void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd* t);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd* t, Vector3* a, void* actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd* t);
extern void func_0200897c(void* self, void* arg);
extern int func_02037e48(u32* p);
extern int func_02037e68(u32* p);
extern void* GetViewObj(int idx);
extern int func_0200c394(char* self, int col1, void* p, Vector3* pos, void** out1, int arg5);

int func_0200c66c(char* self, Vector3* pos, void** out2, void** out1, int arg5)
{
    Vector3 tmp;
    Vector3 clsn;
    dBgCh_Lin line;
    dBgCh_Gnd ground;
    int hval;
    int col1;
    int col2;
    char* p;
    void* obj;
    int res;

    p = *(char**)(self + 0x110);
    obj = 0;
    {
        int b = *(u16*)(p + 0xc);
        b = b == 0xbf;
        if (b != 0) {
            res = *(int*)(p + 0x644);
            if ((u32)res == 0x80000000) res = *(int*)(p + 0x60);
            obj = p;
        }
    }

    if (obj != 0 && data_0209f2f8 != 0x1c) {
        col1 = *(int*)((char*)obj + 0x65c) & 0xff;
        col2 = *(int*)((char*)obj + 0x660) & 0xff;
        hval = res;
    } else {
        _ZN9dBgCh_LinC1Ev(&line);
        func_0200897c(self, &line);
        {
            int tx, tz, ty;
            tx = pos->x;
            tz = pos->z;
            ty = pos->y + 0x100000;
            tmp.x = tx;
            tmp.y = ty;
            tmp.z = tz;
        }
        _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(&line, pos, &tmp, 0);
        if (_ZN9dBgCh_Lin10DetectClsnEv(&line) != 0) {
            _ZN9dBgCh_Lin10GetClsnPosEv(&clsn, &line);
            tmp.x = clsn.x;
            tmp.y = clsn.y;
            tmp.z = clsn.z;
        }
        _ZN9dBgCh_GndC1Ev(&ground);
        func_0200897c(self, &ground);
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&ground, &tmp, 0);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&ground) == 0) {
            hval = pos->y;
            col1 = 0;
            col2 = 0x3f;
        } else {
            hval = ground._[0x11];
            col1 = func_02037e48((u32*)&ground._[5]) & 0xff;
            col2 = func_02037e68((u32*)&ground._[5]) & 0xff;
        }
        _ZN9dBgCh_GndD1Ev(&ground);
        _ZN9dBgCh_LinD1Ev(&line);
    }

    if (col1 == 0 || (*(u32*)(self + 0x154) & 0x400) != 0) {
        if (*(u16*)(*(char**)(self + 0x13c) + 0x26) & 0x10) {
            if (*(void**)(self + 0x140)) *out1 = *(void**)(self + 0x140);
            if (*(void**)(self + 0x144)) *out2 = *(void**)(self + 0x144);
        }
    } else {
        int b0;
        if (col2 != 0x3f) *out2 = GetViewObj(col2);
        if (func_0200c394(self, col1, *out2, pos, out1, arg5) == 0) {
            if ((u32)*(char**)(self + 0x13c) <= (u32)&data_02086ff4 || *out1 == (void*)&data_02087274) {
                if (*out2 != 0) {
                    if (*(unsigned char*)(*out2) == 6) *out1 = (void*)&data_0208742c;
                }
            }
        } else {
            if (col2 != 0x3f) {
                b0 = *(unsigned char*)(*out2);
                if (b0 == 4) {
                    *out1 = (void*)&data_020873dc;
                    if (*(unsigned char*)((char*)*out2 + 1) == 2) {
                        if (*(void**)(self + 0x140) != *out1)
                            *(u32*)(((int)self + 0x154)) |= 4;
                    }
                    res = hval;
                } else if (b0 == 6) {
                    *out1 = (void*)&data_0208742c;
                } else {
                    if ((*(u16*)(*(char**)(self + 0x13c) + 0x26) & 4) == 0) {
                        if (b0 == 0) {
                            if (*out1 == (void*)&data_02087274 && data_0209f2f8 == 0x2d) {
                                *(int*)(self + 0x120) = *(int*)(self + 0x98);
                                *(int*)(self + 0x124) = *(int*)(self + 0x9c);
                                *(int*)(self + 0x128) = *(int*)(self + 0xa0);
                            }
                            *out1 = (void*)&data_020871ac;
                        } else if (b0 == 1) {
                            *out1 = (void*)&data_020871d4;
                        } else if (b0 == 5) {
                            *out1 = (void*)&data_02087404;
                        }
                    }
                }
                *(u32*)(((int)self + 0x154)) &= ~0x100;
            }
        }
        if (*out1 != (void*)&data_02087094) *(void**)(self + 0x140) = *out1;
        if (*out1 != (void*)&data_0208733c) {
            if (*out2 != 0 && *(unsigned char*)(*out2) != 2)
                *(void**)(self + 0x144) = *out2;
        }
    }

    return res;
}
