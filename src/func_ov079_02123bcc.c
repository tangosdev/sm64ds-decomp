typedef short s16; typedef unsigned short u16; typedef long long s64;
struct Vector3 { int x, y, z; };
extern int Vec3_HorzDist(const struct Vector3 *a, const struct Vector3 *b);
extern s16 data_02082214[];

int func_ov079_02123bcc(char *self) {
    int v[6];
    int dist, rangle, idx, hc, hs, idx2, nx, nz;
    char *sub; char *p; s16 cosv, sinv, sin2, cos2;
    int a0, a1, a2, raw, ang;
    sub = *(char**)(((int)self + (*(int*)(self+0x3ec))*4 + 0x39c) & 0xFFFFFFFFFFFFFFFFLL);
    p = (char*)(((int)sub + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
    v[0] = *(int*)p; v[1] = *(int*)(p+4); v[2] = *(int*)(p+8);
    if (v[1] > *(int*)(self+0x60)) return 0;
    dist = Vec3_HorzDist((struct Vector3*)v, (struct Vector3*)(self+0x5c));
    a0 = *(s16*)(self+0x8e); a1 = *(s16*)(self+0x3e2); a2 = *(s16*)(self+0x3b8);
    raw = (a0 + a1) << 16; ang = a2 + (raw >> 16); rangle = raw >> 16;
    idx = (u16)(s16)ang >> 4; cosv = data_02082214[idx*2+1];
    hc = (int)(((s64)dist * cosv + 0x800) >> 12);
    if (hc < 0) hc = -hc;
    if (hc <= 0x3c000) {
        sinv = data_02082214[idx*2];
        hs = (int)(((s64)dist * sinv + 0x800) >> 12);
        if (hs < 0) hs = -hs;
        if (hs <= 0xe1000) {
            int vx, vy, vz;
            int i;
            idx2 = (u16)rangle >> 4;
            sin2 = data_02082214[idx2*2]; cos2 = data_02082214[idx2*2+1];
            vx = v[0]; vy = v[1]; vz = v[2];
            nx = (s16)sin2 * 5 + vx;
            nz = (s16)cos2 * 5 + vz;
            i = *(int*)(self+0x3ec);
            sub = *(char**)(((int)self + i*4 + 0x39c) & 0xFFFFFFFFFFFFFFFFLL);
            *(int*)(sub+0x5c) = nx;
            *(int*)(sub+0x60) = vy;
            *(int*)(sub+0x64) = nz;
            return 1;
        }
    }
    return 0;
}
