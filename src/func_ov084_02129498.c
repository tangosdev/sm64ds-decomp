extern void _ZN7fBase_c18MarkForDestructionEv(void*);
extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);
void func_ov084_02129498(char* r0) {
  if ((*(unsigned char*)(r0 + 0x113) & 0xf) < 6)
    _ZN7fBase_c18MarkForDestructionEv(r0);
  else
    _ZN8dActor_c24KillAndTrackInDeathTableEv(r0);
}
