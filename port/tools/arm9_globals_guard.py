"""Fail the build when a hand-hosted arm9 DS global is smaller than what the
ROM's own code writes through it.

WHY THIS IS A GATE AND NOT A COMMENT
------------------------------------
This defect class has now shipped five times (memory note
sm64ds-port-undersized-globals): data_0209f4ae, data_020a4c4c/5c,
data_0209d74c, data_ov002_0210ffbc, and on 2026-09-15 data_0209fd5c and
data_0209fd20.  Every instance is the same shape.  A DS global is hosted with
port/hal/auto_bss.cpp's generous `int name[8]` default, the ROM routine that
owns it writes its FULL extent, and the write lands on whatever the linker put
next.  It never presents as a bug in the subsystem that owns the buffer: it
presents as something unrelated being corrupted thousands of bytes away.
data_0209d74c cleared 3808 bytes through hal_wipes[0]'s vtable pointer and
faulted HUD::Behavior the first time Mario talked.

The data_0209d74c fix carried this comment, in auto_bss.cpp, since 2026-08-05:

    "Re-audit with the delta-to-next-symbol rule after adding any symbol here."

Nobody did.  The file grew from 152 symbols to 205, and the re-audit that was
finally run on 2026-09-15 (run link100, lane ARM9GLOBALS) found two more live
instances seventy lines above that very comment.  A comment asking a human to
remember is not a control.  This is the control.

WHAT IT CHECKS, AND WHAT IT DELIBERATELY DOES NOT
-------------------------------------------------
RULE FILL   A fill/copy primitive is handed the symbol's ADDRESS together with
            a CONSTANT byte count bigger than the host object.  No index has to
            be out of range and no pointer has to be wrong: the routine writes
            past the object every single time it runs.  This is the rule that
            catches data_0209d74c and data_0209fd5c.

RULE INDEX  ROM code indexes the symbol with a RUNTIME index, and the host
            object is smaller than the symbol's ROM span.  The bound is a
            runtime value, so the write is not provably out of range on any
            given call -- but the ROM span says how many elements the object is
            supposed to hold, and a host object smaller than that cannot hold
            them.  This is the rule that catches data_0209fd20.  Of the 60
            variable-indexed arm9 symbols, 58 already have a host object at
            least as big as their ROM span, so this rule is precise rather than
            noisy.

RULE PIN    port/tools/romdata.py's NAMED list carries explicit `name:0xSIZE`
            pins.  A pin that disagrees with the delta-to-next-symbol rule is
            either deliberate or a mistake, and the gate refuses to guess:
            every disagreeing pin must be adjudicated in the allow-list with a
            written reason.  Five are, today, and all five are deliberate.

NOT CHECKED, on purpose:
  - A CONSTANT offset read or written past a host object.  There are 25 of
    these and ALL 25 are correct: the COMM / WMBSS / GXBANK / MPN_BSS macros
    emit into `.dsstate$<tag><NNNN>` sections that MSVC orders by suffix, so a
    ROM record that dsd split into one symbol per field is laid back down in
    ROM order and the access lands on the byte the DS would have given it.
    Proving that requires the linked map's host deltas, and gating on it would
    turn 25 correct accesses into 25 build failures.  ARM9GLOBALS verified all
    25 against the map; out/ARM9GLOBALS/arm9space.py is the verifier.
  - A fill whose byte count is a VARIABLE.  It cannot be ruled in or out
    statically.  Counted and printed, never failed on.
  - The overlay half.  Lane GLOBALS swept 11,780 hosted overlay globals and
    ruled 112 of them DO-NOT-FIX load-bearing RTTI overhangs.  This gate covers
    arm9 only, but the allow-list format already carries an OVERHANG class so
    those 112 rows can be moved in later without redesigning anything.

FALSE POSITIVES THIS ALREADY WEEDS
----------------------------------
Each of these turned a correct piece of code into a "defect" on ARM9GLOBALS'
first pass, and each is a fixture below, so none of them can come back quietly.

  - A HOSTED SYMBOL INSIDE A DESTINATION EXPRESSION.
    MultiStore_Int(li, cp + (data_0209fc78 << 5), 0x800) names a hosted symbol
    in the destination argument, but the symbol is the ROW COUNTER and the
    destination is the local cp.  The destination must BE the argument, casts
    and address-of aside, and nothing else.
  - POINTER SYMBOLS READ AS ARRAYS.  `extern void *data_0209f318;` is a
    four-byte POINTER, so data_0209f340[11] and (char *)data_0209f318 + 0x114
    read THROUGH it into the object it points at and never touch the hosted
    symbol at all.  Pointer-ness is read per file.
  - COMMENTS AND STRING LITERALS, stripped before any pattern runs.
    auto_bss.cpp is roughly half commentary.
  - extern DECLARATIONS READ AS DEFINITIONS.  They allocate nothing.

SELF-VALIDATION, PRINTED ON EVERY RUN
-------------------------------------
A gate that has only ever passed proves nothing.  This one runs its rule engine
against six built-in fixtures before it looks at the tree: three that MUST be
reported and three that MUST NOT.  The fixtures are synthetic rather than the
real defects, because the real defects are fixed now and a gate that validates
itself against the tree stops validating the day the tree is correct.  If any
fixture comes back with the wrong verdict the guard fails the build and says
the rule engine is broken -- so deleting or weakening a rule breaks the build
immediately instead of silently disarming the check, which is the failure this
whole file exists to prevent.

Run link100, lane BSSFIX.  Executing lane ARM9GLOBALS' finding and closing the
hole that let it happen.
"""
import bisect
import os
import re
import sys

