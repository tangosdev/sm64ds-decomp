extern int func_ov091_02133254(char*);
extern int data_ov091_02135138[];

int _ZN6Thwomp13InitResourcesEv(char *c) {
    *(int*)(c + 0x320) = (int)data_ov091_02135138;
    int r = func_ov091_02133254(c);
    char *p = c + 0x300;
    *(int*)(c + 0x398) = 0;
    *(short*)(p + 0xa0) = 0;
    *(char*)(c + 0x3a2) = 0;
    return r;
}
