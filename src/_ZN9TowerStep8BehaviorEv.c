extern int DecIfAbove0_Byte(void*);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned,unsigned,unsigned,void*,unsigned);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int func_020393a4(int*,int);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void*,int,int);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
int _ZN9TowerStep8BehaviorEv(char* c){
  if (DecIfAbove0_Byte((char*)c+0x31e) == 0) {
    short* p = (short*)(((int)c + 0x94) & 0xFFFFFFFFFFFFFFFF);
    *p = *p + 0x100;
    *(short*)(c+0x8e) = *(short*)(c+0x94);
    *(int*)(c+0x320) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(int*)(c+0x320), 3, 0x88, (void*)(c+0x74), 0);
    if ((*(short*)(c+0x94) & 0x7fff) == 0) *(char*)(c+0x31e) = 0x3c;
  }
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  func_020393a4((int*)(c+0x124), 0x320000);
  if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x320000, 0))
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
  return 1;
}