NAMERE = r'data_0[0-9a-fA-F]{7}'

# ---------------------------------------------------------------- utilities


_STRIP_RE = re.compile(
    r'//[^\n]*'                           # line comment
    r'|/\*.*?(?:\*/|\Z)'                  # block comment, unterminated allowed
    r'|"(?:\\.|[^"\\\n])*"?'              # string literal
    r"|'(?:\\.|[^'\\\n])*'?",             # character literal
    re.S)


def _blank(m):
    """Blank a comment or literal, keeping newlines so line numbers survive.

    String literals keep their quotes: the COMM / WMBSS / GXBANK / MPN_BSS
    hosting macros are spelled MACRO("name", data_XXXXXXXX, size) and the
    inventory matches that leading "..." argument, so blanking the quotes too
    would quietly drop every macro-hosted symbol from the inventory.
    """
    s = m.group(0)
    q = s[0] if s[0] in '"\'' else ''
    if q:
        return q + ' ' * max(0, len(s) - 2) + (q if len(s) > 1 else '')
    return ''.join(ch if ch == '\n' else ' ' for ch in s)


def strip_code(src):
    """Blank out comments and string literals, preserving line numbering.

    Regex-driven rather than a character loop: this runs over every .c/.cpp in
    src/ and port/ on every build, and the character loop cost 37 seconds
    against a whole-build guard budget of about 11.  strip_code_slow below is
    the original, kept as the reference the --selftest compares against.
    """
    return _STRIP_RE.sub(_blank, src)


def strip_code_slow(src):
    """The original character loop.  Reference implementation only."""
    out, i, n = [], 0, len(src)
    while i < n:
        c = src[i]
        if c == '/' and i + 1 < n and src[i + 1] == '/':
            j = src.find('\n', i)
            if j < 0:
                out.append(' ' * (n - i))
                break
            out.append(' ' * (j - i))
            i = j
        elif c == '/' and i + 1 < n and src[i + 1] == '*':
            j = src.find('*/', i + 2)
            j = n if j < 0 else j + 2
            out.append(''.join(ch if ch == '\n' else ' ' for ch in src[i:j]))
            i = j
        elif c in '"\'':
            q, j = c, i + 1
            while j < n:
                if src[j] == '\\':
                    j += 2
                    continue
                if src[j] == q:
                    j += 1
                    break
                if src[j] == '\n':
                    break
                j += 1
            out.append('"' + ' ' * max(0, j - i - 2) + '"')
            i = j
        else:
            out.append(c)
            i += 1
    return ''.join(out)


def split_args(s):
    args, depth, cur = [], 0, []
    for ch in s:
        if ch in '([{':
            depth += 1
        elif ch in ')]}':
            depth -= 1
        if ch == ',' and depth == 0:
            args.append(''.join(cur).strip())
            cur = []
        else:
            cur.append(ch)
    if cur:
        args.append(''.join(cur).strip())
    return args


def evalsz(expr):
    e = expr.strip().replace('kPortMaxPlayers', '4')
    if not re.fullmatch(r'[0-9a-fA-FxX\*\+\-/\s\(\)]+', e or ''):
        return None
    try:
        v = eval(e, {"__builtins__": {}}, {})
    except Exception:
        return None
    return int(v) if isinstance(v, (int, float)) and v == int(v) else None


# ------------------------------------------------------------- the ROM side

SPAT = re.compile(r"^(\S+)\s+kind:(\S+)\s+addr:0x([0-9a-fA-F]+)")
SECPAT = re.compile(r"^\s+(\.\S+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)\s+kind:(\S+)")
MODULES = {"main": "arm9", "itcm": "arm9/itcm", "dtcm": "arm9/dtcm"}


