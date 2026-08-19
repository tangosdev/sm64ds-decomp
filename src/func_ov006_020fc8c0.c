// NONMATCHING: pure register allocation (div=51). Same length (0xf0) and same
// instruction sequence as the ROM; mwcc colours the six loop invariants
// (p, i, sin table, matrix ptr, sprite table, -1) into r4-r8/fp in a different
// order. Logic verified instruction-by-instruction vs the ROM disassembly.
// dScMgPachinko_c per-frame bob-omb sprite render (30 slots, rotated OAM).
extern const short data_02082214[];
extern void* data_ov006_02136cd4[];
void func_ov004_020b023c(void* a0, int a1, int a2, int a3, void* a4);

void func_ov006_020fc8c0(char* p)
{
    void** t = data_ov006_02136cd4;
    int m[4];
    int i;
    for (i = 0; i < 30; i++) {
        if (*(unsigned char*)(p + 0x468d)) {
            int s = *(int*)(p + 0x4680);
            int a = *(unsigned short*)(p + 0x4684) >> 4;
            int rx = (int)(((long long)data_02082214[a * 2 + 1] * s + 0x800) >> 12);
            int ry = (int)(((long long)data_02082214[a * 2] * s + 0x800) >> 12);
            int bx = (*(int*)(p + 0x4660) + *(int*)(p + 0x4670)) >> 12;
            int by = (*(int*)(p + 0x4664) + *(int*)(p + 0x4674)) >> 12;
            m[0] = rx;
            m[1] = ry;
            m[2] = -ry;
            m[3] = rx;
            func_ov004_020b023c(t[*(unsigned char*)(p + 0x4693)], bx, by, -1, m);
        }
        p += 0x38;
    }
}
