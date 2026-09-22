#!/usr/bin/env python3
"""bc3dc moveq/movne ordering sweep."""
import pathlib, subprocess, sys

REPO = pathlib.Path(__file__).resolve().parent.parent
P = REPO / "src" / "_sweep8.c"
F = "-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc"
BODY = """
    unsigned short **arr;
    unsigned short *p;
%s
    arr = *(unsigned short ***)((char *)c + 0x14);
    p = arr[i];
    if (p == 0) return;
%s
    func_ov007_020c9214(*(void **)((char *)c + 0x18), i, %s,
        *(unsigned short *)((char *)p + 2), a2, 0x1000);
"""
HDR = "extern int func_ov007_020c9214();\nvoid func_ov007_020bc3dc(void *c, int i, int a2) {"


def go(name, decl, pre, arg):
    src = HDR + BODY % (decl, pre, arg) + "}\n"
    P.write_text(src, encoding="utf-8")
    cmd = [sys.executable, str(REPO / "tools/match.py"), "--c", str(P), "--func",
           "func_ov007_020bc3dc", "--addr", "0x020bc3dc", "--size", "0x58",
           "--version", "1.2/sp2p3", "--flags", F, "--brief",
           "--bin", str(REPO / "extracted/dsd/arm9_overlays/ov007.bin"), "--base", "0x020ad660"]
    o = subprocess.run(cmd, capture_output=True, text=True, cwd=str(REPO)).stdout
    if "MATCHING VERSIONS: 1.2/sp2p3" in o:
        r = "MATCH"
    elif "compile failed" in o:
        r = "COMPILE_FAIL"
    else:
        r = "?"
        for ln in o.splitlines():
            if "sp2p3:" in ln:
                r = ln.strip()
                break
    print(f"{name}: {r}")
    return r == "MATCH"


variants = [
    ("eq0_tern", "    int lr;", "    lr = (*(unsigned char *)((char *)p + 4) == 0) ? 0 : 1;", "lr"),
    ("ne0_tern", "    int lr;", "    lr = (*(unsigned char *)((char *)p + 4) != 0) ? 1 : 0;", "lr"),
    ("eq0_revtern", "    int lr;", "    lr = (*(unsigned char *)((char *)p + 4) == 0) ? 0 : 1;", "lr"),
    ("ne_else_eq", "    int lr;", "    if (*(unsigned char *)((char *)p + 4) != 0) lr = 1; else lr = 0;", "lr"),
    ("eq_ifelse", "    int lr;", "    if (*(unsigned char *)((char *)p + 4) == 0) { lr = 0; } else { lr = 1; }", "lr"),
    ("ne_ifelse", "    int lr;", "    if (*(unsigned char *)((char *)p + 4) != 0) { lr = 1; } else { lr = 0; }", "lr"),
    ("byte_cast", "    unsigned char b; int lr;", "    b = *(unsigned char *)((char *)p + 4);\n    lr = (b == 0) ? 0 : 1;", "lr"),
    ("byte_ne", "    unsigned char b; int lr;", "    b = *(unsigned char *)((char *)p + 4);\n    lr = (b != 0) ? 1 : 0;", "lr"),
    ("short_lr", "    short lr;", "    lr = (*(unsigned char *)((char *)p + 4) == 0) ? 0 : 1;", "lr"),
    ("stack_lr", "    int lr;", "    lr = (*(unsigned char *)((char *)p + 4) == 0) ? 0 : 1;\n    { int t = lr; lr = t; }", "lr"),
    ("direct_ne", "", "    ", "*(unsigned char *)((char *)p + 4) != 0"),
    ("direct_eq", "", "    ", "*(unsigned char *)((char *)p + 4) == 0"),
    ("int_byte", "    int lr; unsigned char b;", "    b = *(unsigned char *)((char *)p + 4);\n    lr = b;\n    if (lr) lr = 1; else lr = 0;", "lr"),
    ("ands_trick", "    unsigned char b; int lr;", "    b = *(unsigned char *)((char *)p + 4);\n    lr = (b & 1) != 0;", "lr"),
    ("sub1", "    unsigned char b; int lr;", "    b = *(unsigned char *)((char *)p + 4);\n    lr = b - 1; lr = (lr >> 31) & 1; lr = 1 - lr;", "lr"),
]
for name, decl, pre, arg in variants:
    if go(name, decl, pre, arg):
        print("*** MATCH:", name)
        break