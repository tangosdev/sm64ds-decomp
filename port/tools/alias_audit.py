#!/usr/bin/env python3
"""Catch the G0 trap: one spelled name, two TUs, two different ROM objects.

THE TRAP, found by lane w20 on ov043 and live in the tree today.

The recovered sources spell shared-header placeholders -- G0, G1, VT, VT0, VT1,
HEAP -- for whatever their own literal pool held. The name carries no address.
port/hal/cxx_aliases.cpp binds each spelled name exactly ONCE, with an
/alternatename pragma, so every TU that spells it collapses onto the same
object. When two TUs mean different objects, one of them is silently wired to
the wrong one:

  src/func_ov043_0211123c.c  (CleanupResources) means its class's own
      SharedFilePtrs, the ROM's 0x021125e8 and 0x021125e0, by G0 and G1
  src/func_ov043_021111e4.c  (the deleting destructor) means the game heap
      pointer 0x020a0eac by G0
  hal/cxx_aliases.cpp already binds _G0 to _data_020a0eac

so the destructor is right and CleanupResources would Release the GAME HEAP
POINTER as a SharedFilePtr. Neither fails at link. Nothing in the map, the byte
gate or the battery can see it. It is only visible by asking what each TU's own
ROM body actually references, which is what this does.

HOW A TU'S INTENDED ADDRESS IS DERIVED, and the limit of it.

The TU's stem names its ROM function (func_ovNNN_ADDR, func_02ADDR, or a
recovered name looked up in config/arm9). Its bytes come from the ROM image the
port itself reads (extracted/overlays/overlay_NNNN.bin at the base
overlays.yaml gives, never the dsd export, which carries relocated words), and
every ARM `LDR Rd, [PC, #imm]` and `ADR Rd, label` in the body is decoded to
the address it loads. That set is what the TU can possibly mean.

The verdict is deliberately one-sided, because only one direction is sound:

  MISMATCH   the bound address is NOT among the addresses this TU's ROM body
             references. That is proof: the binding is wrong for this TU. It
             cannot be explained away.
  consistent the bound address IS among them. That is NOT proof the binding is
             right -- a body references several addresses and only one of them
             is the one the placeholder stands for. It only means this TU does
             not contradict the binding.

So a flagged collision is a real defect and a silent name is not a clean bill
of health. Anywhere a body cannot be read (a thumb function, no size in the
config, an address outside the image, a stem no config symbol claims) the TU is
reported UNRESOLVED with the reason and is never guessed at. A tool that
invents addresses here is worse than no tool: this is the one check standing
between a wrong binding and a silent wrong pointer at run time.

FIVE THINGS THAT LOOK LIKE FINDINGS AND ARE NOT, each of which this got wrong
once before it was right, and each of which costs the tool its credibility if
it comes back:

  a mention in a COMMENT. src/_ZN11CannonHatch16CleanupResourcesEv.cpp was
      fixed off the placeholders and now spells its files by address, but its
      header comment explains the old G0/G1 bug at length. Comments and string
      literals are blanked before the identifier search.
  a TU already RENAMED per source. port/CMakeLists.txt compiles
      src/func_ov100_02147054.c with -DG0=port_pathlift_file0, so that TU never
      references the shared name. Renames are read, and the rename's own target
      is checked instead, resolved one /alternatename hop.
  the TU that DEFINES the target. KillPlayer is bound to KillPlayer's own
      address and does not call itself.
  an ordinary CALLER of a function-valued alias. A call is a BL and never
      lands in the literal pool, so branches are decoded too. Skipping them
      turned about thirty ordinary callers of AngleDiff, Vec3_Add and friends
      into collisions.
  a LOCAL TYPE OF THE SAME NAME. src/func_02021bec.c writes
      `typedef struct { Method m[1]; } VT;` and uses that type to walk a
      vtable. Its `VT` is its own; it has nothing to do with the shared
      placeholder, and the object it compiles to has exactly one undefined
      external, _func_02021a04. This reported it as the one LINKED MISMATCH in
      the tree -- the loudest row the tool can print, on a file with nothing
      wrong with it. A typedef name and an object name share C's ordinary
      identifier namespace, so a TU that typedefs the placeholder name CANNOT
      also be referencing the placeholder object: the shadow is total, and
      those TUs are dropped from the name's user list with a count printed so
      the drop is visible rather than silent.

On the 4888-linked baseline what survives all five is G0, G1 and VT, which are
precisely the three shared-header placeholders cxx_aliases.cpp binds and warns
about in its own comments.

THE LIKELY-ADDRESS ANNOTATION IS A SUGGESTION, NOT A FINDING. Placeholders are
numbered in the order the recovery pass met the literal loads, so lining the
placeholder names up with the ordered data loads usually names the intended
object exactly (on ov043 it gives G0=0x021125e8 G1=0x021125e0 for
CleanupResources, which is w20's hand derivation to the digit). Usually is not
always -- the compiler is free to order a literal pool as it likes -- so it is
printed as `likely` and never as the verdict.

    python port/tools/alias_audit.py [repo-root] [--name G0] [--all] [-v]

  --name NAME  audit one spelled name only
  --all        list every audited name, including the ones with no collision
  -v           per-TU detail for every audited name, not just the flagged ones

With no repo-root this audits the checkout the script lives in, not the current
directory (same rule as linkage.py).
"""

