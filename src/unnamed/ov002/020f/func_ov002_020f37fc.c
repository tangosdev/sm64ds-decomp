extern void func_ov002_020f3828(void *c);

void func_ov002_020f37fc(char *c)
{
    func_ov002_020f3828(c);
    *(unsigned char*)(c + 0x15c) = 1;
    *(unsigned char*)(c + 0x15d) = 2;
    *(unsigned char*)(c + 0x15e) = 0;
}
