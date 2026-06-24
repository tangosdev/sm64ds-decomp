//cpp
typedef int Fix12i;
extern "C" void func_ov091_02133020(char* c);
extern "C" void func_ov091_02132ff4(char* c);
extern "C" void func_ov091_02132f04(char* c);
extern "C" void func_ov091_02132e98(char* c);
extern "C" void func_ov091_02132e64(char* c);
extern "C" void func_ov025_02111a84(char* c);
extern "C" void func_ov025_021119f4(char* c);
extern "C" int func_ov025_021119a4(char* c);
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(void* p);
extern "C" void func_ov091_02133098(char* c);
extern "C" int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* p, Fix12i a, Fix12i b);
extern "C" int func_ov091_02132dc0(char* c);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(void* p);

extern "C" int func_ov025_02111b64(char* thiz)
{
    char* c = thiz;
    switch (*(int*)(c + 0x398)) {
    case 0: func_ov091_02133020(c); break;
    case 1: func_ov091_02132ff4(c); break;
    case 2: func_ov091_02132f04(c); break;
    case 3: func_ov091_02132e98(c); break;
    case 4: func_ov091_02132e64(c); break;
    case 5: func_ov025_02111a84(c); break;
    case 6: func_ov025_021119f4(c); break;
    case 7: func_ov025_021119a4(c); break;
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    func_ov091_02133098(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0 ||
        func_ov091_02132dc0(c) != 0) {
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    }
    return 1;
}
