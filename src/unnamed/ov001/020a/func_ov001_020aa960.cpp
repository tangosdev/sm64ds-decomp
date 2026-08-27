//cpp
extern "C" {
extern int func_ov001_020aa79c(int x);
extern int _ZN6Player15IsCollectingCapEv(void* p);
extern unsigned char data_ov001_020ad628[];
extern unsigned char data_ov001_020ad630[];

int func_ov001_020aa960(int x, char* arg){
  if((data_ov001_020ad628[x] & 0xc0) == 0xc0)
    data_ov001_020ad628[x] &= ~0x40;
  unsigned char v = data_ov001_020ad628[x];
  if(v & 8){
    data_ov001_020ad628[x] = v & ~8;
    return 1;
  }
  if(data_ov001_020ad630[x] == 0)
    return 0;
  if(v & 0x40)
    return 0;
  if(x == *(unsigned char*)(arg+0x6d9))
    return 0;
  if(func_ov001_020aa79c(x))
    return 0;
  if((data_ov001_020ad628[x] & 3) == 2)
    return 0;
  if(_ZN6Player15IsCollectingCapEv(arg))
    goto ret1;
  if(x != *(int*)(arg+8))
    goto ret1;
  if(data_ov001_020ad628[x] & 3)
    return 0;
ret1:
  return 1;
}
}
