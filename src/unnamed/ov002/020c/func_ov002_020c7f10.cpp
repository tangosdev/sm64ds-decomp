//cpp
struct State;
extern State data_ov002_0211022c;
extern "C" {
int _ZN6Player7IsStateERNS_5StateE(char* c, State& s);
void func_ov002_020ce9c8(char* c);
void func_ov002_020c7ff8(char* c);
void _ZN6Player7SetAnimEji5Fix12IiEj(char* c, unsigned int a, int b, int d, unsigned int e);

int func_ov002_020c7f10(char* c)
{
  unsigned char v;
  if (!_ZN6Player7IsStateERNS_5StateE(c, data_ov002_0211022c)) goto fail;
  if (*(unsigned char*)(c + 0x6e3) != 0) goto fail;
  v = *(unsigned char*)(c + 0x70a);
  if (v == 0 || v == 1 || v == 0x11 || (unsigned char)(v + 0xfe) <= 1) {
    if (*(unsigned char*)(c + 0x70c) != 0) goto fail;
    func_ov002_020ce9c8(c);
    *(int*)(c + 0x9c) = 0;
    *(unsigned char*)(c + 0x706) = 1;
    *(unsigned char*)(c + 0x70c) = 1;
    func_ov002_020c7ff8(c);
    return 1;
  }
  if (v == 8) {
    func_ov002_020ce9c8(c);
    *(int*)(c + 0x9c) = 0;
    *(unsigned char*)(c + 0x706) = 1;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x6b, 0x40000000, 0x1000, 0);
    *(unsigned char*)(c + 0x6e3) = 9;
    goto fail;
  }
fail:
  return 0;
}
}
