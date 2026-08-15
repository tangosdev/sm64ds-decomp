#!/usr/bin/env python3
"""Exhaustive access-form sweep for batch16 regperm floors."""
import json
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
PROBE = REPO / "src" / "_sweep4.c"
FLAGS = "-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc"
FLAGS_CPP = FLAGS.replace("-lang c99", "-lang c++")
WINS = REPO / "progress" / "batch16_sweep4_wins.jsonl"


def match(func, addr, size, bin_path, base, flags):
    cmd = [sys.executable, str(REPO / "tools/match.py"), "--c", str(PROBE),
           "--func", func, "--addr", addr, "--size", size, "--version", "1.2/sp2p3",
           "--flags", flags, "--brief"]
    if bin_path:
        cmd += ["--bin", str(REPO / bin_path), "--base", base]
    out = subprocess.run(cmd, capture_output=True, text=True, cwd=str(REPO)).stdout
    if "compile failed" in out:
        return "COMPILE_FAIL", out
    if "MATCHING VERSIONS: 1.2/sp2p3" in out:
        return "MATCH", out
    for line in out.splitlines():
        if "sp2p3:" in line:
            return line.strip(), out
        if "size differs" in line:
            return line.strip(), out
    return "?", out


def bank(name, src, out_path, wrap=None):
    text = src.strip() + "\n"
    if wrap:
        text = wrap(src.strip())
    out_path.write_text(text, encoding="utf-8")
    with WINS.open("a", encoding="utf-8") as f:
        f.write(json.dumps({"name": name, "file": str(out_path)}) + "\n")


def run_group(label, func, addr, size, bp, bb, flags, variants, out_path, wrap=None):
    print(f"\n=== {label} ({len(variants)} variants) ===")
    best = ("", 9999)
    for vname, src in variants.items():
        PROBE.write_text(src.strip() + "\n", encoding="utf-8")
        res, _ = match(func, addr, size, bp, bb, flags)
        if res == "MATCH":
            print(f"  {vname}: MATCH ***")
            bank(func, src, out_path, wrap)
            return True
        print(f"  {vname}: {res}")
        if res.endswith("differ"):
            try:
                n = int(res.split()[0].split("/")[-1] or res.split()[2])
            except Exception:
                n = 9999
            if "2 word" in res and best[1] > 2:
                best = (vname, 2)
            elif n < best[1]:
                best = (vname, n)
    if best[0]:
        print(f"  best: {best[0]} ({best[1]})")
    return False


E3DC = {
    "baseline": open(REPO / "src/func_0206e3dc.c", encoding="utf-8").read(),
    "manual_bytes": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    char spc[12];
    unsigned int r;
    *(char **)(spc + 0) = buf;
    *(unsigned int *)(spc + 4) = len;
    *(int *)(spc + 8) = 0;
    r = func_0206e4a4(func_0206e450, spc);
    if (buf == 0) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len != 0) { char *end = buf + len; end[-1] = 0; }
    return r;
}""",
    "z_first": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
struct Ctx { int z; char *buf; unsigned int len; };
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    struct Ctx ctx;
    unsigned int r;
    ctx.z = 0;
    ctx.buf = buf;
    ctx.len = len;
    r = func_0206e4a4(func_0206e450, &ctx);
    if (buf == 0) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len != 0) { char *end = buf + len; end[-1] = 0; }
    return r;
}""",
    "ptr_ctx": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
struct Ctx { char *buf; unsigned int len; int z; };
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    struct Ctx ctx;
    struct Ctx *ip;
    unsigned int r;
    ip = &ctx;
    ip->buf = buf;
    ip->len = len;
    ip->z = 0;
    r = func_0206e4a4(func_0206e450, ip);
    if (buf == 0) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len != 0) { char *end = buf + len; end[-1] = 0; }
    return r;
}""",
    "fn_ptr_first": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
struct Ctx { char *buf; unsigned int len; int z; };
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    struct Ctx ctx;
    unsigned int r;
    void (*fn)(void);
    fn = func_0206e450;
    ctx.buf = buf;
    ctx.len = len;
    ctx.z = 0;
    r = func_0206e4a4(fn, &ctx);
    if (buf == 0) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len != 0) { char *end = buf + len; end[-1] = 0; }
    return r;
}""",
    "split_z": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
struct Ctx { char *buf; unsigned int len; int z; };
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    struct Ctx ctx;
    unsigned int r;
    int z;
    ctx.buf = buf;
    ctx.len = len;
    z = 0;
    ctx.z = z;
    r = func_0206e4a4(func_0206e450, &ctx);
    if (buf == 0) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len != 0) { char *end = buf + len; end[-1] = 0; }
    return r;
}""",
}

BF36C_WRAP = lambda body: "//cpp\n" + body
BF36C_HEAD = """typedef int Fix12i;
extern "C" void func_ov002_020bfa74(void);
extern "C" void func_ov002_020c0108(void *self, int x);
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *c);
extern "C" void func_ov002_020ce798(void *self);
extern "C" void func_ov002_020bf36c(char *self, void *arg) {
%s
}"""

BF36C = {
    "baseline": open(REPO / "src/func_ov002_020bf36c.cpp", encoding="utf-8").read(),
    "actor_r4": """//cpp