class RomMap(object):
    """config/arm9 symbol spans: delta to the next symbol, clamped to the
    containing delinks section.  The same rule port/tools/ovdata.py uses for
    the overlays, so both halves of the class are measured the same way."""

    def __init__(self, root):
        self.syms, self.secs = {}, {}
        for key, rel in MODULES.items():
            rows = []
            p = os.path.join(root, "config", rel, "symbols.txt")
            if os.path.exists(p):
                with open(p, encoding="utf-8", errors="replace") as fh:
                    for line in fh:
                        m = SPAT.match(line.strip())
                        if m:
                            rows.append((int(m.group(3), 16), m.group(1)))
            rows.sort()
            self.syms[key] = rows
            secs = []
            p = os.path.join(root, "config", rel, "delinks.txt")
            if os.path.exists(p):
                with open(p, encoding="utf-8", errors="replace") as fh:
                    for line in fh:
                        if line.startswith("src/") or line.startswith("asm/"):
                            break
                        m = SECPAT.match(line)
                        if m:
                            secs.append((int(m.group(2), 16), int(m.group(3), 16)))
            self.secs[key] = secs

    def span(self, addr):
        """(span_bytes, next_symbol_name) or (None, None) outside arm9."""
        for key, secs in self.secs.items():
            for lo, hi in secs:
                if lo <= addr < hi:
                    rows = self.syms[key]
                    addrs = [r[0] for r in rows]
                    i = bisect.bisect_right(addrs, addr)
                    if i < len(rows):
                        nxt_a, nxt_n = rows[i]
                        end = min(nxt_a, hi)
                    else:
                        nxt_n, end = None, hi
                    return end - addr, nxt_n
        return None, None


# ------------------------------------------------------ the host inventory

TYPESZ = {
    'char': 1, 'signed char': 1, 'unsigned char': 1, 'u8': 1, 's8': 1,
    'bool': 1, '_Bool': 1,
    'short': 2, 'unsigned short': 2, 'signed short': 2, 'u16': 2, 's16': 2,
    'short int': 2, 'fx16': 2,
    'int': 4, 'unsigned': 4, 'unsigned int': 4, 'signed int': 4, 'u32': 4,
    's32': 4, 'long': 4, 'unsigned long': 4, 'signed long': 4, 'float': 4,
    'fx32': 4,
    'long long': 8, 'unsigned long long': 8, 'u64': 8, 's64': 8, 'double': 8,
    'void': 1,
}

MACRO_SIZE_ARG = {'COMM', 'WMBSS', 'GXBANK', 'MPN_BSS'}
# How far back a definition's prefix can reach from the symbol name: type,
# qualifiers and any __declspec(allocate(...)).  The longest in the tree is
# well under this.
DECL_WINDOW = 400
MACRO = re.compile(r'\b([A-Z][A-Z0-9_]{2,})\s*\(\s*"[^"]*"\s*,\s*(' + NAMERE + r')\s*,\s*([^,\)]+)')

DECL = re.compile(
    r'(?P<pre>(?:^|[;{}])\s*(?:DSSTATE_\w+\s*)?(?:extern\s*"C"\s*)?'
    r'(?:__declspec\s*\(\s*allocate\s*\(\s*"(?P<sec>[^"]*)"\s*\)\s*\)\s*)?'
    r'(?:__declspec\s*\(\s*align\s*\([^)]*\)\s*\)\s*)?'
    r'(?:alignas\s*\([^)]*\)\s*)?'
    r'(?:extern\s*"C"\s*)?'
    r'(?P<kw>(?:static|const|volatile|struct|union|extern)\s+)*'
    # (?!DSSTATE_) so the type group cannot swallow the DSSTATE_BEGIN /
    # DSSTATE_END bracket macros as if they were a type name.  Without it
    # `DSSTATE_BEGIN\nunsigned char data_020a0c68[0x10];` reads as a type of
    # "DSSTATE_BEGIN unsigned char", which is in no size table, so the symbol
    # silently loses its host size and every rule then skips it.
    r'(?P<type>(?!DSSTATE_)(?:unsigned|signed|long|short|int|char|float|double|void|bool|_Bool'
    r'|u8|u16|u32|u64|s8|s16|s32|s64|fx16|fx32|[A-Za-z_]\w*)'
    r'(?:\s+(?:unsigned|signed|long|short|int|char|double))*)\s*'
    r'(?P<ptr>[\*&\s]*))'
    r'(?P<name>' + NAMERE + r')\s*(?P<dims>(?:\[[^\]]*\]\s*)*)\s*(?P<tail>[;=,])',
    re.M)


def _recorder(inv, rom, src, rel):
    """Build the row-recording callback for one source file.

    Module level rather than a closure inside the walk so the windowed scan
    and the whole-file reference scan provably record rows the same way; if
    they did not, --deep-selftest would be comparing two different things.
    """
    def record(name, host, form, pos):
        addr = int(name.split('_')[1], 16)
        span, nxt = rom.span(addr)
        if span is None:
            return                  # not an arm9 symbol
        row = inv.get(name)
        if row is None:
            row = inv[name] = dict(name=name, addr=addr, rom_span=span,
                                   rom_next=nxt, host=None, form=form,
                                   file=rel, line=0)
        if host is not None and row['host'] is None:
            row.update(host=host, form=form, file=rel,
                       line=src.count('\n', 0, pos) + 1)
    return record


