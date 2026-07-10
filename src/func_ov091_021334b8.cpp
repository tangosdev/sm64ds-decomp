//cpp
struct Vector3 { int x, y, z; };
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(void *c);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(void *c);
extern "C" int _ZN5Actor18GetBitInDeathTableEv(void *c);
extern "C" void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void *c, Vector3 const &v, unsigned int n, int f, short s);
extern "C" void _ZN5Actor17TrackInDeathTableEv(void *c);
extern "C" void func_ov091_02133498(char *c);

extern "C" void func_ov091_021334b8(char *c, int flag)
{
    if (flag != 0) {
        unsigned char n = *(unsigned char *)(c + 0x31e);
        int *p = (int *)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFFLL);
        int m = n * 0x3c;
        int y = *p;
        *p = y - (m << 12);
        *(unsigned char *)(c + 0x31e) = 0;
    } else {
        int *p = (int *)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFFLL);
        int y = *p;
        *p = y - 0x3c000;
        int off = 0x31e;
        unsigned char *q = (unsigned char *)(((int)c + off) & 0xFFFFFFFFFFFFFFFFLL);
        *q = *q - 1;
    }
    *(unsigned char *)(c + 0x31f) = 0xf;
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    if (*(unsigned char *)(c + 0x31e) != 0) return;

    struct {
        int vx, vy, vz;
        int v2x, v2y, v2z;
    } st;
    st.vx = *(int *)(c + 0x5c);
    st.vy = *(int *)(c + 0x60);
    st.vz = *(int *)(c + 0x64);
    st.vy = st.vy + 0x17c000;
    if (*(unsigned char *)(c + 0x320) == 0) {
        if (_ZN5Actor18GetBitInDeathTableEv(c) == 0) {
            st.v2x = st.vx;
            st.v2y = st.vy;
            st.v2z = st.vz;
            _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, *(struct Vector3 *)&st.v2x, 5, 0x5000, 0);
        }
    }
    _ZN5Actor17TrackInDeathTableEv(c);
    func_ov091_02133498(c);
}
