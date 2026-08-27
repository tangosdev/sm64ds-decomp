//cpp
typedef int Fix12i;
struct BCA_File;
struct BlendModelAnim;
extern "C" {
void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, void*, int, int, Fix12i, unsigned short);
void func_02012694(int, void*);
extern int data_ov016_02114d20[];
int func_ov016_02111860(char* c){
  void* anim = (void*)data_ov016_02114d20[1];
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x350, anim, 8, 0x40000000, 0x1000, 0);
  func_02012694(0xfa, c+0x74);
  return 1;
}
}