class Actor;
""" + BF36C_HEAD % """    Actor *r4;
    unsigned char r2;
    Fix12i saved;
    r4 = (Actor *)self;
    r2 = ((unsigned char *)r4)[0x709];
    if (r2 == 0) func_ov002_020bfa74();
    if ((*(unsigned char *)((char *)r4 + 0x6e9) & 1) != 0 || *(unsigned char *)((char *)r4 + 0x706) != 0)
        func_ov002_020c0108((char *)r4, 1);
    if (*(int *)((char *)r4 + 0x37c) != 0) return;
    saved = *(Fix12i *)((char *)r4 + 0x98);
    if ((*(unsigned char *)((char *)r4 + 0x6e9) & 1) != 0)
        *(Fix12i *)((char *)r4 + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)((char *)r4 + 0x558) + 0x800) >> 12);
    _ZN5Actor9UpdatePosEP12CylinderClsn((char *)r4, arg);
    *(Fix12i *)((char *)r4 + 0x98) = saved;
    func_ov002_020ce798((char *)r4);""",
    "index_load": BF36C_HEAD % """    char *r6;
    unsigned char r2;
    Fix12i saved;
    r6 = self;
    r2 = ((unsigned char *)r6)[0x709];
    if (r2 == 0) func_ov002_020bfa74();
    if ((r6[0x6e9] & 1) != 0 || r6[0x706] != 0) func_ov002_020c0108(r6, 1);
    if (*(int *)(r6 + 0x37c) != 0) return;
    saved = *(Fix12i *)(r6 + 0x98);
    if ((r6[0x6e9] & 1) != 0)
        *(Fix12i *)(r6 + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)(r6 + 0x558) + 0x800) >> 12);
    _ZN5Actor9UpdatePosEP12CylinderClsn(r6, arg);
    *(Fix12i *)(r6 + 0x98) = saved;
    func_ov002_020ce798(r6);""",
    "us_load": BF36C_HEAD % """    char *r6;
    unsigned char r2;
    Fix12i saved;
    unsigned short us;
    r6 = self;
    us = *(unsigned short *)(r6 + 0x709);
    r2 = (unsigned char)us;
    if (r2 == 0) func_ov002_020bfa74();
    if ((*(unsigned char *)(r6 + 0x6e9) & 1) != 0 || *(unsigned char *)(r6 + 0x706) != 0)
        func_ov002_020c0108(r6, 1);
    if (*(int *)(r6 + 0x37c) != 0) return;
    saved = *(Fix12i *)(r6 + 0x98);
    if ((*(unsigned char *)(r6 + 0x6e9) & 1) != 0)
        *(Fix12i *)(r6 + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)(r6 + 0x558) + 0x800) >> 12);
    _ZN5Actor9UpdatePosEP12CylinderClsn(r6, arg);
    *(Fix12i *)(r6 + 0x98) = saved;
    func_ov002_020ce798(r6);""",
}

DOSET_WRAP = lambda body: '//cpp\nextern "C" {\n' + body + '\n}\n'
DOSET_HEAD = """int _ZN5Model23AddToCommonModelDataArrER8BMD_File(void *);
unsigned int func_02046564(void *);
void *_ZN6Memory13operator_new2Ej(unsigned int);
int func_020462d0(void *, void *);
int _ZN15ModelComponents21UpdateVertsUsingBonesEv(void *);
int func_02016b24(void *, int);
int _ZN5Model12SetPolygonIDEi(void *, int);
int _ZN5Model9DoSetFileEPcii(void *c, void *file, int a, int b) {
%s
}"""

DOSET = {
    "baseline": open(REPO / "src/_ZN5Model9DoSetFileEPcii.cpp", encoding="utf-8").read(),
    "slot_ptr": DOSET_HEAD % """    char *r7;
    void **slot;
    void *alloc;
    r7 = (char *)c;
    _ZN5Model23AddToCommonModelDataArrER8BMD_File(file);
    alloc = _ZN6Memory13operator_new2Ej(func_02046564(file));
    slot = (void **)(r7 + 0x4c);
    *slot = alloc;
    if (*slot == 0) return 0;
    func_020462d0(r7 + 8, file);
    _ZN15ModelComponents21UpdateVertsUsingBonesEv(r7 + 8);
    if (a != 0) func_02016b24(c, 0x8000);
    if (b < 0) return 1;
    _ZN5Model12SetPolygonIDEi(c, b & 0xff);
    return 1;""",
    "index_slot": DOSET_HEAD % """    char *r7;
    void *alloc;
    r7 = (char *)c;
    _ZN5Model23AddToCommonModelDataArrER8BMD_File(file);
    alloc = _ZN6Memory13operator_new2Ej(func_02046564(file));
    ((void **)r7)[0x13] = alloc;
    if (((void **)r7)[0x13] == 0) return 0;
    func_020462d0(r7 + 8, file);
    _ZN15ModelComponents21UpdateVertsUsingBonesEv(r7 + 8);
    if (a != 0) func_02016b24(c, 0x8000);
    if (b < 0) return 1;
    _ZN5Model12SetPolygonIDEi(c, b & 0xff);
    return 1;""",
    "double_deref": DOSET_HEAD % """    char *r7;
    void *alloc;
    void **slot;
    r7 = (char *)c;
    _ZN5Model23AddToCommonModelDataArrER8BMD_File(file);
    alloc = _ZN6Memory13operator_new2Ej(func_02046564(file));
    slot = (void **)(r7 + 0x4c);
    *slot = alloc;
    if (**(void ***)(&slot) == 0) return 0;
    func_020462d0(r7 + 8, file);
    _ZN15ModelComponents21UpdateVertsUsingBonesEv(r7 + 8);
    if (a != 0) func_02016b24(c, 0x8000);
    if (b < 0) return 1;
    _ZN5Model12SetPolygonIDEi(c, b & 0xff);
    return 1;""",
}

BC3DC = {
    "baseline": open(REPO / "src/func_ov007_020bc3dc.c", encoding="utf-8").read(),
    "ands_flag": """extern int func_ov007_020c9214();