def host_inventory(root, rom):
    """{name: row} for every hand-hosted arm9 global defined in port/."""
    inv = {}
    portdir = os.path.join(root, "port")
    for dirpath, dirnames, filenames in os.walk(portdir):
        dirnames[:] = [d for d in dirnames if d not in ('.git', '__pycache__')]
        for fn in filenames:
            if os.path.splitext(fn)[1] not in ('.c', '.cpp', '.h', '.hpp', '.inc'):
                continue
            path = os.path.join(dirpath, fn)
            try:
                with open(path, encoding="utf-8", errors="replace") as fh:
                    raw = fh.read()
            except OSError:
                continue
            if 'data_0' not in raw:
                continue
            src = strip_code(raw)
            rel = os.path.relpath(path, root).replace('\\', '/')

            record = _recorder(inv, rom, src, rel)

            for m in MACRO.finditer(src):
                if m.group(1) in MACRO_SIZE_ARG:
                    record(m.group(2), evalsz(m.group(3)),
                           'macro:' + m.group(1), m.start())

            # DECL is an expensive regex -- enough optional groups that running
            # it across a whole file backtracks for tens of seconds, which on
            # this tree was 34 of the guard's 37 seconds and the whole reason a
            # per-build gate looked too slow to run per build.  A definition
            # never spans more than the statement it is in, so it is matched in
            # a window that ends at the statement's semicolon and starts a few
            # hundred characters back, which is more than the longest
            # __declspec(allocate(...)) prefix in the tree.  --selftest proves
            # the windowed inventory is identical to the whole-file one.
            for nm in re.finditer(NAMERE, src):
                lo = max(0, nm.start() - DECL_WINDOW)
                semi = src.find(';', nm.end())
                hi = len(src) if semi < 0 else min(semi + 1, len(src))
                if hi - nm.end() > DECL_WINDOW:
                    hi = nm.end() + DECL_WINDOW
                for m in DECL.finditer(src[lo:hi]):
                    if m.group('name') != nm.group(0):
                        continue
                    _decl_record(m, src, lo, record)
    return inv


def _decl_record(m, src, lo, record):
    """Turn one DECL match into an inventory row.  Split out of the walk so the
    windowed and whole-file passes provably share one body."""
    if re.search(r'\bextern\b(?!\s*"C")', m.group('pre')):
        return                          # a declaration allocates nothing
    typ = m.group('type').strip()
    ptr = (m.group('ptr') or '').strip()
    dims = m.group('dims') or ''
    base = 4 if ptr.count('*') >= 1 else TYPESZ.get(typ)
    nelem, ok = 1, True
    for d in re.findall(r'\[([^\]]*)\]', dims):
        v = evalsz(d) if d.strip() else None
        if v is None:
            ok = False
            break
        nelem *= v
    total = base * nelem if (base is not None and ok) else None
    # Report the SYMBOL's position, not the match's.  A match starts back at
    # the previous statement's ';', and blanked comment blocks are just
    # whitespace the regex walks straight through, so m.start() lands up in
    # the commentary above the definition.  auto_bss.cpp is roughly half
    # commentary, so that was routinely ten or more lines off the line a
    # reader actually needs.  The symbol's own offset is exact, and it is
    # also identical whether the scan was windowed or whole-file.
    record(m.group('name'), total,
           'decl:' + typ + (' ' + ptr if ptr else '') + dims,
           lo + m.start('name'))



# ------------------------------------------------------------- the analysis

FILLERS = {
    "MultiStore_Int": 2, "MultiStore_Zero": 2,
    "memset": 2, "memcpy": 2, "memmove": 2,
    "MI_CpuFill8": 2, "MI_CpuFill16": 2, "MI_CpuFill32": 2,
    "MI_CpuClear8": 1, "MI_CpuClear16": 1, "MI_CpuClear32": 1,
    "MIi_CpuClear32": 1, "MIi_CpuCopy32": 2, "MIi_CpuFill32": 2,
    "MI_CpuCopy8": 2, "MI_CpuCopy16": 2, "MI_CpuCopy32": 2,
    "MI_CpuFillFast": 2, "MI_CpuClearFast": 1, "MI_CpuCopyFast": 2,
}
CALL = re.compile(r'\b(' + '|'.join(map(re.escape, FILLERS)) + r')\s*\(')
CONST = re.compile(r'^\s*\(?\s*(0[xX][0-9a-fA-F]+|\d+)\s*\)?\s*$')
DESTONLY = re.compile(NAMERE + r'(?:\s*\[\s*(?:0[xX][0-9a-fA-F]+|\d+)\s*\])?')
IDX = re.compile(r'(?<![A-Za-z0-9_])(' + NAMERE + r')\s*\[([^\]\n]{0,70})\]')
EXT = re.compile(
    r'\bextern\s+(?:volatile\s+|const\s+)*'
    r'((?:unsigned|signed)?\s*(?:char|short|int|long|float|double|void|bool'
    r'|u8|u16|u32|u64|s8|s16|s32|s64|fx16|fx32))\s*(\**)\s*'
    r'(' + NAMERE + r')\s*(\[[^\]]*\])?')


def unwrap(arg):
    """Strip casts and parens so the destination argument can be compared to
    the bare symbol.  Returns the naked expression."""
    b = arg.strip()
    while True:
        nb = re.sub(r'^\(\s*(?:const\s+|volatile\s+|unsigned\s+|signed\s+)*'
                    r'[A-Za-z_]\w*\s*\**\s*\)\s*', '', b).strip()
        if nb.startswith('(') and nb.endswith(')'):
            nb = re.sub(r'^\(\s*(.*)\s*\)$', r'\1', nb).strip()
        if nb == b:
            return b.lstrip('&').strip()
        b = nb


