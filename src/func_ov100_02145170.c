// @symbol func_ov100_02145170
/* recovered: shared common types */
#include "common.h"
extern void Vec3_RotateYAndTranslate(struct Vector3 *res, const struct Vector3 *translation, short angY, const struct Vector3 *v);
void func_ov100_02145170(char *r0, char *r1, struct Vector3 *a, struct Vector3 *b){
 struct Vector3 *v;
 if(*(int*)(r0+0x88)>0){*(short*)(r1+0x8e)=*(short*)(r0+0x8e)+0x8000;v=a;}
 else{*(short*)(r1+0x8e)=*(short*)(r0+0x8e);v=b;}
 *(short*)(r1+0x94)=*(short*)(r1+0x8e);
 Vec3_RotateYAndTranslate((struct Vector3*)(r1+0x5c),(struct Vector3*)(r0+0x5c),*(short*)(r0+0x8e),v);
}
