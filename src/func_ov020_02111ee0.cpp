//cpp
#include "dActor_c.h"
#include "ModelBase.h"

struct ModelAnim;
struct BCA_File;
int ApproachLinear(short& value, short target, short step);

extern "C" {
int func_ov020_02111418(char *c);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim*, BCA_File*, int, int, u16);
extern int data_ov020_02114aa0[];
extern int data_ov020_02114aa8[];
void func_ov020_02111ee0(char* c){
  int r = func_ov020_02111418(c);
  if(r) return;
  if(ApproachLinear(*(short*)(c+0x8c), -0x2000, 0x200)){
    int s;
    *(int*)(c+0x98) = 0;
    s = ((ModelBase*)(c+0x110))->SetFile((BMD_File*)data_ov020_02114aa0[1], 1, -1);
    if(s == 0) return;
    *(int*)(c+0x424) = 2;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim*)(c+0x110), (BCA_File*)data_ov020_02114aa8[1], 0x40000000, 0x1000, 0);
    *(unsigned char*)(c+0x450) = 1;
    *(short*)(c+0x100) = 0;
    {
      int* p60 = (int*)(((int)c + 0x60));
      *p60 = *p60 + 0x32000;
    }
    *(int*)(c+0x43c) = -0x19000;
  }
  ((dActor_c*)c)->UpdatePos(0);
}
}
