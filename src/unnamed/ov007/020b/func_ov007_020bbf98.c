void func_ov007_020bbf98(int** o, unsigned int mask){
  int i = 0;
  int* base = *o;
  int n = ((int*)base[0])[9];
  int off;
  int* fld;
  if (n <= 0) return;
  off = i;
  do {
    fld = (int*)(((int)((char*)base[1] + off) + 0x24));
    i++;
    fld[0] = (fld[0] & ~0xf) | mask;
    off += 0x30;
  } while (i < n);
}
