extern void func_ov006_020f72c0(void* c);
extern void func_ov006_020f6c90(void* c);
extern void FreeGfxSlotsById(int x);
extern void func_ov006_020c1764(void* p);

void func_ov006_020f7280(char* c){
    func_ov006_020f72c0(c);
    func_ov006_020f6c90(c);
    FreeGfxSlotsById(0x1d);
    func_ov006_020c1764(c + 0x4f38);
    *(int*)(c + 0x53d4) = 1;
}
