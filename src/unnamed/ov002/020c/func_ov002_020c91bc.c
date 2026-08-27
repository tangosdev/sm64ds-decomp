extern unsigned char data_0209d660;
extern int data_0209b454;
extern int _ZN6Player12FinishedAnimEv(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* st);
extern int data_ov002_0211067c;
extern int data_ov002_0211013c;

int func_ov002_020c91bc(char* c)
{
    if (data_0209d660 != 0) return 1;
    *(int *)((int)(c + 0xb0)) &= ~0x800000;
    data_0209b454 &= ~0x800000;
    if (_ZN6Player12FinishedAnimEv(c)) {
        if (*(unsigned char*)(c + 0x706) != 0)
            _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211067c);
        else
            _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
    }
    return 0;
}
