extern void func_ov007_020b8d48(void* a, void* b);
void func_ov007_020b8cfc(void* a, void* b, int n){
  int i = 0;
  if(n <= 0) return;
  do {
    func_ov007_020b8d48(a, b);
    i++;
  } while(i < n);
}
