typedef unsigned char u8;
int _ZN9ActorBase14BeforeBehaviorEv(char* c){
  if(*(u8*)(c+0xf)!=0) goto ret0;
  if((*(u8*)(c+0x13)&2)==0) goto ret1;
ret0:
  return 0;
ret1:
  return 1;
}
