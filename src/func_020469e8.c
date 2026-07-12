extern void Crash(void);
extern void func_02045e44(void* self, unsigned int value, int index);

void func_020469e8(void* arg0, char* sl, int sb)
{
    int j;

    for (j = 0; j < *(unsigned short*)(sl + 0x10); j++) {
        char* src = *(char**)(sl + 0x14) + j * 0x1c;
        unsigned short srcIdx;
        char* dst;
        int* dstBase;

        srcIdx = *(unsigned short*)src;
        if (srcIdx == 0xffff) Crash();

        dstBase = *(int**)((char*)arg0 + 4);
        srcIdx = *(unsigned short*)src;
        dst = (char*)dstBase + srcIdx * 0x30;
        func_02045e44(arg0, 1, srcIdx);

        if (*(unsigned short*)(src + 8) == 1) {
            unsigned short idx = *(unsigned short*)(src + 0xa);
            *(int*)(dst + 8) = *(int*)(*(int**)(sl + 4) + idx);
        } else {
            unsigned short idx = *(unsigned short*)(src + 0xa);
            *(int*)(dst + 8) = *(int*)(*(int**)(sl + 4) + (idx + sb));
        }

        if (*(unsigned short*)(src + 0xc) == 1) {
            unsigned short idx = *(unsigned short*)(src + 0xe);
            *(int*)(dst + 0xc) = *(int*)(*(int**)(sl + 4) + idx);
        } else {
            unsigned short idx = *(unsigned short*)(src + 0xe);
            *(int*)(dst + 0xc) = *(int*)(*(int**)(sl + 4) + (idx + sb));
        }

        if (*(unsigned short*)(src + 0x10) == 1) {
            unsigned short idx = *(unsigned short*)(src + 0x12);
            *(unsigned short*)(dst + 0x10) = *(unsigned short*)(*(unsigned short**)(sl + 8) + idx);
        } else {
            unsigned short idx = *(unsigned short*)(src + 0x12);
            *(unsigned short*)(dst + 0x10) = *(unsigned short*)(*(unsigned short**)(sl + 8) + (idx + sb));
        }

        if (*(unsigned short*)(src + 0x14) == 1) {
            unsigned short idx = *(unsigned short*)(src + 0x16);
            *(int*)(dst + 0x14) = *(int*)(*(int**)(sl + 0xc) + idx);
        } else {
            unsigned short idx = *(unsigned short*)(src + 0x16);
            *(int*)(dst + 0x14) = *(int*)(*(int**)(sl + 0xc) + (idx + sb));
        }

        if (*(unsigned short*)(src + 0x18) == 1) {
            unsigned short idx = *(unsigned short*)(src + 0x1a);
            *(int*)(dst + 0x18) = *(int*)(*(int**)(sl + 0xc) + idx);
        } else {
            unsigned short idx = *(unsigned short*)(src + 0x1a);
            *(int*)(dst + 0x18) = *(int*)(*(int**)(sl + 0xc) + (idx + sb));
        }
    }
}
