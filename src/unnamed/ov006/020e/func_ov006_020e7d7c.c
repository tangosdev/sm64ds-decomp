typedef int Fix12;
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, Fix12, unsigned int, unsigned int);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
struct G2 { int w[2]; };
extern void* data_ov006_02141e8c;
extern struct G2 data_ov006_0213c754;
void func_ov006_020e7d7c(char *c){
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xc, *(void**)((char*)&data_ov006_02141e8c+4), 0x40000000, 0x800, 0);
    if(*(int*)(c+0x20c)==0)
        _ZN5Sound12PlayBank2_2DEj(0x191);
    *(struct G2*)(c+0x210) = data_ov006_0213c754;
}
