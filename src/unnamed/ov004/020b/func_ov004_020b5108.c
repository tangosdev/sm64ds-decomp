extern void Vec2_Sub(int* o, int* a, int* b);
extern void func_0203d680(int* out, const int* in, int scale);
extern void func_ov004_020b1b08(int x);
struct P { int w[2]; };
extern struct P data_ov004_020bc800;
extern struct P data_ov004_020bc820;

void func_ov004_020b5108(char* c, int* in)
{
    int loc0[2];
    int loc2[2];
    *(unsigned char*)(c + 0x22) = 1;
    *(int*)(c + 8) = 0xc000;
    *(int*)(c + 0xc) = 0xc000;
    *(int*)(c + 0x10) = in[0];
    *(int*)(c + 0x14) = in[1];
    Vec2_Sub(loc0, (int*)(c + 0x10), (int*)(c + 8));
    func_0203d680(loc2, loc0, 0x100);
    struct P* bc = &data_ov004_020bc800;
    *(int*)(c + 0x18) = loc2[0];
    *(int*)(c + 0x1c) = loc2[1];
    {
        int v = *(int*)(c + 0x18);
        if (v < 0) v = -v;
        *(int*)(c + 0x18) = v;
    }
    {
        int v = *(int*)(c + 0x1c);
        if (v < 0) v = -v;
        *(int*)(c + 0x1c) = v;
    }
    if (*(int*)c == bc->w[0]) {
        if (*(int*)(c + 4) == bc->w[1]) goto end;
        if (*(int*)c == 0) goto end;
    }
    func_ov004_020b1b08(1);
end:
    *(struct P*)c = data_ov004_020bc820;
}
