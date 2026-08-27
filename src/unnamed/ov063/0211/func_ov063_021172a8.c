extern void func_ov063_02118914(char* c);
extern void func_ov063_0211873c(char* c);
extern void func_ov063_02118f50(void* c);
extern void func_ov063_021177b0(char* c);
extern void func_ov063_02118458(char* c);
extern void func_ov063_0211776c(char* c);
extern void func_ov063_02117364(char* c);
extern void func_ov063_02117cdc(char* c);
extern void func_ov063_02117b0c(char* c);
extern void func_ov063_0211aa34(char* c);

void func_ov063_021172a8(void* thiz)
{
    char* c = (char*)thiz;
    switch (*(unsigned char*)(c + 0x5cc)) {
    case 0: func_ov063_02118914(c); break;
    case 1: func_ov063_0211873c(c); break;
    case 2: func_ov063_02118f50(c); break;
    case 3: func_ov063_021177b0(c); break;
    case 4: func_ov063_02118458(c); break;
    case 5: func_ov063_0211776c(c); break;
    case 6: func_ov063_02117364(c); break;
    case 7: func_ov063_02117cdc(c); break;
    case 8: func_ov063_02117b0c(c); break;
    }
    {
        int state = *(unsigned char*)(c + 0x5cc);
        if (state == 7) return;
        if (state == 6) {
            if (*(unsigned short*)(c + 0x5c2) > 0x5a) return;
        }
        func_ov063_0211aa34(c);
    }
}
