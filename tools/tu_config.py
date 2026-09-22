#!/usr/bin/env python3
"""Emit a ph-shaped (TU-granular) dsd config root beside the per-function one.

At the time this experiment was written, `config/**/delinks.txt` was measured as
11,170 per-function entries, one section each, `.text`/`.init` only, with a 92-byte
median. That is historical input evidence, not a live invariant: shared-source ranges
now exist. zeldaret/ph -- same dsd 0.11.0, same file format -- wrote 75 entries,
median 1,272 bytes, one to six sections each. That file is the ONLY thing that decides
the treemap/objdiff unit, so the shape of the report is entirely a property of this one
text file.

This generator writes a PARALLEL root (`config_tu/` by default) holding the same
modules in the TU shape. Nothing here touches `config/`, `src/` or `src_tu/`, and
nothing here can reach the ROM build: `config/**/delinks.txt` remains the sole
authority over what mwccarm compiles, and the generated `config.yaml` points its
`delinks_path`/`build_path` at a separate directory so a `dsd delink` run against
this root cannot overwrite the real build's objects.

MODULE-LEVEL EXCLUSIVITY
------------------------
A module lives in the old shape or the new one, never both -- a half-converted
module would have two entries claiming the same bytes, and dsd would either
reject it or (worse) silently let one win. `--module` names the modules to
convert. The generated root holds a `delinks.txt` for those modules and NOTHING
ELSE: every other reference in `config.yaml`, including the converted modules'
own unchanged `symbols.txt`/`relocs.txt`, points back at `config/arm9/`. So the
presence of a file in this root IS the definition of a converted module, and the
set is also written to `<out>/tu_modules.json` for tools that want to ask rather
than walk. `--self-contained` copies the whole tree instead, at the cost of 8.4MB
of byte-identical duplicate.

WHAT IS EVIDENCED, AND WHAT IS NOT
----------------------------------
`.text` comes from tools/tu_map.py: one contiguous run per TU, forced by the
linker never interleaving two objects. That is the easy half.

Every other section is attributed only where the ROM says so, by four rules that
each fail closed. All four rest on the same structural fact -- object k's
contribution to EVERY section precedes object k+1's -- so any attribution whose
per-TU intervals are not ascending and disjoint is discarded wholesale rather
than patched:

  .init   -- `__sinit_*` ordinal == TU ordinal. Gated on the sinit count, the
             `.ctor` word count and the TU count all being equal; without that
             equality one TU could carry two sinits and another none, and the
             ordinal map would be off by one from there on.
  .ctor   -- each `.ctor` word is a relocation to exactly one `__sinit_*`, so the
             word inherits that sinit's TU. Exact, one word per TU.
  .bss    -- a `.bss` object referenced only from things already attributed to TU
             k -- its `.text`, its own `__sinit_*` block, or a `.data` record the
             rule below has already tied to it -- belongs to TU k. Statics are
             constructed by their own TU's static initialiser, which is what makes
             the `.init` side of this a real signal and not a coincidence. This
             rule runs LAST for that reason.
  .data   -- DIRECTIONAL. A run is attributed to TU k when every code pointer
             *inside* it lands in TU k's `.text` (a vtable or a spawn-info table
             emitted by that TU), then by transitive closure over runs whose only
             referrers are already-attributed runs of TU k (a vtable's typeinfo
             pointer, and the typeinfo's name pointer). The reverse direction --
             "TU k's code loads this address" -- is NOT used, and that is not
             fussiness: in ov045 the reverse direction produces three anchors
             whose TU order contradicts the link order, which is positive proof
             that some of ov045's `.data` belongs to an object that contributes
             no `.text` at all. Using it would have swept 1,760 bytes of a
             seventh, data-only object into FireSeaElevator.

Runs that no rule reaches stay unclaimed and fall to the module's gap object.
That is deliberate: an unclaimed byte is honest, an invented owner is not. Run
with `--report` to see exactly which bytes each rule did and did not reach.

Usage:
    python tools/tu_map.py                                  # prerequisite
    python tools/tu_config.py --module ov045 --report
    python tools/tu_config.py --module ov045 --strict-text  # no growth allowed
    python tools/tu_config.py --module ov045 --sections text,init,ctor,bss
    tools/bin/dsd.exe delink  -c config_tu/arm9/config.yaml
    tools/bin/dsd.exe objdiff -c config_tu/arm9/config.yaml -o build/tu/objdiff

`delink` must run BEFORE `objdiff`: objdiff's report walks the target objects and
aborts on the first one that is not there yet.
"""
import argparse
import bisect
import collections
import json
import os
import pathlib
import re
import shutil
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import enroll as EN          # noqa: E402  read_delinks: header / `complete` marks
import modules as MOD        # noqa: E402
import tu_manifest as TUM    # noqa: E402  the TU manifest's on-disk shape
import tu_map as TM          # noqa: E402  sections/symbols/relocs/analyse
import tubuild as TB         # noqa: E402  parse_delinks_file/entry_sections

