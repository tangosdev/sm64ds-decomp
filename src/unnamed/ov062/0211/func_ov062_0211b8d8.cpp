//cpp
struct Animation { int GetFrameCount() const; };
extern void* data_ov062_0211e17c;
extern "C" int func_ov062_0211c658(unsigned char* c, void* p);
extern "C" int func_ov062_0211b8d8(unsigned char* c) {
    int f = *(int*)(c + 0x38c);
    int fc = ((Animation*)(c + 0x384))->GetFrameCount();
    if ((int)((unsigned int)(f << 4) >> 16) >= fc - 1) {
        *(short*)(c + 0x444) = 0x1e;
        func_ov062_0211c658(c, &data_ov062_0211e17c);
    }
    return 1;
}
