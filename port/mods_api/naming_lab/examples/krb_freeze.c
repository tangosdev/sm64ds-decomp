/* naming-lab example: freeze every Goomba (daKrb_c) by skipping its Behavior.

     port daKrb_c::Behavior  thiscall
     public: virtual int __thiscall daKrb_c::Behavior(void)
     ROM ov084 0x0212b6ec _ZN7daKrb_c8BehaviorEv

   apply:  python port/mods_api/naming_lab/patch.py apply daKrb_c::Behavior port/mods_api/naming_lab/examples/krb_freeze.c
   undo:   python port/mods_api/naming_lab/patch.py undo daKrb_c::Behavior

   Watch a focused Goomba's fields in the dashboard: the position (0x5c..0x64)
   and everything else Behavior writes stop changing while this is applied and
   resume on undo. A field that still changes is written somewhere else (the
   actor base, the renderer, another actor): that split is the point.

   Behavior returns 1 on its normal path (src/game/actors/d_a_krb.cpp); 0 would
   tell the actor base the actor is gone. */
#include <windows.h>

typedef int (__fastcall *orig_t)(void *self, void *edx);
static orig_t orig;
static void *(__cdecl *sym)(const char *);
static unsigned int calls;

__declspec(dllexport) void __cdecl lab_patch_init(void *(__cdecl *s)(const char *), void *o)
{
    sym = s;
    orig = (orig_t)o;
}

__declspec(dllexport) int __fastcall lab_patch(void *self, void *edx)
{
    ++calls;
    (void)orig;     /* call orig(self, edx) to run the real body */
    return 1;
}
