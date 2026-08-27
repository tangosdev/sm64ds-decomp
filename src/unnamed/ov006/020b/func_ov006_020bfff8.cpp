//cpp
extern "C" {
void MulVec3Mat4x3(void* v, void* m, void* out);
int _ZN4cstd4fdivEii(int a, int b);
extern short data_02082214[];

void func_ov006_020bfff8(char* r4, void* r1, int* r6, int* r5) {
    int out[3];
    MulVec3Mat4x3(r1, r4, out);
    int ang = *(short*)(r4 + 0xb8) >> 4;
    int i = ang * 2;
    int cosv = data_02082214[i];
    int sinv = data_02082214[i + 1];
    int f1 = _ZN4cstd4fdivEii(cosv, sinv);
    int r = (int)(((long long)f1 * (long long)(-out[2]) + 0x800) >> 0xc);
    int f2 = _ZN4cstd4fdivEii(out[1], r);
    int t1 = (int)(((long long)f2 * (long long)0x5f800u + 0x800) >> 0xc);
    r5[0] = -((t1 + (int)0xfffa0800) >> 0xc);
    int g = (int)(((long long)r * (long long)0x1555u + 0x800) >> 0xc);
    int f3 = _ZN4cstd4fdivEii(out[0], g);
    int t3 = (int)(((long long)f3 * (long long)0x7f800u + 0x800) >> 0xc);
    r6[0] = (t3 + 0x7f800) >> 0xc;
}
}
