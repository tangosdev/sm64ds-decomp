void func_ov004_020b0cac(int a, int b, int c, int d, int e, int f);
void func_ov004_020ae274(void* c);
extern int data_ov004_020beb6c;
extern int data_ov004_020bc8ac;
extern int data_ov004_020bc8b0;
extern int data_ov004_020bc8a0;
struct P2 { int a, b; };
extern struct P2 data_ov004_020bc9e4;

void func_ov004_020b7854(char* c){
    int arg0;
    *(int*)(c+0x1c) = 0xb4;
    func_ov004_020ae274((void*)4);
    arg0 = 3;
    if (data_ov004_020bc8a0 != 0x1d) { arg0 = data_ov004_020bc8a0; data_ov004_020bc8a0 = 0x1d; }
    func_ov004_020b0cac(arg0, data_ov004_020bc8b0, data_ov004_020bc8ac - (data_ov004_020beb6c + 0xc0), -1, -1, 0xa);
    *(struct P2*)(c+8) = data_ov004_020bc9e4;
}
