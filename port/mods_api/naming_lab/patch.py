"""Hot-patch a function of the running port: write a replacement in C, compile it
to a DLL, and have the naming-lab plugin jump to it. Undo puts the original back.

    python patch.py find daKrb_c::Behavior          which port functions match
    python patch.py new  daKrb_c::Behavior -o p.c   write a skeleton that calls the original
    python patch.py apply daKrb_c::Behavior p.c     compile p.c, detour the function to it
    python patch.py undo daKrb_c::Behavior          (or: undo all)

A target is a qualified name (daKrb_c::Behavior), a ROM symbol (_ZN7daKrb_c8BehaviorEv,
func_ov084_02122fc8), a symtab name, or a port address (0x1033c2d0).

This is a command-line tool on purpose: compiling and loading code stays with the
person at the keyboard. The dashboard (lab_server.py) only reads and watches.

How it works. The traced build (build/port-trace) compiles every function with
/Gh, so each body opens with a 5-byte `call _penter`. The plugin's `detour`
command overwrites those 5 bytes with `jmp lab_patch` and hands the patch
`orig` = entry + 5, the untouched rest of the body, so a patch can run code
before and after the original or replace it. The DLL is loaded from a temp
copy, so you can edit, rebuild and apply again while the game runs.

What a patch shows you is how the PORT behaves. It can test a guess about what a
function or a field does; the name still needs the ROM (melonDS) to agree.
"""
import argparse
import os
import re
import socket
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import lab_server as L  # noqa: E402

PLUGIN = ('127.0.0.1', 47621)
VCVARS = os.path.join(os.environ.get('ProgramFiles(x86)', r'C:\Program Files (x86)'),
                      r'Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat')
OUT = os.path.join(L.REPO, 'build', 'naming_lab', 'patches')


def ask(cmd, prefix, timeout=2.0):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(0.25)
    s.sendto(cmd.encode(), PLUGIN)
    end = time.time() + timeout
    while time.time() < end:
        try:
            d = s.recvfrom(65535)[0].decode(errors='replace')
        except socket.timeout:
            continue
        except ConnectionResetError:
            sys.exit('no game: start naming-lab.cmd first')
        if d.startswith(prefix):
            return d
    sys.exit(f'no reply to "{cmd}" (is the game running with the naming-lab mod?)')


def symdb():
    return L.SymDB(os.path.join(L.REPO, 'build', 'port-trace', 'walk_window.exe'))


def matches(db, q):
    q = q.strip()
    if re.fullmatch(r'(0x)?[0-9a-fA-F]{8}', q):
        s = db.fn.get(int(q, 16))
        return [s] if s else []
    out = []
    for s in db.fn.values():
        names = [n.lstrip('_') for n in s.names]
        if q == s.qual or q in s.names or q.lstrip('_') in names or (s.rom and q == s.rom.get('name')):
            out.append(s)
    # the port's extern "C" shim and its thiscall body are the same ROM function;
    # patch the body, which every caller reaches
    if len(out) > 1 and any(s.thiscall for s in out):
        out = [s for s in out if s.thiscall]
    return out


def one(db, q):
    hits = matches(db, q)
    if len(hits) == 1:
        return hits[0]
    if not hits:
        sys.exit(f'nothing called {q!r}; try: python patch.py find {q.split("::")[-1]}')
    print(f'{q!r} is ambiguous; pick one by address:', file=sys.stderr)
    for s in hits:
        print(f'  0x{s.addr:08x}  {s.sig}', file=sys.stderr)
    sys.exit(2)


def rom_text(s):
    r = s.rom
    return f"ROM {r['module']} 0x{r['addr']:08x} {r['name']}" if r else 'no ROM address known'


def cmd_find(a):
    db = symdb()
    q = a.query
    hits = [s for s in db.fn.values() if q in s.qual or any(q in n for n in s.names)]
    for s in sorted(hits, key=lambda s: s.qual)[:a.limit]:
        print(f'0x{s.addr:08x}  {s.kind:8}  {s.sig}   [{rom_text(s)}]')
    if len(hits) > a.limit:
        print(f'... {len(hits) - a.limit} more (--limit)')


