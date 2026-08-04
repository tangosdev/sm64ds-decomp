//cpp
struct Row { char pad[0x24]; };
struct C { struct Row rows[1]; };
extern "C" void func_ov006_0211e020(C* c, int i);
extern "C" void func_ov006_0211e0c8(C* c) {
    int i;
    struct Row* r = c->rows;
    for (i = 0; i < 0x10; i++) {
        if (((unsigned char*)r + 0x4000)[0x677] != 0 && ((unsigned char*)r + 0x4000)[0x678] == 5)
            func_ov006_0211e020(c, i);
        r++;
    }
}
