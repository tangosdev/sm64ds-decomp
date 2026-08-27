//cpp
extern "C" int _ZNK9Animation12WillHitFrameEi(void*, int);
extern "C" int func_0201267c(int, void*);
extern "C" void func_ov064_02116374(char *c) {
    if (_ZNK9Animation12WillHitFrameEi(c+0x160, 4) == 0) {
        if (_ZNK9Animation12WillHitFrameEi(c+0x160, 7) == 0)
            return;
    }
    func_0201267c(0xcc, c+0x74);
}
