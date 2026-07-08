//cpp
extern "C" {
extern int SILVER_NUMBER_MODEL_PTR[];
extern int SILVER_NUMBER_TEXSEQ_PTR[];
extern int RED_NUMBER_MODEL_PTR[];
extern int NUMBER_TEXSEQ_PTR[];
void _ZN13SharedFilePtr7ReleaseEv(void*);
int func_ov002_020f093c(char* c){
  if(*(int*)(c+8) & 0x10){
    _ZN13SharedFilePtr7ReleaseEv(SILVER_NUMBER_MODEL_PTR);
    _ZN13SharedFilePtr7ReleaseEv(SILVER_NUMBER_TEXSEQ_PTR);
  } else {
    _ZN13SharedFilePtr7ReleaseEv(RED_NUMBER_MODEL_PTR);
    _ZN13SharedFilePtr7ReleaseEv(NUMBER_TEXSEQ_PTR);
  }
  return 1;
}
}
