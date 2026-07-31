extern int Vec2_Len(const void* v);
extern int func_0203d434(void* p);
extern void func_0203d630(void* p, int m);

#pragma opt_strength_reduction off
void func_ov006_02115150(char* self)
{
    int i;
    char* obj = self + 0x48d4;
    for (i = 0; i < 0x10; i++) {
        unsigned char* flag = (unsigned char*)(int)(((long long)(int)(self + i + 0x4804)) & 0xFFFFFFFFFFFFFFFFLL);
        if (*flag != 0) {
            *(int*)(((int)(self + i*8) + 0x4854) & 0xFFFFFFFFFFFFFFFFLL) += *(int*)(self + i*8 + 0x48d4);
            *(int*)(((int)(self + i*8) + 0x4858) & 0xFFFFFFFFFFFFFFFFLL) += *(int*)(self + i*8 + 0x48d8);
            {
                int m = Vec2_Len(obj) * 6 / 8;
                if (func_0203d434(obj)) {
                    func_0203d630(obj, m);
                }
            }
            {
                int* counter = (int*)(int)(((long long)(int)(self + i*4 + 0x4814)) & 0xFFFFFFFFFFFFFFFFLL);
                *counter += 1;
                if (*counter >= 0xf) {
                    *flag = 0;
                    *counter = 0;
                }
            }
        }
        obj += 8;
    }
}