void func_ov007_020bc3dc(void *c, int i, int a2) {
    unsigned short **arr;
    unsigned short *p;
    unsigned char b;
    int flag;
    arr = *(unsigned short ***)((char *)c + 0x14);
    p = arr[i];
    if (p == 0) return;
    b = *(unsigned char *)((char *)p + 4);
    flag = (int)(b & 1u);
    func_ov007_020c9214(*(void **)((char *)c + 0x18), i, flag, *(unsigned short *)((char *)p + 2), a2, 0x1000);
}""",
    "sub_flag": """extern int func_ov007_020c9214();
void func_ov007_020bc3dc(void *c, int i, int a2) {
    unsigned short **arr;
    unsigned short *p;
    int flag;
    arr = *(unsigned short ***)((char *)c + 0x14);
    p = arr[i];
    if (p == 0) return;
    flag = 1 - (*(unsigned char *)((char *)p + 4) == 0);
    func_ov007_020c9214(*(void **)((char *)c + 0x18), i, flag, *(unsigned short *)((char *)p + 2), a2, 0x1000);
}""",
    "lr_local": """extern int func_ov007_020c9214();
void func_ov007_020bc3dc(void *c, int i, int a2) {
    unsigned short **arr;
    unsigned short *p;
    int lr;
    arr = *(unsigned short ***)((char *)c + 0x14);
    p = arr[i];
    if (p == 0) return;
    if (*(unsigned char *)((char *)p + 4) == 0) lr = 0;
    else lr = 1;
    func_ov007_020c9214(*(void **)((char *)c + 0x18), i, lr, *(unsigned short *)((char *)p + 2), a2, 0x1000);
}""",
}

if __name__ == "__main__":
    ok = False
    ok |= run_group("e3dc", "func_0206e3dc", "0x0206e3dc", "0x74", None, None, FLAGS,
                    E3DC, REPO / "src/func_0206e3dc.c")
    ok |= run_group("bf36c", "func_ov002_020bf36c", "0x020bf36c", "0xa0",
                    "extracted/dsd/arm9_overlays/ov002.bin", "0x020ad660", FLAGS_CPP,
                    {k: (v if k == "baseline" else BF36C_WRAP(v)) for k, v in BF36C.items()},
                    REPO / "src/func_ov002_020bf36c.cpp", BF36C_WRAP)
    ok |= run_group("doset", "_ZN5Model9DoSetFileEPcii", "0x02016bf8", "0xa0", None, None, FLAGS_CPP,
                    {k: (v if k == "baseline" else DOSET_WRAP(v)) for k, v in DOSET.items()},
                    REPO / "src/_ZN5Model9DoSetFileEPcii.cpp",
                    lambda s: DOSET_WRAP(s) if not s.startswith("//cpp") else s + "\n")
    ok |= run_group("bc3dc", "func_ov007_020bc3dc", "0x020bc3dc", "0x58",
                    "extracted/dsd/arm9_overlays/ov007.bin", "0x020ad660", FLAGS,
                    BC3DC, REPO / "src/func_ov007_020bc3dc.c")
    print("\nANY MATCH:", ok)