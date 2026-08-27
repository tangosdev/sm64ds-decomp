//cpp
struct Animation {
    void Advance();
};

extern "C" unsigned char DecIfAbove0_Byte(unsigned char *p);
extern "C" int func_ov070_02120da8(char *c, int a);

extern "C" int func_ov070_02120cac(char *c) {
    ((Animation *)(c + 0x124))->Advance();
    unsigned char r = DecIfAbove0_Byte((unsigned char *)(c + 0x430));
    if (r == 0) {
        func_ov070_02120da8(c, 1);
    }
    return 1;
}
