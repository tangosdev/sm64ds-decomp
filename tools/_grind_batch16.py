#!/usr/bin/env python3
"""Extended manual + permuter driver for batch16 floors."""
import json
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
PROBE = REPO / "src" / "_grind16.c"
FLAGS = "-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc"
FLAGS_CPP = FLAGS.replace("-lang c99", "-lang c++")


def match(func, addr, size, bin_path, base, flags):
    cmd = [
        sys.executable, str(REPO / "tools/match.py"), "--c", str(PROBE),
        "--func", func, "--addr", addr, "--size", size, "--version", "1.2/sp2p3",
        "--flags", flags, "--brief",
    ]
    if bin_path:
        cmd += ["--bin", str(REPO / bin_path), "--base", base]
    out = subprocess.run(cmd, capture_output=True, text=True, cwd=str(REPO)).stdout
    if "compile failed" in out:
        return "COMPILE_FAIL"
    if "MATCHING VERSIONS: 1.2/sp2p3" in out:
        return "MATCH"
    for line in out.splitlines():
        if "sp2p3:" in line or "size differs" in line:
            return line.strip()
    return "?"


def run(name, func, addr, size, bin_path, base, flags, variants):
    print(f"\n=== {name} ===")
    best = None
    for vname, src in variants.items():
        PROBE.write_text(src.strip() + "\n", encoding="utf-8")
        res = match(func, addr, size, bin_path, base, flags)
        if res == "MATCH":
            print(f"  {vname}: MATCH ***")
            (REPO / "progress").mkdir(exist_ok=True)
            out = REPO / f"src/{func}.{'cpp' if flags == FLAGS_CPP else 'c'}"
            if name == "doset":
                out = REPO / "src/_ZN5Model9DoSetFileEPcii.cpp"
                src = "//cpp\nextern \"C\" {\n" + src.strip() + "\n}\n"
            elif name == "bf36c":
                out = REPO / "src/func_ov002_020bf36c.cpp"
                src = "//cpp\n" + src.strip() + "\n"
            out.write_text(src, encoding="utf-8")
            return vname
        print(f"  {vname}: {res}")
        if best is None or (res.endswith("differ") and "2 word" in res):
            best = (vname, res)
    return None


E3DC = {
    "ip_before_pool": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
struct Ctx { char *buf; unsigned int len; int z; };
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    struct Ctx ctx;
    unsigned int r;
    void (*fn)(void);
    fn = func_0206e450;
    asm { mov ip, #0 }
    ctx.buf = buf;
    ctx.len = len;
    ctx.z = 0;
    r = func_0206e4a4(fn, &ctx);
    if (buf == 0) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len != 0) { char *end = buf + len; end[-1] = 0; }
    return r;
}""",
    "memleft_style": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
struct Ctx { char *buf; unsigned int len; int z; };
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    struct Ctx ctx;
    unsigned int r;
    int z;
    char *b;
    b = buf;
    z = (int)(((long long)(int)b) & 0xFFFFFFFFFFFFFFFFLL);
    z = 0;
    ctx.buf = b;
    ctx.len = len;
    ctx.z = z;
    r = func_0206e4a4(func_0206e450, &ctx);
    if (buf == 0) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len != 0) { char *end = buf + len; end[-1] = 0; }
    return r;
}""",
    "manual_stack": """extern void func_0206e450(void);
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
    "z_last": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
struct Ctx { char *buf; unsigned int len; int z; };
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    struct Ctx ctx;
    unsigned int r;
    ctx.buf = buf;
    ctx.len = len;
    ctx.z = 0;
    asm { mov ip, #0; str ip, [sp, #8] }
    r = func_0206e4a4(func_0206e450, &ctx);
    if (buf == 0) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len != 0) { char *end = buf + len; end[-1] = 0; }
    return r;
}""",
}

