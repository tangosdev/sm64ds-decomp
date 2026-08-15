#!/usr/bin/env python3
"""Targeted follow-up sweep from sweep6 signals."""
import pathlib, subprocess, sys

REPO = pathlib.Path(__file__).resolve().parent.parent
P = REPO / "src" / "_sweep7.c"
F = "-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc"
FC = F.replace("-lang c99", "-lang c++")


def m(func, addr, size, bp, bb, flags):
    cmd = [sys.executable, str(REPO / "tools/match.py"), "--c", str(P), "--func", func,
           "--addr", addr, "--size", size, "--version", "1.2/sp2p3", "--flags", flags, "--brief"]
    if bp:
        cmd += ["--bin", str(REPO / bp), "--base", bb]
    o = subprocess.run(cmd, capture_output=True, text=True, cwd=str(REPO)).stdout
    if "MATCHING VERSIONS: 1.2/sp2p3" in o:
        return "MATCH"
    if "compile failed" in o:
        return "COMPILE_FAIL"
    for ln in o.splitlines():
        if "sp2p3:" in ln:
            return ln.strip()
    return "?"


def t(name, src, func, addr, size, bp, bb, flags):
    P.write_text(src.strip() + "\n", encoding="utf-8")
    r = m(func, addr, size, bp, bb, flags)
    print(f"  {name}: {r}")
    return r == "MATCH"


print("bc3dc")
for n, s in {
    "ternary": """extern int func_ov007_020c9214();
void func_ov007_020bc3dc(void *c, int i, int a2) {
    unsigned short **arr;
    unsigned short *p;
    int flag;
    arr = *(unsigned short ***)((char *)c + 0x14);
    p = arr[i];
    if (p == 0) return;
    flag = (*(unsigned char *)((char *)p + 4) != 0) ? 1 : 0;
    func_ov007_020c9214(*(void **)((char *)c + 0x18), i, flag,
        *(unsigned short *)((char *)p + 2), a2, 0x1000);
}""",
    "lr_ifelse": """extern int func_ov007_020c9214();
void func_ov007_020bc3dc(void *c, int i, int a2) {
    unsigned short **arr;
    unsigned short *p;
    int lr;
    arr = *(unsigned short ***)((char *)c + 0x14);
    p = arr[i];
    if (p == 0) return;
    if (*(unsigned char *)((char *)p + 4) == 0) lr = 0;
    else lr = 1;
    func_ov007_020c9214(*(void **)((char *)c + 0x18), i, lr,
        *(unsigned short *)((char *)p + 2), a2, 0x1000);
}""",
    "inv_cmp": """extern int func_ov007_020c9214();
void func_ov007_020bc3dc(void *c, int i, int a2) {
    unsigned short **arr;
    unsigned short *p;
    int lr;
    arr = *(unsigned short ***)((char *)c + 0x14);
    p = arr[i];
    if (p == 0) return;
    lr = 1;
    if (*(unsigned char *)((char *)p + 4) == 0) lr = 0;
    func_ov007_020c9214(*(void **)((char *)c + 0x18), i, lr,
        *(unsigned short *)((char *)p + 2), a2, 0x1000);
}""",
    "unsigned_flag": """extern int func_ov007_020c9214();
void func_ov007_020bc3dc(void *c, int i, int a2) {
    unsigned short **arr;
    unsigned short *p;
    unsigned int flag;
    arr = *(unsigned short ***)((char *)c + 0x14);
    p = arr[i];
    if (p == 0) return;
    flag = *(unsigned char *)((char *)p + 4);
    if (flag) flag = 1;
    func_ov007_020c9214(*(void **)((char *)c + 0x18), i, (int)flag,
        *(unsigned short *)((char *)p + 2), a2, 0x1000);
}""",
    "bool_byte": """extern int func_ov007_020c9214();
void func_ov007_020bc3dc(void *c, int i, int a2) {
    unsigned short **arr;
    unsigned short *p;
    unsigned char b;
    arr = *(unsigned short ***)((char *)c + 0x14);
    p = arr[i];
    if (p == 0) return;
    b = *(unsigned char *)((char *)p + 4);
    func_ov007_020c9214(*(void **)((char *)c + 0x18), i, b != 0,
        *(unsigned short *)((char *)p + 2), a2, 0x1000);
}""",
}.items():
    if t(n, s, "func_ov007_020bc3dc", "0x020bc3dc", "0x58",
           "extracted/dsd/arm9_overlays/ov007.bin", "0x020ad660", F):
        break

