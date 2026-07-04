extern void func_ov006_020c42bc(void);
extern void func_ov006_020c712c(void);
extern void func_ov006_020ee4e0(char *p);
extern void func_ov006_020ee3bc(char *c);
extern int data_ov006_02140428;
extern int data_ov006_02140304;
void func_ov006_020ee508(char *c){
    int *acc = (int*)(((int)c + 0x500c) & 0xFFFFFFFFFFFFFFFF);
    *acc = *acc + (((int*)(c+0x5000))[4] >> 12);
    if(((int*)(c+0x5000))[3] > 0x1000){
        *acc -= 0x1000;
    }
    func_ov006_020c42bc();
    func_ov006_020c712c();
    if(data_ov006_02140428 == 0){
        func_ov006_020ee4e0(c);
        return;
    }
    if(data_ov006_02140304 != 0)
        return;
    func_ov006_020ee3bc(c);
}
