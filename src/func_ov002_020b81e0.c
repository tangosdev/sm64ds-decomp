// @symbol func_ov002_020b81e0
// recovered name: WaterfallMist_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* daObjMarioCap_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern int _ZN6Player17SetNoControlStateEhih(char* p, unsigned char a, int b, unsigned char d);
extern void _ZN6Player18SetNewHatCharacterEjjb(char* p, unsigned int a, unsigned int b, int d);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* bca, int a, int b, unsigned int e);
extern void func_ov002_020b7f2c(char* c, void* p);
extern void* data_ov002_0210de30[];
extern int data_ov002_0210df54;

void func_ov002_020b81e0(char* c, char* arg)
{
    if (_ZN6Player17SetNoControlStateEhih(arg, 8, -1, 0) == 1) {
        _ZN6Player18SetNewHatCharacterEjjb(arg, *(int*)(c + 0x3f4) & 0xff, 1, 0);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov002_0210de30[1], 0x40000000, 0x1000, 0);
        c[0x3fe] = 1;
    }
    *(char**)(c + 0x3c0) = arg;
    func_ov002_020b7f2c(c, &data_ov002_0210df54);
}
