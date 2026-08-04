extern void* func_ov006_020c7300(void*);
extern void* func_ov006_020c4060(void*);
extern void func_ov004_020adb1c(int);
extern void func_ov004_020b0a54(int);
extern void func_ov006_020c42bc(void);
extern void func_ov006_020c712c(void);
extern int data_ov006_02140308;
struct G2 { int w[2]; };
extern struct G2 data_ov006_0213cc74;
void func_ov006_020ef480(char *c){
    void *r = func_ov006_020c7300(c);
    if(r){
        r = func_ov006_020c4060(r);
        if(r){
            func_ov004_020adb1c(data_ov006_02140308);
            func_ov004_020b0a54(0x12);
            *(unsigned char*)(c+0xc3)=0;
            *(struct G2*)(c+0x5000+4) = data_ov006_0213cc74;
        }
    }
    func_ov006_020c42bc();
    func_ov006_020c712c();
}
