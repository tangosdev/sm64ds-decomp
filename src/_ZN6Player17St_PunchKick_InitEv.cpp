//cpp
extern "C" {
extern int _ZN6Player15IsCollectingCapEv(void*);
extern int func_ov002_020dc020(void*);
extern int _ZN12CylinderClsn5ClearEv(void*);
extern int _ZN12CylinderClsn6UpdateEv(void*);
int _ZN6Player17St_PunchKick_InitEv(void* c){
  *(int*)((char*)c+0x684)=*(int*)((char*)c+0x60);
  *(char*)((char*)c+0x726)=0;
  *(short*)((char*)c+0x6a4)=2;
  *(char*)((char*)c+0x6e6)=0;
  if(*(unsigned char*)((char*)c+0x6e2)==2 || _ZN6Player15IsCollectingCapEv(c)) return 1;
  if(*(unsigned char*)((char*)c+0x703)==0){
    func_ov002_020dc020(c);
    _ZN12CylinderClsn5ClearEv((char*)c+0x314);
    _ZN12CylinderClsn6UpdateEv((char*)c+0x314);
  } else {
    _ZN12CylinderClsn5ClearEv((char*)c+0x314);
  }
  return 1;
}
}
