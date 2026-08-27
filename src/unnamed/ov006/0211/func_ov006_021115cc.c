extern void func_ov006_02114800(int a, void* p, int c);
extern void func_ov006_02115598(int a, void* p, int c, int d, int e);
extern void func_ov006_02114fec(int a);
extern void func_02012718(void *a, int b);
void func_ov006_021115cc(char* c);
void func_ov006_021115cc(char* c){
    int tmp1[2];
    int tmp2[2];
    *(int*)(c + 0x34) = 0x3c;
    *(unsigned char*)(c + 0x31) = 1;
    tmp1[0] = *(int*)(c + 8);
    tmp1[1] = *(int*)(c + 0xc);
    func_ov006_02114800(*(int*)(c + 4), tmp1, 0);
    tmp2[0] = *(int*)(c + 8);
    tmp2[1] = *(int*)(c + 0xc);
    func_ov006_02115598(*(int*)(c + 4), tmp2, 0x7d0, 0, 1);
    func_ov006_02114fec(*(int*)(c + 4));
    func_02012718((void*)0xe0, *(int*)(c + 8));
}