CONFIG = REPO / "config" / "arm9"
TU_MAP = REPO / "build" / "tu_map.json"

# tu_map.SYM_RE only matches kinds that carry a parenthesised attribute list, so
# `data_ov045_02113180 kind:bss addr:0x02113180` is invisible to it -- correctly,
# because it inventories functions. Attribution needs the object STARTS in every
# section, including .bss, so this reads the same file one field wider.
ANY_SYM_RE = re.compile(r"^(\S+)\s+kind:(\w+)(?:\(([^)]*)\))?\s+addr:0x([0-9a-fA-F]+)")
PATH_LINE = re.compile(r"^(\s*(?:rom_config|build_path|delinks_path|object):\s+)(\S+)(\s*)$")

SECTION_ORDER = (".text", ".init", ".ctor", ".rodata", ".data", ".bss")
ALL_RULES = ("text", "init", "ctor", "bss", "data", "rodata")


class TuConfigError(RuntimeError):
    pass


# ---------------------------------------------------------------- inputs

def load_tu_map(path=TU_MAP):
    if not path.is_file():
        raise TuConfigError(f"missing {path}; run `python tools/tu_map.py` first")
    return json.loads(path.read_text(encoding="utf-8"))


def all_symbols(mod):
    """[(name, kind, addr)] for every symbol, .bss objects included."""
    out = []
    for line in mod["syms"].read_text(encoding="utf-8", errors="ignore").splitlines():
        m = ANY_SYM_RE.match(line)
        if m:
            out.append((m.group(1), m.group(2), int(m.group(4), 16)))
    return out


def symbol_runs(lo, hi, syms):
    """[(start, end)] partitioning [lo, hi) at every symbol address inside it.

    A run is dsd's own idea of "one object": the bytes from one symbol up to the
    next. It over-splits -- dsd cuts a long `_ZTS` string in two -- which is safe
    here because a rule may attribute several consecutive runs to one TU but can
    never attribute half a run to two."""
    starts = sorted({a for (_n, _k, a) in syms if lo <= a < hi} | {lo})
    return [(starts[i], starts[i + 1] if i + 1 < len(starts) else hi)
            for i in range(len(starts))]


# ---------------------------------------------------------------- attribution

class Attribution:
    """Per-TU section ranges plus the reason each one was or was not claimed."""

    def __init__(self, module, tus, secs):
        self.module = module
        self.tus = tus                                  # [(start, end, [classes])]
        self.secs = secs
        self.ranges = [dict() for _ in tus]             # TU index -> {sec: (lo, hi)}
        self.report = []                                # [(section, verdict, detail)]

    def claim(self, section, idx, lo, hi):
        self.ranges[idx][section] = (lo, hi)

    def note(self, section, verdict, detail):
        self.report.append((section, verdict, detail))

    def claimed_bytes(self, section):
        return sum(hi - lo for r in self.ranges
                   for (s, (lo, hi)) in r.items() if s == section)


def _ordered_disjoint(claims, n_tus):
    """True when TU k's interval precedes TU k+1's, as the linker guarantees.

    This is the gate every non-.text rule fails closed on. It is not a sanity
    check bolted on afterwards: the rules below each infer ownership from a local
    signal, and the ONLY thing that makes a set of local inferences into a valid
    object layout is that they come out in link order."""
    got = [(k, claims[k]) for k in range(n_tus) if k in claims]
    for i in range(len(got) - 1):
        if got[i][1][1] > got[i + 1][1][0]:
            return False
    return True


def attribute_text(att, tus):
    for i, (lo, hi, _c) in enumerate(tus):
        att.claim(".text", i, lo, hi)
    att.note(".text", "claimed",
             f"{len(tus)} contiguous runs from build/tu_map.json, tiling "
             f"{tus[0][0]:#010x}-{tus[-1][1]:#010x}")


