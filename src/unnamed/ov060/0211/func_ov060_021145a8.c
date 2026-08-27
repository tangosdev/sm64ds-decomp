void func_ov060_021145a8(void *c) {
    if (*(unsigned char*)((char*)c + 0x414) != 2) return;
    if (!(*(int*)((char*)c + 0x418) & 0x10000)) return;
    if (*(int*)((char*)c + 0x3f4) > 0x3e8000) {
        *(int*)((char*)c + 0x98) = 0x1e000;
    }
}
