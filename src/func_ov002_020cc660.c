#include "types.h"
extern s32 ApproachAngle(s16 *target, s16 from, s16 start, s16 speed, s16 max);
void func_ov002_020cc660(char *self, int angle){
 s16 v,w; int delta; int third;
 delta=angle-*(s16*)(self+0x94);
 third=*(int*)(self+0x98)/12;
 v=(s16)(int)(((long long)delta*third+0x800)>>12);
 if(v<-0x1800)v=-0x1800; else if(v>0x1800)v=0x1800;
 ApproachAngle((s16*)(self+0x75e),v,8,0x200,0x20);
 *(s16*)(self+0x762)=-*(s16*)(self+0x75e);
 *(s16*)(self+0x90)=*(s16*)(self+0x75e);
 w=(s16)(int)(((long long)*(int*)(self+0x98)*0xaa+0x800)>>12);
 if(w<0)w=0; else if(w>0x1000)w=0x1000;
 ApproachAngle((s16*)(self+0x760),w,8,0x200,0x20);
 *(u8*)(self+0x742)=1;
 *(int*)(self+0x690)=0x46000;
}
