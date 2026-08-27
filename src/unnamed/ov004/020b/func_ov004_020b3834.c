extern int _Z15ApproachLinear2Rsss(short* v, short t, short s);
extern void func_ov004_020b37f0(int* obj);

void func_ov004_020b3834(int* obj){
    char* c = (char*)obj;
    if (*(int*)(c + 0x24) < 0x30){
        *(unsigned*)(c + 0x24) += 1;
        return;
    }
    if (_Z15ApproachLinear2Rsss((short*)(c + 0x12), *(short*)(c + 0x16), 0x10)){
        func_ov004_020b37f0(obj);
    }
}
