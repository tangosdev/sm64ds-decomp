#!/usr/bin/env python3
"""Census of the ARM-vs-x86 narrow-return trap, by DEFINITION, across src/ and port/.

On the DS the callee sign- or zero-extends a narrow (s8/u8/s16/u16/bool) return
into the whole of r0 before it returns. MSVC x86 leaves the upper bytes of EAX
unspecified for a narrow return, so a caller whose declaration names a WIDER
return than the body it reaches reads junk in those bytes on the PC.

What this counts, and how it differs from the first census (which keyed on any
line that began with a narrow type and a name, so a narrow PROTOTYPE in a caller
counted as the definition -- AngleDiff and dBgActor_c::IsClsnInRange are both
DEFINED int and were both listed):

  * a DEFINITION is a function header followed by its body `{`, found in src/
    (the matched bodies) and in port/ (host copies and seams, which a caller
    can reach instead of the src body);
  * its return width is read from the header, with `bool` narrow only where
    the host compiles the TU as C++ (types.h makes it `int` in C);
  * a DECLARER is any other declaration or definition of the same flat name
    (or, for a C++ member, of the same member inside a class body naming the
    same class), anywhere under src/, include/ and port/, and it is WIDE when
    its return type is a full register;
  * every /alternatename row naming the function is listed with the MSVC
    return code of each decorated side;
  * for each wide declarer the lines that USE the result are listed.

Usage:  python port/tools/narrowret_census.py <tree> [--map build/port/walk_window.map]
                                   [--out-txt F] [--out-tsv F]
"""
import argparse
import os
import re
import sys

NARROW_TYPES = {
    "s8": ("s8", True), "u8": ("u8", False), "s16": ("s16", True),
    "u16": ("u16", False), "short": ("s16", True), "signed short": ("s16", True),
    "short int": ("s16", True), "signed short int": ("s16", True),
    "unsigned short": ("u16", False), "unsigned short int": ("u16", False),
    "char": ("s8", True), "signed char": ("s8", True),
    "unsigned char": ("u8", False), "bool": ("bool", False),
    "BOOL8": ("u8", False),
}
WIDE_TYPES = {"s32", "u32", "int", "unsigned", "unsigned int", "signed int",
              "long", "unsigned long", "signed", "BOOL", "Fix12i", "OnYoshiEatReturnVal"}

TYPE_RE = (r"(?:(?:const|volatile)\s+)*"
           r"(?:unsigned\s+short(?:\s+int)?|signed\s+short(?:\s+int)?|short(?:\s+int)?|"
           r"unsigned\s+char|signed\s+char|char|unsigned\s+int|signed\s+int|"
           r"unsigned\s+long|unsigned|signed|long|int|bool|BOOL|BOOL8|Fix12i|"
           r"OnYoshiEatReturnVal|s8|u8|s16|u16|s32|u32)")
# A function header: optional storage words, a return type, a (possibly
# qualified) name, a parenthesised parameter list with no ; { } inside.
HDR_RE = re.compile(
    r"(?m)^[ \t]*(?P<pre>(?:(?:extern\s+\"C\"|extern|static|inline|__inline|"
    r"virtual|__declspec\(\w+\))\s+)*)"
    r"(?P<ret>" + TYPE_RE + r")\s+(?P<name>(?:\w+::)*~?\w+)\s*"
    r"\((?P<params>[^;{}()]*(?:\([^;{}()]*\)[^;{}()]*)*)\)\s*(?:const\s*)?"
    r"(?P<tail>[;{])")

COMMENT_RE = re.compile(r"//[^\n]*|/\*.*?\*/", re.S)
ALT_RE = re.compile(r"/alternatename:([^=\s\"]+)=([^\s\"]+)")
SKIP_NAMES = {"if", "for", "while", "switch", "return", "sizeof", "main"}

# MSVC decorated return codes (after the calling-convention letter).
MSVC_RET = {"C": "s8", "D": "char", "E": "u8", "F": "s16", "G": "u16",
            "H": "int", "I": "u32", "J": "long", "K": "ulong", "_N": "bool",
            "X": "void", "M": "float", "N": "double"}


def strip_comments(t):
    # Keep line numbers: replace each comment by the same number of newlines.
    return COMMENT_RE.sub(lambda m: "\n" * m.group(0).count("\n"), t)


def norm_type(s):
    s = re.sub(r"\b(const|volatile)\b", "", s)
    return " ".join(s.split())


