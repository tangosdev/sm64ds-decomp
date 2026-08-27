//cpp
struct Base {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18();
    virtual int m4c(int arg);   // vtable offset 0x4c
    char pad[0xa4];
    int a8;                     // offset 0xa8
};

struct Pair { int v[2]; };

extern "C" {
extern Base *data_ov004_020beb68;
extern Pair data_ov004_020bc98c;
void func_ov004_020ae20c(void);
void func_ov004_020ae2c8(void);
void func_ov004_020b29a0(Base *c, int arg);
void func_ov004_020b7a18(char *c);
}

void func_ov004_020b7a18(char *c)
{
    Base *g = data_ov004_020beb68;
    int cond = g ? g->a8 : 0;
    if (cond == 0) {
        g = data_ov004_020beb68;
        if (!g) return;
        if (!g->m4c(*(int *)(c + 0x18))) return;
        func_ov004_020ae20c();
        *(int *)(c + 0x1c) = 0xb4;
        *(Pair *)(c + 8) = data_ov004_020bc98c;
        return;
    }
    g = data_ov004_020beb68;
    if (!g) return;
    if (!g->m4c(*(int *)(c + 0x18))) return;
    func_ov004_020ae20c();
    func_ov004_020ae2c8();
    func_ov004_020b29a0(g, *(int *)(c + 0x18));
    *(int *)(c + 0x1c) = 0;
    *(int *)(c + 0x18) = -1;
    *(int *)(c + 0x20) = 0;
    *(int *)(c + 0x24) = 0;
}
