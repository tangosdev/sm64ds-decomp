extern int func_ov077_02126d5c(char*, int*);
extern int data_ov077_02127ce8[];

int func_ov077_02126a50(char *c) {
    unsigned short h = *(unsigned short*)(c + 0x100);
    if (h == 0) {
        func_ov077_02126d5c(c, data_ov077_02127ce8);
    }
    return 1;
}