def class_at(text, pos, stack_cache):
    """Innermost class/struct whose body encloses pos (crude brace walk)."""
    stack = []
    i = 0
    tok = re.compile(r"\b(class|struct)\s+(\w+)[^;{()]*\{|\{|\}")
    for m in tok.finditer(text, 0, pos):
        if m.group(0) == "}":
            if stack:
                stack.pop()
        elif m.group(1):
            stack.append(m.group(2))
        else:
            stack.append(None)
    for c in reversed(stack):
        if c:
            return c
    return None


def msvc_ret_code(dec):
    """Return-type code of a decorated MSVC function name, or None."""
    m = re.match(r"\?[^@]+@(?:[^@]+@)*@([A-Z]{2,3}|Y[AGI])(.*)$", dec)
    if not m:
        m2 = re.match(r"\?\w+@@Y[AGI](.*)$", dec)
        if not m2:
            return None
        rest = m2.group(1)
    else:
        rest = m.group(2)
    if rest.startswith("_N"):
        return "bool"
    return MSVC_RET.get(rest[:1])


def load_hostgen_list(tree):
    p = os.path.join(tree, "port", "CMakeLists.txt")
    try:
        t = open(p, encoding="utf-8", errors="replace").read()
    except OSError:
        return set()
    m = re.search(r"set\(PORT_HOSTGEN_TU\n(.*?)\)", t, re.S)
    s = set()
    if m:
        for line in m.group(1).splitlines():
            line = line.strip()
            if line and not line.startswith("#"):
                s.add(line)
    for var in ("SAMECLASS_GX_TU", "SAMECLASS_BOOT_TU"):
        m = re.search(r"set\(" + var + r"\n(.*?)\)", t, re.S)
        if m:
            for line in m.group(1).split():
                if line.startswith("src/"):
                    s.add(line)
    return s


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("tree")
    ap.add_argument("--map", default=None)
    ap.add_argument("--out-txt", default=None)
    ap.add_argument("--out-tsv", default=None)
    a = ap.parse_args()
    tree = os.path.abspath(a.tree)
    hostgen = load_hostgen_list(tree)
    rom = set()
    for root, dirs, names in os.walk(os.path.join(tree, "config")):
        for n in names:
            if n == "symbols.txt":
                for line in open(os.path.join(root, n), encoding="utf-8", errors="replace"):
                    m = re.match(r"(\S+) kind:function", line)
                    if m:
                        rom.add(m.group(1))
    routes = {}
    bn = os.path.join(tree, "build", "port", "build.ninja")
    if os.path.exists(bn):
        for line in open(bn, encoding="utf-8", errors="replace"):
            m = re.match(r"build CMakeFiles[\\/](port_slice_shared|walk_window)\.dir[\\/](\S+?)\.obj:", line)
            if not m:
                continue
            path = m.group(2).replace("\\", "/").replace("$:", ":")
            i = path.find("host-src/src/")
            if i >= 0:
                routes["src/" + path[i + len("host-src/src/"):]] = "hostgen-C++"
                continue
            j = path.find("/src/")
            k = path.find("/port/")
            if j >= 0 and path.endswith((".c", ".cpp")):
                routes.setdefault("src/" + path[j + 5:], "raw-" + ("C" if path.endswith(".c") else "C++"))

    files = []
    for top in ("src", "include", "port"):
        for root, dirs, names in os.walk(os.path.join(tree, top)):
            dirs[:] = [d for d in dirs if d not in ("build", ".git", "tmp")]
            for n in names:
                if n.endswith((".c", ".cpp", ".h", ".hpp", ".inc")):
                    files.append(os.path.join(root, n))

    linked = set()
    if a.map and os.path.exists(a.map):
        for line in open(a.map, encoding="utf-8", errors="replace"):
            m = re.match(r"\s*[0-9a-f]{4}:[0-9a-f]{8}\s+\S+\s+[0-9a-f]{8}\s+(?:f\s+)?(?:i\s+)?(\S+)\s*$", line)
            if m:
                linked.add(m.group(1).split(":")[-1].lower())

    def rel(f):
        return os.path.relpath(f, tree).replace(os.sep, "/")

    def route_of(f):
        r = rel(f)
        if r.startswith("src/"):
            for ext in (".c", ".cpp"):
                pass
            base = r[:-2] if r.endswith(".c") else r[:-4]
            if (base + ".cpp") in routes and routes[base + ".cpp"] == "hostgen-C++":
                return "hostgen-C++"
            return routes.get(r, "unbuilt")
        return "port-C" if r.endswith(".c") else "port-C++"

    def lang_of(f):
        return "C" if route_of(f) in ("raw-C", "port-C") else "C++"

    _fn_cache = {}

    def flat_names(key):
        if key in _fn_cache:
            return _fn_cache[key]
        _fn_cache[key] = _flat_names(key)
        return _fn_cache[key]

    def _flat_names(key):
        if "::" not in key:
            return [key]
        parts = key.split("::")
        cls, meth = parts[-2], parts[-1]
        pre = "_ZN%d%s%d%s" % (len(cls), cls, len(meth), meth)
        prek = "_ZNK%d%s%d%s" % (len(cls), cls, len(meth), meth)
        return sorted(n for n in rom if n.startswith(pre + "E") or n.startswith(prek + "E"))

    def is_linked(f):
        if not linked:
            return "?"
        b = os.path.basename(f).lower()
        return "yes" if (b + ".obj") in linked else "no"

    texts = {}
    headers = []   # (file, line, pre, ret, name, params, tail, cls)
    for f in files:
        try:
            raw = open(f, encoding="utf-8", errors="replace").read()
        except OSError:
            continue
        t = strip_comments(raw)
        texts[f] = (raw, t)
        for m in HDR_RE.finditer(t):
            name = m.group("name")
            short = name.split("::")[-1]
            if short in SKIP_NAMES:
                continue
            line = t.count("\n", 0, m.start("name")) + 1
            cls = None
            if "::" in name:
                cls = name.split("::")[-2]
            elif f.endswith((".cpp", ".h", ".hpp", ".inc")):
                cls = class_at(t, m.start(), None)
            headers.append((f, line, m.group("pre"), norm_type(m.group("ret")),
                            name, m.group("params"), m.group("tail"), cls))

    # Narrow DEFINITIONS: a body, not static (a static cannot be declared
    # wider elsewhere), not inside a class body unless qualified.
    defs = {}
    for f, line, pre, ret, name, params, tail, cls in headers:
        if tail != "{" or "static" in pre:
            continue
        if f.endswith((".h", ".hpp")) and "::" not in name:
            continue
        if ret not in NARROW_TYPES:
            continue
        width, signed = NARROW_TYPES[ret]
        lang = lang_of(f)
        if ret == "bool" and lang == "C":
            continue          # typedef int bool in C: a full register
        key = name if "::" in name else name
        defs.setdefault(key, []).append((f, line, ret, width, signed, lang, cls))

    # Also every WIDE definition, so a narrow-declared caller of a wide body
    # (the reverse class, out of scope) can be listed as a finding.
    wide_defs = {}
    for f, line, pre, ret, name, params, tail, cls in headers:
        if tail == "{" and "static" not in pre and ret in WIDE_TYPES:
            wide_defs.setdefault(name, []).append((f, line, ret))

    # Keep ROM functions only: a flat name the config knows, or a member whose
    # flat Itanium name the config knows. Host-only helpers are not the trap.
    defs = {k: v for k, v in defs.items()
            if (("::" in k and flat_names(k)) or ("::" not in k and k in rom))}

    # Declarers of each narrow definition: every other header naming it --
    # the flat name (a C spelling, an Itanium string used as a C name, or a
    # member's flat name), or for a member the same member of the same class.
    rows = []
    for key, dl in sorted(defs.items()):
        short = key.split("::")[-1]
        dcls = key.split("::")[-2] if "::" in key else None
        flats = set(flat_names(key))
        for f, line, pre, ret, name, params, tail, cls in headers:
            nshort = name.split("::")[-1]
            hit = False
            if name in flats and "::" not in name:
                hit = True
            elif dcls is not None and nshort == short:
                owner = name.split("::")[-2] if "::" in name else cls
                hit = owner == dcls
            if not hit:
                continue
            if any(f == df and line == dline for df, dline, *_ in dl):
                continue
            kind = "def" if tail == "{" else "decl"
            wide = ret in WIDE_TYPES
            if ret == "bool" and lang_of(f) == "C":
                wide = True
            rows.append((key, f, line, kind, ret, wide))

    # /alternatename rows.
    alts = []
    for f in files:
        raw, t = texts.get(f, ("", ""))
        for m in ALT_RE.finditer(raw):
            alts.append((f, raw.count("\n", 0, m.start()) + 1, m.group(1), m.group(2)))

    out = []
    tsv = ["function\tdef_file:line\tdef_ret\twidth\tsigned\tdef_lang\t"
           "declarer_file:line\tdecl_kind\tdecl_ret\twide\tlinked\tuse_line\tuse_expr"]
    nwide = 0
    wide_funcs = set()
    wide_files = set()
    for key, dl in sorted(defs.items(), key=lambda kv: kv[0].lower()):
        drows = [r for r in rows if r[0] == key]
        wrows = [r for r in drows if r[5]]
        d0 = dl[0]
        out.append("== %s  defined %s (%s%s) at %s  [%s, %s]  flat: %s" % (
            key, d0[2], d0[3], ", signed" if d0[4] else ", unsigned",
            ", ".join("%s:%d" % (rel(x[0]), x[1]) for x in dl), d0[5],
            route_of(d0[0]), " ".join(flat_names(key)) or "-"))
        short = key.split("::")[-1]
        for fr, af, al, lhs, rhs in [(x[0], x[0], x[1], x[2], x[3]) for x in alts]:
            pass
        for af, al, lhs, rhs in alts:
            if any(x in lhs or x in rhs for x in [short] + flat_names(key)):
                out.append("   alias %s:%d  %s [%s] = %s [%s]" % (
                    rel(af), al, lhs, msvc_ret_code(lhs) or "C",
                    rhs, msvc_ret_code(rhs) or "C"))
        for _, f, line, kind, ret, wide in sorted(drows, key=lambda r: (rel(r[1]), r[2])):
            tag = "WIDE" if wide else ("narrow" if ret in NARROW_TYPES else "other")
            out.append("   %-6s %s:%d  %s %s  linked=%s" % (
                tag, rel(f), line, kind, ret, is_linked(f)))
            if wide:
                nwide += 1
                wide_funcs.add(key)
                wide_files.add(rel(f))
                raw, t = texts[f]
                raw_lines = raw.splitlines()
                call_re = re.compile(r"\b" + re.escape(short) + r"\s*\(")
                uses = []
                for cm in call_re.finditer(t):
                    ln = t.count("\n", 0, cm.start()) + 1
                    if ln == line:
                        continue
                    src_line = raw_lines[ln - 1].strip() if ln - 1 < len(raw_lines) else ""
                    if re.match(r"^(extern\s+)?(\"C\"\s+)?" + TYPE_RE + r"\s+\w+\s*\(", src_line):
                        continue
                    uses.append((ln, src_line))
                for ln, s in uses:
                    out.append("          %d: %s" % (ln, s[:160]))
                    tsv.append("\t".join([key, "%s:%d" % (rel(d0[0]), d0[1]), d0[2], d0[3],
                                          "s" if d0[4] else "u", d0[5],
                                          "%s:%d" % (rel(f), line), kind, ret, "WIDE",
                                          is_linked(f), str(ln), s.replace("\t", " ")[:200]]))
                if not uses:
                    tsv.append("\t".join([key, "%s:%d" % (rel(d0[0]), d0[1]), d0[2], d0[3],
                                          "s" if d0[4] else "u", d0[5],
                                          "%s:%d" % (rel(f), line), kind, ret, "WIDE",
                                          is_linked(f), "-", "(no use in this file)"]))
        out.append("")

    # Reverse-class findings: a narrow declaration of a WIDE definition.
    rev = []
    for name, wl in wide_defs.items():
        short = name.split("::")[-1]
        for f, line, pre, ret, nm, params, tail, cls in headers:
            if nm != name or tail != ";" or ret not in NARROW_TYPES:
                continue
            if ret == "bool" and lang_of(f) == "C":
                continue
            rev.append((name, wl[0], f, line, ret))
    head = ["narrow-return DEFINITIONS (with a body) in src/ and port/: %d" % len(defs),
            "wide declarers of them: %d, of %d functions, in %d files" % (
                nwide, len(wide_funcs), len(wide_files)),
            "reverse-class findings (a narrow DECLARATION of a body defined wide; "
            "out of scope): %d" % len(rev), ""]
    out = head + out
    out.append("== REVERSE CLASS (finding only): narrow declaration of a wide definition")
    for name, (wf, wl, wr), f, line, ret in sorted(rev, key=lambda r: (r[0], rel(r[2]), r[3])):
        out.append("   %s defined %s at %s:%d; declared %s at %s:%d" % (
            name, wr, rel(wf), wl, ret, rel(f), line))
    text = "\n".join(out) + "\n"
    if a.out_txt:
        open(a.out_txt, "w", encoding="utf-8").write(text)
    if a.out_tsv:
        open(a.out_tsv, "w", encoding="utf-8").write("\n".join(tsv) + "\n")
    print("\n".join(head))


if __name__ == "__main__":
    main()
