typedef unsigned char u8;
typedef unsigned short u16;

extern void* func_ov006_020c7300(void* c);
extern void* func_ov006_020c4060(void* r);
extern void func_ov004_020b0a54(int a);
extern void func_ov006_020c42bc(void);
extern void func_ov006_020c712c(void);

struct G2 { int w[2]; };
extern struct G2 data_ov006_0213cb74;

void func_ov006_020ee3ec(char* c)
{
    void* r = func_ov006_020c7300(c);
    if (r) {
        r = func_ov006_020c4060(r);
        if (r) {
            func_ov004_020b0a54(0x12);
            *(u8*)(c + 0xc3) = 0;
            *(u16*)(c + 0x5000 + 0x14) = 0;
            *(struct G2*)(c + 0x5000 + 4) = data_ov006_0213cb74;
        }
    }
    func_ov006_020c42bc();
    func_ov006_020c712c();
}
