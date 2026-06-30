extern void func_ov004_020af68c(void* a0, int a1, int a2, int a3, int a4);
extern int data_ov006_021342bc[];
extern int data_ov006_021343b0[];

typedef struct {
    int x;              /* 0x00 */
    int y;              /* 0x04 */
    char pad2[6];
    unsigned char flag; /* 0x0e */
    unsigned char idx;  /* 0x0f */
} Rec;

typedef struct {
    char head[0x6260];
    Rec recs[2];
} Obj;

void func_ov006_020d63d4(Obj* a0) {
    int i;
    for (i = 0; i < 2; i++) {
        if (a0->recs[i].flag != 0) {
            int xv = a0->recs[i].x >> 12;
            int yv = a0->recs[i].y >> 12;
            int k = a0->recs[i].idx;
            int v = (i == 0) ? data_ov006_021343b0[k] : data_ov006_021342bc[k];
            func_ov004_020af68c((void*)v, xv, yv, -1, 1);
        }
    }
}
