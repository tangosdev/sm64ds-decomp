extern char *unk_380FFF4;

int fn_023951A4(int *block) {
    unsigned short *head;
    unsigned short *body;
    unsigned short *p;
    int one;

    body = *(unsigned short **)((char *)block + 8);
    head = body - 2;
    if (head[0] != 0xB6B8 || head[1] != 0x1D46) {
        one = 1;
        body[5] = (unsigned short)one;
        head[0] = 0xB6B8;
        head[1] = 0x1D46;
        p = *(unsigned short **)((char *)block + 8);
        p = (unsigned short *)((char *)p + 0xC);
        p[0] = 0xB6B8;
        p[1] = 0x1D46;
        {
            unsigned short *counter = (unsigned short *)(unk_380FFF4 + 0x3B2);
            *counter = (unsigned short)(*counter + 1);
        }
        return one;
    }
    return 0;
}
