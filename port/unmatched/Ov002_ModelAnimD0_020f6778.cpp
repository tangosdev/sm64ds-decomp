/* HOST COPY of src/func_ov002_020f6778.cpp -- the cutscene ModelAnim's DELETING
 * DESTRUCTOR, and the THIRD instance of this class's one-slot dispatch shift.
 *
 * THE DIVERGENCE. Its last act before chaining to ~ModelAnim is to destroy the
 * TextureSequence it owns, and it does that through a hand-indexed vtable word:
 *
 *     p = *(void **)(c + 0x7c);
 *     if (p != 0) (*(VFN)((*(int **)p)[1]))(p);      /· ROM slot 1 ·/
 *
 * ROM slot 1 is the deleting destructor, the same Itanium shape as the model
 * tables one file over. MSVC folds the D1/D0 pair into a single slot 0, and the
 * port's own seat says so in its own words -- hal/model_dtor_seat.cpp writes
 *
 *     _ZTV15TextureSequence[0] = texseq_d0
 *         "ROM from:0x0208e7d8 -> 0x02015a00 (_ZTV15TextureSequence+4, slot 1)"
 *
 * and seats NOTHING at index 1, because there is no MSVC index for the ROM's
 * second destructor slot. src/_ZN15TextureSequenceC1Ev.c stores the array base
 * as the vptr, so index 1 is a plain null and this line called it.
 *
 * MEASURED, and the arithmetic is what named it. The run's quarantine line
 * reads
 *
 *     [quarantine] actor 300472B4 id 352 (CUTSCENE_OBJECT) faulted
 *                  (code c0000005 +ffc00000) -- FROZEN, frame continues
 *
 * and +ffc00000 is address 0 expressed as an RVA against this image's 0x00400000
 * base: 0 - 0x400000 = 0xffc00000. A call through a null function pointer, on
 * the object the cast trace names as the Lakitu, at the moment script 2 hands
 * over to script 3. There is exactly one null vtable slot reachable on that
 * path and this is it.
 *
 * THE CHANGE, both halves of it:
 *   - index 1 -> index 0, the folded deleting slot in MSVC numbering
 *   - the call type becomes __fastcall (self in ecx), because what is seated
 *     there is hal/model_dtor_seat.cpp's `texseq_d0`, declared
 *     `static void __fastcall texseq_d0(void *s, void *)`. The matched source's
 *     `typedef void (*VFN)(void *)` is the ARM shape and would have entered a
 *     thiscall thunk cdecl even once the index was right.
 *
 * Everything else is the matched source line for line. Src line commented out
 * in port/slice_intro.txt.
 */
#include "SharedFilePtr.h"

extern "C" {
extern int data_ov002_0210bcc4[];
extern int data_ov002_0210bce8[];
extern int func_020072c0[];
void _ZN9ModelAnimD2Ev(void*);
void __destroy_arr(void*, int, int, void*);
void _ZN6Memory16operator_delete2EPv(void*);

/* MSVC numbering and MSVC convention; see the header. */
typedef void(__fastcall *VFN)(void*, void*);

// PORT_HOST_ABI: MSVC folds the ROM's D1/D0 destructor pair into slot 0 and the seated deleting dtor is __fastcall (self in ecx)
void* func_ov002_020f6778(char* c){
  void* p;
  int i;
  *(int*)c = (int)data_ov002_0210bcc4;
  *(int*)(c+0x50) = (int)data_ov002_0210bce8;
  p = *(void**)(c+0x70);
  if (p != 0) ((SharedFilePtr *)(p))->Release();
  for (i = 0; i < *(unsigned char*)(c+0x80); i++) {
    p = (*(void***)(c+0x74))[i];
    if (p != 0) ((SharedFilePtr *)(p))->Release();
  }
  if (*(void**)(c+0x7c) != 0) {
    for (i = 0; i < *(unsigned char*)(c+0x81); i++) {
      p = (*(void***)(c+0x78))[i];
      if (p != 0) ((SharedFilePtr *)(p))->Release();
    }
    p = *(void**)(c+0x7c);
    if (p != 0) {
      (*(VFN)((*(int**)p)[0]))(p, 0);
    }
  }
  _ZN9ModelAnimD2Ev(c);
  __destroy_arr(c+0x64, 1, 0xc, (void*)func_020072c0);
  _ZN6Memory16operator_delete2EPv(c);
  return c;
}
}
