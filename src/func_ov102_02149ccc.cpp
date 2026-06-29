//cpp
// func_ov102_02149ccc at 0x02149ccc
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov102).
struct Actor;
struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void call(Actor *player);
};

extern "C" Actor *_ZN5Actor13ClosestPlayerEv(char *self);
extern "C" int Vec3_HorzDist(const void *a, const void *b);
extern "C" int data_0209caa0[];

extern "C" void func_ov102_02149ccc(char *self)
{
    Actor *player;
    if (!(data_0209caa0[1] & 0x80000000)) {
        int b = (int)(*(unsigned short *)(self + 0xc) == 0x14);
        if (b)
            return;
    }
    player = _ZN5Actor13ClosestPlayerEv(self);
    if (*(unsigned char *)((char *)player + 0x703) == 0)
        return;
    if (Vec3_HorzDist(self + 0x5c, (char *)player + 0x5c) >= 0xc8000)
        return;
    if (*(int *)(self + 0x60) <= *(int *)((char *)player + 0x60))
        return;
    ((Obj *)self)->call(player);
}
