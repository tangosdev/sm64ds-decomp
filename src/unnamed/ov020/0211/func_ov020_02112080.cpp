//cpp
extern "C" void func_ov020_02111fc4(char *c);
extern "C" void func_ov020_02111ee0(char *c);
extern "C" void func_ov020_02111c30(char *c);
extern "C" void func_ov020_02111b28(char *c);
extern "C" void func_ov020_02111aa8(char *c);
extern "C" void func_ov020_021119dc(char *c);

extern "C" void func_ov020_02112080(char *c)
{
    switch (*(int *)(c + 0x424)) {
    case 0: func_ov020_02111fc4(c); break;
    case 1: func_ov020_02111ee0(c); break;
    case 2: func_ov020_02111c30(c); break;
    case 3: func_ov020_02111b28(c); break;
    case 4: func_ov020_02111aa8(c); break;
    case 5: func_ov020_021119dc(c); break;
    }
}
