//cpp
extern "C" void _ZN6Camera25SaveCameraStateBeforeTalkEv(void *obj) {
    if (*(unsigned int *)((char *)obj + 0x154) & 0x4000U) return;
    *(unsigned int *)((char *)obj + 0xb0) = *(unsigned int *)((char *)obj + 0x80);
    *(unsigned int *)((char *)obj + 0xb4) = *(unsigned int *)((char *)obj + 0x84);
    *(unsigned int *)((char *)obj + 0xb8) = *(unsigned int *)((char *)obj + 0x88);
    *(unsigned int *)((char *)obj + 0xbc) = *(unsigned int *)((char *)obj + 0x8c);
    *(unsigned int *)((char *)obj + 0xc0) = *(unsigned int *)((char *)obj + 0x90);
    *(unsigned int *)((char *)obj + 0xc4) = *(unsigned int *)((char *)obj + 0x94);
    *(unsigned int *)((int *)(((int)obj + 0x154) & 0xFFFFFFFFFFFFFFFF)) |= 0x4000U;
}
