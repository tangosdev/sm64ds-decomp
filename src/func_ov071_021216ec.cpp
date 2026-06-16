//cpp
extern "C" {
    void func_ov071_021215c0(void *c);
    void func_0200f760(void *c, void *p);
    void func_ov071_02120c90(void *c);
}

struct CylinderClsn {
    void Clear();
    void Update();
};

extern "C" int func_ov071_021216ec(char *c) {
    func_ov071_021215c0(c);
    func_0200f760(c, c + 0x174);
    *(short*)(c + 0x20c) = *(short*)(c + 0x8e);
    ((CylinderClsn*)(c + 0x174))->Clear();
    ((CylinderClsn*)(c + 0x174))->Update();
    func_ov071_02120c90(c);
    return 1;
}
