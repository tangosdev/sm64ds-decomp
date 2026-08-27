extern void func_ov006_020ca840(void *c);
struct P2 { int a, b; };
extern struct P2 data_ov006_0213fbf0;
void func_ov006_02123c78(char *c) {
    func_ov006_020ca840(c);
    *(int*)(c + 0x7b84) = 0x5a;
    *(struct P2*)(c + 0x5004) = data_ov006_0213fbf0;
}
