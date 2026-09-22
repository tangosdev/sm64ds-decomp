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
  int new_var; int *q;
"""
TAIL = "  return 1;\n}\n"
FIRST = """  if (*(int *)(c + 0x3f8) != 0 && _ZNK9Animation12WillHitFrameEi(c + 0x350, 0x14)) {
    func_ov002_020db54c(*(int *)(c + 0x3f8), 0x28000, 0x50000, *(short *)(c + 0x8e));
    *(int *)(c + 0x3f8) = 0;
    func_02012694(0x126, c + 0x74);
  }
"""


def t(name, mid):
    P.write_text(HDR + FIRST + mid + TAIL, encoding="utf-8")
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


for n, m in {
    "if1": "  if (1) { new_var = _ZN9Animation8FinishedEv(c + 0x350); q = (int *)(c + 0x128); }\n  if (new_var) { *q &= ~2; func_ov062_02116cd8(c, data_ov062_0211ded0); }\n",
    "dowhile0": "  do { new_var = _ZN9Animation8FinishedEv(c + 0x350); q = (int *)(c + 0x128); } while (0);\n  if (new_var) { *q &= ~2; func_ov062_02116cd8(c, data_ov062_0211ded0); }\n",
    "comma": "  new_var = _ZN9Animation8FinishedEv(c + 0x350), q = (int *)(c + 0x128);\n  if (new_var) { *q &= ~2; func_ov062_02116cd8(c, data_ov062_0211ded0); }\n",
    "fin_then_q": "  new_var = _ZN9Animation8FinishedEv(c + 0x350);\n  q = (int *)(c + 0x128);\n  if (new_var) { *q &= ~2; func_ov062_02116cd8(c, data_ov062_0211ded0); }\n",
    "if1_rev": "  if (1) { q = (int *)(c + 0x128); new_var = _ZN9Animation8FinishedEv(c + 0x350); }\n  if (new_var) { *q &= ~2; func_ov062_02116cd8(c, data_ov062_0211ded0); }\n",
    "if1_brace": "  if (1)\n  {\n    new_var = _ZN9Animation8FinishedEv(c + 0x350);\n    q = (int *)(c + 0x128);\n  }\n  if (new_var)\n  {\n    *q &= ~2;\n    func_ov062_02116cd8(c, data_ov062_0211ded0);\n  }\n",
    "volatile_fin": "  volatile int vf;\n  if (1) { vf = _ZN9Animation8FinishedEv(c + 0x350); new_var = vf; q = (int *)(c + 0x128); }\n  if (new_var) { *q &= ~2; func_ov062_02116cd8(c, data_ov062_0211ded0); }\n",
    "while0_assign": "  while (0) { new_var = 0; q = 0; }\n  new_var = _ZN9Animation8FinishedEv(c + 0x350);\n  q = (int *)(c + 0x128);\n  if (new_var) { *q &= ~2; func_ov062_02116cd8(c, data_ov062_0211ded0); }\n",
    "if0_else": "  if (0) { } else { new_var = _ZN9Animation8FinishedEv(c + 0x350); q = (int *)(c + 0x128); }\n  if (new_var) { *q &= ~2; func_ov062_02116cd8(c, data_ov062_0211ded0); }\n",
    "label_block": "  new_var = _ZN9Animation8FinishedEv(c + 0x350);\n  if (!new_var) goto skip;\n  q = (int *)(c + 0x128);\n  *q &= ~2;\n  func_ov062_02116cd8(c, data_ov062_0211ded0);\nskip:\n",
}.items():
    if t(n, m):
        print("*** MATCH", n)
        break