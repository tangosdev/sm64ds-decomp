//cpp
typedef struct Pair { int a, b; } Pair;
extern "C" {
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, void*, int, int, int, unsigned short);
extern void Vec3_Sub(void* out, void* a, void* b);
extern int NormalizeVec3IfNonZero(void* v);
extern int Vec3_HorzAngle(void* a, void* b);
extern int data_ov006_0213acc0[];
extern int data_ov006_0213ac68[];
void func_ov006_020c057c(char* c) {
    int sp8[3];
    int sp14[3];
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x18, (void*)*(int*)(c+0x14), 0, 0, 0x800, 0);
    if (*(int*)(c+0xe4) < 3) {
        *(int *)(((int)c + 0xe4) & 0xFFFFFFFFFFFFFFFF) += 1;
    } else {
        *(int*)(c+0xe4) = 0;
    }
    *(int*)(c+0xb0) = (int)((char*)data_ov006_0213acc0 + (*(int*)(c+0xe4) << 3));
    *(int*)(c+0xe0) = 0;
    {
        int t = *(int*)(c+0xe0) * 0xc;
        int* arr = (int*)((int*)*(int*)(c+0xb0))[1];
        int* el = (int*)((char*)arr + t);
        *(int*)(c+0xc8) = el[0];
        *(int*)(c+0xcc) = el[1];
        *(int*)(c+0xd0) = el[2];
    }
    *(int *)(((int)c + 0xe0) & 0xFFFFFFFFFFFFFFFF) += 1;
    Vec3_Sub(sp8, c+0xc8, (char*)((int*)*(int*)(c+0xb0))[1] + *(int*)(c+0xe0) * 0xc);
    *(int*)(c+0xd4) = sp8[0];
    *(int*)(c+0xd8) = sp8[1];
    *(int*)(c+0xdc) = sp8[2];
    if (NormalizeVec3IfNonZero(c+0xd4) == 0) {
        *(int*)(c+0xd4) = 0xb50;
        *(int*)(c+0xd8) = 0xb50;
        *(int*)(c+0xdc) = 0;
    }
    {
        int t = *(int*)(c+0xe0) * 0xc;
        int* arr = (int*)((int*)*(int*)(c+0xb0))[1];
        int* el = (int*)((char*)arr + t);
        sp14[0] = el[0];
        sp14[1] = el[1];
        sp14[2] = el[2];
    }
    *(short*)(c+0xee) = (short)Vec3_HorzAngle(c+0xc8, sp14);
    {
        int a = data_ov006_0213ac68[0];
        int b = data_ov006_0213ac68[1];
        *(int*)(c+0xb4) = b ? a : a;
        *(int*)(c+0xb8) = b;
    }
}
}
