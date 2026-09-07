#!/usr/bin/env python3
"""Asm-hybrid + access-shape sweep for batch16 regperm floors."""
import pathlib, subprocess, sys

REPO = pathlib.Path(__file__).resolve().parent.parent
P = REPO / "src" / "_sweep6.c"
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
bc = {
    "asm_lr_order": """extern int func_ov007_020c9214();
void func_ov007_020bc3dc(void *c, int i, int a2) {
    unsigned short **arr;
    unsigned short *p;
    unsigned char b;
    int lr;
    arr = *(unsigned short ***)((char *)c + 0x14);
    p = arr[i];
    if (p == 0) return;
    b = *(unsigned char *)((char *)p + 4);
    asm {
        cmp b, #0
        moveq lr, #0
        movne lr, #1
    }
    func_ov007_020c9214(*(void **)((char *)c + 0x18), i, lr, *(unsigned short *)((char *)p + 2), a2, 0x1000);
}""",
    "asm_lr_r3": """extern int func_ov007_020c9214();
void func_ov007_020bc3dc(void *c, int i, int a2) {
    unsigned short **arr;
    unsigned short *p;
    unsigned char b;
    int lr;
    arr = *(unsigned short ***)((char *)c + 0x14);
    p = arr[i];
    if (p == 0) return;
    b = *(unsigned char *)((char *)p + 4);
    asm {
        mov r3, b
        cmp r3, #0
        moveq lr, #0
        movne lr, #1
    }
    func_ov007_020c9214(*(void **)((char *)c + 0x18), i, lr, *(unsigned short *)((char *)p + 2), a2, 0x1000);
}""",
    "byte_to_lr": """extern int func_ov007_020c9214();
void func_ov007_020bc3dc(void *c, int i, int a2) {
    unsigned short **arr;
    unsigned short *p;
    unsigned char b;
    int lr;
    arr = *(unsigned short ***)((char *)c + 0x14);
    p = arr[i];
    if (p == 0) return;
    b = *(unsigned char *)((char *)p + 4);
    lr = 1;
    if (b == 0) lr = 0;
    func_ov007_020c9214(*(void **)((char *)c + 0x18), i, lr, *(unsigned short *)((char *)p + 2), a2, 0x1000);
}""",
}
for n, s in bc.items():
    if t(n, s, "func_ov007_020bc3dc", "0x020bc3dc", "0x58",
           "extracted/dsd/arm9_overlays/ov007.bin", "0x020ad660", F):
        break

print("e3dc")
e3 = {
    "asm_ip_zero": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
struct Ctx { char *buf; unsigned int len; int z; };
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    struct Ctx ctx;
    unsigned int r;
    ctx.buf = buf;
    ctx.len = len;
    asm { mov ip, #0; str ip, [sp, #8] }
    r = func_0206e4a4(func_0206e450, &ctx);
    if (buf == 0) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len != 0) { char *end = buf + len; end[-1] = 0; }
    return r;
}""",
    "scalar_stack": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    char *b;
    unsigned int l, z;
    unsigned int r;
    b = buf;
    l = len;
    asm { mov ip, #0; mov z, ip }
    r = func_0206e4a4(func_0206e450, (void *)0);
    asm {
        add r1, sp, #0
        str b, [sp]
        str l, [sp, #4]
        mov ip, #0
        str ip, [sp, #8]
    }
    r = func_0206e4a4(func_0206e450, (void *)0);
    if (buf == 0) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len != 0) { char *end = buf + len; end[-1] = 0; }
    return r;
}""",
    "idx_z": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
struct Ctx { char *buf; unsigned int len; int z[1]; };
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    struct Ctx ctx;
    unsigned int r;
    ctx.buf = buf;
    ctx.len = len;
    ctx.z[0] = 0;
    r = func_0206e4a4(func_0206e450, &ctx);
    if (buf == 0) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len != 0) { char *end = buf + len; end[-1] = 0; }
    return r;
}""",
    "char_z": """extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void *ctx);
