#pragma optimize_for_size on

extern void func_ov007_020b01f4(void);
extern void func_ov007_020b01a4(void);
extern void func_ov007_020afc44(void);
extern void func_ov007_020af84c(void);
extern void func_ov007_020af59c(void);
extern void func_ov007_020af4dc(void);
extern void func_ov007_020beeb0(void);

typedef struct {
    char pad0[8];
    short *stateField;
    char pad1[8];
    int flag14;
} S;

extern S *data_ov007_0210342c;

void func_ov007_020af390(void)
{
    S *p = data_ov007_0210342c;
    short state = *p->stateField;

    switch (state) {
    case 0:
        func_ov007_020b01f4();
        break;
    case 1:
        func_ov007_020b01a4();
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        func_ov007_020afc44();
        break;
    case 7:
    case 8:
        func_ov007_020af84c();
        break;
    case 9:
    case 10:
        func_ov007_020af59c();
        break;
    case 11:
        func_ov007_020af4dc();
        break;
    case 12:
        p->flag14 = 1;
        break;
    }
    func_ov007_020beeb0();
}
