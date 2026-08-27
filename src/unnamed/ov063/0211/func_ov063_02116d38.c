extern int Snufit_Spawn(void);
extern void func_ov063_02116e14(void* c);
extern void func_ov063_02116df0(void* c);
extern void func_ov063_02116dbc(void* c);
extern void func_ov063_02116d98(void* c);
extern int func_ov063_0211aa34(void* c);
void func_ov063_02116d38(char* c){
    switch(*(unsigned char*)(c+0x5cc)){
        case 0: Snufit_Spawn(); break;
        case 1: func_ov063_02116e14(c); break;
        case 2: func_ov063_02116df0(c); break;
        case 3: func_ov063_02116dbc(c); break;
        case 4: func_ov063_02116d98(c); break;
    }
    func_ov063_0211aa34(c);
}