def whole_file_inventory(root, rom):
    """The unwindowed reference scan, for --deep-selftest only.

    host_inventory() matches DECL in a bounded window around each symbol
    because running that regex across whole files backtracked for 34 of the
    guard's 37 seconds, which is the difference between a gate that can run
    on every build and one that cannot.  The speedup is only legitimate if
    it finds the same thing, so this is the thing it is compared against.
    Run `--deep-selftest` after ANY change to DECL or DECL_WINDOW.
    """
    inv = {}
    for dirpath, dirnames, filenames in os.walk(os.path.join(root, 'port')):
        dirnames[:] = [d for d in dirnames if d not in ('.git', '__pycache__')]
        for fn in filenames:
            if os.path.splitext(fn)[1] not in ('.c', '.cpp', '.h', '.hpp', '.inc'):
                continue
            path = os.path.join(dirpath, fn)
            try:
                with open(path, encoding='utf-8', errors='replace') as fh:
                    raw = fh.read()
            except OSError:
                continue
            if 'data_0' not in raw:
                continue
            src = strip_code(raw)
            rel = os.path.relpath(path, root).replace(os.sep, '/')
            rec = _recorder(inv, rom, src, rel)
            for m in MACRO.finditer(src):
                if m.group(1) in MACRO_SIZE_ARG:
                    rec(m.group(2), evalsz(m.group(3)), 'macro:' + m.group(1), m.start())
            for m in DECL.finditer(src):
                _decl_record(m, src, 0, rec)
    return inv


def deep_selftest(root):
    rom = RomMap(root)
    fast, ref = host_inventory(root, rom), whole_file_inventory(root, rom)
    keys = sorted(set(fast) | set(ref))
    diff = [k for k in keys if fast.get(k) != ref.get(k)]
    print('windowed rows: %d   whole-file rows: %d' % (len(fast), len(ref)))
    for k in diff[:10]:
        print('  DIFF %s' % k)
        print('    windowed : %s' % (fast.get(k),))
        print('    wholefile: %s' % (ref.get(k),))
    print('rows that differ: %d' % len(diff))
    return not diff


def analyse(inv, units):
    """The whole rule engine, over an inventory and a list of (path, text).

    Pure, so the fixtures below can drive it with synthetic input.  Returns
    (findings, stats).
    """
    findings, stats = [], dict(fill_const=0, fill_var=0, indexed=0)
    for path, raw in units:
        if 'data_0' not in raw:
            continue
        t = strip_code(raw)

        # ---- per-file element width and POINTER-ness.  A pointer symbol
        # indexed or offset dereferences THROUGH the four-byte object and
        # never touches the hosted storage, so it is not an over-reach.
        isptr = {}
        for m in EXT.finditer(t):
            star, nm, dims = m.group(2), m.group(3), m.group(4)
            if nm in inv:
                isptr[nm] = isptr.get(nm, False) or bool(star and not dims)

        # ---- RULE FILL
        for m in CALL.finditer(t):
            fn = m.group(1)
            i = m.end()
            depth, j = 1, i
            while j < len(t) and depth:
                if t[j] in '([':
                    depth += 1
                elif t[j] in ')]':
                    depth -= 1
                j += 1
            args = split_args(t[i:j - 1])
            szpos = FILLERS[fn]
            if szpos >= len(args):
                continue
            dest = None
            for a in args[:szpos]:
                b = unwrap(a)
                if DESTONLY.fullmatch(b) and b.split('[')[0] in inv:
                    dest = b.split('[')[0]
                    break
            if dest is None:
                continue
            cm = CONST.match(args[szpos])
            if not cm:
                stats['fill_var'] += 1
                continue
            stats['fill_const'] += 1
            nbytes = int(cm.group(1), 0)
            row = inv[dest]
            if row['host'] is not None and nbytes > row['host']:
                ls = t.rfind('\n', 0, m.start()) + 1
                le = t.find('\n', j)
                findings.append(dict(
                    name=dest, cls='FILL', host=row['host'], need=nbytes,
                    rom_span=row['rom_span'], file=path,
                    line=t.count('\n', 0, m.start()) + 1,
                    text=t[ls:le if le > 0 else len(t)].strip()[:150],
                    why='%s writes %d bytes through a %d-byte host object, so '
                        '%d bytes of whatever the linker put next are '
                        'overwritten every time it runs'
                        % (fn, nbytes, row['host'], nbytes - row['host'])))

        # ---- RULE INDEX
        for m in IDX.finditer(t):
            nm, ix = m.group(1), m.group(2).strip()
            if nm not in inv or isptr.get(nm):
                continue
            ls = t.rfind('\n', 0, m.start()) + 1
            if re.search(r'\b(extern|static)\b', t[ls:m.start()]):
                continue
            if CONST.fullmatch(ix):
                continue                # constant offsets: see the header
            stats['indexed'] += 1
            row = inv[nm]
            if (row['host'] is not None and row['rom_span'] is not None
                    and row['host'] < row['rom_span']):
                le = t.find('\n', m.end())
                findings.append(dict(
                    name=nm, cls='INDEX', host=row['host'],
                    need=row['rom_span'], rom_span=row['rom_span'], file=path,
                    line=t.count('\n', 0, m.start()) + 1,
                    text=t[ls:le if le > 0 else len(t)].strip()[:150],
                    why='ROM code indexes this with a runtime index and the '
                        'host object is %d bytes against a ROM span of %d, so '
                        'it cannot hold the elements the ROM expects'
                        % (row['host'], row['rom_span'])))
    return findings, stats


