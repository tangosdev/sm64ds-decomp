struct Animation { char unk[0x100]; };
extern int _ZNK9Animation13GetFrameCountEv(struct Animation *);

int func_ov100_0214491c(void *c) {
    int n = _ZNK9Animation13GetFrameCountEv((struct Animation *)((char *)c + 0x124));
    int f = (int)((unsigned short)(n - 0x18)) << 12;
    *(int *)((char *)c + 0x12c) = f;
    *(unsigned char *)((char *)c + 0x145) = 0;
    return 1;
}