import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import linkage  # noqa: E402  (for map_symbols and the shared undecorate rule)

ARM9_BASE = 0x02004000        # extracted/arm9_dec.bin is flat at this address
DS_LO, DS_HI = 0x01FF0000, 0x02400000   # itcm through the end of main RAM

ALIAS = re.compile(r"/alternatename:([^=\s\"]+)=([^\"\s]+)")
CONFIG_ROW = re.compile(
    r"^(\S+)\s+kind:(\S+?)\((\w+)(?:,size=(0x[0-9a-fA-F]+|\d+))?\)"
    r".*?addr:(0x[0-9a-fA-F]+)")
DATA_NAME = re.compile(r"^data_(?:ov(\d{3})_)?([0-9a-fA-F]{8})$")
FUNC_OV = re.compile(r"^func_ov(\d{3})_([0-9a-fA-F]{8})$")
FUNC_ADDR = re.compile(r"^func_(0[12][0-9a-fA-F]{6})$")
YAML_ID = re.compile(r"^\s*-\s*id:\s*(\d+)\s*$")
YAML_FIELD = re.compile(r"^\s*(base_address|code_size):\s*(\d+)\s*$")


def default_root():
    here = os.path.dirname(os.path.abspath(__file__))
    return os.path.dirname(os.path.dirname(here))


def source_name(decorated):
    """The identifier a /alternatename side spells, undecorated.

    `_G0` -> G0 (32-bit MSVC's single cdecl underscore), `?G0@@3PAHA` -> G0
    (the C++ mangling a TU gets when the declaration is outside extern "C"),
    `__ZN4Heap8AllocateEji` -> _ZN4Heap8AllocateEji (the ROM's C++ name carried
    as a C identifier that already begins with an underscore -- strip exactly
    one, the rule linkage.py's docstring spells out).
    """
    if decorated.startswith("?"):
        return decorated[1:].split("@@", 1)[0]
    return linkage.undecorate(decorated)


ADDR_SUFFIX = re.compile(r"_0[12][0-9a-fA-F]{6}$")


def is_placeholder(name):
    """True for a spelled name that carries no address of its own.

    func_*, data_* and _Z* names each name one ROM object, so two TUs spelling
    one of those mean the same thing by construction and cannot collide. So do
    the address-suffixed ones the recovery pass invents around a known address
    (FUN_02029a68, Math_Function_0203b0fc, __sinit_ov045_02112280) -- the
    address IS in the name, and auditing them only adds rows that can never be
    a finding. What is left is the shared-header placeholders and the role
    names, which is exactly the population this trap lives in.
    """
    if name.startswith(("func_", "data_", "_Z")):
        return False
    return not ADDR_SUFFIX.search(name)


