extern int data_ov007_0210342c;
extern void func_ov007_020b1cf0(void);
extern void func_ov007_020b1224(void);
extern void func_ov007_020b1b74(void);
extern void func_ov007_020b1b70(void);
extern void func_ov007_020b1b4c(void);
extern void func_ov007_020b1718(void);
extern void func_ov007_020b166c(void);
extern void func_ov007_020b155c(void);
extern void func_ov007_020b11bc(void);
extern void func_ov007_020b10dc(void);
extern void func_ov007_020b0a20(void);
extern void func_ov007_020b0834(void);

void func_ov007_020b0548(void){
    char* r1 = *(char**)&data_ov007_0210342c;
    int st = *(short*)(*(char**)(r1 + 8));
    switch (st) {
    case 0: func_ov007_020b1cf0(); func_ov007_020b1224(); break;
    case 1: func_ov007_020b1b74(); break;
    case 2: func_ov007_020b1b70(); break;
    case 3: func_ov007_020b1b4c(); break;
    case 4: func_ov007_020b1718(); break;
    case 5: func_ov007_020b166c(); break;
    case 6: func_ov007_020b155c(); break;
    case 7: *(int*)(r1 + 0x14) = 1; break;
    case 8: func_ov007_020b11bc(); func_ov007_020b1224(); break;
    case 10: func_ov007_020b10dc(); break;
    case 11: func_ov007_020b0a20(); break;
    case 9: func_ov007_020b0834(); break;
    default: break;
    }
}