def attribute_init(att, mod, secs, syms, tus):
    """sinit ordinal == TU ordinal, gated on the three counts agreeing."""
    if ".init" not in secs:
        return att.note(".init", "absent", "module has no .init section")
    ilo, ihi, _ = secs[".init"]
    sinits = sorted(a for (n, k, a) in syms
                    if k == "function" and n.startswith("__sinit") and ilo <= a < ihi)
    n_ctor = ((secs[".ctor"][1] - secs[".ctor"][0]) // 4) if ".ctor" in secs else 0
    if not sinits:
        return att.note(".init", "unattributed", "no __sinit_* symbols in .init")
    if not (len(sinits) == len(tus) == n_ctor):
        return att.note(".init", "unattributed",
                        f"count gate failed: {len(sinits)} sinits, {n_ctor} .ctor "
                        f"words, {len(tus)} TUs -- an ordinal map needs all three equal")
    for i, a in enumerate(sinits):
        att.claim(".init", i, a, sinits[i + 1] if i + 1 < len(sinits) else ihi)
    att.note(".init", "claimed",
             f"{len(sinits)} sinit blocks tiling {ilo:#010x}-{ihi:#010x}; "
             f"ordinal k -> TU k (counts agree: sinits == .ctor words == TUs)")
    return None


def attribute_ctor(att, secs, syms, rl, tus, self_mod):
    """Each .ctor word relocates to exactly one __sinit_*; it inherits that TU."""
    if ".ctor" not in secs or secs[".ctor"][0] >= secs[".ctor"][1]:
        return att.note(".ctor", "absent", "module has no non-empty .ctor section")
    if ".init" not in secs:
        return att.note(".ctor", "unattributed", "no .init to point into")
    clo, chi, _ = secs[".ctor"]
    ilo, ihi, _ = secs[".init"]
    sinits = sorted(a for (n, k, a) in syms
                    if k == "function" and n.startswith("__sinit") and ilo <= a < ihi)
    if len(sinits) != len(tus):
        return att.note(".ctor", "unattributed",
                        f"{len(sinits)} sinits vs {len(tus)} TUs")
    order = {a: i for i, a in enumerate(sinits)}
    words = {}
    for (frm, _k, to, m) in rl:
        if clo <= frm < chi and m == self_mod and to in order:
            words.setdefault(frm, set()).add(order[to])
    if len(words) != (chi - clo) // 4 or any(len(v) != 1 for v in words.values()):
        return att.note(".ctor", "unattributed",
                        f"{len(words)} of {(chi - clo) // 4} words resolve to a "
                        f"single sinit; need all of them")
    claims = {}
    for frm, v in words.items():
        claims[next(iter(v))] = (frm, frm + 4)
    if not _ordered_disjoint(claims, len(tus)) or len(claims) != len(tus):
        return att.note(".ctor", "unattributed",
                        "ctor word order does not match sinit order")
    for k, (lo, hi) in claims.items():
        att.claim(".ctor", k, lo, hi)
    att.note(".ctor", "claimed",
             f"{len(claims)} words tiling {clo:#010x}-{chi:#010x}, one per TU, "
             f"each resolved through its own relocation")
    return None


def _fill_and_gate(att, section, owners, runs, tus, why):
    """Turn {run -> TU} into one interval per TU, filling the holes between them.

    The fill is proof rather than preference: object k's contribution to a section
    is ONE contiguous run, so a run lying strictly between two runs already known
    to be object k's is object k's too, whether or not anything references it."""
    if not owners:
        att.note(section, "unattributed", why + " -- no run could be attributed")
        return
    spans = {}
    for (lo, hi), k in owners.items():
        s, e = spans.get(k, (lo, hi))
        spans[k] = (min(s, lo), max(e, hi))
    if not _ordered_disjoint(spans, len(tus)):
        att.note(section, "unattributed",
                 why + " -- per-TU intervals are not in link order; "
                 "the evidence contradicts a per-TU partition and is discarded")
        return
    for k, (lo, hi) in spans.items():
        att.claim(section, k, lo, hi)
    total = sum(hi - lo for (lo, hi) in spans.values())
    sec_lo, sec_hi, _ = att.secs[section]
    att.note(section, "claimed",
             f"{len(spans)}/{len(tus)} TUs, {total} of {sec_hi - sec_lo} bytes; " + why)


def attribute_bss(att, secs, syms, rl, tus, self_mod, owner_of):
    """A .bss object referenced only from things already known to be TU k's is TU k's.

    "Already known" is the point. A static's address is loaded by the code that
    uses it (TU k's `.text`), by the static initialiser that constructs it (TU k's
    `__sinit_*` block, attributed above) and sometimes only by a `.data` record
    that the `.data` rule has already tied to TU k -- ov045's FloatingFloorBfs,
    TiltingPlatformBfs and daObjKm2_Fall_Block_c each reach two of their four `.bss`
    objects that way and no other. Running this rule after the pointer rule turns
    those from unowned into owned; running it before leaves 48 bytes on the floor."""
    if ".bss" not in secs or secs[".bss"][0] >= secs[".bss"][1]:
        return att.note(".bss", "absent", "module has no non-empty .bss section")
    lo, hi, _ = secs[".bss"]
    runs = symbol_runs(lo, hi, syms)
    refs = collections.defaultdict(set)
    for (frm, _k, to, m) in rl:
        if m != self_mod or not (lo <= to < hi):
            continue
        k = owner_of(frm)
        refs[to].add(-1 if k is None else k)     # -1: referenced from something unowned
    owners = {}
    for (a, b) in runs:
        seen = set()
        for t, ks in refs.items():
            if a <= t < b:
                seen |= ks
        if len(seen) == 1 and -1 not in seen:
            owners[(a, b)] = next(iter(seen))
    _fill_and_gate(att, ".bss", owners, runs, tus,
                   f"{len(owners)}/{len(runs)} symbol runs whose every self-module "
                   f"referrer is already attributed to one and the same TU")
    return None


def attribute_pointer_section(att, section, secs, syms, rl, tus, self_mod):
    """Directional rule for .rodata/.data: follow the pointers the run CONTAINS.

    Seed: a run whose self-module code pointers all land in one TU's `.text`.
    Close: a run whose only self-module referrers are runs already attributed to
    that same TU (`_ZTV` -> `_ZTI` -> `_ZTS`).

    What this deliberately does not do is read a load of the run's address from TU
    k's code as ownership. That direction is what "uses" looks like, and ov045
    proves it: three such loads, one each from FireSeaElevator, PoleLift and
    ExtendingPlatform's InitResources, point at 0x021125d0, 0x02112510 and
    0x021125b0 -- addresses whose order is 1, 2, 0 against a .text order of 0, 1,
    2. No contiguous per-object layout satisfies that, so those three runs cannot
    be the referencing TUs' own data; they belong to an object with no .text."""
    if section not in secs or secs[section][0] >= secs[section][1]:
        return att.note(section, "absent", f"module has no non-empty {section}")
    lo, hi, _ = secs[section]
    runs = symbol_runs(lo, hi, syms)
    idx = [a for (a, _b) in runs]

    def run_of(addr):
        i = bisect.bisect_right(idx, addr) - 1
        return runs[i] if 0 <= i < len(runs) and runs[i][0] <= addr < runs[i][1] else None

    owners, outgoing, incoming = {}, collections.defaultdict(set), collections.defaultdict(set)
    for (frm, _k, to, m) in rl:
        if m != self_mod:
            continue
        r = run_of(frm)
        if r is not None:
            t = _tu_of(to, tus)
            if t is not None:
                outgoing[r].add(t)
        r2 = run_of(to)
        if r2 is not None:
            incoming[r2].add(frm)

    for r, ts in outgoing.items():
        if len(ts) == 1:
            owners[r] = next(iter(ts))
    seeds = len(owners)

    changed = True
    while changed:                                # transitive closure, monotone
        changed = False
        for r in runs:
            if r in owners or r not in incoming:
                continue
            src = {owners.get(run_of(f)) for f in incoming[r]}
            if len(src) == 1 and None not in src:
                owners[r] = next(iter(src))
                changed = True

    _fill_and_gate(att, section, owners, runs, tus,
                   f"{seeds} seed run(s) whose own code pointers land in one TU, "
                   f"{len(owners) - seeds} more by closure over their referrers, "
                   f"of {len(runs)} symbol runs")
    return None


def _tu_of(addr, tus):
    for i, (lo, hi, _c) in enumerate(tus):
        if lo <= addr < hi:
            return i
    return None


def _block_of(addr, blocks):
    for i, (lo, hi) in enumerate(blocks):
        if lo <= addr < hi:
            return i
    return None


def attribute(mod, tus, secs, rules):
    syms = all_symbols(mod)
    rl = TM.relocs(mod)
    self_mod = _self_module_token(mod["name"])
    att = Attribution(mod["name"], tus, secs)

    ilo = secs.get(".init")
    sinits = sorted(a for (n, k, a) in syms
                    if k == "function" and n.startswith("__sinit")
                    and ilo and ilo[0] <= a < ilo[1]) if ilo else []
    init_blocks = [(sinits[i], sinits[i + 1] if i + 1 < len(sinits) else ilo[1])
                   for i in range(len(sinits))] if sinits else []

    def owner_of(addr):
        """Which TU owns `addr`, over everything attributed SO FAR.

        The rules run in dependence order -- .text, then .init (its own ordinal
        map), then .ctor, then the pointer sections, and .bss last because it is
        the only one that has to consult the others' answers."""
        k = _tu_of(addr, tus)
        if k is not None:
            return k
        k = _block_of(addr, init_blocks)
        if k is not None:
            return k
        for i, r in enumerate(att.ranges):
            for (lo, hi) in r.values():
                if lo <= addr < hi:
                    return i
        return None

    attribute_text(att, tus)
    if "init" in rules:
        attribute_init(att, mod, secs, syms, tus)
    if "ctor" in rules:
        attribute_ctor(att, secs, syms, rl, tus, self_mod)
    for sec, rule in ((".rodata", "rodata"), (".data", "data")):
        if rule in rules:
            attribute_pointer_section(att, sec, secs, syms, rl, tus, self_mod)
    if "bss" in rules:
        attribute_bss(att, secs, syms, rl, tus, self_mod, owner_of)
    return att


def _self_module_token(name):
    """How a module names ITSELF in its own relocs.txt: `overlay(45)`, `main`, ..."""
    if name.startswith("ov"):
        return f"overlay({int(name[2:])})"
    return name


# ---------------------------------------------------------------- rendering

def tu_source_path(module, tu, manifest_by_span):
    """The path a TU entry is filed under.

    Prefer the recovered source config/tu_manifest.d/ already names for this
    exact span, so a TU that has a real reconstructed file keeps its identity.
    Otherwise derive one -- the entry is rom-bytes, so dsd never opens it, but a
    stable readable name is what makes the objdiff unit list legible."""
    lo, hi, classes = tu
    hit = manifest_by_span.get((module, lo, hi))
    if hit:
        return hit
    stem = classes[0] if classes else f"{module}_{lo:08x}"
    return f"src_tu/{module}/{stem}.cpp"


def manifest_spans(path=None):
    out = {}
    if not TUM.exists(path):
        return out
    for e in TUM.load(path).get("entries", []):
        for s in e.get("sections", []):
            if s.get("name") == ".text":
                out[(e["module"], int(s["start"], 16), int(s["end"], 16))] = e["source"]
    return out


def carry_through(original, tu_entries):
    """Original per-function entries for ranges no attribution rule reached.

    A rule that declines is the honest outcome, but "unattributed" and "nobody
    owns it" are not the same thing: the per-function config already claims all
    of `.init` in every module that has one, and in 19 of the 74 code modules the
    sinit-ordinal rule declines because the sinit count and the TU count disagree.
    Dropping those entries would take bytes that a named file owned and hand them
    to the gap object -- a silent regression. So they come across verbatim.

    Such a module ends up TU-granular in `.text` and still per-function in
    `.init`. That is a worse-looking report than ov045's and a truthful one."""
    _h, entries = TB.parse_delinks_file(original)
    claimed = collections.defaultdict(list)
    for _p, ranges in tu_entries:
        for s, iv in ranges.items():
            claimed[s].append(iv)
    out = []
    for path, body in entries:
        ivs = TB.entry_sections(body)
        uncovered = [(s, lo, hi) for (s, lo, hi) in ivs
                     if not _covers(claimed[s], [(lo, hi)])]
        if not uncovered:
            continue
        if len(uncovered) != len(ivs):
            raise TuConfigError(
                f"{path}: partially absorbed -- {len(ivs) - len(uncovered)} of "
                f"{len(ivs)} ranges are claimed by a TU entry and the rest are not; "
                f"carrying it through would double-claim the absorbed ones")
        out.append((path, {s: (lo, hi) for (s, lo, hi) in uncovered}))
    return out


def render(header_lines, entries):
    """entries = [(path, {section: (lo, hi)})], already in link order."""
    out = list(header_lines)
    for path, ranges in entries:
        out.append(f"{path}:")
        for sec in SECTION_ORDER:
            if sec in ranges:
                lo, hi = ranges[sec]
                out.append(f"    {sec} start:0x{lo:08x} end:0x{hi:08x}")
        out.append("")
    return "\n".join(out).rstrip("\n") + "\n"


# ---------------------------------------------------------------- verification

def verify(module, original, entries, secs, func_addrs, strict_text=False):
    """Fail loudly unless the new entries are a legal replacement for the old ones.

    Three things are non-negotiable, and each of them fails silently downstream:

      * NO OVERLAP. Two entries claiming the same bytes means two files own them;
        dsd picks one and nothing later reports which.
      * NOTHING UNCLAIMED. Every byte a named file owned before must still be
        owned by a named file. A byte that quietly falls back to the module's gap
        object still links and still matches, so no byte gate would notice.
      * EVERY FUNCTION IN EXACTLY ONE `.text` ENTRY. The union test cannot see a
        function claimed twice, because overlapping entries have the right union.

    Growing `.text` past the old union is allowed and reported, not rejected: the
    per-function config leaves holes wherever a function was never enrollable (a
    Thumb stub, a 2-byte-aligned address, a NONMATCHING hatch), and a TU span
    legitimately covers those -- they were always part of that object. Pass
    `strict_text` to demand the old union exactly instead; ov045 satisfies it."""
    problems = []
    _orig_header, orig_entries = TB.parse_delinks_file(original)
    old = collections.defaultdict(list)
    for _p, body in orig_entries:
        for (s, lo, hi) in TB.entry_sections(body):
            old[s].append((lo, hi))
    new = collections.defaultdict(list)
    for path, ranges in entries:
        for s, (lo, hi) in ranges.items():
            new[s].append((lo, hi))

    for s, ivs in sorted(new.items()):
        ivs = sorted(ivs)
        for i in range(len(ivs) - 1):
            if ivs[i][1] > ivs[i + 1][0]:
                problems.append(f"{module} {s}: OVERLAP {ivs[i]} and {ivs[i + 1]} -- "
                                f"two files would own the same bytes")
        if s not in secs:
            problems.append(f"{module} {s}: no such section in the module header")
            continue
        slo, shi, _ = secs[s]
        for (lo, hi) in ivs:
            if lo < slo or hi > shi or lo >= hi:
                problems.append(f"{module} {s}: entry {lo:#x}-{hi:#x} is not inside "
                                f"the declared section {slo:#x}-{shi:#x}")

    grew = 0
    for s in sorted(old):
        o, n = _coverage(old[s]), _coverage(new.get(s, []))
        missed = [iv for iv in o if not _covers(n, [iv])]
        if missed:
            problems.append(f"{module} {s}: the new entries no longer cover "
                            f"{_fmt(missed)} -- those bytes would silently fall "
                            f"back to the module's gap object")
        if s == ".text":
            if strict_text and o != n:
                problems.append(f"{module} .text: coverage changed under --strict-text. "
                                f"was {_fmt(o)}, now {_fmt(n)}")
            grew = sum(b - a for a, b in n) - sum(b - a for a, b in o)

    # every function symbol lands in exactly one .text entry
    fns = sorted(a for a in func_addrs if _in(a, secs.get(".text")))
    ivs = sorted(new.get(".text", []))
    for a in fns:
        hits = sum(1 for (lo, hi) in ivs if lo <= a < hi)
        if hits != 1:
            problems.append(f"{module}: function at {a:#010x} is in {hits} .text "
                            f"entries, want exactly 1")
    if problems:
        raise TuConfigError("\n".join(problems))
    return {"functions": len(fns), "textGrewBy": grew,
            "text_bytes": sum(b - a for a, b in _coverage(new[".text"]))}


def _in(a, sec):
    return sec is not None and sec[0] <= a < sec[1]


def _coverage(ivs):
    out = []
    for lo, hi in sorted(ivs):
        if out and lo <= out[-1][1]:
            out[-1][1] = max(out[-1][1], hi)
        else:
            out.append([lo, hi])
    return [tuple(x) for x in out]


def _covers(outer, inner):
    o = _coverage(outer)
    return all(any(a <= lo and hi <= b for (a, b) in o) for (lo, hi) in inner)


def _fmt(ivs):
    return ", ".join(f"{a:#x}-{b:#x}" for a, b in ivs) or "(nothing)"


# ---------------------------------------------------------------- config root

CFG_REF = re.compile(r"^(\s*)(delinks|symbols|relocations):\s+(\S+)\s*$")
MODULE_NAME = re.compile(r"^\s*(?:-\s+)?name:\s+(\S+)\s*$")


def rewrite_config_yaml(src, dst_root, build_root, local_modules=None):
    """Copy config.yaml, re-rooting only what dsd WRITES.

    `local_modules` (a set of module names, or None for "all of them") is what
    makes a sparse root possible. Only a converted module needs a file of its own
    here -- its rewritten `delinks.txt`. Its `symbols.txt` and `relocs.txt` are
    unchanged, and so is everything belonging to the other 105 modules, so those
    references point back at `config/arm9/` instead of being copied. dsd accepts
    a reference that leaves the config root (verified against 0.11.0), and the
    alternative is 8.4 MB of byte-identical duplicate in the repository whose only
    job is to drift.

    The side effect is the useful one: under a sparse root, "has a delinks.txt in
    config_tu/" IS the definition of a converted module, so the exclusivity rule
    cannot be violated by editing one file and forgetting another.

    `rom_config` and every `object:` keep pointing at the same files the real
    config names -- they are read-only inputs and duplicating them would be a
    second copy of the ROM. `build_path`/`delinks_path` are redirected, because a
    delink run against this root must not be able to overwrite build/delinks/*.o
    while the real build is using them (see the concurrent-build hazard in
    notes/rom-build.md).

    Paths are normalised textually, NEVER with Path.resolve(). `extracted/` is a
    junction into whichever checkout holds the ROM dump, and resolving through it
    rewrites `../../extracted/dsd/config.yaml` into an absolute path in somebody
    else's worktree -- a generated config that silently reads another checkout's
    inputs and breaks the moment that checkout moves."""
    lines, current = [], None
    for line in src.read_text(encoding="utf-8").splitlines():
        m = PATH_LINE.match(line)
        n = MODULE_NAME.match(line)
        r = CFG_REF.match(line)
        if n:
            current = n.group(1)
        if m and m.group(2).startswith("."):
            key = m.group(1).strip().rstrip(":")
            target = os.path.normpath(os.path.join(str(src.parent), m.group(2)))
            if key == "build_path":
                target = str(build_root)
            elif key == "delinks_path":
                target = str(build_root / "delinks")
            rel = pathlib.Path(os.path.relpath(target, str(dst_root))).as_posix()
            line = f"{m.group(1)}{rel}{m.group(3)}"
        elif r and local_modules is not None:
            keep = r.group(2) == "delinks" and current in local_modules
            if not keep:
                target = os.path.normpath(os.path.join(str(src.parent), r.group(3)))
                rel = pathlib.Path(os.path.relpath(target, str(dst_root))).as_posix()
                line = f"{r.group(1)}{r.group(2)}: {rel}"
        lines.append(line)
    (dst_root / "config.yaml").write_text("\n".join(lines) + "\n",
                                          encoding="utf-8", newline="\n")


def generate(module_names, out_root, build_root, rules, report=False,
             strict_text=False, self_contained=False):
    tumap = load_tu_map()
    mods = {m["name"]: m for m in MOD.modules()}
    manifest = manifest_spans()

    for name in module_names:
        if name not in mods:
            raise TuConfigError(f"module {name} not in the registry")
        if name not in tumap["modules"]:
            raise TuConfigError(f"module {name} has no build/tu_map.json record "
                                f"(data-only module, or tu_map has not been re-run)")

    if out_root.exists():
        shutil.rmtree(out_root)
    out_root.parent.mkdir(parents=True, exist_ok=True)
    if self_contained:
        shutil.copytree(CONFIG, out_root)
        rewrite_config_yaml(CONFIG / "config.yaml", out_root, build_root)
    else:
        out_root.mkdir(parents=True)
        rewrite_config_yaml(CONFIG / "config.yaml", out_root, build_root,
                            local_modules=set(module_names))

    summary = {}
    for name in module_names:
        mod = mods[name]
        rec = tumap["modules"][name]
        secs = TM.sections(mod)
        tus = [(int(u["start"], 16), int(u["end"], 16), u["classes"])
               for u in rec["units"]]
        att = attribute(mod, tus, secs, rules)

        original = TM.cfgdir(mod) / "delinks.txt"
        tu_entries = [(tu_source_path(name, tu, manifest), att.ranges[i])
                      for i, tu in enumerate(tus)]
        carried = carry_through(original, tu_entries)
        entries = sorted(tu_entries + carried,
                         key=lambda pr: min(lo for (lo, _hi) in pr[1].values()))
        stats = verify(name, original, entries, secs,
                       [a for (_n, k, a) in all_symbols(mod) if k == "function"],
                       strict_text=strict_text)
        header, _marks = EN.read_delinks(original)
        target = out_root / original.relative_to(CONFIG)
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(render(header, entries), encoding="utf-8", newline="\n")

        _oh, oe = TB.parse_delinks_file(original)
        summary[name] = {
            "entriesBefore": len(oe), "entriesAfter": len(entries),
            "tuEntries": len(tu_entries), "carriedEntries": len(carried),
            "functions": stats["functions"], "textBytes": stats["text_bytes"],
            "textGrewBy": stats["textGrewBy"],
            "sections": {s: att.claimed_bytes(s) for s in SECTION_ORDER
                         if att.claimed_bytes(s)},
            "attribution": [{"section": s, "verdict": v, "detail": d}
                            for (s, v, d) in att.report],
        }
        if report:
            print(f"\n=== {name}: {len(oe)} entries -> {len(entries)} ===")
            for (s, v, d) in att.report:
                print(f"  {s:8} {v:14} {d}")
            for i, (p, r) in enumerate(entries):
                bits = " ".join(f"{s}:{r[s][0]:#x}-{r[s][1]:#x}"
                                for s in SECTION_ORDER if s in r)
                print(f"  [{i}] {p}\n        {bits}")

    (out_root.parent / "tu_modules.json").write_text(
        json.dumps({
            "schemaVersion": 1,
            "about": "Modules held in the TU-granular delinks shape in this root. "
                     "A module is in exactly one shape: if it is listed here its "
                     "entries come from tools/tu_config.py, and config/ still holds "
                     "the per-function entries that the ROM build uses. Nothing in "
                     "this root affects the ROM build.",
            "generator": "tools/tu_config.py",
            "sourceConfig": CONFIG.relative_to(REPO).as_posix(),
            "selfContained": bool(self_contained),
            "rules": sorted(rules),
            "modules": summary,
        }, indent=1) + "\n", encoding="utf-8", newline="\n")
    return summary


def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--module", action="append", default=[],
                    help="module to convert to TU shape (repeatable, or comma-separated)")
    ap.add_argument("--out", default="config_tu",
                    help="parallel config root to write (default config_tu)")
    ap.add_argument("--build-root", default="build/tu",
                    help="where the generated config points dsd's build_path/"
                         "delinks_path (default build/tu)")
    ap.add_argument("--sections", default=",".join(ALL_RULES),
                    help=f"attribution rules to run; any of {','.join(ALL_RULES)}")
    ap.add_argument("--self-contained", action="store_true",
                    help="copy the whole config tree instead of referencing the "
                         "unconverted modules in config/arm9 (8.4MB of duplicate; "
                         "use only if the root must stand alone)")
    ap.add_argument("--strict-text", action="store_true",
                    help="demand the new .text entries cover EXACTLY the union the "
                         "per-function entries did, not a superset (ov045 passes)")
    ap.add_argument("--report", action="store_true",
                    help="print the per-section attribution verdicts and the entries")
    a = ap.parse_args()

    names = [n for spec in a.module for n in spec.split(",") if n]
    if not names:
        sys.exit("nothing to do: pass --module ov045")
    rules = {r.strip() for r in a.sections.split(",") if r.strip()}
    bad = rules - set(ALL_RULES)
    if bad:
        sys.exit(f"unknown --sections rule(s): {sorted(bad)}")

    # No .resolve(): see rewrite_config_yaml -- the repo's gitignored inputs are
    # junctions and resolving leaks another checkout's path into the output.
    out_root = pathlib.Path(os.path.normpath(REPO / a.out / "arm9"))
    build_root = pathlib.Path(os.path.normpath(REPO / a.build_root))
    try:
        summary = generate(names, out_root, build_root, rules, report=a.report,
                           strict_text=a.strict_text,
                           self_contained=a.self_contained)
    except TuConfigError as e:
        sys.exit(f"tu_config: {e}")
    for name, s in summary.items():
        grew = (f", .text grew {s['textGrewBy']}B past the old union"
                if s["textGrewBy"] else "")
        carried = (f", {s['carriedEntries']} per-function entries carried through"
                   if s["carriedEntries"] else "")
        print(f"{name}: {s['entriesBefore']} entries -> {s['entriesAfter']} "
              f"({s['tuEntries']} TU, {s['functions']} functions), sections claimed: "
              + ", ".join(f"{k} {v}B" for k, v in s["sections"].items())
              + carried + grew)
    print(f"wrote {out_root.parent.relative_to(REPO).as_posix()}/  "
          f"(delinks -> {build_root.relative_to(REPO).as_posix()}/delinks)")


if __name__ == "__main__":
    main()