def symbol_index(root):
    """{name: (module, addr, size, kind)} over every config symbol file."""
    out = {}
    files = [(os.path.join(root, "config", "arm9", "symbols.txt"), "arm9"),
             (os.path.join(root, "config", "arm9", "itcm", "symbols.txt"), "itcm"),
             (os.path.join(root, "config", "arm9", "dtcm", "symbols.txt"), "dtcm")]
    ovdir = os.path.join(root, "config", "arm9", "overlays")
    if os.path.isdir(ovdir):
        for ov in sorted(os.listdir(ovdir)):
            p = os.path.join(ovdir, ov, "symbols.txt")
            if os.path.isfile(p):
                files.append((p, ov))
    for path, mod in files:
        try:
            with open(path, errors="replace") as f:
                for line in f:
                    m = CONFIG_ROW.match(line)
                    if m:
                        name, kind, iset, size, addr = m.groups()
                        out.setdefault(name, (mod, int(addr, 16),
                                              int(size, 0) if size else None,
                                              (kind, iset)))
        except OSError:
            pass
    return out


class Images(object):
    """The ROM images, loaded once, with the source rule baked in.

    extracted/overlays/ is the correct image source. extracted/dsd/arm9_overlays/
    disagrees on 89 of the 103 overlays (the compressed ones), and its words are
    relocated on top, so a literal read out of it is not the ROM's.
    """

    def __init__(self, root):
        self.root = root
        self._ov = {}
        self._arm9 = None
        self._bases = None

    def bases(self):
        if self._bases is None:
            self._bases = {}
            path = os.path.join(self.root, "extracted", "dsd", "arm9_overlays",
                                "overlays.yaml")
            cur = None
            try:
                with open(path, errors="replace") as f:
                    for line in f:
                        m = YAML_ID.match(line)
                        if m:
                            cur = int(m.group(1))
                            self._bases.setdefault(cur, {})
                            continue
                        m = YAML_FIELD.match(line)
                        if m and cur is not None:
                            self._bases[cur][m.group(1)] = int(m.group(2))
            except OSError:
                pass
        return self._bases

    def for_module(self, mod):
        """(bytes, base) for a module, or (None, reason)."""
        if mod == "arm9":
            if self._arm9 is None:
                p = os.path.join(self.root, "extracted", "arm9_dec.bin")
                if not os.path.isfile(p):
                    return None, "no extracted/arm9_dec.bin"
                with open(p, "rb") as f:
                    self._arm9 = f.read()
            return self._arm9, ARM9_BASE
        m = re.match(r"^ov(\d{3})$", mod or "")
        if not m:
            return None, "no ROM image for module %r" % mod
        ovid = int(m.group(1))
        if ovid not in self._ov:
            rec = self.bases().get(ovid)
            if not rec or "base_address" not in rec:
                self._ov[ovid] = (None, "overlays.yaml has no base for ov%03d"
                                  % ovid)
            else:
                p = os.path.join(self.root, "extracted", "overlays",
                                 "overlay_%04d.bin" % ovid)
                if not os.path.isfile(p):
                    self._ov[ovid] = (None, "no %s" % p)
                else:
                    with open(p, "rb") as f:
                        self._ov[ovid] = (f.read(), rec["base_address"])
        return self._ov[ovid]


def _u32(img, base, addr):
    o = addr - base
    if o < 0 or o + 4 > len(img):
        return None
    return struct.unpack_from("<I", img, o)[0]


def _ror32(v, n):
    n &= 31
    return ((v >> n) | (v << (32 - n))) & 0xFFFFFFFF


