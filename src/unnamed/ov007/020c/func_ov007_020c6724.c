extern void func_ov007_020c6d90(char* self, int i);
extern void func_ov007_020c28ac(void* a);

void func_ov007_020c6724(char* self, int flag) {
    if (flag != 0) {
        int i;
        for (i = 0; i < *(int*)(self + 0xc); i++) {
            func_ov007_020c6d90(self, i);
        }
    }
    func_ov007_020c28ac(*(void**)(self + 0x20));
    *(int*)(self + 8) = 0;
    *(int*)(self + 0x14) = 0;
}
