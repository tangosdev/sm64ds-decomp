//cpp
/* func_ov006_0210d6b8 at 0x0210d6b8 (ov006), size 0x88
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
extern "C" {
extern void *_ZN9ActorBasenwEj(unsigned int sz);
extern void func_ov004_020b2adc(void *);
extern void func_ov006_0210c2b0(char *p);
extern void func_ov006_0210c208(char *p);
extern void *data_ov006_0213eb40[];
extern void *data_ov006_0213e5d4[];
extern void func_020ad494();
void *func_ov006_0210d6b8(void);
void *func_ov006_0210d6b8(void) {
    char *p = (char *)_ZN9ActorBasenwEj(0x4710);
    if (p) {
        func_ov004_020b2adc(p);
        *(void ***)p = data_ov006_0213eb40;
        char *base = (char *)(((int)p + 0x4660) & 0xFFFFFFFFFFFFFFFF);
        *(void **)base = (void *)&func_020ad494;
        *(void ***)base = data_ov006_0213e5d4;
        func_ov006_0210c2b0(p + 0x4684);
        func_ov006_0210c2b0(p + 0x4690);
        func_ov006_0210c208(p + 0x469c);
    }
    return p;
}
}