def data_loads(img, base, addr, size):
    """Ordered, de-duplicated DS addresses an ARM body reaches.

    Three instruction forms carry an address on this compiler:
      LDR Rd, [PC, #imm]   the literal pool word AT pc+8+-imm IS the address
      ADR Rd, label        ADD/SUB Rd, PC, #rotated-imm8, the address itself
      B / BL label         pc+8 + (sign-extended imm24 << 2), the callee

    BRANCHES ARE NOT OPTIONAL HERE. Without them every alias whose target is a
    FUNCTION reads as a mismatch on every TU that calls it, because a call is a
    BL and never appears in the literal pool. The first version of this file
    left them out and reported 33 collisions, of which about thirty were
    ordinary callers of AngleDiff, KillPlayer, Vec3_Add and friends. A tool
    with that much noise gets ignored, and then it might as well not exist.

    Decoded by hand rather than through a disassembler so the audit has no
    import that can be missing on a lane box. The encodings are fixed and the
    whole game is ARM; a thumb body is refused upstream rather than misread.
    """
    out = []
    for pc in range(addr, addr + size, 4):
        w = _u32(img, base, pc)
        if w is None:
            break
        if (w >> 28) == 0xF:
            continue                      # unconditional-instruction space
        v = None
        if ((((w >> 26) & 3) == 1) and (((w >> 25) & 1) == 0)
                and (((w >> 20) & 1) == 1) and (((w >> 22) & 1) == 0)
                and (((w >> 16) & 0xF) == 15)):
            imm = w & 0xFFF
            lit = pc + 8 + (imm if (w >> 23) & 1 else -imm)
            v = _u32(img, base, lit)
        elif ((((w >> 25) & 7) == 1) and (((w >> 16) & 0xF) == 15)
              and (((w >> 21) & 0xF) in (2, 4))):
            val = _ror32(w & 0xFF, ((w >> 8) & 0xF) * 2)
            v = pc + 8 + val if ((w >> 21) & 0xF) == 4 else pc + 8 - val
        elif ((w >> 25) & 7) == 5:                       # B / BL
            imm = w & 0xFFFFFF
            if imm & 0x800000:
                imm -= 0x1000000
            v = pc + 8 + (imm << 2)
        if v is None or not (DS_LO <= v < DS_HI):
            continue
        if v not in out:
            out.append(v)
    return out


def tu_site(stem, syms):
    """(module, addr, size, why-not) for a src TU stem."""
    m = FUNC_OV.match(stem)
    if m:
        mod, addr = "ov" + m.group(1), int(m.group(2), 16)
        row = syms.get(stem)
        size = row[2] if row else None
        if size is None:
            return mod, addr, None, "no size for %s in config/arm9" % stem
        return mod, addr, size, None
    m = FUNC_ADDR.match(stem)
    if m:
        row = syms.get(stem)
        if not row:
            return "arm9", int(m.group(1), 16), None, \
                "no config row for %s" % stem
        return row[0], row[1], row[2], (None if row[2] else
                                        "no size for %s" % stem)
    row = syms.get(stem)
    if not row:
        return None, None, None, "no config symbol named %s" % stem
    mod, addr, size, kind = row
    if kind[1] != "arm":
        return mod, addr, size, "%s is %s, not arm" % (stem, kind[1])
    if size is None:
        return mod, addr, None, "no size for %s in config/arm9" % stem
    return mod, addr, size, None


def target_address(rhs_name, syms):
    """The ROM address an alias target names, or (None, reason)."""
    m = DATA_NAME.match(rhs_name)
    if m:
        return int(m.group(2), 16), None
    row = syms.get(rhs_name)
    if row:
        return row[1], None
    m = FUNC_OV.match(rhs_name) or FUNC_ADDR.match(rhs_name)
    if m:
        return int(m.groups()[-1], 16), None
    return None, "%s is a host symbol, it has no ROM address" % rhs_name


SSFP = re.compile(r"set_source_files_properties\s*\((.*?)\)", re.S)
QUOTED = re.compile(r"\"([^\"]*)\"")


