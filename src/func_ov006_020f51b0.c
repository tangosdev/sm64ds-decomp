extern void func_ov006_020f51f0(void* c);
extern void func_ov006_020f4cd8(void* c);
extern void FreeGfxSlotsById(int x);
extern void func_ov006_020c1764(void* p);

void func_ov006_020f51b0(char* c){
    func_ov006_020f51f0(c);
    func_ov006_020f4cd8(c);
    FreeGfxSlotsById(0x1d);
    func_ov006_020c1764(c + 0x4f38);
    *(int*)(c + 0x5314) = 1;
}
