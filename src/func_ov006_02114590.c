/* func_ov006_02114590 at 0x02114590
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern void func_0203d6d0(int* o, int* a, int* b);
extern int func_0203d4d0(void* p, int* a, int* b);

int func_ov006_02114590(int a0, int* p, int* q0, int* q1, int* q2){
  int v0[2], v1[2], v2[2];
  int t0[2], t1[2], t2[2];
  long long d0, d1, d2;
  func_0203d6d0(t0, p, q0);
  v0[0] = t0[0]; v0[1] = t0[1];
  func_0203d6d0(t1, p, q1);
  v1[0] = t1[0]; v1[1] = t1[1];
  func_0203d6d0(t2, p, q2);
  v2[0] = t2[0]; v2[1] = t2[1];
  d0 = func_0203d4d0(p, v0, v1);
  d1 = func_0203d4d0(p, v1, v2);
  d2 = func_0203d4d0(p, v2, v0);
  return (d0 >= 0 && d1 >= 0 && d2 >= 0) || (d0 <= 0 && d1 <= 0 && d2 <= 0);
}
