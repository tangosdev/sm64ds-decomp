extern int func_ov063_02119b84(void);
extern void func_ov063_02119894(void* c);
extern void func_ov063_02119870(void* c);
extern void func_ov063_0211975c(void* c);
extern void func_ov063_02119a2c(void* c);
extern int func_ov063_0211aa34(void* c);
void func_ov063_02119274(char* c){
    switch(*(unsigned char*)(c+0x5cc)){
        case 0: func_ov063_02119b84(); break;
        case 1: func_ov063_02119894(c); break;
        case 2: func_ov063_02119870(c); break;
        case 3: func_ov063_0211975c(c); break;
        case 4: func_ov063_02119a2c(c); break;
    }
    func_ov063_0211aa34(c);
}
