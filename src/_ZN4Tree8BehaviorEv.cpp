//cpp
// @symbol _ZN4Tree8BehaviorEv
/* recovered: real class-form method. Replaces the hand-written extern "C"
 * definition left by the #1719-era note; the body is unchanged -- it walks
 * the heap-allocated dCcPos_c node list shared through data_ov002_02110a48
 * and touches none of this class's own storage. */
#include "Tree.h"

extern "C" {
extern int _ZN5dCc_c5ClearEv(void*);
extern int _ZN5dCc_c6UpdateEv(void*);
extern char* data_ov002_02110a48[5];
}

s32 Tree::Behavior() {
  char** pp = data_ov002_02110a48;
  int i;
  for(i=0;i<5;i++){
    char* p = *pp;
    while(p){
      _ZN5dCc_c5ClearEv(p+0xc);
      _ZN5dCc_c6UpdateEv(p+0xc);
      p = *(char**)(p+0x48);
    }
    pp++;
  }
  return 1;
}
