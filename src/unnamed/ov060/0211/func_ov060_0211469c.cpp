//cpp
extern "C" void func_ov060_02111cc0(void *c, int a1, int a2);
struct Animation {
    virtual ~Animation() {}
    int WillHitFrame(int frame) const;
};
extern "C" int func_ov060_0211469c(void *c) {
    func_ov060_02111cc0(c, 0xb, 0x40000000);
    int r = ((Animation*)((char*)c + 0x124))->WillHitFrame(0x20);
    if (r != 0) return 1;
    return 0;
}
