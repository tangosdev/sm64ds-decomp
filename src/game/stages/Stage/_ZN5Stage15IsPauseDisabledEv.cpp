//cpp
/* Stage::IsPauseDisabled() at 0x0202c8f8. Static: reads only globals, and no
 * caller sets up an object. */
#include "Stage.h"

extern "C" {
extern int data_0209f394[];
extern int func_ov002_020bd8ac(unsigned char *p);
}

int Stage::IsPauseDisabled(){
  unsigned char *p=*(unsigned char**)data_0209f394;
  if(p!=0 && func_ov002_020bd8ac(p)==0) return 1;
  return 0;
}
