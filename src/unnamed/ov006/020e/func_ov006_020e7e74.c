typedef int Fix12;
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, Fix12, unsigned int, unsigned int);
extern void func_ov006_020e7940(char *p, short v);
extern void func_ov006_020e794c(int *p, int v);
extern void func_ov006_020e792c(char* c, short v);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
struct G2 { int w[2]; };
extern void* data_ov006_02141e5c;
extern struct G2 data_ov006_0213c764;
void func_ov006_020e7e74(char *c){
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xc, *(void**)((char*)&data_ov006_02141e5c+4), 0x40000000, 0x800, 0);
    func_ov006_020e7940(c+0x84, 0xf);
    func_ov006_020e794c((int*)(c+0x84), 0x1000);
    func_ov006_020e792c(c+0x84, 3);
    if(*(int*)(c+0x20c)==0)
        _ZN5Sound12PlayBank2_2DEj(0x18e);
    *(struct G2*)(c+0x210) = data_ov006_0213c764;
}
