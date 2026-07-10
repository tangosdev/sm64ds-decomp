// Scans data_ov006_02140418 entries (stride 0xb8) from data_ov006_02140420:
// returns 0 if any entry's pair at +0x3c mismatches data_ov006_0213b040
// (y-mismatch tolerated when x is 0), else 1. Leaf, no callees.
typedef struct Ent {
    char _pad0[0x3c];  // 0x00
    int x;             // 0x3c
    int y;             // 0x40
    char _pad44[0x74]; // 0x44 (stride 0xb8)
} Ent;

extern int data_ov006_02140418;
extern Ent* data_ov006_02140420;
extern int data_ov006_0213b040[2];

int func_ov006_020c7300(void) {
    int i;
    for (i = 0; i < data_ov006_02140418; i++) {
        int* v = (int*)(((long long)(int)&data_ov006_02140420[i].x) & 0xFFFFFFFFFFFFFFFFLL);
        volatile int* q = (volatile int*)data_ov006_0213b040;
        if (v[0] != data_ov006_0213b040[0]
            || (v[1] != q[1] && data_ov006_02140420[i].x != 0))
            return 0;
    }
    return 1;
}
