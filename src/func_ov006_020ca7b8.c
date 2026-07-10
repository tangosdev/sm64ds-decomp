// Scans data_ov006_021405bc entries (stride 0xdc) from data_ov006_02140554:
// returns 0 if any entry's pair at +0x70 mismatches data_ov006_0213b10c
// (y-mismatch tolerated when x is 0), else 1. Leaf, no callees.
typedef struct Ent {
    char _pad0[0x70];  // 0x00
    int x;             // 0x70
    int y;             // 0x74
    char _pad78[0x64]; // 0x78 (stride 0xdc)
} Ent;

extern int data_ov006_021405bc;
extern Ent* data_ov006_02140554;
extern int data_ov006_0213b10c[2];

int func_ov006_020ca7b8(void) {
    int i;
    for (i = 0; i < data_ov006_021405bc; i++) {
        int* v = (int*)(((long long)(int)&data_ov006_02140554[i].x) & 0xFFFFFFFFFFFFFFFFLL);
        volatile int* q = (volatile int*)data_ov006_0213b10c;
        if (v[0] != data_ov006_0213b10c[0]
            || (v[1] != q[1] && data_ov006_02140554[i].x != 0))
            return 0;
    }
    return 1;
}