def per_source_renames(root):
    """{TU stem: {spelled name: replacement}} from port/CMakeLists.txt.

    THE REMEDY FOR THIS TRAP IS A PER-SOURCE -D, so a tool that does not read
    them reports every already-fixed TU as broken. src/func_ov100_02147054.c is
    the live example: the CMake block near "G0=port_pathlift_file0" compiles it
    with G0/G1/G2 renamed to private names, so that TU never references the
    global G0 at all and flagging it is a false positive -- exactly the kind
    that trains people to ignore the tool.
    """
    out = {}
    path = os.path.join(root, "port", "CMakeLists.txt")
    try:
        with open(path, errors="replace") as f:
            text = f.read()
    except OSError:
        return out
    for block in SSFP.findall(text):
        if "COMPILE_DEFINITIONS" not in block:
            continue
        head, _s, tail = block.partition("COMPILE_DEFINITIONS")
        stems = []
        for q in QUOTED.findall(head):
            if q.endswith((".c", ".cpp")):
                stems.append(os.path.splitext(os.path.basename(q))[0])
        defs = {}
        for q in QUOTED.findall(tail):
            for one in q.split(";"):
                if "=" in one:
                    k, v = one.split("=", 1)
                    defs[k.strip()] = v.strip()
        for stem in stems:
            out.setdefault(stem, {}).update(defs)
    return out


def all_aliases(root):
    """{spelled name: [target name]} over every /alternatename in port/.

    cxx_aliases.cpp is the registry, but a per-source rename points at a
    private name that a class's own host TU binds (hal/actor_classes_ov100pl.cpp
    holds the pathlift ones), so resolving a rename to a ROM address needs the
    whole set.
    """
    out = {}
    for dirpath, _d, files in os.walk(os.path.join(root, "port")):
        for fn in files:
            if not fn.endswith((".cpp", ".c", ".h")):
                continue
            try:
                with open(os.path.join(dirpath, fn), errors="replace") as f:
                    text = f.read()
            except OSError:
                continue
            for lhs, rhs in ALIAS.findall(text):
                out.setdefault(source_name(lhs), []).append(source_name(rhs))
    return out


_STRIP = re.compile(
    r"//[^\n]*"                     # line comment
    r"|/\*.*?\*/"                   # block comment
    r"|\"(?:\\.|[^\"\\])*\""        # string literal
    r"|'(?:\\.|[^'\\])*'",          # char literal
    re.S)


def code_only(text):
    """Source with comments and literals blanked out.

    NOT cosmetic. src/_ZN11CannonHatch16CleanupResourcesEv.cpp was already
    fixed off the placeholders and now spells its two SharedFilePtrs by
    address, but its header comment explains the old G0/G1 bug at length. A
    plain identifier search over the raw text reads that comment as a live
    reference and reports a MISMATCH on a file that has nothing wrong with it.
    Newlines are preserved so nothing else shifts.
    """
    return _STRIP.sub(lambda m: re.sub(r"[^\n]", " ", m.group(0)), text)


_TYPEDEF = re.compile(r"\btypedef\b")


def tag_only(name, code):
    """True when every mention of `name` is a struct/union/enum TAG.

    C keeps tag names in their own namespace, so `struct VT { ... }` and the
    placeholder object VT can coexist in one TU -- but a TU that only ever
    writes `struct VT` never references the object, and reporting it is a false
    finding. src/func_ov004_020b75e4.c and its two siblings are exactly that:
    they declare `struct VT` for a vtable shape and were reported as MISMATCH
    rows against the VT binding for as long as this tool has run, while their
    objects carry no undefined _VT at all.

    Conservative in the safe direction: one mention that is NOT preceded by a
    tag keyword and the TU is treated as a real user, so the worst case is the
    old behaviour.
    """
    word = re.compile(r"\b%s\b" % re.escape(name))
    tagged = re.compile(r"\b(?:struct|union|enum)\s+%s\b" % re.escape(name))
    spans = {m.start() for m in tagged.finditer(code)}
    for m in word.finditer(code):
        # the tag match starts at the keyword, so the name sits further in
        if not any(s <= m.start() < s + len(m.group(0)) + 16 and
                   code[s:m.end()].split()[-1] == name for s in spans):
            return False
    return True
# The two declarator shapes a typedef name can end in: a plain identifier
# (with optional array suffix), or a function/array-pointer's `(*name)`.
_TD_PLAIN = re.compile(r"(\w+)\s*(?:\[[^\]]*\])*\s*$")
_TD_PTR = re.compile(r"\(\s*\**\s*(\w+)\s*\)\s*(?:\([^)]*\)|\[[^\]]*\])\s*$")