# --------------------------------------------------------- RULE PIN (romdata)

def strip_py_comments(block):
    """Drop Python line comments, KEEPING string literals.

    strip_code() is the wrong tool here and silently returned nothing the first
    time: romdata.py's NAMED rows ARE string literals ("data_0208e498:0x18"),
    so blanking strings blanks exactly the data this rule reads.  A rule that
    reads nothing passes everything.
    """
    out = []
    for line in block.splitlines():
        q, i = None, 0
        while i < len(line):
            c = line[i]
            if q:
                if c == '\\':
                    i += 1
                elif c == q:
                    q = None
            elif c in '"\'':
                q = c
            elif c == '#':
                line = line[:i]
                break
            i += 1
        out.append(line)
    return '\n'.join(out)


def romdata_pins(root, rom):
    """Explicit name:0xSIZE pins in romdata.py's NAMED list that disagree with
    the delta rule.  Each must be adjudicated in the allow-list."""
    p = os.path.join(root, "port", "tools", "romdata.py")
    if not os.path.exists(p):
        return [], None
    with open(p, encoding="utf-8", errors="replace") as fh:
        text = fh.read()
    m = re.search(r'^NAMED\s*=\s*\[', text, re.M)
    if not m:
        return [], None
    depth, i = 1, m.end()
    while i < len(text) and depth:
        if text[i] == '[':
            depth += 1
        elif text[i] == ']':
            depth -= 1
        i += 1
    block = strip_py_comments(text[m.end():i - 1])
    out, nparsed = [], 0
    for mm in re.finditer(r'"(' + NAMERE + r'):\s*(0[xX][0-9a-fA-F]+|\d+)"', block):
        name, pin = mm.group(1), int(mm.group(2), 0)
        nparsed += 1
        addr = int(name.split('_')[1], 16)
        span, nxt = rom.span(addr)
        if span is None or pin == span:
            continue
        out.append(dict(
            name=name, cls='PIN', host=pin, need=span, rom_span=span,
            file='port/tools/romdata.py', line=0, text='"%s:%#x"' % (name, pin),
            why='an explicit size pin of %#x against a delta-rule span of %#x. '
                'Deliberate or a mistake, and the gate will not guess: '
                'adjudicate it in the allow-list' % (pin, span)))
    return out, nparsed


# ---------------------------------------------------------- the allow-list

def read_allow(path):
    """symbol CLASS reason.  A reason is REQUIRED: the whole point of the list
    is that an exception is written down and argued, not silently taken."""
    allow, errors = {}, []
    if not path or not os.path.exists(path):
        return allow, errors
    with open(path, encoding="utf-8", errors="replace") as fh:
        for n, line in enumerate(fh, 1):
            s = line.strip()
            if not s or s.startswith('#'):
                continue
            parts = s.split(None, 2)
            if len(parts) < 3:
                errors.append("%s:%d: need 'SYMBOL CLASS reason', got: %s"
                              % (os.path.basename(path), n, s[:70]))
                continue
            sym, cls, reason = parts[0], parts[1].upper(), parts[2].strip()
            if cls not in ('FILL', 'INDEX', 'PIN', 'OVERHANG'):
                errors.append("%s:%d: unknown class %r (FILL INDEX PIN OVERHANG)"
                              % (os.path.basename(path), n, cls))
                continue
            if len(reason) < 20:
                errors.append("%s:%d: the reason for %s is too short to be a "
                              "reason: %r" % (os.path.basename(path), n, sym, reason))
                continue
            allow[(sym, cls)] = reason
    return allow, errors


# ------------------------------------------------------------- self-validation

