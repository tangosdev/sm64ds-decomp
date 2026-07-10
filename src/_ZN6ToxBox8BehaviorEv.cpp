//cpp
struct Vector3 { int x, y, z; };
struct Mtx43 { int m[12]; };
struct ToxBox;

typedef void (ToxBox::*PMF)();
struct TableEnt { PMF pmf; };
extern TableEnt data_ov092_02132568[];

struct ToxBox {
    char pad[0x568];
    int state;
};

#define LA(p) (((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL)

extern "C" {
void func_ov092_02131aec(void* c);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* self, int a, int b);
void func_ov092_02131a88(char* self);
void _ZN12CylinderClsn5ClearEv(void* m);
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* m, Vector3* v);
void _ZN12CylinderClsn6UpdateEv(void* m);
}

extern "C" int _ZN6ToxBox8BehaviorEv(ToxBox* self) {
    char* c = (char*)self;
    Vector3 v;
    *(unsigned char*)(c+0x576) = 1;
    *(unsigned char*)(c+0x577) = 0;
    {
        int idx = self->state;
        (self->*data_ov092_02132568[idx].pmf)();
        {
            unsigned short* cnt = (unsigned short*)LA(c+0x564);
            (*cnt)++;
        }
        if (idx != self->state) {
            *(unsigned short*)(c+0x564) = 0;
        }
    }
    func_ov092_02131aec(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0) {
        func_ov092_02131a88(c);
    }
    if (*(unsigned char*)(c+0x577) == 1) {
        if (*(short*)(c+0x8c) != 0 || *(short*)(c+0x8e) != 0 || *(short*)(c+0x90) != 0) {
            *(Mtx43*)(c+0x528) = *(Mtx43*)(c+0xf0);
            *(short*)(c+0x8c) = 0;
            *(short*)(c+0x8e) = 0;
            *(short*)(c+0x90) = 0;
        }
    }
    _ZN12CylinderClsn5ClearEv(c+0x4e8);
    v.x = 0;
    v.y = -0xfa000;
    v.z = 0;
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c+0x4e8, &v);
    _ZN12CylinderClsn6UpdateEv(c+0x4e8);
    return 1;
}