BC3DC = {
    "asm_lr_tail": """extern int func_ov007_020c9214();
void func_ov007_020bc3dc(void *c, int i, int a2)
{
    unsigned short **arr;
    unsigned short *p;
    int flag;
    arr = *(unsigned short ***)((char *)c + 0x14);
    p = arr[i];
    if (p == 0) return;
    flag = (*(unsigned char *)((char *)p + 4) != 0);
    asm {
        cmp r3, #0
        moveq lr, #0
        movne lr, #1
        mov r2, lr
    }
    func_ov007_020c9214(*(void **)((char *)c + 0x18), i, flag, *(unsigned short *)((char *)p + 2), a2, 0x1000);
}""",
    "bool_branches": """typedef enum { FALSE, TRUE } Bool;
extern int func_ov007_020c9214();
void func_ov007_020bc3dc(void *c, int i, int a2)
{
    unsigned short **arr;
    unsigned short *p;
    Bool d;
    arr = *(unsigned short ***)((char *)c + 0x14);
    p = arr[i];
    if (p == 0) return;
    if (*(unsigned char *)((char *)p + 4) == 0) d = FALSE;
    else d = TRUE;
    func_ov007_020c9214(*(void **)((char *)c + 0x18), i, d, *(unsigned short *)((char *)p + 2), a2, 0x1000);
}""",
    "c99d8_style": """typedef enum { FALSE, TRUE } Bool;
extern int func_ov007_020c9214();
void func_ov007_020bc3dc(void *c, int i, int a2)
{
    unsigned short **arr;
    unsigned short *p;
    void *ip;
    Bool d;
    arr = *(unsigned short ***)((char *)c + 0x14);
    p = arr[i];
    if (p == 0) return;
    ip = p;
    if (*(unsigned char *)((char *)ip + 4) == 0) d = FALSE;
    else d = TRUE;
    func_ov007_020c9214(*(void **)((char *)c + 0x18), i, d, *(unsigned short *)((char *)ip + 2), a2, 0x1000);
}""",
}

BF36C_HEAD = """typedef int Fix12i;
void func_ov002_020bfa74(void);
void func_ov002_020c0108(void *self, int x);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *c);
void func_ov002_020ce798(void *self);
void func_ov002_020bf36c(char *self, void *arg)
{
%s
}"""

BF36C = {
    "r6_base": BF36C_HEAD % """    char *r6;
    unsigned char r2;
    Fix12i saved;
    r6 = self;
    r2 = *(unsigned char *)(r6 + 0x709);
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
    "load_before_prologue": BF36C_HEAD % """    unsigned char r2;
    Fix12i saved;
    asm { ldrb r2, [r0, #0x709] }
    if (*(unsigned char *)(self + 0x709) == 0) func_ov002_020bfa74();
    if ((*(unsigned char *)(self + 0x6e9) & 1) != 0 || *(unsigned char *)(self + 0x706) != 0)
        func_ov002_020c0108(self, 1);
    if (*(int *)(self + 0x37c) != 0) return;
    saved = *(Fix12i *)(self + 0x98);
    if ((*(unsigned char *)(self + 0x6e9) & 1) != 0)
        *(Fix12i *)(self + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)(self + 0x558) + 0x800) >> 12);
    _ZN5Actor9UpdatePosEP12CylinderClsn(self, arg);
    *(Fix12i *)(self + 0x98) = saved;
    func_ov002_020ce798(self);""",
    "e73ac_goto": """typedef int Fix12i;
void func_ov002_020bfa74(void);
void func_ov002_020c0108(void *self, int x);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *c);
void func_ov002_020ce798(void *self);
void func_ov002_020bf36c(char *self, void *arg)
{
    int a, b, c;
    unsigned char r2;
    Fix12i saved;
    r2 = *(unsigned char *)(self + 0x709);
    if (r2 == 0) goto call_bfa74;
    if ((*(unsigned char *)(self + 0x6e9) & 1) != 0 || *(unsigned char *)(self + 0x706) != 0)
        func_ov002_020c0108(self, 1);
    if (*(int *)(self + 0x37c) != 0) return;
    saved = *(Fix12i *)(self + 0x98);
    if ((*(unsigned char *)(self + 0x6e9) & 1) != 0)
        *(Fix12i *)(self + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)(self + 0x558) + 0x800) >> 12);
    _ZN5Actor9UpdatePosEP12CylinderClsn(self, arg);
    *(Fix12i *)(self + 0x98) = saved;
    func_ov002_020ce798(self);
    return;
