typedef unsigned int u32;

extern void func_ov007_020c86c4(void* p);
extern void func_ov007_020c897c(void* p);
extern void func_ov007_020c8970(void* p);
extern void func_ov007_020c8688(void* p, int a);
extern void func_ov007_020c86a8(void* p, int a);
extern void func_ov007_020c8c88(void* p, int a);
extern void func_ov007_020c8bc8(void* p, int a);
extern void func_ov007_020c8b34(void* p, int a);

typedef void (*VFN)(void);

void func_ov007_020c8498(char* r5)
{
    int r4 = 0;
    VFN r6;

    if(*(int*)(r5 + 0x20) == 1){
        func_ov007_020c86c4(r5);
    }else if(*(int*)(r5 + 0x20) == 2){
        func_ov007_020c897c(r5);
    }

    if(*(int*)(r5 + 8) == 0) return;

    if(*(int*)(r5 + 8) == 1){
        int* v14 = (int*)((((long long)(int)(r5 + 0x14)) & 0xFFFFFFFFFFFFFFFFLL));
        *v14 += *(int*)(r5 + 0x18);
        if(*(int*)(r5 + 0x14) > 0x1000){
            *(int*)(r5 + 0x14) = 0x1000;
            switch(*(int*)(r5 + 0xc)){
            case 0:
                func_ov007_020c8970(r5);
                break;
            case 1:
                *(int*)(r5 + 8) = 3;
                break;
            case 2:
                if(*(int*)(r5 + 0x1c) == 1) func_ov007_020c8688(r5, *(int*)(r5 + 4));
                else func_ov007_020c86a8(r5, *(int*)(r5 + 4));
                break;
            case 3:
                if(*(int*)(r5 + 0x1c) == 1) func_ov007_020c86a8(r5, *(int*)(r5 + 4));
                else func_ov007_020c8688(r5, *(int*)(r5 + 4));
                break;
            }

            r6 = *(VFN*)(r5 + 0x24);
            if(r6 != 0){
                r6();
            }
            if(r6 != *(VFN*)(r5 + 0x24)) goto next;
            if((u32)*(int*)(r5 + 0xc) <= 1) *(int*)(r5 + 0x24) = 0;
        }
    }

next:
    if(*(int*)(r5 + 8) == 0) return;

    switch(*(int*)(r5 + 0x10)){
    case 0:
        r4 = *(int*)(r5 + 0x14);
        break;
    case 1:
        r4 = *(int*)(r5 + 0x14) << 1;
        if(r4 > 0x1000) r4 = -(r4 - 0x2000);
        break;
    case 2:
        r4 = (int)(((long long)*(int*)(r5 + 0x14) * *(int*)(r5 + 0x14)) >> 12);
        break;
    case 3:
        break;
    }

    if(*(int*)(r5 + 0x1c) == 1) r4 = 0x1000 - r4;

    switch(*(int*)(r5)){
    case 1:
        func_ov007_020c8c88(r5, r4);
        break;
    case 2:
    case 3:
        func_ov007_020c8bc8(r5, r4);
        break;
    case 4:
        func_ov007_020c8b34(r5, r4);
        break;
    default:
        break;
    }
}