def typedef_names(code):
    """Every identifier this TU introduces with a `typedef`.

    Walks each `typedef` forward to the semicolon that ends it, tracking brace
    and paren depth so the `;` inside `typedef struct { int a; } NAME;` does
    not end the scan early -- a naive `[^;]*` stops at the struct's first
    member and never sees the name at all, which is exactly the case this was
    written for. The span is then split on top-level commas and each declarator
    reduced to the identifier it names.

    Approximate on purpose, in the safe direction: a declarator this cannot
    reduce contributes nothing, so the worst case is the old behaviour.
    """
    out = set()
    for m in _TYPEDEF.finditer(code):
        i = m.end()
        depth = 0
        while i < len(code):
            ch = code[i]
            if ch in "{([":
                depth += 1
            elif ch in "})]":
                depth -= 1
            elif ch == ";" and depth <= 0:
                break
            i += 1
        span = code[m.end():i]
        # split on commas at depth 0: `typedef int A, B;`
        parts, depth, start = [], 0, 0
        for k, ch in enumerate(span):
            if ch in "{([":
                depth += 1
            elif ch in "})]":
                depth -= 1
            elif ch == "," and depth == 0:
                parts.append(span[start:k])
                start = k + 1
        parts.append(span[start:])
        for p in parts:
            p = p.strip()
            hit = _TD_PTR.search(p) or _TD_PLAIN.search(p)
            if hit:
                out.add(hit.group(1))
    return out


def src_files(root):
    """[(stem, path)] for every TU under src/."""
    out = []
    for dirpath, _d, files in os.walk(os.path.join(root, "src")):
        for fn in files:
            if fn.endswith((".c", ".cpp")):
                out.append((os.path.splitext(fn)[0],
                            os.path.join(dirpath, fn)))
    return out


