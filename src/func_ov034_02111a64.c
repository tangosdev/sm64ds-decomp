extern void _Z14ApproachLinearRiii(int* p, int a, int b);
extern int _ZN5Actor18HorzAngleToCPlayerEv(void* c);
extern int _ZN6Player9StartTalkER9ActorBaseb(void* self, void* actor, int b);
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern short Vec3_HorzAngle(const void* a, const void* b);
extern void _Z14ApproachLinearRsss(short* p, short a, short b);
extern int func_ov002_020c51d0(void* c, void* st);
extern int _ZN9Animation8FinishedEv(void* a);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* self, void* actor, unsigned int msg, const void* pos, unsigned int a, unsigned int b);
extern void func_0201267c(int a, void* p);
extern void func_ov034_021125b8(void* c, int i);

extern void* data_0209f318[];
extern short data_ov034_02113820[];

struct Vec3 { int x, y, z; };

void func_ov034_02111a64(char* c)
{
    struct Vec3 st;
    char* talk;

    _Z14ApproachLinearRiii((int*)(c + 0x98), 0, 0x1000);
    *(short*)(c + 0x8d8) = (short)_ZN5Actor18HorzAngleToCPlayerEv(c);

    talk = *(char**)(c + 0x8c8);
    st.x = *(int*)(c + 0x3cc);
    st.y = *(int*)(c + 0x3d0);
    st.z = *(int*)(c + 0x3d4);
    st.y = st.y + 0x8c000;
    if (!_ZN6Player9StartTalkER9ActorBaseb(talk, c, 1))
        return;

    _ZN6Camera9SetFlag_3Ev(data_0209f318[0]);
    *(unsigned char*)(c + 0x8e1) = 1;

    _Z14ApproachLinearRsss((short*)(*(char**)(c + 0x8c8) + 0x8e),
        Vec3_HorzAngle(c + 0x3cc, *(char**)(c + 0x8c8) + 0x5c), 0x100);

    func_ov002_020c51d0(talk, &st);

    if (!_ZN9Animation8FinishedEv(c + 0x160))
        return;

    {
        unsigned int idx = (4 - *(unsigned char*)(c + 0x8db)) & 0xff;
        if (idx >= 4)
            idx = 0;
        if (!_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(talk, c, data_ov034_02113820[idx], &st, 1, 0))
            return;
    }

    func_0201267c(0x11b, c + 0x74);
    func_ov034_021125b8(c, 6);
}
