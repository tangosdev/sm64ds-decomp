extern int Bowser_IsAnimAtLastFrame(void *);

void func_ov060_02114ff8(void *c) {
    int r = Bowser_IsAnimAtLastFrame(c);
    if (r) {
        *(int*)((char*)c + 0x40c) = 0;
    }
}
