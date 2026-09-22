#!/usr/bin/env python3
import pathlib, subprocess, sys
REPO = pathlib.Path(__file__).resolve().parent.parent
P = REPO / "src" / "_sweep9.c"
FC = "-O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc"
B = """//cpp
typedef int Fix12i;
extern "C" void func_ov002_020bfa74(void);
extern "C" void func_ov002_020c0108(void*, int);
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(void*, void*);
extern "C" void func_ov002_020ce798(void*);
extern "C" void func_ov002_020bf36c(char *self, void *arg) {
%s
}"""

def t(n, body):
    P.write_text(B % body, encoding="utf-8")
    cmd = [sys.executable, str(REPO/"tools/match.py"), "--c", str(P), "--func",
           "func_ov002_020bf36c", "--addr", "0x020bf36c", "--size", "0xa0",
           "--version", "1.2/sp2p3", "--flags", FC, "--brief",
           "--bin", str(REPO/"extracted/dsd/arm9_overlays/ov002.bin"), "--base", "0x020ad660"]
    o = subprocess.run(cmd, capture_output=True, text=True, cwd=str(REPO)).stdout
    r = "COMPILE_FAIL" if "compile failed" in o else "?"
    if "MATCHING VERSIONS: 1.2/sp2p3" in o: r = "MATCH"
    else:
        for ln in o.splitlines():
            if "sp2p3:" in ln: r = ln.strip(); break
    print(f"{n}: {r}")

for n,b in {
"volatile_ptr": """unsigned char r2; Fix12i saved;
{ volatile unsigned char *vp = (volatile unsigned char *)(self + 0x709); r2 = *vp; }
if (!r2) func_ov002_020bfa74();
if ((*(unsigned char *)(self + 0x6e9) & 1) || *(unsigned char *)(self + 0x706)) func_ov002_020c0108(self, 1);
if (*(int *)(self + 0x37c)) return;
saved = *(Fix12i *)(self + 0x98);
if ((*(unsigned char *)(self + 0x6e9) & 1)) *(Fix12i *)(self + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)(self + 0x558) + 0x800) >> 12);
_ZN5Actor9UpdatePosEP12CylinderClsn(self, arg); *(Fix12i *)(self + 0x98) = saved; func_ov002_020ce798(self);""",
"short_load": """short r2s; unsigned char r2; Fix12i saved;
r2s = *(short *)(self + 0x708); r2 = (unsigned char)(r2s >> 8);
if (!r2) func_ov002_020bfa74();
if ((*(unsigned char *)(self + 0x6e9) & 1) || *(unsigned char *)(self + 0x706)) func_ov002_020c0108(self, 1);
if (*(int *)(self + 0x37c)) return;
saved = *(Fix12i *)(self + 0x98);
if ((*(unsigned char *)(self + 0x6e9) & 1)) *(Fix12i *)(self + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)(self + 0x558) + 0x800) >> 12);
_ZN5Actor9UpdatePosEP12CylinderClsn(self, arg); *(Fix12i *)(self + 0x98) = saved; func_ov002_020ce798(self);""",
"pre_r5": """char *r6; void *r5; unsigned char r2; Fix12i saved;
r6 = self; r2 = *(unsigned char *)(r6 + 0x709); r5 = arg;
if (!r2) func_ov002_020bfa74();
if ((*(unsigned char *)(r6 + 0x6e9) & 1) || *(unsigned char *)(r6 + 0x706)) func_ov002_020c0108(r6, 1);
if (*(int *)(r6 + 0x37c)) return;
saved = *(Fix12i *)(r6 + 0x98);
if ((*(unsigned char *)(r6 + 0x6e9) & 1)) *(Fix12i *)(r6 + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)(r6 + 0x558) + 0x800) >> 12);
_ZN5Actor9UpdatePosEP12CylinderClsn(r6, r5); *(Fix12i *)(r6 + 0x98) = saved; func_ov002_020ce798(r6);""",
"asm_ldrb_only": """unsigned char r2; Fix12i saved;
asm { ldrb r2, [self, #0x709] }
if (!r2) func_ov002_020bfa74();
if ((*(unsigned char *)(self + 0x6e9) & 1) || *(unsigned char *)(self + 0x706)) func_ov002_020c0108(self, 1);
if (*(int *)(self + 0x37c)) return;
saved = *(Fix12i *)(self + 0x98);
if ((*(unsigned char *)(self + 0x6e9) & 1)) *(Fix12i *)(self + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)(self + 0x558) + 0x800) >> 12);
_ZN5Actor9UpdatePosEP12CylinderClsn(self, arg); *(Fix12i *)(self + 0x98) = saved; func_ov002_020ce798(self);""",
}.items():
    t(n,b)