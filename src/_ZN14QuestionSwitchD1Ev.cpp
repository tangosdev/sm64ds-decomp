//cpp
// @symbol _ZN14QuestionSwitchD1Ev
/* recovered: named members + shared header */
#include "QuestionSwitch.h"
extern "C" {
extern int _ZN9ModelAnimD1Ev(char*);
extern int _ZN18MovingMeshColliderD1Ev(char*);
extern int _ZN5ModelD1Ev(char*);
extern int _ZN5ActorD2Ev(char*);
extern int _ZTV14QuestionSwitch[];
extern int _ZTV8Platform[];
char* _ZN14QuestionSwitchD1Ev(struct QuestionSwitch *self) {
  *(int**)((char*)self) = _ZTV14QuestionSwitch;
  _ZN9ModelAnimD1Ev((char*)&self->mModelAnim);
  _ZN18MovingMeshColliderD1Ev((char*)&self->mMovingMeshCollider);
  _ZN18MovingMeshColliderD1Ev((char*)&self->unk_324);
  *(int**)((char*)self) = _ZTV8Platform;
  _ZN18MovingMeshColliderD1Ev((char*)&self->unk_124);
  _ZN5ModelD1Ev((char*)&self->mModel);
  _ZN5ActorD2Ev(((char*)self));
  return ((char*)self);
}
}
