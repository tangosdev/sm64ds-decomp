"""Resolve the placeholder names an earlier decompilation pass left behind.

Files recovered by an automated pass carry names it never resolved:

    /* vtable identified: VT0 = _ZTV11dCapEnemy_c */
    extern void *G0;
    ...
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);

`G0` is not a symbol the ROM defines, so the reference is unresolvable and
`eligible.py` refuses to enroll the file. It survives because `match.py` compares
the relocated word as a wildcard -- the byte gate never looks at what a
relocation points to, so any name at all passes.

## Where the answer comes from

Not from disassembling. `dsd` already analysed every relocation in the ROM and
wrote the results to `config/**/relocs.txt`:

    from:0x021111e8 kind:load to:0x021099e4 module:overlay(2)

Compiling the file says which byte offset each placeholder patches; adding the
function's address gives the `from:` to look up; dsd supplies the target address
*and* the module that owns it. `symbols.txt` for that module then supplies the
name.

The module matters and cannot be skipped: overlays are overlaid, so one address
names a different symbol in each overlay that covers it. Resolving by address
alone picks between ten candidates by luck. dsd's `module:` field is the answer,
and where dsd itself could not decide it says so -- `module:overlays(2,9,14)` --
and those are reported rather than guessed at.

Nothing is rewritten unless every placeholder in the file resolves to exactly one
symbol, and every rewritten file is byte-verified and reverted on failure.

Usage:
    python tools/resolve_placeholders.py                 # report only
    python tools/resolve_placeholders.py --apply
    python tools/resolve_placeholders.py --apply --limit 100
"""
import argparse
import collections
import concurrent.futures
import io
import json
import pathlib
import re
import sys

from elftools.elf.elffile import ELFFile

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import match as M          # noqa: E402
import modules as MOD      # noqa: E402
from enroll import candidates  # noqa: E402

ELIGIBILITY = REPO / "build" / "rombuild-eligibility.json"

# The spellings the recovery pass invented for symbols it never resolved.
INVENTED = re.compile(r"^(G|VT|HEAP)\d*$")


def is_stand_in(name, header_declared):
    """Is `name` a stand-in rather than a real symbol?

    Two shapes, and the second is the larger one:

      G0, VT1, HEAP   the pass's own placeholder spellings, declared per-file or
                      in decl_common.h

      _ZTV10dBgActor_c  a name that *looks* real -- it is declared in a shared
                      header, so it reads as a proper symbol -- but no module
                      defines it. 196 files reference that one name, and the
                      relocations show they mean 196 different symbols. One
                      declaration standing in for many, exactly like VT, but
                      spelled plausibly enough that nobody noticed.

    Both are reached the same way: the name resolves to nothing, and the ROM's
    own relocation says what was meant. A name absent from the headers *and*
    from the config is left alone -- there is no declaration to carry, and it is
    likelier to be a genuine gap than a stand-in."""
    return bool(INVENTED.match(name)) or name in header_declared

def decl_types():
    """{name: declaration template} for every name the headers declare.

    Read rather than hardcoded: the recovery pass emitted G0, G1, G2, ... as it
    needed them, and a fixed table silently misses whichever it invented last."""
    out = {}
    for h in (REPO / "include").rglob("*.h"):
        for m in re.finditer(r"^\s*extern\s+([^;=]*?)\b(\w+)\s*(\[\s*\])?\s*;",
                             h.read_text(encoding="utf-8", errors="ignore"), re.M):
            ty, nm, arr = m.group(1).strip(), m.group(2), m.group(3) or ""
            # Every declared name, not only the invented spellings: a stand-in
            # that lives in a shared header needs its template carried across to
            # the real name just as much as a VT does.
            out.setdefault(nm, f"extern {ty} @{arr};")
    return out


RELOC = re.compile(r"^from:(0x[0-9a-fA-F]+)\s+kind:(\S+)\s+to:(0x[0-9a-fA-F]+)\s+"
                   r"module:(\S+)")
SYMLINE = re.compile(r"^(\S+)\s+kind:\S*.*?\baddr:(0x[0-9a-fA-F]+)")
GENERIC = re.compile(r"^(data|func)_(ov\d+_)?[0-9a-f]{8}$")


def module_key(spec):
    """dsd's `module:` field -> the config directory name, or None if it is not a
    single definite module."""
    if spec == "main":
        return "arm9"
    if spec in ("itcm", "dtcm"):
        return spec
    m = re.match(r"^overlay\((\d+)\)$", spec)
    return f"ov{int(m.group(1)):03d}" if m else None


