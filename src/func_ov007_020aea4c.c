extern int func_ov007_020c937c(void*);
extern int func_ov007_020c3d1c(void*);

int func_ov007_020aea4c(char* c) {
    func_ov007_020c937c(*(void**)(c + 4));
    return func_ov007_020c3d1c(c);
}
