extern void func_ov007_020bfe4c(int a, int b, int c, int d, void* out);
extern void func_ov007_020c30ac(int a, int b, void* c, int d, int e);

void func_ov007_020bf630(int* self, int* p1, int p2, int p3, int arg5) {
    int local[3];
    func_ov007_020bfe4c((int)p1, p2 << 0xc, p3 << 0xc, -p1[0xb], local);
    {
        int* q = (int*)self[1];
        func_ov007_020c30ac(self[0], q[0] * q[1], local, arg5, 0);
    }
}