print("e3dc")
for n, s in {
    "reorder_fields": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
struct Ctx { int z; unsigned int len; char *buf; };
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    struct Ctx ctx; unsigned int r;
    ctx.z = 0; ctx.len = len; ctx.buf = buf;
    r = func_0206e4a4(func_0206e450, &ctx);
    if (!buf) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len) { char *e = buf + len; e[-1] = 0; }
    return r;
}""",
    "split_locals": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
struct Ctx { char *buf; unsigned int len; int z; };
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    char *b; unsigned int l; int z; struct Ctx ctx; unsigned int r;
    b = buf; l = len; z = 0;
    ctx.buf = b; ctx.len = l; ctx.z = z;
    r = func_0206e4a4(func_0206e450, &ctx);
    if (!buf) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len) { char *e = buf + len; e[-1] = 0; }
    return r;
}""",
    "u32_z": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
struct Ctx { char *buf; unsigned int len; unsigned int z; };
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    struct Ctx ctx; unsigned int r;
    ctx.buf = buf; ctx.len = len; ctx.z = 0;
    r = func_0206e4a4(func_0206e450, &ctx);
    if (!buf) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len) { char *e = buf + len; e[-1] = 0; }
    return r;
}""",
    "short_z": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
struct Ctx { char *buf; unsigned int len; short z; };
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    struct Ctx ctx; unsigned int r;
    ctx.buf = buf; ctx.len = len; ctx.z = 0;
    r = func_0206e4a4(func_0206e450, &ctx);
    if (!buf) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len) { char *e = buf + len; e[-1] = 0; }
    return r;
}""",
}.items():
    if t(n, s, "func_0206e3dc", "0x0206e3dc", "0x74", None, None, F):
        break

print("bf36c")
B = """//cpp
typedef int Fix12i;
extern "C" void func_ov002_020bfa74(void);
extern "C" void func_ov002_020c0108(void*, int);
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(void*, void*);
extern "C" void func_ov002_020ce798(void*);
extern "C" void func_ov002_020bf36c(char *self, void *arg) {
%s
}"""
for n, s in {
    "asm_ldrb": B % """unsigned char r2; Fix12i saved;
asm { ldrb r2, [self, #0x709] }
if (r2 == 0) func_ov002_020bfa74();
if ((*(unsigned char *)(self + 0x6e9) & 1) || *(unsigned char *)(self + 0x706))
    func_ov002_020c0108(self, 1);
if (*(int *)(self + 0x37c)) return;
saved = *(Fix12i *)(self + 0x98);
if ((*(unsigned char *)(self + 0x6e9) & 1))
    *(Fix12i *)(self + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)(self + 0x558) + 0x800) >> 12);
_ZN5Actor9UpdatePosEP12CylinderClsn(self, arg);
*(Fix12i *)(self + 0x98) = saved;
func_ov002_020ce798(self);""",
    "asm_ldrb_r6": B % """char *r6; unsigned char r2; Fix12i saved;
r6 = self;
asm { ldrb r2, [r6, #0x709] }
if (r2 == 0) func_ov002_020bfa74();
if ((*(unsigned char *)(r6 + 0x6e9) & 1) || *(unsigned char *)(r6 + 0x706))
    func_ov002_020c0108(r6, 1);
if (*(int *)(r6 + 0x37c)) return;
saved = *(Fix12i *)(r6 + 0x98);
if ((*(unsigned char *)(r6 + 0x6e9) & 1))
    *(Fix12i *)(r6 + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)(r6 + 0x558) + 0x800) >> 12);
_ZN5Actor9UpdatePosEP12CylinderClsn(r6, arg);
*(Fix12i *)(r6 + 0x98) = saved;
func_ov002_020ce798(r6);""",
    "u8_tmp": B % """unsigned char r2, tmp; Fix12i saved;
tmp = ((unsigned char *)self)[0x709];
r2 = tmp;
if (r2 == 0) func_ov002_020bfa74();
if ((*(unsigned char *)(self + 0x6e9) & 1) || *(unsigned char *)(self + 0x706))
    func_ov002_020c0108(self, 1);
if (*(int *)(self + 0x37c)) return;
saved = *(Fix12i *)(self + 0x98);
if ((*(unsigned char *)(self + 0x6e9) & 1))
    *(Fix12i *)(self + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)(self + 0x558) + 0x800) >> 12);
_ZN5Actor9UpdatePosEP12CylinderClsn(self, arg);
*(Fix12i *)(self + 0x98) = saved;
func_ov002_020ce798(self);""",
}.items():
    if t(n, s, "func_ov002_020bf36c", "0x020bf36c", "0xa0",
           "extracted/dsd/arm9_overlays/ov002.bin", "0x020ad660", FC):
        break

