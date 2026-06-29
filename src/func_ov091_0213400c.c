extern int SetAnim(char* anim, void* file, int a, int b, unsigned int u);
struct S { int w[2]; };
extern struct S data_ov091_02135684;
int func_ov091_0213400c(char *p) {
    SetAnim(p+0x300, ((void**)&data_ov091_02135684)[1], 0, 0x1000, 0);
    return 1;
}
