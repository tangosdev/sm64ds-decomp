//cpp
/* Scene::BeforeBehavior() at 0x0202e3d4, size 0x1fc
 * Matched byte-for-byte with mwccarm 1.2/sp2p3
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
typedef unsigned short u16;
typedef unsigned char u8;

struct A {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3(int, int);
    virtual void v4(int, int);
    virtual int v5();
    virtual int v6();
};

struct BTable {
    void (*w0)(void);
    void (*w1)(void);
    void (*w2)(void);
    void (*w3)(void);
    void (*w4)(void*, int, int);
};

struct FaderBrightness {
    int vtable_dummy;
    int field4;
};

extern "C" {
extern int _ZN9ActorBase14BeforeBehaviorEv(void* self);
extern void func_02023544(void);
extern void _ZN15FaderBrightness14SetForwardTimeEj(FaderBrightness* self, unsigned int t);
extern int _ZN15FaderBrightness7IsAtEndEv(FaderBrightness* self);
extern void _ZN5Scene14StartSceneFadeEjjt(unsigned int a, unsigned int b, u16 c);
extern void _ZN9ActorBase18MarkForDestructionEv(void* self);
extern int func_020431c4(void* thiz);

extern u8 data_0209f1e0;
extern void* data_0209f5c0;
extern void* data_0209f1e4;
extern FaderBrightness data_0209f5d0;
extern BTable* data_0209f5e8;
extern u16 data_02092664;
extern A* data_0209f5bc;

int _ZN5Scene14BeforeBehaviorEv(char* self)
{
    if (!_ZN9ActorBase14BeforeBehaviorEv(self)) return 0;

    if (data_0209f1e0 != 0) {
        int t = (*(u16*)((char*)data_0209f5c0 + 0xc) == 0);
        if (t != 0) {
            func_02023544();
        } else {
            if (data_0209f1e4 == 0) {
                data_0209f5d0.field4 = 0;
                _ZN15FaderBrightness14SetForwardTimeEj(&data_0209f5d0, 0x10);
                data_0209f1e4 = &data_0209f5d0;
            } else if (_ZN15FaderBrightness7IsAtEndEv(&data_0209f5d0)) {
                _ZN5Scene14StartSceneFadeEjjt(1, 0, 0);
                data_0209f5e8->w4(&data_0209f5e8, 0, 0);
                _ZN9ActorBase18MarkForDestructionEv(self);
            }
            return 0;
        }
    }

    if (data_02092664 != 0x187) {
        if (data_0209f5bc->v5() != 0) {
            data_0209f5bc->v4(0x1e, 0);
        } else if (data_0209f5bc->v6() != 0) {
            _ZN9ActorBase18MarkForDestructionEv(self);
        }
        return 1;
    }

    if ((*(u8*)(self + 0x13) & 1) != 0) {
        if (func_020431c4(self) == 0) {
            u8* p13 = (u8*)(((long long)(int)(self + 0x13)) & 0xFFFFFFFFFFFFFFFFLL);
            *p13 &= ~1;
            *p13 &= ~4;
        }
        return 0;
    } else {
        if (data_0209f5bc->v6() != 0) {
            data_0209f5bc->v3(0x1e, 0);
        }
        return 1;
    }
}
}
