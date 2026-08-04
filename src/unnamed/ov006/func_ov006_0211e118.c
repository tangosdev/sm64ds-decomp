extern void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);
extern void* data_ov006_02134210[];

struct Elem {
    int x;
    int y;
    int unk8;
    unsigned char idx;
    unsigned char unkd;
    unsigned char flag;
    unsigned char unkf;
};

struct Foo {
    char pad[0x4960];
    struct Elem arr[16];
};

void func_ov006_0211e118(struct Foo* a0) {
    int i;
    for (i = 0; i < 16; i++) {
        if (a0->arr[i].flag) {
            Hud_RenderSprite(data_ov006_02134210[a0->arr[i].idx], a0->arr[i].x >> 12, a0->arr[i].y >> 12, -1, -1);
        }
    }
}
