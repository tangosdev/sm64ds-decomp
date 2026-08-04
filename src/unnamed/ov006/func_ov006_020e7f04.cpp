//cpp
extern "C" {
int _ZNK9Animation12WillHitFrameEi(void*,int);
void _ZN5Sound12PlayBank2_2DEj(unsigned int);
void func_ov006_020e7f04(char* c){
  if(*(int*)(c+0x20c)) return;
  if(!_ZNK9Animation12WillHitFrameEi(c+0x5c,0)){
    if(!_ZNK9Animation12WillHitFrameEi(c+0x5c,0x39)) return;
  }
  _ZN5Sound12PlayBank2_2DEj(0x18f);
}
}
