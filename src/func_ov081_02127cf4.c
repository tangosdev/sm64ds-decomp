/* func_ov081_02127cf4 at 0x02127cf4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov081).
 */
struct Vector3 { int x, y, z; };
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const struct Vector3* pos);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN5Actor10PoofDustAtERK7Vector3(void* self, const struct Vector3* vec);
extern void func_ov081_02127be0(void* self);
extern void _ZN9ActorBase18MarkForDestructionEv(void* self);

void func_ov081_02127cf4(char* c) {
  struct Vector3 vec;
  struct Vector3 vec2;
  _ZN5Sound9PlayBank3EjRK7Vector3(0x41, (const struct Vector3*)(c + 0x74));
  int x = *(int*)(c + 0x5c);
  int y = *(int*)(c + 0x60) + 0x96000;
  int z = *(int*)(c + 0x64);
  vec.x = x;
  vec.y = y;
  vec.z = z;
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x74, vec.x, vec.y, vec.z);
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x75, vec.x, vec.y, vec.z);
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x76, vec.x, vec.y, vec.z);
  ((int*)&vec2)[0] = ((int*)&vec)[0];
  ((int*)&vec2)[1] = ((int*)&vec)[1];
  ((int*)&vec2)[2] = ((int*)&vec)[2];
  _ZN5Actor10PoofDustAtERK7Vector3(c, &vec2);
  func_ov081_02127be0(c);
  _ZN9ActorBase18MarkForDestructionEv(c);
}