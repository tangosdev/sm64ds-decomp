extern void func_ov006_020f0eac(void* c);
extern void func_ov006_020f1e90(void* c);
extern void func_ov006_020f1fcc(void* c);
extern void func_ov006_020f10ec(void* c);

void func_ov006_020f319c(char* c){
    func_ov006_020f0eac(c);
    if (*(int*)(c + 0x4f78) == 3) return;
    func_ov006_020f1e90(c);
    func_ov006_020f1fcc(c);
    func_ov006_020f10ec(c);
}
