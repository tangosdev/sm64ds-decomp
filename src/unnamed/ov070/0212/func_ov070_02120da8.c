extern char data_ov070_02123668;
extern void func_ov070_02120d70(char *c);
void func_ov070_02120da8(char *c, int a) {
    *(int *)(c + 0x41c) = (int)&data_ov070_02123668 + (a << 4);
    func_ov070_02120d70(c);
}
