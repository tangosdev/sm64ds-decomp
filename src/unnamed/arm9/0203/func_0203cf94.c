/* The squared distance between two Vector3s, in 64 bits and shifted down
   by 12 (fx32). dCamera_c compares it against 0x40000000. */
long long func_0203cf94(int* a, int* b){
  long long acc = (long long)(a[0] - b[0]) * (a[0] - b[0]);
  acc += (long long)(a[1] - b[1]) * (a[1] - b[1]);
  acc += (long long)(a[2] - b[2]) * (a[2] - b[2]);
  return acc >> 12;
}
