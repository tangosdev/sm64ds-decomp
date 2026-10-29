/* The model record func_ov007_020bc6d4 builds; names as in
 * func_ov007_020be9ac and func_ov007_020bc7dc, only the slots this file
 * writes. */
typedef struct Ov007Model {
    char pad_00[0x94];
    int visible;  /* 0x94: func_ov007_020bbff0 renders only while it is nonzero */
    int animTick; /* 0x98: 1 on the frames that step the animations */
} Ov007Model;

typedef char
    Ov007Model_animTick_must_be_at_0x98[(int)&((Ov007Model *)0)->animTick == 0x98 ? 1 : -1];

/* data_ov007_0210342c: the overlay's manager (G keeps the extern's old
 * spelling); names as in func_ov007_020b2370, only the slots this
 * file reads. */
typedef struct G {
    char pad[0xf4];
    Ov007Model *model; /* 0xf4: func_ov007_020b2998 stores func_ov007_020b2bd4's model */
    char pad2[0x100 - 0xf4 - 4];
    char *figure;    /* 0x100: func_ov007_020beaa0 */
    char *altFigure; /* 0x104: func_ov007_020be850 returns it when +0xec is 1 */
} G;

typedef char G_altFigure_must_be_at_0x104[(int)&((G *)0)->altFigure == 0x104 ? 1 : -1];

extern G* data_ov007_0210342c;

extern void func_ov007_020bc374(char* self, int b, int c, int d);
extern void func_ov007_020bc3dc(char* self, int b, int c, int d);
extern void func_ov007_020bc2e4(void* c, int i, int a2);
extern void func_ov007_020bc32c(void* c, int i, int a2);
extern void func_ov007_020be980(void* p, int i, int a2);
extern void func_ov007_020be964(void* p);

void func_ov007_020b2614(int self, int b, int c) {
    if (c > 0) {
        func_ov007_020bc374((char *)data_ov007_0210342c->model, self, b, c);
    } else {
        func_ov007_020bc3dc((char *)data_ov007_0210342c->model, self, b, c);
        data_ov007_0210342c->model->visible = 1;
        data_ov007_0210342c->model->animTick = 1;
    }
    func_ov007_020bc2e4(data_ov007_0210342c->model, self, b);
    func_ov007_020bc32c(data_ov007_0210342c->model, self, b);
    if (self == 0) {
        func_ov007_020be980(data_ov007_0210342c->figure, 0, b);
        func_ov007_020be980(data_ov007_0210342c->altFigure, 0, b);
    } else {
        func_ov007_020be964(data_ov007_0210342c->figure);
        func_ov007_020be964(data_ov007_0210342c->altFigure);
    }
}
