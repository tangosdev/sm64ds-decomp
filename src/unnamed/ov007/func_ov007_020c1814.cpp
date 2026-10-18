//cpp
extern "C" {
extern int data_ov007_02104bdc;
extern int data_ov007_02104be8;
extern void func_ov007_020c184c(void);
void func_ov007_020c1814(int textureOffset, int paletteOffset) {
    func_ov007_020c184c();
    data_ov007_02104bdc = textureOffset;
    data_ov007_02104be8 = paletteOffset;
}
}