def fixtures():
    """(label, inventory, units, must_fire, note).

    Synthetic on purpose.  Validating against the tree's real defects stops
    validating anything the day the tree is correct, which is today.
    """
    def inv(name, host, rom_span):
        return {name: dict(name=name, addr=int(name.split('_')[1], 16),
                           rom_span=rom_span, rom_next=None, host=host,
                           form='fixture', file='fixture', line=0)}

    return [
        ("FILL fires on an undersized whole-object write",
         inv('data_09000010', 32, 1256),
         [('fixture.c', 'void f(void){ MultiStore_Int(0, &data_09000010, 0xf00); }')],
         True, "the data_0209fd5c shape: 0xf00 written through 32 bytes"),

        ("FILL stays quiet when the host object is big enough",
         inv('data_09000020', 3840, 3840),
         [('fixture.c', 'void f(void){ MultiStore_Int(0, &data_09000020, 0xf00); }')],
         False, "the data_0209d74c shape after its fix"),

        ("INDEX fires on a runtime index into an undersized object",
         inv('data_09000030', 32, 60),
         [('fixture.c', 'void f(void){ data_09000030[n] = w; }')],
         True, "the data_0209fd20 shape: 32 bytes for a 60-byte ROM span"),

        ("INDEX stays quiet once the host object covers the ROM span",
         inv('data_09000040', 60, 60),
         [('fixture.c', 'void f(void){ data_09000040[n] = w; }')],
         False, "the data_0209fd20 shape after its fix"),

        ("a hosted symbol in the DESTINATION EXPRESSION is not the destination",
         inv('data_09000050', 4, 4),
         [('fixture.c', 'void f(void){ MultiStore_Int(li, cp + (data_09000050 << 5), 0x800); }')],
         False, "the row-counter false positive: the destination is the local cp"),

        ("a POINTER symbol indexed dereferences through it, not into it",
         inv('data_09000060', 4, 4),
         [('fixture.c', 'extern void *data_09000060;\nvoid f(void){ g(data_09000060[11]); }')],
         False, "the false positive that killed four of five first-run findings"),
    ]


STRIP_SAMPLE = (
    'int a; // data_09000001 in a line comment\n'
    '/* data_09000002 in a block\n   comment, over two lines */\n'
    'const char *s = "data_09000003 in a string";\n'
    "char c = '/', d = '\\\\', e = '\\'';\n"
    'COMM("tag", data_09000004, 0x10);\n'
    'void f(void){ MultiStore_Int(0, &data_09000005, 0xf00); }\n'
)


def strip_check():
    """The fast strip_code must agree with the original character loop.

    strip_code runs over every .c/.cpp in src/ and port/ on every build, so it
    was rewritten from a character loop into a regex.  This pins the rewrite to
    the behaviour the rules were validated against.  The two differ in ONE way
    on purpose: the loop blanked a character literal using double quotes, this
    one keeps single quotes, which is the stricter of the two because a blanked
    '...' can no longer look like the "name" argument of a hosting macro.  So
    the comparison normalises the quote character and nothing else.
    """
    norm = lambda s: s.replace("'", '"')
    return norm(strip_code(STRIP_SAMPLE)) == norm(strip_code_slow(STRIP_SAMPLE))


def pin_reader_check():
    """RULE PIN's reader must keep quoted rows and drop commented-out ones.

    This is a fixture because it is a bug that already happened: the first
    version of RULE PIN ran romdata.py's NAMED block through strip_code(),
    which blanks string literals -- and the NAMED rows ARE string literals, so
    the rule read nothing and passed everything.  A rule that reads nothing is
    indistinguishable from a rule that finds nothing, which is the worst thing
    a gate can be.
    """
    sample = ('    "data_09000070:0x18",      # a live pinned row\n'
              '    # "data_09000080:0x40",    a row commented out\n'
              '    "data_09000090",           # no pin, not a row this rule reads\n')
    got = re.findall(r'"(' + NAMERE + r'):\s*(0[xX][0-9a-fA-F]+|\d+)"',
                     strip_py_comments(sample))
    return [n for n, _ in got] == ['data_09000070']


def self_check(verbose=True):
    pin_ok, strip_ok = pin_reader_check(), strip_check()
    ok = pin_ok and strip_ok
    if verbose:
        print("  self-check (the rule engine, before the tree is read):")
        print("    [%s] %-62s %s"
              % ("PASS" if pin_ok else "FAIL",
                 "PIN reads quoted rows and skips commented-out ones",
                 "reader live" if pin_ok else "reader is blind, so RULE PIN passes everything"))
        print("           the bug that already happened: stripping strings "
              "blanked the rows themselves")
        print("    [%s] %-62s %s"
              % ("PASS" if strip_ok else "FAIL",
                 "the fast comment/string stripper matches the original loop",
                 "agrees" if strip_ok else "diverged, so every rule reads different text"))
        print("           the rules were validated against the loop; the regex "
              "must not change what they see")
    for label, inv, units, must_fire, note in fixtures():
        found, _ = analyse(inv, units)
        fired = bool(found)
        good = (fired == must_fire)
        ok = ok and good
        if verbose:
            print("    [%s] %-62s %s"
                  % ("PASS" if good else "FAIL", label,
                     ("fired" if fired else "quiet")
                     + (" as required" if good else " but must %s"
                        % ("fire" if must_fire else "stay quiet"))))
            print("           %s" % note)
    return ok


# ---------------------------------------------------------------------- main