def params(sig):
    """the parameter list of an undecorated MSVC signature -> [type]"""
    i = sig.rfind(')')
    depth, j = 0, i
    while j >= 0:
        depth += sig[j] == ')'
        depth -= sig[j] == '('
        if depth == 0:
            break
        j -= 1
    inner = sig[j + 1:i].strip()
    if inner in ('', 'void'):
        return []
    out, depth, cur = [], 0, ''
    for ch in inner:
        depth += ch in '<('
        depth -= ch in '>)'
        if ch == ',' and depth == 0:
            out.append(cur.strip())
            cur = ''
        else:
            cur += ch
    return out + [cur.strip()]


def word_sized(t):
    t = t.replace('const ', '').replace('volatile ', '').strip()
    return t.endswith(('*', '&')) or t in (
        'int', 'unsigned int', 'long', 'unsigned long', 'short', 'unsigned short', 'char',
        'unsigned char', 'signed char', 'bool', 'float', 'Fix12<int>', 'u32', 's32', 'u16', 's16', 'u8', 's8')


def skeleton(s):
    full = s.sig
    ps = params(full)
    ret = 'void' if re.search(r'\bvoid\s+__\w+\s', full + ' ') or full.startswith('void ') else 'int'
    odd = [t for t in ps if not word_sized(t)]
    args = ''.join(f', unsigned int a{i} /* {t} */' for i, t in enumerate(ps))
    names = ''.join(f', a{i}' for i in range(len(ps)))
    if s.thiscall:
        decl = f'{ret} (__fastcall *orig_t)(void *self, void *edx{args})'
        sig = f'{ret} __fastcall lab_patch(void *self, void *edx{args})'
        call = f'orig(self, edx{names})'
    elif s.cls and any(n.startswith('__Z') for n in s.names):
        # the port's extern "C" __ZN... shim of a member: `this` is the first
        # stack argument (it loads ecx and jumps to the thiscall body)
        decl = f'{ret} (__cdecl *orig_t)(void *self{args})'
        sig = f'{ret} __cdecl lab_patch(void *self{args})'
        call = f'orig(self{names})'
    else:
        a0 = args[2:] if args else 'void'
        decl = f'{ret} (__cdecl *orig_t)({a0})'
        sig = f'{ret} __cdecl lab_patch({a0})'
        call = f'orig({names[2:]})'
    warn = ''
    if odd:
        warn = ('/* CHECK: these parameters are not one 4-byte word each, so the\n'
                '   placeholders below do not match the stack: ' + '; '.join(odd) + ' */\n')
    body = f'return {call};' if ret != 'void' else f'{call};'
    return f'''/* naming-lab patch for {s.qual}
     port 0x{s.addr:08x}  {s.kind}
     {full}
     {rom_text(s)}

   apply:  python port/mods_api/naming_lab/patch.py apply 0x{s.addr:08x} <this file>
   undo:   python port/mods_api/naming_lab/patch.py undo 0x{s.addr:08x}

   A thiscall member is written as __fastcall with an unused edx: `this` in ecx,
   the rest on the stack, callee pops. Every parameter is a placeholder word;
   cast it to what you are testing. `sym("name")` finds any port symbol (the
   names in build/port-trace/walk_window.symtab) for reading game state. */
#include <windows.h>

{warn}typedef {decl};
static orig_t orig;
static void *(__cdecl *sym)(const char *);
static unsigned int calls;

__declspec(dllexport) void __cdecl lab_patch_init(void *(__cdecl *s)(const char *), void *o)
{{
    sym = s;
    orig = (orig_t)o;
}}

__declspec(dllexport) {sig}
{{
    ++calls;
    {body}
}}
'''


def cmd_new(a):
    db = symdb()
    s = one(db, a.target)
    text = skeleton(s)
    if a.o:
        if os.path.exists(a.o) and not a.force:
            sys.exit(f'{a.o} exists (--force to overwrite)')
        open(a.o, 'w', encoding='utf-8', newline='\n').write(text)
        print(f'wrote {a.o}')
    else:
        sys.stdout.write(text)