def load_relocs():
    """{module: {from_addr: (to_addr, module_spec)}}"""
    out = {}
    for p in (REPO / "config").rglob("relocs.txt"):
        mod = p.parent.name
        d = {}
        for ln in p.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = RELOC.match(ln)
            if m:
                d[int(m.group(1), 16)] = (int(m.group(3), 16), m.group(4))
        out[mod] = d
    return out


def load_symbols():
    """{module: {addr: [names]}}"""
    out = {}
    for p in (REPO / "config").rglob("symbols.txt"):
        idx = collections.defaultdict(list)
        for ln in p.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = SYMLINE.match(ln)
            if m:
                idx[int(m.group(2), 16)].append(m.group(1))
        out[p.parent.name] = idx
    return out


def name_at(syms, mod, addr):
    """The one symbol `mod` defines at `addr`, or (None, why)."""
    names = syms.get(mod, {}).get(addr)
    if not names:
        return None, f"{mod} defines nothing at {addr:#010x}"
    uniq = sorted(set(names))
    if len(uniq) == 1:
        return uniq[0], None
    # Several names for one address inside one module are aliases: identical after
    # linking, so any is correct. Prefer the one that says something.
    named = [n for n in uniq if not GENERIC.match(n)]
    return (named[0] if named else uniq[0]), None