def main(argv):
    here = os.path.dirname(os.path.abspath(__file__))
    root = os.path.abspath(os.path.join(here, "..", ".."))
    allow_path = os.path.join(root, "port", "arm9_globals_allow.txt")
    args = list(argv[1:])
    i = 0
    while i < len(args):
        if args[i] == '--allow':
            allow_path = args[i + 1]
            i += 2
        elif args[i] == '--root':
            root = os.path.abspath(args[i + 1])
            i += 2
        elif args[i] == '--deep-selftest':
            print('arm9_globals_guard: windowed vs whole-file inventory')
            sys.exit(0 if deep_selftest(root) else 1)
        elif args[i] == '--selftest':
            print("arm9_globals_guard: rule-engine self-check only")
            sys.exit(0 if self_check() else 1)
        else:
            i += 1                       # a map path, accepted and unused

    print("arm9_globals_guard: hosted arm9 globals vs what the ROM writes")

    if not self_check():
        print()
        print("REFUSING THE BUILD: the rule engine did not reproduce its own")
        print("fixtures.  A rule has been deleted, weakened or broken, which")
        print("silently disarms the check -- which is the exact failure this")
        print("guard exists to prevent.  Fix the engine, do not delete the")
        print("fixture.  port/tools/arm9_globals_guard.py")
        return 1

    allow, allow_errors = read_allow(allow_path)
    if allow_errors:
        print()
        print("REFUSING THE BUILD: the allow-list is malformed.")
        for e in allow_errors:
            print("  " + e)
        print("Every row is 'SYMBOL CLASS reason' and the reason is required:")
        print("an exception has to be argued in writing, not taken silently.")
        return 1

    rom = RomMap(root)
    inv = host_inventory(root, rom)

    units = []
    for base in ("src", "src_tu", "port"):
        d = os.path.join(root, base)
        if not os.path.isdir(d):
            continue
        for dirpath, dirnames, filenames in os.walk(d):
            dirnames[:] = [x for x in dirnames if x not in ('.git', '__pycache__')]
            for fn in filenames:
                if not fn.endswith(('.c', '.cpp')):
                    continue
                p = os.path.join(dirpath, fn)
                try:
                    with open(p, encoding="utf-8", errors="replace") as fh:
                        units.append((os.path.relpath(p, root).replace('\\', '/'),
                                      fh.read()))
                except OSError:
                    pass

    findings, stats = analyse(inv, units)
    pins, nparsed = romdata_pins(root, rom)
    findings.extend(pins)
    if nparsed == 0:
        print()
        print("REFUSING THE BUILD: RULE PIN found romdata.py's NAMED list but "
              "read no pinned rows out of it.")
        print("That is a parse failure, not a clean result: a rule that reads "
              "nothing passes everything.")
        return 1

    live, waived = [], []
    for f in findings:
        key = (f['name'], f['cls'])
        (waived if key in allow else live).append(f)

    sized = sum(1 for r in inv.values() if r['host'] is not None)
    print("  %d hand-hosted arm9 globals (%d with a computed host span); "
          "%d ROM call sites with a constant byte count, %d with a variable "
          "one, %d runtime-indexed reads"
          % (len(inv), sized, stats['fill_const'], stats['fill_var'],
             stats['indexed']))

    if waived:
        seen = set()
        print("  %d adjudicated, each with a written reason:" % len(waived))
        for f in sorted(waived, key=lambda x: x['name']):
            key = (f['name'], f['cls'])
            if key in seen:
                continue
            seen.add(key)
            print("    %-18s %-6s %s" % (f['name'], f['cls'], allow[key][:96]))

    stale = sorted(set(allow) - set((f['name'], f['cls']) for f in findings))
    for sym, cls in stale:
        if cls == 'OVERHANG':
            continue                     # reserved for the overlay half
        print("    note: allow-list row %s %s no longer matches anything; it "
              "can be removed" % (sym, cls))

    if not live:
        print("  OK: nothing writes past a hosted arm9 global.")
        return 0

    print()
    print("REFUSING THE BUILD: %d hosted arm9 global(s) are smaller than what "
          "the ROM writes through them." % len(set(f['name'] for f in live)))
    print()
    for f in sorted(live, key=lambda x: -(x['need'] - (x['host'] or 0))):
        print("  %s  [%s]" % (f['name'], f['cls']))
        print("      hosted %s bytes, needs %s (ROM span %s)"
              % (f['host'], f['need'], f['rom_span']))
        print("      %s:%d" % (f['file'], f['line']))
        if f['text']:
            print("      %s" % f['text'])
        print("      %s" % f['why'])
        row = inv.get(f['name'])
        if row is not None and row['host'] is not None:
            print("      the host definition is %s:%d  (%s)"
                  % (row['file'], row['line'], row['form']))
        print()
    print("This does not present as a bug in the subsystem that owns the")
    print("buffer.  It presents as something unrelated being corrupted")
    print("thousands of bytes away: data_0209d74c cleared 3808 bytes through")
    print("hal_wipes[0]'s vtable pointer and faulted HUD::Behavior the first")
    print("time Mario talked.  Size the host definition to the ROM's own span")
    print("(port/hal/auto_bss.cpp has worked examples), or, if the access is")
    print("genuinely correct, add a row to")
    try:
        shown = os.path.relpath(allow_path, root).replace('\\', '/')
    except ValueError:
        shown = allow_path
    print("  %s" % shown)
    print("with the argument for it written out.")
    return 1


if __name__ == '__main__':
    sys.exit(main(sys.argv))
