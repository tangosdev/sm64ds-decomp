extern int func_ov007_020c43bc(void* a, void* b);
extern void func_ov007_020c4388(void* a, void* b);
extern void func_ov007_020c9f10(void* a, void* b, int c, int d);
extern void func_ov007_020c9d90(void* a, void* b);
extern char* data_ov007_0210342c;
extern char* data_ov007_02104bd4;

struct Buf { int w[34]; };

void func_ov007_020bf57c(char* arg0, int idx)
{
    char* g = data_ov007_0210342c;
    int* e = *(int**)((char*)(*(int**)((char*)g + (idx + 0xe) * 4 + 0x114)) + 4);
    char* h = data_ov007_02104bd4;
    void* r4 = *(void**)((char*)(h + idx * 4) + 0x10);

    struct Buf buf;
    buf = *(struct Buf*)e;
    buf.w[4] = -*(int*)(arg0 + 0x2c);
    func_ov007_020c43bc(&buf, arg0);
    func_ov007_020c4388(&buf, arg0);
    func_ov007_020c9f10(r4, &buf.w[5], buf.w[8], buf.w[9]);
    func_ov007_020c9d90(r4, *(void**)(data_ov007_02104bd4));
}