print("doset")
D = """//cpp
extern "C" {
int _ZN5Model23AddToCommonModelDataArrER8BMD_File(void*);
unsigned int func_02046564(void*);
void* _ZN6Memory13operator_new2Ej(unsigned int);
int func_020462d0(void*, void*);
int _ZN15ModelComponents21UpdateVertsUsingBonesEv(void*);
int func_02016b24(void*, int);
int _ZN5Model12SetPolygonIDEi(void*, int);
int _ZN5Model9DoSetFileEPcii(void *c, void *file, int a, int b) {
%s
}}"""
for n, s in {
    "slot_ptr": D % """char *r7; void **slot;
r7 = (char *)c;
_ZN5Model23AddToCommonModelDataArrER8BMD_File(file);
slot = (void **)(r7 + 0x4c);
*slot = _ZN6Memory13operator_new2Ej(func_02046564(file));
if (*slot == 0) return 0;
func_020462d0(r7 + 8, file);
_ZN15ModelComponents21UpdateVertsUsingBonesEv(r7 + 8);
if (a) func_02016b24(c, 0x8000);
if (b < 0) return 1;
_ZN5Model12SetPolygonIDEi(c, b & 0xff);
return 1;""",
    "r2_temp": D % """char *r7; void *r2; void *alloc;
r7 = (char *)c;
_ZN5Model23AddToCommonModelDataArrER8BMD_File(file);
alloc = _ZN6Memory13operator_new2Ej(func_02046564(file));
*(void **)(r7 + 0x4c) = alloc;
r2 = *(void **)(r7 + 0x4c);
if (r2 == 0) return 0;
func_020462d0(r7 + 8, file);
_ZN15ModelComponents21UpdateVertsUsingBonesEv(r7 + 8);
if (a) func_02016b24(c, 0x8000);
if (b < 0) return 1;
_ZN5Model12SetPolygonIDEi(c, b & 0xff);
return 1;""",
    "reload_short": D % """char *r7; void *alloc; short chk;
r7 = (char *)c;
_ZN5Model23AddToCommonModelDataArrER8BMD_File(file);
alloc = _ZN6Memory13operator_new2Ej(func_02046564(file));
*(void **)(r7 + 0x4c) = alloc;
chk = (short)(*(void **)(r7 + 0x4c) != 0);
if (chk == 0) return 0;
func_020462d0(r7 + 8, file);
_ZN15ModelComponents21UpdateVertsUsingBonesEv(r7 + 8);
if (a) func_02016b24(c, 0x8000);
if (b < 0) return 1;
_ZN5Model12SetPolygonIDEi(c, b & 0xff);
return 1;""",
}.items():
    if t(n, s, "_ZN5Model9DoSetFileEPcii", "0x02016bf8", "0xa0", None, None, FC):
        break