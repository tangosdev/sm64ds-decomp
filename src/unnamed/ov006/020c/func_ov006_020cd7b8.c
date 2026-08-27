extern void func_ov006_020bfff8(void* a, void* b, int* c, int* d);
extern int func_ov004_020b04c0(void);
extern void func_ov006_020ef05c(int a, int b, int c);
extern void* data_ov006_02141a50;
extern void* data_ov006_02141a40;
struct P2 { int a, b; };
extern struct P2 data_ov006_0213b32c;

void func_ov006_020cd7b8(char* c, int arg1)
{
    int v0, v1;
    if (*(int*)(c + 0xc) > 0) {
        func_ov006_020bfff8(data_ov006_02141a50, (void*)(c + 8), &v0, &v1);
        v1 = v1 - (func_ov004_020b04c0() + 0xc0);
    } else {
        func_ov006_020bfff8(data_ov006_02141a40, (void*)(c + 8), &v0, &v1);
    }
    func_ov006_020ef05c(v0 << 0xc, v1 << 0xc, (short)arg1);
    *(struct P2*)c = data_ov006_0213b32c;
}
