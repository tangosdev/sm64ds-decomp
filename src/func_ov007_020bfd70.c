// NONMATCHING: near-miss 14/55 (fdiff, mwccarm 1.2/sp2p3). Logic verified vs ROM.
// Remaining diff is a pure +1 scratch-register rotation on the block between the
// two calls: ROM colors raw=r1/i=r2/z=r3/cosv=r1/soff=lr and never touches r0
// until `mov r0,#0x800`; the candidate starts at r0. Levers tried: ang-variable
// reuse (i/i+1 same-reg RMW: works), named tab pointer (kills pool remat: works),
// dead-def of first call's return + #pragma opt_dead_assignments off (fixed
// tab->ip and the sinv ldrsh sink position: works), sinv-as-call-arg (sinks load:
// works), param-reuse of v, decl-order permutations, C++ frontend (all inert on
// the final rotation). Counts as decompiled, not matched.
#pragma opt_dead_assignments off
extern int MulVec3Mat4x3(void* v, void* m, void* out);
extern int _ZN4cstd4fdivEii(int a, int b);
extern short data_02082214[];

void func_ov007_020bfd70(char* obj, void* v, int* px, int* py) {
    int out[3];
    int cosv;
    int ang;
    int ang2;
    int z;
    short* tab;
    int r, f, a, b;
    r = MulVec3Mat4x3(v, obj + 0x44, out);
    ang = (*(unsigned short*)(obj + 0xd4) >> 4) * 2;
    z = out[2];
    tab = data_02082214;
    cosv = tab[ang];
    ang2 = ang + 1;
    if (z < 0) z = -z;
    r = (int)(((long long)cosv * z + 0x800) >> 12);
    f = _ZN4cstd4fdivEii(tab[ang2], r);
    a = (int)(((long long)f * out[1] + 0x800) >> 12);
    b = (int)(((long long)f * out[0] + 0x800) >> 12);
    px[0] = ((b * 0x60) >> 12) + 0x80;
    py[0] = ((-a * 0x60) >> 12) + 0x60;
}
