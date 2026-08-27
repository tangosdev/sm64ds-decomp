typedef int Fix12;
typedef short s16;
struct M8 { int a, b; };
extern int _Z15ApproachLinear2Rsss(short* p, short b, short c);
extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void* self, void* f, int a, int b, Fix12 d, unsigned short e);
extern char data_ov006_0213ac60[];

void func_ov006_020c1604(char* c, int unused, short a2, int a3) {
  if (*(s16*)(c+0x1dc) == 0) {
    *(s16*)(c+0x1d8) = 4;
    *(s16*)(c+0x1da) = 3;
  } else {
    *(s16*)(c+0x1d8) = 1;
    *(s16*)(c+0x1da) = 1;
  }
  if (_Z15ApproachLinear2Rsss((short*)(c+0x1dc), 5, 1) != 0) {
    *(s16*)(c+0x1dc) = 0;
  }
  *(int*)(c+0x26c) = 0;
  *(s16*)(c+0x1de) = a2;
  *(int*)(c+0x1d4) = a3;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x1c, *(void**)(c+0x204), 0, 0x40000000, 0x800, 0);
  *(struct M8*)c = *(struct M8*)data_ov006_0213ac60;
}
