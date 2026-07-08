struct V3 { int x, y, z; };
struct State;
extern void func_ov002_020bda48(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* thiz, struct State* s);
extern void func_ov002_020bd9ec(char* c, unsigned int a);
extern void func_ov002_020c43c4(char* c, int a);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, void* pos);

extern struct State _ZN6Player10ST_BALLOONE;

void _ZN6Player16InitBalloonMarioEv(char* thiz)
{
    struct V3 v;

    if (*(unsigned char *)(thiz + 0x6fd) != 0) return;

    func_ov002_020bda48(thiz);
    *(unsigned char *)(thiz + 0x6fd) = 1;
    *(short *)(thiz + 0x6c0) = 0x258;
    _ZN6Player11ChangeStateERNS_5StateE(thiz, &_ZN6Player10ST_BALLOONE);
    func_ov002_020bd9ec(thiz, 0x30);
    func_ov002_020c43c4(thiz, 2);

    v.x = *(int *)(thiz + 0x5c);
    v.y = *(int *)(thiz + 0x60);
    v.z = *(int *)(thiz + 0x64);
    v.y = *(int *)(thiz + 0x60) + 0x50000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xc5, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xc6, v.x, v.y, *(volatile int *)&v.z);

    _ZN5Sound9PlayBank0EjRK7Vector3(0xd7, thiz + 0x74);
}
