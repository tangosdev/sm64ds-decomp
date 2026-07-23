typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
enum { false, true };
struct Vector3 { int x, y, z; };
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern s16 Vec3_HorzAngle(const struct Vector3* v0, const struct Vector3* v1);
extern int AngleDiff(int a, int b);
extern int _ZN6Player9StartTalkER9ActorBaseb(char* p, char* a, int b);
extern void func_ov002_020bbd5c(char* c, int i);

int func_ov002_020bb520(char* self){
  unsigned int id = *(unsigned int*)(self+0x344);
  if (id == 0) return 0;
  if ((*(int*)(self+0x340) & 0x8000000) == 0) return 0;
  if (*(u8*)(self+0x58e) == 0) return 0;
  {
    char* other = (char*)_ZN5Actor10FindWithIDEj(id);
    if (other == 0) goto fail;
    {
      int b = (int)(*(u16*)(other+0xc) == 0xbf);
      if (b != false) goto success;
    }
  fail:
    return 0;
  success:
    {
      int ang = Vec3_HorzAngle((struct Vector3*)(self+0x5c), (struct Vector3*)(other+0x5c));
      if (AngleDiff(ang, *(s16*)(self+0x8e)) > 0x4000) return 0;
      *(int*)(self+0x598) = (int)other;
      if (_ZN6Player9StartTalkER9ActorBaseb(other, self, 0) == 0) return 0;
      func_ov002_020bbd5c(self, 1);
      return 1;
    }
  }
}
