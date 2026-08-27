//cpp
extern "C" {
int _ZN9Animation8FinishedEv(void*);
int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,int,int,int,unsigned);
int _ZN6Player12Unk_020ca150Eh(void*, unsigned char);
int _ZN6Player12Unk_020c9e5cEh(void*, unsigned char);
extern int data_ov065_0211d770[];
void func_ov065_021182e4(char* c){
  switch(*(unsigned char*)(c+0x11b0)){
  case 0:
    if(_ZN9Animation8FinishedEv(c+0x13c) == 0) return;
    *(unsigned char*)(c+0x11b0) = 1;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xec, data_ov065_0211d770[1], 0, 0x1000, 0);
    if(*(int*)(c+0x1190) == 0) return;
    _ZN6Player12Unk_020ca150Eh((void*)*(int*)(c+0x1190), 5);
    return;
  case 1:
    if(*(int*)(c+0x1190) != 0){
      if(_ZN6Player12Unk_020c9e5cEh((void*)*(int*)(c+0x1190), 5) != 0) return;
    }
    *(unsigned char*)(c+0x11b4) = 0;
    *(unsigned char*)(c+0x11b0) = 0;
    *(int*)(c+0x1190) = 0;
    return;
  }
}
}
