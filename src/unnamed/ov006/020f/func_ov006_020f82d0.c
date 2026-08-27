struct Obj { char pad[1]; };
void func_ov006_020f82d0(struct Obj *o, int n) {
    *(short*)((char*)o + 0x28) = (short)((4 - n % 5) * 2);
    *(unsigned char*)((char*)o + 0x2d) = 5;
}
