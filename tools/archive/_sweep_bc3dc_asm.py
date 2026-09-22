#!/usr/bin/env python3
import pathlib, subprocess, sys
REPO = pathlib.Path(__file__).resolve().parent.parent
P = REPO / "src" / "_sweep_bc3dc.c"
F = "-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc"

def t(name, body):
    P.write_text(body, encoding="utf-8")
    cmd = [sys.executable, str(REPO/"tools/match.py"), "--c", str(P), "--func",
           "func_ov007_020bc3dc", "--addr", "0x020bc3dc", "--size", "0x58",
           "--version", "1.2/sp2p3", "--flags", F, "--brief",
           "--bin", str(REPO/"extracted/dsd/arm9_overlays/ov007.bin"), "--base", "0x020ad660"]
    o = subprocess.run(cmd, capture_output=True, text=True, cwd=str(REPO)).stdout
    r = "COMPILE_FAIL" if "compile failed" in o else "?"
    if "MATCHING VERSIONS: 1.2/sp2p3" in o: r = "MATCH"
    else:
        for ln in o.splitlines():
            if "sp2p3:" in ln: r = ln.strip(); break
    print(f"{name}: {r}")

HDR = "extern int func_ov007_020c9214();\nvoid func_ov007_020bc3dc(void *c, int i, int a2) {\n"
TAIL = "}\n"

for n, mid in {
"asm_ip_lr": """  unsigned short **arr; unsigned short *p; int lr;
  arr = *(unsigned short ***)((char *)c + 0x14); p = arr[i]; if (!p) return;
  asm { ldrb r3, [ip, #4]; cmp r3, #0; moveq lr, #0; movne lr, #1; }
  func_ov007_020c9214(*(void **)((char *)c + 0x18), i, lr, *(unsigned short *)((char *)p + 2), a2, 0x1000);
""",
"asm_p_lr": """  unsigned short **arr; unsigned short *p; int lr;
  arr = *(unsigned short ***)((char *)c + 0x14); p = arr[i]; if (!p) return;
  asm { ldrb r3, [p, #4]; cmp r3, #0; moveq lr, #0; movne lr, #1; }
  func_ov007_020c9214(*(void **)((char *)c + 0x18), i, lr, *(unsigned short *)((char *)p + 2), a2, 0x1000);
""",
"asm_tail": """  unsigned short **arr; unsigned short *p; int lr;
  arr = *(unsigned short ***)((char *)c + 0x14); p = arr[i]; if (!p) return;
  lr = (*(unsigned char *)((char *)p + 4) == 0) ? 0 : 1;
  asm { cmp lr, #0; moveq lr, #0; movne lr, #1; }
  func_ov007_020c9214(*(void **)((char *)c + 0x18), i, lr, *(unsigned short *)((char *)p + 2), a2, 0x1000);
""",
}.items():
    t(n, HDR + mid + TAIL)