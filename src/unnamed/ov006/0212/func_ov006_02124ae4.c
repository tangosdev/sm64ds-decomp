extern int func_ov006_020c1718(int* r0);
extern void func_ov004_020b56c8(int x);
extern void func_ov004_020b5ed0(void);
extern int data_ov004_020bfa18;

void func_ov006_02124ae4(char* c)
{
    if (func_ov006_020c1718((int*)(c + 0x4f38)) == 0) return;
    if (*(int*)(c + 0x5000 + 0x1b8) == 9) {
        func_ov004_020b56c8(data_ov004_020bfa18 << 1);
        *(int*)(c + 0x5000 + 0x1b8) = 0xc;
    } else {
        func_ov004_020b5ed0();
        *(int*)(c + 0x5000 + 0x1b8) = 0xb;
    }
}
