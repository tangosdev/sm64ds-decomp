extern void func_0207328c(void* a, int b, int c, void* d);
extern void func_ov004_020b29c0(void* c);
extern void func_ov006_0210d894(void);
extern void func_0203d47c(void);
extern int data_ov006_0213eefc[];

void* func_ov006_0210d740(char* c){
    *(void**)c = data_ov006_0213eefc;
    func_0207328c(c + 0x599c, 0x40, 0x24, (void*)func_ov006_0210d894);
    func_0207328c(c + 0x48d4, 0x10, 8, (void*)func_0203d47c);
    func_0207328c(c + 0x4854, 0x10, 8, (void*)func_0203d47c);
    func_0207328c(c + 0x47c8, 5, 8, (void*)func_0203d47c);
    func_ov004_020b29c0(c);
    return c;
}
