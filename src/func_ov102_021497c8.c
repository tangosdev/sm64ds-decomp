extern int func_ov102_02149078(void* self);
extern void func_ov102_02149da8(void* self, int a);

void func_ov102_021497c8(void* self, void* arg) {
    int v = *(int*)((char*)self + 0x3e8);
    if (v == 1) return;
    if (func_ov102_02149078(self)) return;
    int val = *(int*)((char*)arg + 8);
    *(unsigned char*)((char*)self + 0x3f2) = val;
    func_ov102_02149da8(self, 1);
}