call_bfa74:
    func_ov002_020bfa74();
    if ((*(unsigned char *)(self + 0x6e9) & 1) != 0 || *(unsigned char *)(self + 0x706) != 0)
        func_ov002_020c0108(self, 1);
    if (*(int *)(self + 0x37c) != 0) return;
    saved = *(Fix12i *)(self + 0x98);
    if ((*(unsigned char *)(self + 0x6e9) & 1) != 0)
        *(Fix12i *)(self + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)(self + 0x558) + 0x800) >> 12);
    _ZN5Actor9UpdatePosEP12CylinderClsn(self, arg);
    *(Fix12i *)(self + 0x98) = saved;
    func_ov002_020ce798(self);
}""",
}

DOSET_HEAD = """int _ZN5Model23AddToCommonModelDataArrER8BMD_File(void *);
unsigned int func_02046564(void *);
void *_ZN6Memory13operator_new2Ej(unsigned int);
int func_020462d0(void *, void *);
int _ZN15ModelComponents21UpdateVertsUsingBonesEv(void *);
int func_02016b24(void *, int);
int _ZN5Model12SetPolygonIDEi(void *, int);
"""

DOSET = {
    "r7_base": DOSET_HEAD + """
int _ZN5Model9DoSetFileEPcii(void *c, void *file, int a, int b)
{
    char *r7;
    void *r2;
    r7 = (char *)c;
    _ZN5Model23AddToCommonModelDataArrER8BMD_File(file);
    r2 = _ZN6Memory13operator_new2Ej(func_02046564(file));
    *(void **)(r7 + 0x4c) = r2;
    if (*(void **)(r7 + 0x4c) == 0) return 0;
    func_020462d0(r7 + 8, file);
    _ZN15ModelComponents21UpdateVertsUsingBonesEv(r7 + 8);
    if (a != 0) func_02016b24(c, 0x8000);
    if (b < 0) return 1;
    _ZN5Model12SetPolygonIDEi(c, b & 0xff);
    return 1;
}""",
    "asm_reload": DOSET_HEAD + """
int _ZN5Model9DoSetFileEPcii(void *c, void *file, int a, int b)
{
    _ZN5Model23AddToCommonModelDataArrER8BMD_File(file);
    *(void **)((char *)c + 0x4c) = _ZN6Memory13operator_new2Ej(func_02046564(file));
    asm { ldr r2, [r7, #0x4c]; cmp r2, #0 }
    if (*(void **)((char *)c + 0x4c) == 0) return 0;
    func_020462d0((char *)c + 8, file);
    _ZN15ModelComponents21UpdateVertsUsingBonesEv((char *)c + 8);
    if (a != 0) func_02016b24(c, 0x8000);
    if (b < 0) return 1;
    _ZN5Model12SetPolygonIDEi(c, b & 0xff);
    return 1;
}""",
    "memleft_mask": DOSET_HEAD + """
int _ZN5Model9DoSetFileEPcii(void *c, void *file, int a, int b)
{
    void *alloc;
    void **slot;
    _ZN5Model23AddToCommonModelDataArrER8BMD_File(file);
    alloc = _ZN6Memory13operator_new2Ej(func_02046564(file));
    slot = (void **)(((long long)(int)((char *)c + 0x4c)) & 0xFFFFFFFFFFFFFFFFLL);
    *slot = alloc;
    if (*slot == 0) return 0;
    func_020462d0((char *)c + 8, file);
    _ZN15ModelComponents21UpdateVertsUsingBonesEv((char *)c + 8);
    if (a != 0) func_02016b24(c, 0x8000);
    if (b < 0) return 1;
    _ZN5Model12SetPolygonIDEi(c, b & 0xff);
    return 1;
}""",
}


if __name__ == "__main__":
    wins = []
    wins.append(run("e3dc", "func_0206e3dc", "0x0206e3dc", "0x74", None, None, FLAGS, E3DC))
    wins.append(run("bc3dc", "func_ov007_020bc3dc", "0x020bc3dc", "0x58",
                    "extracted/dsd/arm9_overlays/ov007.bin", "0x020ad660", FLAGS, BC3DC))
    wins.append(run("bf36c", "func_ov002_020bf36c", "0x020bf36c", "0xa0",
                    "extracted/dsd/arm9_overlays/ov002.bin", "0x020ad660", FLAGS, BF36C))
    wins.append(run("doset", "_ZN5Model9DoSetFileEPcii", "0x02016bf8", "0xa0", None, None, FLAGS, DOSET))
    print("\nWINS:", [w for w in wins if w])