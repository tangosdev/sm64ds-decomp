extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int a, int b, unsigned int u);
struct G { int w[2]; };
extern struct G data_ov072_02122c9c;

int func_ov072_02121c94(char *p) {
    *(unsigned char *)(p + 0x36e) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p + 0xd4, (void *)data_ov072_02122c9c.w[1], 0, 0x1000, 0);
    *(int *)(p + 0x368) = 0;
    return 1;
}
