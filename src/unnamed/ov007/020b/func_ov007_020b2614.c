typedef struct G {
    char pad[0xf4];
    char* p_f4;
    char pad2[0x100 - 0xf4 - 4];
    char* p_100;
    char* p_104;
} G;

extern G* data_ov007_0210342c;

extern void func_ov007_020bc374(char* self, int b, int c, int d);
extern void func_ov007_020bc3dc(char* self, int b, int c, int d);
extern void func_ov007_020bc2e4(void* c, int i, int a2);
extern void func_ov007_020bc32c(void* c, int i, int a2);
extern void func_ov007_020be980(void* p, int i, int a2);
extern void func_ov007_020be964(void* p);

void func_ov007_020b2614(int self, int b, int c) {
    if (c > 0) {
        func_ov007_020bc374((char*)data_ov007_0210342c->p_f4, self, b, c);
    } else {
        func_ov007_020bc3dc((char*)data_ov007_0210342c->p_f4, self, b, c);
        *(int*)((char*)data_ov007_0210342c->p_f4 + 0x94) = 1;
        *(int*)((char*)data_ov007_0210342c->p_f4 + 0x98) = 1;
    }
    func_ov007_020bc2e4(data_ov007_0210342c->p_f4, self, b);
    func_ov007_020bc32c(data_ov007_0210342c->p_f4, self, b);
    if (self == 0) {
        func_ov007_020be980(data_ov007_0210342c->p_100, 0, b);
        func_ov007_020be980(data_ov007_0210342c->p_104, 0, b);
    } else {
        func_ov007_020be964(data_ov007_0210342c->p_100);
        func_ov007_020be964(data_ov007_0210342c->p_104);
    }
}
