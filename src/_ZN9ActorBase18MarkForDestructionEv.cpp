//cpp
extern "C" {
void _ZN9ActorBase18MarkForDestructionEv(unsigned char *c){
  if(*(unsigned char*)(c+0xf)!=0) return;
  unsigned char b = (*(unsigned char*)(c+0xe)==2);
  if(b!=0) return;
  *(unsigned char*)(c+0xf)=1;
  (*(void(**)(unsigned char*))(*(unsigned int*)c+0x30))(c);
}
}
