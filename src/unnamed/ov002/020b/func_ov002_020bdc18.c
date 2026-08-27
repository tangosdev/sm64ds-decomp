extern int func_ov002_020edca4(void*);
void func_ov002_020bdc18(char* c){
  int i;
  for(i=0;i<5;i++){
    int* a=*(int**)(c+0x588);
    if(a[i]!=0){
      func_ov002_020edca4((void*)a[i]);
      a=*(int**)(c+0x588);
      a[i]=0;
    }
  }
}