def main():
    args = list(sys.argv[1:])
    verbose = "-v" in args
    if verbose:
        args.remove("-v")
    show_all = "--all" in args
    if show_all:
        args.remove("--all")
    only = None
    if "--name" in args:
        i = args.index("--name")
        only = args[i + 1]
        del args[i:i + 2]
    bad = [a for a in args if a.startswith("--")]
    if bad:
        sys.exit("alias_audit: unknown flag(s): %s" % " ".join(bad))
    root = os.path.abspath(args[0] if args else default_root())

    aliases_path = os.path.join(root, "port", "hal", "cxx_aliases.cpp")
    if not os.path.isfile(aliases_path):
        sys.exit("alias_audit: no %s" % aliases_path)
    print("repo root  : %s" % root)
    print("bindings   : %s" % aliases_path)

    with open(aliases_path, errors="replace") as f:
        text = f.read()
    bindings = {}          # spelled name -> {target source name: [decorated lhs]}
    for lhs, rhs in ALIAS.findall(text):
        name = source_name(lhs)
        if not is_placeholder(name):
            continue
        if only and name != only:
            continue
        bindings.setdefault(name, {}).setdefault(source_name(rhs), []).append(lhs)

    syms = symbol_index(root)
    images = Images(root)
    renames = per_source_renames(root)
    aliases = all_aliases(root)
    print("renames    : %d TU(s) carry a per-source -D in port/CMakeLists.txt"
          % len(renames))

    # Which TUs actually reach the binary, so a collision can be called live or
    # latent. A map is optional here -- the audit is a source-level check and
    # must still run on a tree that has never been built.
    linked = set()
    mapfile = os.path.join(root, "build", "port", "walk_window.map")
    if os.path.isfile(mapfile) and os.path.getsize(mapfile) >= 4096:
        for _s, obj in linkage.map_symbols(mapfile):
            base = obj.split(":", 1)[-1]
            linked.add(os.path.splitext(os.path.splitext(base)[0])[0])
        print("map        : %s (%d linked objects)" % (mapfile, len(linked)))
    else:
        print("map        : none readable, link status will read 'unknown'")

    tus = src_files(root)
    print("src TUs    : %d" % len(tus))
    print("spelled    : %d placeholder name(s) bound in cxx_aliases.cpp"
          % len(bindings))
    print()

    flagged = 0
    audited = 0
    live_defects = []
    for name in sorted(bindings):
        word = re.compile(r"\b%s\b" % re.escape(name))
        users = []
        shadowed = []
        for stem, path in tus:
            try:
                with open(path, errors="replace") as f:
                    body = f.read()
            except OSError:
                continue
            code = code_only(body)
            if not word.search(code):
                continue
            # A TU that typedefs this name means its own type by it. The
            # placeholder is an object or a function, and a typedef name and an
            # object name share C's ordinary identifier namespace, so the two
            # cannot both be live in one TU. Recorded, not silently dropped.
            if name in typedef_names(code):
                shadowed.append(stem)
                continue
            # ...and the same for a TU that only ever uses the name as a
            # struct/union/enum tag. Recorded next to the typedef shadows
            # rather than silently dropped, for the same reason.
            if tag_only(name, code):
                shadowed.append(stem)
                continue
            users.append((stem, path, body))
        if not users:
            continue
        audited += 1

        targets = bindings[name]
        tgt_addrs = {}
        for tname in targets:
            tgt_addrs[tname] = target_address(tname, syms)

        rows = []
        for stem, path, body in users:
            renamed = renames.get(stem, {}).get(name)
            mod, addr, size, why = tu_site(stem, syms)
            if why:
                rows.append((stem, mod, addr, None, "UNRESOLVED", why))
                continue
            img, base = images.for_module(mod)
            if img is None:
                rows.append((stem, mod, addr, None, "UNRESOLVED", base))
                continue
            loads = data_loads(img, base, addr, size)
            if not loads:
                rows.append((stem, mod, addr, [], "UNRESOLVED",
                             "body at 0x%08x loads no DS address" % addr))
                continue
            if renamed:
                # This TU does not reference the shared name at all. Check the
                # rename's own target instead, resolved one alias hop.
                chain = [renamed] + aliases.get(renamed, [])
                got = [target_address(c, syms)[0] for c in chain]
                got = [a for a in got if a is not None]
                ok = any(a in loads for a in got)
                rows.append((stem, mod, addr, loads,
                             "renamed-ok" if ok else "RENAMED-MISMATCH",
                             "%s -> %s" % (name, " -> ".join(chain))))
                continue
            # The TU that DEFINES the target is not a reference to it. Without
            # this every role alias whose target is a real function flags its
            # own definition: KillPlayer at 0x02029b30 is bound to 0x02029b30
            # and does not call itself, so it read as a mismatch against the
            # binding that names it.
            if any(a == addr for a, _r in tgt_addrs.values()):
                rows.append((stem, mod, addr, loads, "definition", None))
                continue
            hit = [t for t, (a, _r) in tgt_addrs.items()
                   if a is not None and a in loads]
            verdict = "consistent" if hit else "MISMATCH"
            rows.append((stem, mod, addr, loads, verdict, hit[0] if hit else None))

        mismatched = [r for r in rows if r[4] == "MISMATCH"]
        consistent = [r for r in rows if r[4] == "consistent"]
        unresolved = [r for r in rows if r[4] == "UNRESOLVED"]
        renamed_ok = [r for r in rows if r[4] == "renamed-ok"]
        renamed_bad = [r for r in rows if r[4] == "RENAMED-MISMATCH"]
        defining = [r for r in rows if r[4] == "definition"]
        mismatched += renamed_bad

        # THE LIVE HALF, collected across every name and reprinted at the end.
        # A MISMATCH row on a TU that does not reach the linker is a note about
        # a file that is not in the binary; a MISMATCH row on a LINKED TU is a
        # wrong pointer in the shipping build today. Run mg6 lane VTF lost a
        # defect in the gap between those two: the VT section prints nineteen
        # MISMATCH rows and exactly one of them was live and unrepaired
        # (src/func_ov006_020dbe64.c, dScMgCoin_c's slot 17). Burying it in
        # nineteen is the same as not reporting it, so it gets its own list.
        for r in mismatched:
            if r[0] in linked:
                live_defects.append((name, r))

        # A collision needs two TUs that cannot both be right about one name.
        collide = bool(mismatched) and bool(consistent or len(mismatched) > 1)
        if not collide and not show_all and not verbose:
            continue
        if collide:
            flagged += 1

        head = "COLLISION" if collide else "ok"
        print("=== %-24s %s ===" % (name, head))
        for tname, (a, reason) in sorted(tgt_addrs.items()):
            where = ("0x%08x" % a) if a is not None else "(%s)" % reason
            print("    bound to %-34s %s   [%s]"
                  % (tname, where, ", ".join(sorted(targets[tname]))))
        print("    %d TU(s) spell it: %d MISMATCH, %d consistent, %d define "
              "the target, %d already renamed per-source, %d unresolved"
              % (len(rows), len(mismatched), len(consistent), len(defining),
                 len(renamed_ok), len(unresolved)))
        if shadowed:
            print("    %d TU(s) typedef this name and mean their own type, so "
                  "they are" % len(shadowed))
            print("    not users of the placeholder: %s"
                  % ", ".join(sorted(shadowed)))

        def show(rs, label):
            for stem, mod, addr, loads, _v, extra in rs:
                live = ("LINKED" if stem in linked
                        else ("not linked" if linked else "link unknown"))
                print("      %-9s %-36s %-6s 0x%08x  %s"
                      % (label, stem, mod or "?", addr or 0, live))
                if loads:
                    print("               loads %s"
                          % " ".join("0x%08x" % v for v in loads))
                    if label == "MISMATCH":
                        idx = placeholder_index(name)
                        if idx is not None and idx < len(loads):
                            print("               likely %s = 0x%08x (load "
                                  "order, a suggestion not a finding)"
                                  % (name, loads[idx]))
                    if isinstance(extra, str) and "->" in extra:
                        print("               %s" % extra)
                elif extra and label == "UNRESOLVED":
                    print("               %s" % extra)

        show(mismatched, "MISMATCH")
        if renamed_ok and (verbose or collide):
            show(renamed_ok, "renamed")
        if verbose or collide:
            show(consistent, "consistent")
        if unresolved:
            show(unresolved, "UNRESOLVED")
        print()

    print("audited %d spelled name(s) that at least one src TU references"
          % audited)
    print("%d collision(s)" % flagged)

    # THE HEADLINE, printed last so a log tail carries it.
    print()
    if live_defects:
        print("LIVE: %d LINKED TU(s) whose ROM body contradicts the binding "
              "they compile against." % len(live_defects))
        print("Each one is a wrong pointer in the shipping build. A per-source "
              "-D in port/CMakeLists.txt")
        print("onto the address the body actually loads is the established "
              "repair.")
        for name, (stem, mod, addr, loads, _v, _x) in sorted(live_defects):
            print("    %-6s %-36s %-6s 0x%08x" % (name, stem, mod or "?",
                                                  addr or 0))
            if loads:
                print("           loads %s"
                      % " ".join("0x%08x" % v for v in loads))
    else:
        print("LIVE: no LINKED TU contradicts its binding.")
        print("That is not a clean bill of health in either direction. This "
              "tool reads SOURCE TEXT for")
        print("who spells a name and cannot see who reaches the linker; the "
              "closure question -- have ALL")
        print("readers been found -- is answered by a COFF census for the "
              "UNDEFINED external over the built")
        print("objects. `consistent` only means the bound address appears "
              "among the ones the body loads.")

    if not flagged:
        print("No spelled name has a TU whose ROM body contradicts its binding.")
    return 1 if (flagged or live_defects) else 0


PLACEHOLDER_N = re.compile(r"^[A-Za-z]+(\d+)$")


def placeholder_index(name):
    """The trailing number on G0/G1/VT0, or None. Used only for the hint."""
    m = PLACEHOLDER_N.match(name)
    return int(m.group(1)) if m else None


if __name__ == "__main__":
    sys.exit(main())
