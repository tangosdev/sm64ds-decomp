extern int func_02021cd4(void*);
int func_02021a04(char* table, char* n){
  void* prev = *(void**)(n+0x14);
  void* next = *(void**)(n+0x18);
  if(prev==0){
    int idx = (*(int*)n) & 0xf;
    *(void**)(table + (idx<<2) + 0x708) = next;
  } else {
    *(void**)((char*)prev+0x18) = next;
  }
  if(next){
    *(void**)((char*)next+0x14) = prev;
  }
  return func_02021cd4(n);
}
