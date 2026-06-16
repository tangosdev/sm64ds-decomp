//cpp
extern "C" {
extern int _Z14ApproachLinearRsss(short*, short, short);
extern int AngleDiff(short, short);
extern int func_ov077_02126d5c(void*, void*);
extern int data_ov077_02127ce8[];
int func_ov077_021269a8(char* c) {
    short tgt = *(short*)(c + 0x420);
    _Z14ApproachLinearRsss((short*)(c + 0x94), tgt, 0x500);
    int diff = AngleDiff(*(short*)(c + 0x94), *(short*)(c + 0x420));
    if (diff < 0x100) {
        *(short*)(c + 0x426) = 0x1e;
        func_ov077_02126d5c(c, data_ov077_02127ce8);
    }
    return 1;
}
}
