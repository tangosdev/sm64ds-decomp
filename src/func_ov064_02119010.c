struct Mtx { int w[12]; };
extern void Matrix4x3_FromTranslation(struct Mtx* m, int x, int y, int z);
void func_ov064_02119010(char* c) {
    int x = *(int*)((char*)c + 0x5c) >> 3;
    int y = (*(int*)((char*)c + 0x60) + *(int*)((char*)c + 0x330)) >> 3;
    int z = *(int*)((char*)c + 0x64) >> 3;
    Matrix4x3_FromTranslation((struct Mtx*)((char*)c + 0xf0), x, y, z);
}
