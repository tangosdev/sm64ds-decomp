void func_02011698(char* table, char* n){
  int idx = (*(int*)n) & 0xf;
  char* base = table + (idx<<2);
  char* head = *(char**)(base+0x508);
  if(head==0){
    *(char**)(base+0x508) = n;
  } else {
    *(char**)(head+0xc) = n;
    *(char**)(n+0x10) = *(char**)(base+0x508);
    *(char**)(base+0x508) = n;
  }
}