def compile_dll(src):
    if not os.path.isfile(VCVARS):
        sys.exit(f'no MSVC x86 toolchain at {VCVARS}')
    os.makedirs(OUT, exist_ok=True)
    stem = re.sub(r'\W', '_', os.path.splitext(os.path.basename(src))[0])
    dll = os.path.join(OUT, stem + '.dll')
    with tempfile.NamedTemporaryFile('w', suffix='.cmd', delete=False, dir=OUT) as f:
        f.write('@echo off\r\n'
                f'call "{VCVARS}" >nul 2>nul || exit /b 1\r\n'
                f'cl /nologo /LD /O1 /MT /W3 "{os.path.abspath(src)}" /Fo"{OUT}\\\\" '
                f'/Fe"{dll}" /link /nologo >"{OUT}\\{stem}.log" 2>&1\r\n')
        bat = f.name
    try:
        r = subprocess.run(['cmd', '/c', bat])
    finally:
        os.unlink(bat)
    log = open(os.path.join(OUT, stem + '.log'), encoding='mbcs', errors='replace').read()
    if r.returncode or not os.path.isfile(dll):
        sys.exit('compile failed:\n' + log)
    return dll


def exports(dll):
    """the DLL's exported names (PE export directory)"""
    import struct
    d = open(dll, 'rb').read()
    pe = struct.unpack_from('<I', d, 0x3c)[0]
    opt = pe + 24
    nsec = struct.unpack_from('<H', d, pe + 6)[0]
    secs = [struct.unpack_from('<IIII', d, opt + struct.unpack_from('<H', d, pe + 20)[0] + 40 * i + 8)
            for i in range(nsec)]

    def off(rva):
        for vs, va, rs, ro in secs:
            if va <= rva < va + max(vs, rs):
                return rva - va + ro
        return None
    ed = off(struct.unpack_from('<I', d, opt + 96)[0])
    if ed is None:
        return []
    n, names = struct.unpack_from('<I', d, ed + 24)[0], off(struct.unpack_from('<I', d, ed + 32)[0])
    out = []
    for i in range(n):
        o = off(struct.unpack_from('<I', d, names + 4 * i)[0])
        out.append(d[o:d.index(b'\0', o)].decode())
    return out


def cmd_apply(a):
    db = symdb()
    s = one(db, a.target)
    dll = compile_dll(a.src)
    # __fastcall exports are decorated (@lab_patch@8); send the name the DLL has
    ex = exports(dll)
    name = next((e for e in ex if e == a.export or re.fullmatch(rf'@?{re.escape(a.export)}@\d+', e)), None)
    if not name:
        sys.exit(f'{os.path.basename(dll)} exports no {a.export} (it has: {", ".join(ex) or "nothing"})')
    r = ask(f'detour {s.addr:x} {dll} {name}', 'detour')
    p = r.split(None, 3)
    ok = len(p) >= 3 and p[2] == '1'
    print(f'{"patched" if ok else "FAILED"}: {s.qual} (0x{s.addr:08x}) -> {os.path.basename(dll)}!{a.export}'
          + (f'  [{p[3]}]' if len(p) > 3 else ''))
    if ok and len(p) > 3 and 'was e8' not in p[3]:
        print('note: the entry was not `call _penter` (e8); `orig` is not a safe continuation. '
              'Run the traced build (build/port-trace).')
    if not ok:
        sys.exit(1)


def cmd_undo(a):
    if a.target == 'all':
        addr = 0
    else:
        addr = one(symdb(), a.target).addr
    r = ask(f'undetour {addr:x}', 'undetour')
    print(f'restored {r.split()[-1]} function(s)')


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    p = sub.add_parser('find')
    p.add_argument('query')
    p.add_argument('--limit', type=int, default=40)
    p.set_defaults(fn=cmd_find)
    p = sub.add_parser('new')
    p.add_argument('target')
    p.add_argument('-o')
    p.add_argument('--force', action='store_true')
    p.set_defaults(fn=cmd_new)
    p = sub.add_parser('apply')
    p.add_argument('target')
    p.add_argument('src')
    p.add_argument('--export', default='lab_patch')
    p.set_defaults(fn=cmd_apply)
    p = sub.add_parser('undo')
    p.add_argument('target')
    p.set_defaults(fn=cmd_undo)
    a = ap.parse_args()
    a.fn(a)


if __name__ == '__main__':
    main()
