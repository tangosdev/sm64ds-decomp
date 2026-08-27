extern void func_ov007_020c6174(void* a, int i, int b);

void func_ov007_020c62cc(char* a, int b){
  int i = 0;
  if (*(int*)(a+8) <= 0) return;
  do {
    func_ov007_020c6174(a, i, b);
    i++;
  } while (i < *(int*)(a+8));
}
