//cpp
extern "C" {
extern int _ZN9ModelAnimD1Ev(char*);
extern int _ZN18MovingMeshColliderD1Ev(char*);
extern int _ZN5ModelD1Ev(char*);
extern int _ZN5ActorD1Ev(char*);
extern int _ZTV14QuestionSwitch[];
extern int _ZTV17ExclamationSwitch[];
char* _ZN14QuestionSwitchD1Ev(char* c){
  *(int**)c = _ZTV14QuestionSwitch;
  _ZN9ModelAnimD1Ev(c+0x6b4);
  _ZN18MovingMeshColliderD1Ev(c+0x4ec);
  _ZN18MovingMeshColliderD1Ev(c+0x324);
  *(int**)c = _ZTV17ExclamationSwitch;
  _ZN18MovingMeshColliderD1Ev(c+0x124);
  _ZN5ModelD1Ev(c+0xd4);
  _ZN5ActorD1Ev(c);
  return c;
}
}
