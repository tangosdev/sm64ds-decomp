extern int AngleDiff(int a, int b);
int func_ov100_02145e74(char* r0, char* r1){
  int v, z, a;
  v = *(int*)(r0+0x80); if (v < 0) v = -v; if (v > 0x82000) goto fail;
  v = *(int*)(r0+0x84); if (v < 0) v = -v; if (v > 0x32000) goto fail;
  z = *(int*)(r0+0x88);
  v = (z < 0) ? -z : z; if (v > 0x78000) goto fail;
  if (z < 0) a = *(short*)(r0+0x8e);
  else a = *(short*)(r0+0x8e) + 0x8000;
  a = (short)a;
  if (AngleDiff(a, *(short*)(r1+0x8e)) < 0x2000) return 1;
fail:
  return 0;
}
