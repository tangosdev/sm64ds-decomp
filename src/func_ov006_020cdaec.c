struct Vec3 { int x, y, z; };

extern int data_ov006_02140808;
extern int data_ov006_02140810;
extern struct Vec3 data_ov006_02140860;
extern struct Vec3 data_ov006_02140890;
extern void* data_ov006_0214095c;
extern void* data_ov006_02140878;
extern void func_020072c0(void);

extern void func_020731dc(int a, int b, void** node);
extern void func_0203cc28(int* p, int angle);
extern void func_0203ce80(struct Vec3* dst, struct Vec3* src);
extern void func_0203cf00(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern void Quaternion_FromVector3(int* q, struct Vec3* v, struct Vec3* axis);
extern void Quaternion_Normalize(int* q);
extern void func_ov006_020cdc8c(void* o);

void func_ov006_020cdaec(char* arg) {
  struct Vec3 a;
  struct Vec3 b;
  struct Vec3 c;
  if ((data_ov006_02140808 & 1) == 0) {
    data_ov006_02140860.x = 0;
    data_ov006_02140860.y = 0x1000;
    data_ov006_02140860.z = 0;
    func_020731dc((int)&data_ov006_02140860, (int)func_020072c0, &data_ov006_0214095c);
    data_ov006_02140808 |= 1;
  }
  if ((data_ov006_02140810 & 1) == 0) {
    data_ov006_02140890.x = 0;
    data_ov006_02140890.y = 0;
    data_ov006_02140890.z = 0x1000;
    func_020731dc((int)&data_ov006_02140890, (int)func_020072c0, &data_ov006_02140878);
    data_ov006_02140810 |= 1;
  }
  func_0203cc28((int*)(arg + 0x38), 0x100);
  func_0203ce80(&a, (struct Vec3*)(arg + 0x38));
  func_0203cf00(&b, (struct Vec3*)(arg + 0x38), &data_ov006_02140890);
  *(int*)(arg + 0x44) = b.x;
  *(int*)(arg + 0x48) = b.y;
  *(int*)(arg + 0x4c) = b.z;
  func_0203ce80(&c, (struct Vec3*)(arg + 0x44));
  Quaternion_FromVector3((int*)(arg + 0x74), &data_ov006_02140860, (struct Vec3*)(arg + 0x38));
  Quaternion_Normalize((int*)(arg + 0x74));
  func_ov006_020cdc8c(arg);
}
