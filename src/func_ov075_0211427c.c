extern int _ZNK9Animation12WillHitFrameEi(void*,int);
extern int _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(void*,void*,void*,int,void*);
extern int func_0201251c(int,int,void*,int);
extern int GLOBAL_CLIPPER[];
extern int VIEW_MATRIX_ASR_3[];
int func_ov075_0211427c(char* c){
  int tmp[2];
  if(_ZNK9Animation12WillHitFrameEi((char*)c+0x50, 4)==0){
    int r=_ZNK9Animation12WillHitFrameEi((char*)c+0x50, 0x22);
    if(r==0) return r;
  }
  _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(GLOBAL_CLIPPER, VIEW_MATRIX_ASR_3, (char*)c+0x118, 0, tmp);
  return func_0201251c(0, 0x20, tmp, *(int*)(c+0x13c));
}
