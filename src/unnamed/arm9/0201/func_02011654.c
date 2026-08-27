extern int func_02011aa0(void*);
int func_02011654(char* table, char* n){
  void* prev = *(void**)(n+0xc);
  void* next = *(void**)(n+0x10);
  if(prev==0){
    int idx = (*(int*)n) & 0xf;
    *(void**)(table + (idx<<2) + 0x508) = next;
  } else {
    *(void**)((char*)prev+0x10) = next;
  }
  if(next){
    *(void**)((char*)next+0xc) = prev;
  }
  return func_02011aa0(n);
}
