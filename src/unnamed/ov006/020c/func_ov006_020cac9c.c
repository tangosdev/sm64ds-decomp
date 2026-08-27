void func_ov006_020cad3c(int sz);
void func_ov006_020ca3a8(char* c);
void func_ov006_020c8ecc(char* o);
void func_ov006_020c8a9c(int a0, int a1);
extern int data_ov006_021405b8;
extern int data_ov006_0213b0f0;
extern int data_ov006_02140598;
extern int data_ov006_021405ac;
extern char* data_ov006_02140554;

void func_ov006_020cac9c(void){
    int i;
    int off;
    func_ov006_020cad3c(0x1000);
    data_ov006_0213b0f0 = 3;
    data_ov006_02140598 = 0;
    data_ov006_021405ac = 0;
    if (data_ov006_021405b8 > 0) {
        i = 0;
        off = 0;
        do {
            func_ov006_020ca3a8(data_ov006_02140554 + off);
            func_ov006_020c8ecc(data_ov006_02140554 + off);
            i++;
            off += 0xdc;
        } while (i < data_ov006_021405b8);
    }
    func_ov006_020c8a9c(0, 0);
}
