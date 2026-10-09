int AngleDiff(int a, int b){
  short d = (short)(a - b);
  if (d < 0) return -d;
  return d;
}
