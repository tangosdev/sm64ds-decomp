//cpp
extern "C" {
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern int func_ov002_020eeca8(void*, void*);
extern int func_ov002_020e28d4(void*, int, int);
extern int _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern int func_ov002_020e2664(void*);
extern u32 _ZNK6Player14GetBodyModelIDEjb(void*, u32, int);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32, void*);
extern int func_ov002_020bedd4(void*);

extern char data_ov002_02110424[];
extern u8 data_020a0e40;
extern u8 data_0209f4ab[];
extern u16 data_0209f49e[];
extern int data_ov002_0211073c[];
}

struct Player {
    int St_Jump_Main();
};

int Player::St_Jump_Main()
{
    void* c = (void*)this;
func_ov002_020eeca8((char*)c + 0x380, c);
  func_ov002_020e28d4(c, 0xd00, 0x800);

  if (*(u8*)((char*)c + 0x6de) == 0) {
    if (*(u16*)((char*)c + 0x6a4) != 0) {
      u16* q = (u16*)(((long long)(int)((char*)c + 0x6ce)) & 0xFFFFFFFFFFFFFFFFLL);
      *q |= 0x100;
    }
    _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_02110424);
  } else {
    {
      int off = data_020a0e40 * 0x18;
      u8* p = (u8*)((char*)data_0209f4ab + off);
      if (*p != 0) {
        *p = 5;
      }
      if (*(u16*)((char*)data_0209f49e + off) & 2) {
        *(u16*)((char*)c + 0x6a4) = 5;
      }
    }

    if (func_ov002_020e2664(c)) return 1;

    if (*(u8*)((char*)c + 0x6e1) == 2) {
      *(s32*)((char*)c + 0x9c) = -0x4000;
      *(s32*)((char*)c + 0xa0) = -0x4b000;
      if (*(s32*)((char*)c + 0xa8) >= 0) {
        *(s32*)((char*)c + 0x9c) = -0x3400;
      }

      {
        u32 id = _ZNK6Player14GetBodyModelIDEjb(c, *(u32*)((char*)c + 8) & 0xff, 0);
        void* anim = *(void**)((char*)c + (id << 2) + 0xdc);
        u32 w = *(u32*)((char*)(((long long)(int)((char*)anim + 0x50)) & 0xFFFFFFFFFFFFFFFFLL) + 8);
        u16 t = (u16)(w >> 12);
        if (t == 4 || t == 0x18 || t == 0x2c) {
          _ZN5Sound9PlayBank0EjRK7Vector3(0xf, (char*)c + 0x74);
        }
      }
    } else {
      int idx = *(int*)((char*)c + 8);
      if (*(u8*)((char*)c + 0x703) != 0) {
        idx = 0;
      }
      int* row = &data_ov002_0211073c[idx * 2];
      int v = row[1];
      void* p2 = (char*)c + (v >> 1);
      int (*f)(void*);
      if (v & 1) {
        f = *(int (**)(void*))((char*)(*(int**)p2) + row[0]);
      } else {
        f = (int (*)(void*))row[0];
      }
      f(p2);
    }
  }

  func_ov002_020bedd4(c);
  return 1;

}