def relocs_for(obj_bytes, func):
    """[(offset_into_func, symbol_name)] for `func`'s .text."""
    elf = ELFFile(io.BytesIO(obj_bytes))
    symtab = elf.get_section_by_name(".symtab")
    sym = next((s for s in symtab.iter_symbols() if s.name == func), None)
    if sym is None:
        return None
    shndx, start, size = sym["st_shndx"], sym["st_value"], sym["st_size"]
    out = []
    for sec in elf.iter_sections():
        if not sec.name.startswith(".rel") or sec.header["sh_info"] != shndx:
            continue
        rsymtab = elf.get_section(sec.header["sh_link"])
        for r in sec.iter_relocations():
            off = r["r_offset"]
            if start <= off < start + size:
                out.append((off - start, rsymtab.get_symbol(r["r_info_sym"]).name))
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("-j", "--jobs", type=int, default=10)
    ap.add_argument("--limit", type=int)
    args = ap.parse_args()

    if not ELIGIBILITY.exists():
        sys.exit(f"missing {ELIGIBILITY} -- run tools/eligible.py first")
    relocs, syms = load_relocs(), load_symbols()
    decl_type = decl_types()

    # Every name the shared headers already declare. A resolved name that is
    # already in here needs no new declaration, and adding one risks the same
    # duplicate-declaration type clash the C-linkage sweep ran into.
    declared = set()
    for h in (REPO / "include").rglob("*.h"):
        for m in re.finditer(r"^\s*extern\s+[^;=]*?\b(\w+)\s*(\[\s*\])?\s*;",
                             h.read_text(encoding="utf-8", errors="ignore"), re.M):
            declared.add(m.group(1))

    info = {}
    for (d, name, rel, addr, size, sec) in candidates()[0]:
        info[rel.replace("\\", "/")] = (name, addr, size, d)
    mods = {("arm9" if m["name"] == "main" else m["name"]): m for m in MOD.modules()}

    jobs = []
    for r in json.loads(ELIGIBILITY.read_text()):
        if not (r.get("reason") or "").startswith("unresolvable"):
            continue
        rel = r["file"].replace("\\", "/")
        miss = r.get("missing") or []
        ph = [s for s in miss if is_stand_in(s, declared)]
        if ph and rel in info:
            jobs.append((rel, ph,
                         [s for s in miss if not is_stand_in(s, declared)]))

    print(f"files with placeholder references: {len(jobs)}")
    print(f"  of those, blocked ONLY by placeholders: "
          f"{sum(1 for _, _, o in jobs if not o)}")

    def verify(f, name, addr, size, label, flags):
        tgt = (M.target_bytes(addr, size) if label == "arm9"
               else M.target_bytes(addr, size, mods[label]["bin"], mods[label]["base"]))
        for v in M.SWEEP:
            o = M.compile_c(f, v, flags)
            if o is None:
                continue
            code, rr = M.extract_func(o, name)
            if code is not None and M.compare(tgt, code, rr, verbose=False)[0]:
                return True
        return False

    def one(job):
        rel, ph, others = job
        f = REPO / rel
        name, addr, size, d = info[rel]
        label = d.relative_to(REPO / "config").as_posix()
        label = "arm9" if label == "arm9" else label.split("/")[-1]
        original = f.read_text(encoding="utf-8", errors="ignore")
        flags = M.DEFAULT_FLAGS
        if f.suffix == ".cpp" or original.startswith("//cpp"):
            flags = flags.replace("-lang c99", "-lang c++")

        obj = next((o for o in (M.compile_c(f, v, flags) for v in M.SWEEP)
                    if o is not None), None)
        if obj is None:
            return rel, "compile failed", {}
        rl = relocs_for(obj, name)
        if rl is None:
            return rel, "function not in object", {}

        table = relocs.get(label, {})
        mapping, bad = {}, []
        for off, sym in rl:
            if sym not in ph:
                continue
            entry = table.get(addr + off)
            if entry is None:
                bad.append((sym, f"no reloc recorded at {addr + off:#010x}"))
                continue
            to, spec = entry
            mod = module_key(spec)
            if mod is None:
                bad.append((sym, f"dsd could not place it: module:{spec}"))
                continue
            real, why = name_at(syms, mod, to)
            if real is None:
                bad.append((sym, why))
                continue
            if mapping.get(sym, real) != real:
                bad.append((sym, f"resolves to both {mapping[sym]} and {real}"))
                continue
            mapping[sym] = real

        left = set(ph) - set(mapping)
        if bad or left:
            return rel, f"unresolved: {bad or sorted(left)}", mapping
        if not args.apply:
            return rel, "resolvable", mapping

        renamed, needed = original, []
        for old, real in sorted(mapping.items(), key=lambda kv: -len(kv[0])):
            pat = r"(?<![\w])" + re.escape(old) + r"(?![\w])"
            # If the placeholder is declared in this file, renaming carries its
            # declaration along. Otherwise the declaration lives in decl_common.h
            # under the placeholder's name and the real name arrives undeclared --
            # unless a header this file includes already declares it.
            local = re.search(r"^\s*extern\s[^;=]*" + pat + r"\s*(\[\s*\])?\s*;",
                              original, re.M)
            renamed = re.sub(pat, real, renamed)
            # A placeholder with no declaration anywhere gets no template; the
            # undeclared variant below is then the only candidate, which is right.
            if not local and old in decl_type:
                needed.append(decl_type[old].replace("@", real))

        def with_decls(text, decls):
            if not decls:
                return text
            # After the last #include, so the file's own headers are seen first.
            incs = list(re.finditer(r"^\s*#\s*include\s.*$", text, re.M))
            at = incs[-1].end() if incs else 0
            return text[:at] + "\n" + "\n".join(dict.fromkeys(decls)) + text[at:]

        # Whether a resolved name needs declaring depends on which headers *this*
        # file includes, which is not worth modelling -- the compiler already knows.
        # Try it declared and undeclared, and keep whichever byte-matches.
        for cand in (with_decls(renamed, needed), renamed):
            f.write_text(cand, encoding="utf-8", newline="\n")
            if verify(f, name, addr, size, label, flags):
                return rel, "rewritten", mapping
        f.write_text(original, encoding="utf-8", newline="\n")
        return rel, "reverted (stopped matching)", mapping
        tgt = (M.target_bytes(addr, size) if label == "arm9"
               else M.target_bytes(addr, size, mods[label]["bin"], mods[label]["base"]))
        for v in M.SWEEP:
            o2 = M.compile_c(f, v, flags)
            if o2 is None:
                continue
            code, rr = M.extract_func(o2, name)
            if code is None:
                continue
            if M.compare(tgt, code, rr, verbose=False)[0]:
                return rel, "rewritten", mapping
        f.write_text(original, encoding="utf-8", newline="\n")
        return rel, "reverted (stopped matching)", mapping

    if args.limit:
        jobs = jobs[:args.limit]
    counts, samples, bad = collections.Counter(), [], collections.Counter()
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for rel, verdict, mapping in ex.map(one, jobs):
            key = verdict.split(":")[0]
            counts[key] += 1
            if key in ("resolvable", "rewritten") and len(samples) < 10:
                samples.append((rel, mapping))
            elif key == "unresolved":
                bad[re.sub(r"0x[0-9a-f]+", "0x…", verdict)[:110]] += 1
            elif key.startswith("reverted"):
                print(f"  {verdict}: {rel}")
    print()
    for k, v in counts.most_common():
        print(f"  {v:5d}  {k}")
    if samples:
        print("\nsample resolutions:")
        for rel, mp in samples:
            print(f"  {rel}")
            for a, b in sorted(mp.items()):
                print(f"      {a:<6} -> {b}")
    if bad:
        print("\nwhy the rest did not resolve:")
        for k, v in bad.most_common(10):
            print(f"  {v:5d}  {k}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
