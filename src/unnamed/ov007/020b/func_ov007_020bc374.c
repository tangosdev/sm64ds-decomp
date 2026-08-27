typedef short s16;
struct M28 { int w[7]; };
extern void func_ov007_020bc3dc(void* self, int b, int c);

void func_ov007_020bc374(char* self, int b, int c, int d) {
    char* src = *(char**)(self + 0x18);
    if (*(s16*)(src + 0xc) != -1) {
        *(struct M28*)*(char**)(self + 0x88) = *(struct M28*)src;
        *(s16*)(self + 0x8c) = (s16)d;
        *(s16*)(self + 0x8e) = *(s16*)(self + 0x8c);
    }
    func_ov007_020bc3dc(self, b, c);
}
