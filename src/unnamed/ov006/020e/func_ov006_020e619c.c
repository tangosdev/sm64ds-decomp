void func_ov006_020e619c(char* c)
{
    int i;
    for (i = 0; i < 0xb; i++) {
        *(unsigned char*)(c + 0x468c) = 0;
        c += 0x30;
    }
}
