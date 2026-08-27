extern void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);
extern int data_ov006_021344ec[];

typedef struct {
    int x;              /* 0x00 */
    int y;              /* 0x04 */
    char pad[7];
    unsigned char idx;  /* 0x0f */
} Rec;

typedef struct {
    char head[0x6280];
    Rec recs[2];
} Obj;

void func_ov006_020d6098(Obj* a0) {
    int i;
    for (i = 0; i < 2; i++) {
        int xv = a0->recs[i].x >> 12;
        int yv = a0->recs[i].y >> 12;
        int k = a0->recs[i].idx;
        if (i != 0) k += 5;
        Hud_RenderSprite((void*)data_ov006_021344ec[k], xv, yv, -1, -1);
    }
}
