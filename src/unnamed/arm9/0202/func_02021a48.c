void func_02021a48(char* table, char* n){
  int idx = (*(int*)n) & 0xf;
  char* base = table + (idx<<2);
  char* head = *(char**)(base+0x708);
  if(head==0){
    *(char**)(base+0x708) = n;
  } else {
    *(char**)(head+0x14) = n;
    *(char**)(n+0x18) = *(char**)(base+0x708);
    *(char**)(base+0x708) = n;
  }
}
