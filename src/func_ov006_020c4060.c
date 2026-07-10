// Scans data_ov006_02140328 entries (stride 0xf0) from data_ov006_02140324:
// returns 0 if any entry's pair at +0x30 mismatches data_ov006_0213afa8
// (y-mismatch tolerated when x is 0), else 1. Leaf, no callees.
typedef struct Ent {
    char _pad0[0x30];  // 0x00
    int x;             // 0x30
    int y;             // 0x34
    char _pad38[0xb8]; // 0x38 (stride 0xf0)
} Ent;

extern int data_ov006_02140328;
extern Ent* data_ov006_02140324;
extern int data_ov006_0213afa8[2];

int func_ov006_020c4060(void) {
    int i;
    for (i = 0; i < data_ov006_02140328; i++) {
        int* v = (int*)(((long long)(int)&data_ov006_02140324[i].x) & 0xFFFFFFFFFFFFFFFFLL);
        volatile int* q = (volatile int*)data_ov006_0213afa8;
        if (v[0] != data_ov006_0213afa8[0]
            || (v[1] != q[1] && data_ov006_02140324[i].x != 0))
            return 0;
    }
    return 1;
}