struct Ctx { char *buf; unsigned int len; char z; char pad[3]; };
unsigned int func_0206e3dc(char *buf, unsigned int len) {
    struct Ctx ctx;
    unsigned int r;
    ctx.buf = buf;
    ctx.len = len;
    ctx.z = 0;
    r = func_0206e4a4(func_0206e450, &ctx);
    if (buf == 0) return r;
    if (r < len) { buf[r] = 0; return r; }
    if (len != 0) { char *end = buf + len; end[-1] = 0; }
    return r;
}""",
}
for n, s in e3.items():
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
bf = {
    "asm_ldrb_r2": B % """unsigned char r2;
Fix12i saved;
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
    "index_load": B % """unsigned char r2;
Fix12i saved;
r2 = ((unsigned char *)self)[0x709];
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
    "late_r5": B % """char *r6;
void *r5;
unsigned char r2;
Fix12i saved;
r6 = self;
r2 = *(unsigned char *)(r6 + 0x709);
if (r2 == 0) func_ov002_020bfa74();
r5 = arg;
if ((*(unsigned char *)(r6 + 0x6e9) & 1) || *(unsigned char *)(r6 + 0x706))
    func_ov002_020c0108(r6, 1);
if (*(int *)(r6 + 0x37c)) return;
saved = *(Fix12i *)(r6 + 0x98);
if ((*(unsigned char *)(r6 + 0x6e9) & 1))
    *(Fix12i *)(r6 + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)(r6 + 0x558) + 0x800) >> 12);
_ZN5Actor9UpdatePosEP12CylinderClsn(r6, r5);
*(Fix12i *)(r6 + 0x98) = saved;
func_ov002_020ce798(r6);""",
    "plain_c": """typedef int Fix12i;
void func_ov002_020bfa74(void);
void func_ov002_020c0108(void*, int);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void*, void*);
void func_ov002_020ce798(void*);
void func_ov002_020bf36c(char *self, void *arg) {
    unsigned char r2;
    Fix12i saved;
    r2 = *(unsigned char *)(self + 0x709);
    if (r2 == 0) func_ov002_020bfa74();
    if ((*(unsigned char *)(self + 0x6e9) & 1) || *(unsigned char *)(self + 0x706))
        func_ov002_020c0108(self, 1);
    if (*(int *)(self + 0x37c)) return;
    saved = *(Fix12i *)(self + 0x98);
    if ((*(unsigned char *)(self + 0x6e9) & 1))
        *(Fix12i *)(self + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)(self + 0x558) + 0x800) >> 12);
    _ZN5Actor9UpdatePosEP12CylinderClsn(self, arg);
    *(Fix12i *)(self + 0x98) = saved;
    func_ov002_020ce798(self);
}""",
}
for n, s in bf.items():
    fl = F if n == "plain_c" else FC
    if t(n, s, "func_ov002_020bf36c", "0x020bf36c", "0xa0",
           "extracted/dsd/arm9_overlays/ov002.bin", "0x020ad660", fl):
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
ds = {
    "asm_ldr_r2": D % """char *r7;
void *alloc;
r7 = (char *)c;
_ZN5Model23AddToCommonModelDataArrER8BMD_File(file);
alloc = _ZN6Memory13operator_new2Ej(func_02046564(file));
*(void **)(r7 + 0x4c) = alloc;
asm { ldr r2, [r7, #0x4c]; cmp r2, #0 }
if (alloc == 0) return 0;
func_020462d0(r7 + 8, file);
_ZN15ModelComponents21UpdateVertsUsingBonesEv(r7 + 8);
if (a) func_02016b24(c, 0x8000);
if (b < 0) return 1;
_ZN5Model12SetPolygonIDEi(c, b & 0xff);
return 1;""",
    "slot_ptr": D % """char *r7;
void **slot;
void *alloc;
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
    "alloc_then_cmp": D % """void *alloc;
_ZN5Model23AddToCommonModelDataArrER8BMD_File(file);
alloc = _ZN6Memory13operator_new2Ej(func_02046564(file));
*(void **)((char *)c + 0x4c) = alloc;
if (alloc == 0) return 0;
func_020462d0((char *)c + 8, file);
_ZN15ModelComponents21UpdateVertsUsingBonesEv((char *)c + 8);
if (a) func_02016b24(c, 0x8000);
if (b < 0) return 1;
_ZN5Model12SetPolygonIDEi(c, b & 0xff);
return 1;""",
}
for n, s in ds.items():
    if t(n, s, "_ZN5Model9DoSetFileEPcii", "0x02016bf8", "0xa0", None, None, FC):
        break