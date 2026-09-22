#!/usr/bin/env python3
import pathlib, subprocess, sys

REPO = pathlib.Path(__file__).resolve().parent.parent
P = REPO / "src" / "_sweep_ov062.c"
F = "-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc"
HDR = """extern int _ZNK9Animation12WillHitFrameEi(char *anim, int f);
extern int func_ov002_020db54c(int p, int a, int b, int s);
extern int func_02012694(int id, void *pos);
extern int _ZN9Animation8FinishedEv(char *anim);
extern int func_ov062_02116cd8(char *c, void *p);
extern int data_ov062_0211ded0[];
int func_ov062_021161a8(char *c) {
"""
TAIL = "}\n"


def t(name, body):
    P.write_text(HDR + body + TAIL, encoding="utf-8")
    cmd = [sys.executable, str(REPO / "tools/match.py"), "--c", str(P),
           "--func", "func_ov062_021161a8", "--addr", "0x021161a8", "--size", "0x90",
           "--version", "1.2/sp2p3", "--flags", F, "--brief",
           "--bin", str(REPO / "extracted/dsd/arm9_overlays/ov062.bin"), "--base", "0x02115ee0"]
    o = subprocess.run(cmd, capture_output=True, text=True, cwd=str(REPO)).stdout
    if "MATCHING VERSIONS: 1.2/sp2p3" in o:
        r = "MATCH"
    elif "compile failed" in o:
        r = "COMPILE_FAIL"
    elif "size differs" in o:
        r = [ln for ln in o.splitlines() if "size differs" in ln][0]
    else:
        r = "?"
        for ln in o.splitlines():
            if "sp2p3:" in ln:
                r = ln.strip()
                break
    print(f"{name}: {r}")
    return r == "MATCH"


variants = {
    "nearmiss_if1": """  int new_var; int *q;
  if (((*((int *)(c + 0x3f8))) != 0) && _ZNK9Animation12WillHitFrameEi(c + 0x350, 0x14)) {
    func_ov002_020db54c(*((int *)(c + 0x3f8)), 0x28000, 0x50000, *((short *)(c + 0x8e)));
    *((int *)(c + 0x3f8)) = 0;
    func_02012694(0x126, c + 0x74);
  }
  if (1) { new_var = _ZN9Animation8FinishedEv(c + 0x350); q = (int *)(c + 0x128); }
  if (new_var) { *q &= ~2; func_ov062_02116cd8(c, data_ov062_0211ded0); }
  return 1;""",
    "simple_if": """  int *q;
  if (((int *)(c + 0x3f8))[0] != 0 && _ZNK9Animation12WillHitFrameEi(c + 0x350, 0x14)) {
    func_ov002_020db54c(*(int *)(c + 0x3f8), 0x28000, 0x50000, *(short *)(c + 0x8e));
    *(int *)(c + 0x3f8) = 0;
    func_02012694(0x126, c + 0x74);
  }
  if (_ZN9Animation8FinishedEv(c + 0x350)) {
    q = (int *)(c + 0x128);
    *q &= ~2;
    func_ov062_02116cd8(c, data_ov062_0211ded0);
  }
  return 1;""",
    "early_ret": """  if (*(int *)(c + 0x3f8) != 0 && _ZNK9Animation12WillHitFrameEi(c + 0x350, 0x14)) {
    func_ov002_020db54c(*(int *)(c + 0x3f8), 0x28000, 0x50000, *(short *)(c + 0x8e));
    *(int *)(c + 0x3f8) = 0;
    func_02012694(0x126, c + 0x74);
  }
  if (!_ZN9Animation8FinishedEv(c + 0x350)) return 1;
  *(int *)(c + 0x128) &= ~2;
  func_ov062_02116cd8(c, data_ov062_0211ded0);
  return 1;""",
    "fin_first": """  int fin; int *q;
  if (*(int *)(c + 0x3f8) != 0 && _ZNK9Animation12WillHitFrameEi(c + 0x350, 0x14)) {
    func_ov002_020db54c(*(int *)(c + 0x3f8), 0x28000, 0x50000, *(short *)(c + 0x8e));
    *(int *)(c + 0x3f8) = 0;
    func_02012694(0x126, c + 0x74);
  }
  fin = _ZN9Animation8FinishedEv(c + 0x350);
  if (fin) {
    q = (int *)(c + 0x128);
    *q &= ~2;
    func_ov062_02116cd8(c, data_ov062_0211ded0);
  }
  return 1;""",
    "goto_skip": """  int fin; int *q;
  if (*(int *)(c + 0x3f8) != 0 && _ZNK9Animation12WillHitFrameEi(c + 0x350, 0x14)) {
    func_ov002_020db54c(*(int *)(c + 0x3f8), 0x28000, 0x50000, *(short *)(c + 0x8e));
    *(int *)(c + 0x3f8) = 0;
    func_02012694(0x126, c + 0x74);
  }
  fin = _ZN9Animation8FinishedEv(c + 0x350);
  if (!fin) goto out;
  q = (int *)(c + 0x128);
  *q &= ~2;
  func_ov062_02116cd8(c, data_ov062_0211ded0);
out:
  return 1;""",
    "q_before_fin": """  int fin; int *q;
  q = (int *)(c + 0x128);
  if (*(int *)(c + 0x3f8) != 0 && _ZNK9Animation12WillHitFrameEi(c + 0x350, 0x14)) {
    func_ov002_020db54c(*(int *)(c + 0x3f8), 0x28000, 0x50000, *(short *)(c + 0x8e));
    *(int *)(c + 0x3f8) = 0;
    func_02012694(0x126, c + 0x74);
  }
  fin = _ZN9Animation8FinishedEv(c + 0x350);
  if (fin) { *q &= ~2; func_ov062_02116cd8(c, data_ov062_0211ded0); }
  return 1;""",
    "deref_idx": """  if (*(int *)(c + 0x3f8) != 0 && _ZNK9Animation12WillHitFrameEi(c + 0x350, 0x14)) {
    func_ov002_020db54c(*(int *)(c + 0x3f8), 0x28000, 0x50000, *(short *)(c + 0x8e));
    *(int *)(c + 0x3f8) = 0;
    func_02012694(0x126, c + 0x74);
  }
  if (_ZN9Animation8FinishedEv(c + 0x350)) {
    ((int *)(c + 0x128))[0] &= ~2;
    func_ov062_02116cd8(c, data_ov062_0211ded0);
  }
  return 1;""",
}
for n, b in variants.items():
    if t(n, b):
        print("*** MATCH", n)
        break