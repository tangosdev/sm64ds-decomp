extern void *LoadFile(int handle);
extern void DecompressLZ16(const void *src, void *dst);
extern void Deallocate(void *ptr);

struct E {
    int a;
    int b;
    char pad0[5];
    unsigned char c;
    unsigned char d;
    char pad1[0x1c - 0xf];
};

struct Obj {
    char pad[0x4bd4];
    struct E member[1];
};

void func_ov006_0211ce94(struct Obj *base, int index) {
    void *data;
    base->member[index].a = 0;
    base->member[index].b = 0;
    base->member[index].d = 0;
    data = LoadFile(0x103);
    DecompressLZ16(data, (void *)0x6400000);
    base->member[index].c = 7;
    Deallocate(data);
}
