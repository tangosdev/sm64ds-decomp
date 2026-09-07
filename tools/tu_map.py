#!/usr/bin/env python3
"""Recover the original translation-unit boundaries the linker erased.

`src/` holds 11,122 one-function files -- a shape the original build never had.
The ROM still carries evidence of the real `.cpp` boundaries, because the linker
emits each `.o`'s `.text` as one contiguous run in link order and never
interleaves two objects. This pass reads that evidence back out.

WHAT DOES *NOT* WORK, AND WHY IT LOOKS LIKE IT SHOULD
-----------------------------------------------------
The obvious detector -- "cut wherever the class name changes" -- is wrong, and
ov062 is exactly the overlay that hides it. There the five classes (Chuckya,
Koopa, KoopaTheQuick, KoopaFlag, Klepto) really do occupy five contiguous runs,
one TU each, so a per-class detector scores 5/5 and looks solved.

It is not. Where two classes share a file, mwcc emits their methods INTERLEAVED
in source order, not grouped by class. ov080:

    _ZN9MontyMoleD1Ev  _ZN9MontyMoleD0Ev
    _ZN13MontyMoleRockD1Ev  _ZN13MontyMoleRockD0Ev
    _ZN9MontyMole16CleanupResourcesEv   _ZN13MontyMoleRock16CleanupResourcesEv
    _ZN9MontyMole6RenderEv              _ZN13MontyMoleRock6RenderEv
    _ZN9MontyMole8BehaviorEv            _ZN13MontyMoleRock8BehaviorEv
    _ZN9MontyMole13InitResourcesEv      _ZN13MontyMoleRock13InitResourcesEv
    MontyMoleRock_Spawn                 MontyMole_Spawn

Cut on name change and ov080's three TUs shatter into thirteen. ov020
(BookShot/BookShotSpawner) and ov063 (daTrs_c/BooCage/BigBooIcon) do the same.

The fix is to stop treating a class as a block and treat it as an INTERVAL.
Because a TU is contiguous, two classes whose address spans overlap cannot be in
different TUs -- one would have to sit inside the other's run. So:

    span(A) overlaps span(B)  =>  A and B share a translation unit

is not a heuristic, it is forced by the linker's own layout rule. Union-find over
that relation collapses MontyMole+MontyMoleRock automatically while leaving
Chuckya and Koopa apart, and it needs no notion of "how interleaved is too
interleaved".

WHAT COUNTS AS A LABEL
----------------------
Naming a function's class two ways, because 3 large code overlays (ov006, ov007,
ov004) are nearly nameless and are also where the map is worth the most:

  * mangled names, via `srcpath.class_of` -- which already encodes the two traps
    (take the OUTER component, so `Sound::Player` not `Player`; a bare
    `_Z<len><name>` is a free function and has no class at all).  `_Spawn` names
    are tree coinages, not mangled class evidence: a unique local vtable relocation
    identifies the factory's class, and that vtable's own slots recover the spelling
    used by the class's mangled methods.
  * vtable slots, via `build/rtti_vtables.json` -- every function a class's
    vtable points at belongs to that class. RTTI carries EAD's real class names,
    so this labels functions in overlays where no symbol was ever named.

`notes/dtor-variant-audit.md` records that mwcc emits the vtable group into the
TU that defines the destructor, so a vtable is also a `.data` anchor for its TU.

SINITS ARE A COUNT, NOT A CUT
-----------------------------
mwcc emits at most one `__sinit_<file>` per TU that has static initialisers, and
`.ctor` lists them in link order. Swept across all 106 modules, `.ctor` entry
count == `__sinit_*` symbol count with zero exceptions, and the ctor targets are
always ascending and inside `.init`.

That makes sinits an excellent CHECK and a poor DETECTOR: they say how many TUs
carry static init and in what order, but a sinit lives in `.init`, not next to
the `.text` it belongs to, so it does not by itself say where a text cut falls.
This pass therefore uses the count as a gate (`sinit_vs_tu`) rather than as a
signal, and reports over- and under-shoot per module instead of forcing a fit.

Only 67 of the 104 `.ctor` sections are non-empty and only 52 modules have two or
more sinits, so a third of code-bearing modules get no help from this gate at all.

WHAT THIS DOES NOT DO
---------------------
It produces a map. It does not merge any files. Consolidation is a separate,
byte-gated change -- a merged TU must emit its functions in exactly the ROM's
order, and `notes/mwccarm-codegen.md` records that TUs carrying `#pragma opt_*`
cannot be merged as-is. Decide that after reading the map, not from this script.

Usage:
    python tools/tu_map.py                          # all modules -> build/tu_map.json
    python tools/tu_map.py --module ov062           # one module, printed
    python tools/tu_map.py --module ov080 --verbose # with per-function detail
    python tools/tu_map.py --blind ov062,ov063,ov080  # negative control (see below)
    python tools/tu_map.py --check                  # run the validation gates
"""
import argparse
import collections
import glob
import json
import os
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import modules as MOD
import srcpath as SP

REPO = pathlib.Path(__file__).resolve().parent.parent
OUT = REPO / "build" / "tu_map.json"
VTABLES = REPO / "build" / "rtti_vtables.json"
NL = chr(10)


def _require_vtables():
    """Refuse to run with the RTTI signal silently switched off.

    `vtable_labels` and `vtable_factory_classes` used to return {} when
    build/rtti_vtables.json was absent. tu_map then completed, printed a
    plausible summary and wrote an authoritative-looking map -- with the
    cartridge's own type evidence contributing nothing.

    Measured 2026-09-06 on one tree: with the file the map is 691 TUs / 333
    classed; without it, 692 / 332. The difference is small enough to look like
    noise and is not, and queue_audit consumes the result: a queue regenerated
    from the map built without it left `Player` at 26794 lines when the tree
    said 26798, while --check and --write both reported success.

    queue_audit already refuses its own missing and stale inputs for exactly
    this reason (see its `stale_input` docstring). tu_map did not refuse its
    own. A missing input here is a silently wrong answer, not a missing one.
    """
    how = "python tools/rtti_vtables.py --out build/rtti_vtables.json"
    if not VTABLES.is_file():
        raise SystemExit(
            f"{VTABLES.relative_to(REPO)} is absent; make it with:" + NL + f"    {how}")
    config = (glob.glob(str(REPO / "config" / "arm9" / "overlays" / "*" / "symbols.txt"))
              + [str(REPO / "config" / "arm9" / "symbols.txt")])
    newest = max((os.path.getmtime(f) for f in config if os.path.exists(f)), default=0)
    if newest > os.path.getmtime(VTABLES):
        raise SystemExit(
            f"{VTABLES.relative_to(REPO)} is older than config/; "
            f"regenerate it with: {how}")

# Nesting threshold for _drop_swallowers, set by --split-swallowers. None = off,
# which keeps the committed map byte-identical until the change is reviewed.
SWALLOWER_K = None

SEC_RE = re.compile(
    r"^\s+(\.\w+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)\s+kind:(\w+)")
SYM_RE = re.compile(
    r"^(\S+)\s+kind:(\w+)\(([^)]*)\)\s+addr:0x([0-9a-fA-F]+)")
RELOC_RE = re.compile(
    r"^from:0x([0-9a-fA-F]+)\s+kind:(\S+)\s+to:0x([0-9a-fA-F]+)\s+module:(\S+)")
SIZE_RE = re.compile(r"size=0x([0-9a-fA-F]+)")


# ---------------------------------------------------------------- parsing

def cfgdir(mod):
    """The config directory holding a module's delinks.txt."""
    return mod["syms"].parent


def sections(mod):
    """{name: (start, end, kind)} from the module's delinks.txt.

    Section lines are the indented ones before the first `src/...:` entry; the
    per-file ranges further down are deliberately NOT read here -- see
    `functions()`."""
    out = {}
    p = cfgdir(mod) / "delinks.txt"
    if not p.is_file():
        return out
    for line in p.read_text(errors="ignore").splitlines():
        if line.startswith("src/"):
            break
        m = SEC_RE.match(line)
        if m:
            out[m.group(1)] = (int(m.group(2), 16), int(m.group(3), 16), m.group(4))
    return out


def symbols(mod):
    """[(name, kind, addr, size)] for every symbol in the module."""
    out = []
    for line in mod["syms"].read_text(errors="ignore").splitlines():
        m = SYM_RE.match(line)
        if not m:
            continue
        attrs = m.group(3)
        sz = SIZE_RE.search(attrs)
        out.append((m.group(1), m.group(2), int(m.group(4), 16),
                    int(sz.group(1), 16) if sz else 0))
    return out


def functions(mod, secs):
    """Address-sorted functions inside `.text`, read from symbols.txt.

    From symbols.txt and NOT from delinks.txt's per-file entries, because
    delinks can leave a hole: ov062 has no entry covering 0x02117724-0x02117994,
    yet `func_ov062_02117724` is right there in symbols.txt, inside the Koopa
    run. Inventorying from delinks would silently drop it and shift every
    boundary computed after it."""
    if ".text" not in secs:
        return []
    lo, hi, _ = secs[".text"]
    fns = [(a, n, s) for (n, k, a, s) in symbols(mod)
           if k == "function" and lo <= a < hi]
    return sorted(fns)


def relocs(mod):
    """[(frm, kind, to, module)] for the module, or [] if it has none."""
    p = mod["relocs"]
    if not p.is_file():
        return []
    out = []
    for line in p.read_text(errors="ignore").splitlines():
        m = RELOC_RE.match(line)
        if m:
            out.append((int(m.group(1), 16), m.group(2),
                        int(m.group(3), 16), m.group(4)))
    return out


# ---------------------------------------------------------------- labelling

def vtable_labels():
    """{module: {func_addr: class}} from build/rtti_vtables.json.

    The point of this table is the anonymous overlays. ov006 has 1866 functions
    and 38 named ones; RTTI still knows what its polymorphic classes are called,
    so a vtable slot lands a real name on code that no symbol ever named."""
    _require_vtables()
    data = json.loads(VTABLES.read_text(errors="ignore"))
    out = collections.defaultdict(dict)
    for cls, rec in data.items():
        if not isinstance(rec, dict) or "slots" not in rec:
            continue
        mod = rec.get("module")
        if not mod:
            continue
        for addr in rec.get("slots") or []:
            if isinstance(addr, int) and addr:
                out[mod][addr] = cls
    return out


def vtable_factory_classes():
    """{module: {vtable_addr: method class}} for unambiguous RTTI vtables.

    The record key is the cartridge's RTTI name, which can differ from this tree's
    already-mangled method names.  `own` slots are the non-inherited entries and
    therefore pair the vtable with the exact class component those methods use.
    """
    _require_vtables()
    data = json.loads(VTABLES.read_text(errors="ignore"))
    candidates = collections.defaultdict(lambda: collections.defaultdict(set))
    for _cls, rec in data.items():
        if not isinstance(rec, dict):
            continue
        mod = rec.get("vtable_module")
        addr = rec.get("vtable")
        method_classes = set()
        for row in rec.get("own", []):
            method_class = SP.class_of(row.get("symbol") or "")
            if method_class:
                method_classes.add(method_class)
        if mod and addr and len(method_classes) == 1:
            candidates[mod][int(addr, 0)].update(method_classes)
    return {mod: {addr: next(iter(classes))
                  for addr, classes in by_addr.items() if len(classes) == 1}
            for mod, by_addr in candidates.items()}


def factory_vtable_labels(fns, rl, vtables_for_module):
    """{factory_addr: method class} when one local vtable settles an alias.

    `<Class>_Spawn` is a source-tree naming convention, not a mangled symbol.  It
    often agrees with the C++ class, but actor variants and later RTTI renames can
    make it lie (`BigBoo_Spawn` and `Boo_Spawn` both construct `daTrs_c`).  A
    relocation from the factory body to exactly one vtable in this module is the
    cartridge's own type evidence, and the vtable's own slots supply the matching
    mangled-method class.  Multiple local vtables are deliberately left unresolved
    rather than guessing which constructed object is the return value.
    """
    local_methods = set()
    for _addr, name, _size in fns:
        method_class = None if name.endswith("_Spawn") else SP.class_of(name)
        if method_class:
            local_methods.add(method_class)
    out = {}
    for addr, name, size in fns:
        if not name.endswith("_Spawn"):
            continue
        targets = {vtables_for_module[to]
                   for frm, _kind, to, _module in rl
                   if addr <= frm < addr + max(size, 4)
                   and to in vtables_for_module
                   and vtables_for_module[to] in local_methods}
        if len(targets) == 1:
            out[addr] = next(iter(targets))
    return out


def label(name, addr, vt_for_module, factory_for_module, blind):
    """The class a function belongs to, or None.

    `blind` drops the mangled-name signal, leaving only RTTI. That is the
    negative control: it simulates an unnamed overlay on one whose answer we
    already know."""
    if not blind:
        cls = factory_for_module.get(addr)
        if cls:
            return cls, "symbol"
        cls = SP.class_of(name)
        if cls:
            return cls, "symbol"
    cls = vt_for_module.get(addr)
    if cls:
        return cls, "vtable"
    return None, None


# ---------------------------------------------------------------- clustering

class Union:
    """Plain union-find over class names."""

    def __init__(self):
        self.parent = {}

    def find(self, x):
        self.parent.setdefault(x, x)
        while self.parent[x] != x:
            self.parent[x] = self.parent[self.parent[x]]
            x = self.parent[x]
        return x

    def union(self, a, b):
        ra, rb = self.find(a), self.find(b)
        if ra != rb:
            self.parent[rb] = ra


def _spans(fns, vt_for_module, factory_for_module, blind, source):
    """{class: (lo, hi)} over functions whose label came from `source`."""
    spans = {}
    for addr, name, size in fns:
        cls, got = label(name, addr, vt_for_module, factory_for_module, blind)
        if not cls or got != source:
            continue
        lo, hi = spans.get(cls, (addr, addr + max(size, 4)))
        spans[cls] = (min(lo, addr), max(hi, addr + max(size, 4)))
    if source == "symbol" and SWALLOWER_K:
        spans = _drop_swallowers(spans, SWALLOWER_K)
    return spans


def _drop_swallowers(spans, k):
    """Remove symbol labels that cannot be one contiguous TU with what they contain.

    The overlap rule -- span(A) overlaps span(B) => same TU -- is forced by the
    linker only if each label occupies exactly ONE object. Two kinds of label break
    that, and `main` is full of both:

      * namespaces used as class labels. `_ZN4CP15...` is direct evidence that the
        function belongs to CP15, but CP15 is a namespace spanning dozens of TUs,
        so its span is not a TU span. Same for IRQ, cstd, GX, Sound, SaveData.
      * genuinely multi-TU classes -- the case section 2 of
        notes/translation-unit-reconstruction-plan.md predicts ("A large class can
        have methods defined across several TUs"). Model, Scene, Stage, Animation,
        MeshCollider and TextureSequence all have RTTI records and still do this,
        so "has a type_info" does NOT separate the two kinds and must not be used.

    Detection needs no list. If A's span strictly contains the COMPLETE spans of k
    or more other labels, A is not one contiguous object with all of them. Drop A's
    span and keep the split; its functions fall through to absorb_unlabelled and
    attach by call graph.

    This is the same treatment a bridging RTTI span already gets in cluster(). The
    asymmetry that docstring describes -- symbols trusted, RTTI not -- is right
    about which class OWNS a function and wrong about TU co-membership.

    Measured: main goes 26 -> 164 TUs and its boundary confidence goes from
    {low:23, medium:2} to {low:119, medium:32, high:12}. At k>=6 NO other module
    changes at all, and the corroborated boundaries of ov020, ov045, ov062, ov063,
    ov080, ov081 and ov090 all hold. k=3 is too aggressive: it splits ov063, whose
    daTrs_c/BooCage/BigBooIcon interleave is one of the cases this tool exists to get
    right. The result is flat across k=6..12, so it is reading a real structural
    feature rather than a tuned cutoff.
    """
    items = list(spans.items())
    if len(items) < 2:
        return spans
    drop = set()
    for c, (lo, hi) in items:
        nested = sum(1 for d, (l2, h2) in items
                     if d != c and l2 >= lo and h2 <= hi)
        if nested >= k:
            drop.add(c)
    return {c: v for c, v in spans.items() if c not in drop} if drop else spans


def _merge_overlapping(spans):
    """Union-find over classes whose spans overlap; returns [(classes, lo, hi)].

    Overlap forces co-membership because a TU is contiguous: if two classes were
    separate objects, one object's run would have to sit inside the other's,
    which the linker never emits."""
    u = Union()
    ordered = sorted(spans.items(), key=lambda kv: kv[1])
    for i, (ca, (lo_a, hi_a)) in enumerate(ordered):
        u.find(ca)
        for cb, (lo_b, hi_b) in ordered[i + 1:]:
            if lo_b >= hi_a:            # sorted by start: nothing later overlaps
                break
            if lo_a < hi_b and lo_b < hi_a:
                u.union(ca, cb)
    groups = collections.defaultdict(list)
    for cls in spans:
        groups[u.find(cls)].append(cls)
    out = [(sorted(v), min(spans[c][0] for c in v), max(spans[c][1] for c in v))
           for v in groups.values()]
    return sorted(out, key=lambda t: t[1])


def cluster(fns, vt_for_module, factory_for_module, blind):
    """Group the module's classes into TU-clusters, then cut `.text` between them.

    Symbol labels first, RTTI labels second, and RTTI is NOT allowed to merge two
    symbol clusters. That asymmetry is the whole correctness story here, and it
    was found by gate V2b rather than reasoned out in advance.

    ov081 is the case. Its mangled names put MrBlizzard at 0x2124090-0x2125f14
    and Snowball at 0x2125f14-0x2126504 -- abutting exactly, not overlapping, so
    two TUs. But the RTTI class `daSnowman_c` spans 0x2125eb8-0x21261d4, straddling
    that boundary, and a naive union merged them into one. The sinit count settles
    it: ov081 has five named classes and five sinits, so five TUs is right and the
    merge was wrong. ov090 fails identically (`daMenbo_c` bridging MantaRay and
    Skeeter; four classes, four sinits).

    The reason RTTI can lie here and symbols cannot: a mangled name is direct
    evidence about the function that carries it, whereas an RTTI span is inferred
    from a vtable's slot list, and a slot can point at inherited or shared code
    that lives in a neighbouring object. So an RTTI span that straddles a symbol
    boundary is split at that boundary rather than believed.

    In a module with no mangled names there are no symbol clusters to protect, so
    RTTI spans stand alone and do all the work -- which is exactly the case the
    RTTI signal was added for, and the negative control measures.

    Returns (clusters, unlabelled)."""
    sym = _merge_overlapping(
        _spans(fns, vt_for_module, factory_for_module, blind, "symbol"))
    rtti = _spans(fns, vt_for_module, factory_for_module, blind, "vtable")

    # An RTTI span may extend or create a cluster, never bridge two symbol ones.
    free = {}
    for cls, (lo, hi) in rtti.items():
        touched = [i for i, (_c, s, e) in enumerate(sym) if lo < e and s < hi]
        if len(touched) > 1:
            continue                    # bridging: drop the span, keep the split
        if touched:
            i = touched[0]
            names, s, e = sym[i]
            sym[i] = (sorted(names + [cls]), min(s, lo), max(e, hi))
        else:
            free[cls] = (lo, hi)

    merged = sorted(sym + _merge_overlapping(free), key=lambda t: t[1])
    clusters = [{"classes": c, "start": s, "end": e, "funcs": []}
                for (c, s, e) in merged]

    unlabelled = []
    for addr, name, size in fns:
        for c in clusters:
            if c["start"] <= addr < c["end"]:
                c["funcs"].append((addr, name, size))
                break
        else:
            unlabelled.append((addr, name, size))
    return clusters, unlabelled


def absorb_unlabelled(clusters, unlabelled, calls):
    """Attach each stray function to a neighbouring cluster when the call graph says so.

    A function called ONLY from inside one cluster's span is a candidate for being
    file-local to that cluster: no mangled name to read a class from is what you
    expect of internal linkage, and internal linkage cannot cross a TU boundary.

    But a single caller-cluster is NOT proof of internal linkage. A perfectly
    ordinary external function that happens to have exactly one caller looks
    identical from the call graph alone. So the call graph proposes and *adjacency
    disposes*: the linker lays a TU's .text down contiguously, so a function that
    belongs to a TU must touch that TU's span. A stray is absorbed only when it
    abuts the cluster -- and absorbing it extends the span, so a contiguous run of
    strays is taken in one function at a time, which is why this iterates.

    Without that guard the call-graph rule alone placed 304 functions into TUs
    they cannot be in, across 59 of 720 units -- e.g. eight functions at
    0x020ccd04..0x020d0bd8 absorbed into ov006's dScMgTrampoline_c
    (0x021207dc..0x0212231c), a third of a megabyte away. A .text that jumps
    0x54000 bytes and comes back is not a translation unit.

    Everything else becomes its own low-confidence TU rather than being guessed
    into a neighbour: an unattributed run between two clusters is genuinely
    ambiguous, and saying so is more useful than picking a side."""
    def _owner(addr):
        """The single cluster every caller of `addr` lies in, else None."""
        owners = set()
        for c_addr in calls.get(addr, set()):
            for i, c in enumerate(clusters):
                if c["start"] <= c_addr < c["end"]:
                    owners.add(i)
                    break
            else:
                owners.add(-1)          # called from outside every cluster
        return owners.pop() if len(owners) == 1 and -1 not in owners else None

    leftovers = list(unlabelled)
    absorbing = True
    while absorbing:                    # a run is absorbed one function per pass
        absorbing = False
        still = []
        for addr, name, size in leftovers:
            i = _owner(addr)
            c = clusters[i] if i is not None else None
            if c is not None and (addr == c["end"] or addr + size == c["start"]):
                c["funcs"].append((addr, name, size))
                c["start"] = min(c["start"], addr)
                c["end"] = max(c["end"], addr + size)
                absorbing = True
            else:
                still.append((addr, name, size))
        leftovers = still

    # Contiguous runs of still-unattributed functions become their own TUs.
    orphan_tus = []
    for addr, name, size in sorted(leftovers):
        if orphan_tus and orphan_tus[-1]["end"] == addr:
            orphan_tus[-1]["funcs"].append((addr, name, size))
            orphan_tus[-1]["end"] = addr + max(size, 4)
        else:
            orphan_tus.append({"classes": [], "start": addr,
                               "end": addr + max(size, 4),
                               "funcs": [(addr, name, size)]})
    for c in clusters:
        c["funcs"].sort()
    out = sorted(clusters + orphan_tus, key=lambda c: c["start"])
    return out


def call_graph(rl, secs):
    """{callee_addr: {caller_addr}} for intra-module code references only.

    Cross-module relocs are dropped: they prove external linkage, which is the
    opposite of the property `absorb_unlabelled` is looking for."""
    if ".text" not in secs:
        return {}
    lo, hi, _ = secs[".text"]
    out = collections.defaultdict(set)
    for frm, _kind, to, mod in rl:
        if mod in ("main", "overlay") or mod.startswith("overlay("):
            # module: is the *target's* module; only same-module edges qualify,
            # and those are the ones dsd leaves unqualified or self-named.
            pass
        if lo <= frm < hi and lo <= to < hi:
            out[to].add(frm)
    return out


# ---------------------------------------------------------------- sinits

def sinits(mod, secs):
    """Address-sorted `__sinit_*` functions, and the .ctor entry count.

    The two must agree -- they do in all 106 modules -- so a mismatch here means
    the config was regenerated against a different ROM, not that the theory
    broke."""
    ordered = sorted(a for (n, k, a, s) in symbols(mod)
                     if k == "function" and n.startswith("__sinit"))
    ctor = secs.get(".ctor")
    n_ctor = (ctor[1] - ctor[0]) // 4 if ctor else 0
    return ordered, n_ctor


# ---------------------------------------------------------------- confidence

def boundary_confidence(prev, nxt):
    """How much to trust the cut BETWEEN two TUs.

    Per-boundary and not per-TU on purpose: the failure mode of this whole pass
    is a cut in the wrong place, and a TU-level score hides that -- an over-merge
    of a labelled TU with a sinit-less neighbour reads as high confidence if you
    only score the TU that has the evidence."""
    a, b = bool(prev["classes"]), bool(nxt["classes"])
    if a and b:
        return "high"                   # named class either side; spans disjoint
    if a or b:
        return "medium"                 # one side is an unattributed run
    return "low"                        # contiguity alone


# ---------------------------------------------------------------- per module

def analyse(mod, vt, vt_addr, blind=False):
    secs = sections(mod)
    fns = functions(mod, secs)
    if not fns:
        return None                     # data-only overlay: no TU problem exists
    rl = relocs(mod)
    factory = factory_vtable_labels(fns, rl, vt_addr.get(mod["name"], {}))
    clusters, unlabelled = cluster(fns, vt.get(mod["name"], {}), factory, blind)
    tus = absorb_unlabelled(clusters, unlabelled, call_graph(rl, secs))
    si, n_ctor = sinits(mod, secs)

    labelled = sum(1 for c in tus if c["classes"])
    boundaries = [boundary_confidence(tus[i], tus[i + 1])
                  for i in range(len(tus) - 1)]

    return {
        "module": mod["name"],
        "functions": len(fns),
        "tus": len(tus),
        "tus_with_class": labelled,
        "sinits": len(si),
        "ctor_entries": n_ctor,
        "sinit_vs_tu": ("ok" if len(si) <= len(tus)
                        else "OVERSHOOT: more sinits than TUs"),
        # An exact match is an independent witness that the cut count is right:
        # the sinits were counted from .ctor and never informed the cuts.
        "corroborated": bool(si) and len(si) == len(tus),
        "boundaries": collections.Counter(boundaries),
        "units": [{"start": hex(c["start"]), "end": hex(c["end"]),
                   "classes": c["classes"],
                   "functions": [n for (_a, n, _s) in c["funcs"]]}
                  for c in tus],
    }


# ---------------------------------------------------------------- gates

KNOWN = {                               # V1: known answers, hand-verified
    "ov062": 5,
    "ov063": 4,
    # ov080 was 3 and failed on main. The constant predates the RTTI label
    # source: `daPicGate_c` occupies its own 6-function run at 0x2126fbc that no
    # mangled name ever named, so the vtable pass labels a FOURTH TU there. No
    # boundary moved -- ov080 is 5 TUs before and after -- only how many of them
    # carry a class. 3 sinits still fits under 5.
    #
    # This is a different count from the docstring's "ov080's three TUs shatter
    # into thirteen", which is about the mangled-name view alone; daPicGate_c has
    # no mangled name and is not part of that illustration.
    "ov080": 4,
    "ov020": 2,
}


def gates(results):
    """V1-V4. Each returns (name, ok, detail)."""
    out = []
    by = {r["module"]: r for r in results}

    for m, want in KNOWN.items():
        r = by.get(m)
        got = r["tus_with_class"] if r else None
        out.append((f"V1 {m} classed-TU count == {want}", got == want,
                    f"got {got}"))

    r = by.get("ov062")
    if r:
        koopa = next((u for u in r["units"] if "Koopa" in u["classes"]), None)
        ok = bool(koopa) and "daNknk_c_classInit_NOKONOKO_S" in koopa["functions"]
        out.append(("V1b ov062 daNknk_c_classInit_NOKONOKO_S is in the Koopa TU", ok,
                    "" if ok else "KoopaSmall was split out"))

    r = by.get("ov063")
    if r:
        boo = next((u for u in r["units"] if "daTrs_c" in u["classes"]), None)
        factories = {"BigBoo_Spawn", "Boo_Spawn"}
        ok = (bool(boo) and factories.issubset(boo["functions"])
              and not any({"BigBoo", "Boo"} & set(u["classes"])
                          for u in r["units"]))
        out.append(("V1c ov063 Boo variants use the daTrs_c factory TU", ok,
                    "" if ok else "coined factory prefix split from daTrs_c"))

    bad = [r["module"] for r in results if r["sinits"] != r["ctor_entries"]]
    out.append(("V2a sinit count == .ctor entry count", not bad, str(bad[:5])))

    over = [r["module"] for r in results if r["sinits"] > r["tus"]]
    out.append(("V2b no module has more sinits than TUs", not over, str(over[:5])))

    dupes = []
    for r in results:
        seen = set()
        for u in r["units"]:
            for f in u["functions"]:
                if f in seen:
                    dupes.append(f"{r['module']}:{f}")
                seen.add(f)
    out.append(("V3 every function in exactly one TU", not dupes, str(dupes[:5])))

    return out


def blind_control(vt, vt_addr):
    """The negative control: re-run the known overlays with names stripped.

    This is the number that matters. Every gate above is scored on overlays whose
    symbols are named, but the map is worth the most in ov006 (1866 functions, 38
    named) where those names do not exist. Running the known answers through the
    RTTI-only path measures what the map is worth THERE, instead of assuming the
    named-overlay score carries over. If blind recall is poor, the honest report
    is that the map covers named overlays and nothing else."""
    mods = {m["name"]: m for m in MOD.modules()}
    rows = []
    for name, want in KNOWN.items():
        m = mods.get(name)
        if not m:
            continue
        sighted = analyse(m, vt, vt_addr, blind=False)
        blind = analyse(m, vt, vt_addr, blind=True)
        rows.append((name, want, sighted["tus"] if sighted else None,
                     blind["tus"] if blind else None,
                     blind["tus_with_class"] if blind else None))
    return rows


# ---------------------------------------------------------------- main

def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--module", help="analyse one module and print it")
    ap.add_argument("--verbose", action="store_true", help="list functions per TU")
    ap.add_argument("--check", action="store_true", help="run the validation gates")
    ap.add_argument("--blind", action="store_true",
                    help="negative control: RTTI only, no mangled names")
    ap.add_argument("--split-swallowers", nargs="?", type=int, const=8,
                    metavar="K", default=None,
                    help="do not merge through a symbol label whose span strictly "
                         "contains >=K complete other spans (default 8); fixes "
                         "main's 2984-function unit, see _drop_swallowers")
    ap.add_argument("--out", default=str(OUT))
    a = ap.parse_args()

    global SWALLOWER_K
    SWALLOWER_K = a.split_swallowers

    vt = vtable_labels()
    vt_addr = vtable_factory_classes()
    mods = MOD.modules()
    if a.module:
        mods = [m for m in mods if m["name"] == a.module]
        if not mods:
            sys.exit(f"module {a.module} not found")

    results = [r for r in (analyse(m, vt, vt_addr, blind=a.blind) for m in mods) if r]

    if a.module:
        r = results[0] if results else None
        if not r:
            print(f"{a.module}: no .text functions (data-only module)")
            return
        print(f"{r['module']}: {r['functions']} functions -> {r['tus']} TUs "
              f"({r['tus_with_class']} with a class), "
              f"{r['sinits']} sinits / {r['ctor_entries']} ctor entries [{r['sinit_vs_tu']}]")
        for u in r["units"]:
            names = ", ".join(u["classes"]) or "(unattributed)"
            print(f"  {u['start']}-{u['end']}  {len(u['functions']):4d} fn  {names}")
            if a.verbose:
                for f in u["functions"]:
                    print(f"        {f}")
        print("  boundaries:", dict(r["boundaries"]))
        return

    if a.check:
        for name, ok, detail in gates(results):
            print(f"  [{'PASS' if ok else 'FAIL'}] {name} {detail if not ok else ''}")
        print("\n  negative control (names stripped, RTTI only):")
        print(f"    {'module':8} {'known':>6} {'sighted':>8} {'blind':>6} {'blind-classed':>14}")
        for row in blind_control(vt, vt_addr):
            print("    {:8} {:6} {:8} {:6} {:14}".format(
                *[("-" if v is None else v) for v in row]))
        return

    conf = collections.Counter()
    for r in results:
        conf.update(r["boundaries"])

    # Under-segmentation is this pass's characteristic failure, and it is silent:
    # a module with no class labels collapses into a handful of enormous
    # contiguous runs and still reports "N TUs" as confidently as ov062 does.
    # main is the extreme -- 3066 functions in 25 runs, when the real answer is
    # certainly in the hundreds. Naming it here is the difference between a map
    # that knows its own edges and one that quietly overstates them.
    # Two conditions, because size alone cries wolf. ov063 packs 141 functions
    # into 4 TUs and is RIGHT -- its 4 sinits corroborate the 4 cuts exactly. So
    # flag only a module that is both coarse AND uncorroborated: sinits == tus is
    # an independent witness that the cut count is correct, and a module with no
    # sinits at all (ov007, 548 functions, zero) has no witness available.
    suspect = sorted(
        ({"module": r["module"], "functions": r["functions"], "tus": r["tus"],
          "per_tu": round(r["functions"] / r["tus"], 1),
          "tus_with_class": r["tus_with_class"], "sinits": r["sinits"]}
         for r in results
         if r["functions"] / r["tus"] > 25
         and not (r["sinits"] and r["sinits"] == r["tus"])),
        key=lambda d: -d["per_tu"])

    payload = {
        "meta": {
            "modules": len(results),
            "total_tus": sum(r["tus"] for r in results),
            "total_functions": sum(r["functions"] for r in results),
            "total_sinits": sum(r["sinits"] for r in results),
            "boundaries": dict(conf),
            "tus_with_class": sum(r["tus_with_class"] for r in results),
            "under_segmented": suspect,
            "caveat": "Boundaries marked 'low' rest on contiguity alone and cannot "
                      "distinguish two classes in one file from two files that "
                      "linked adjacently. Modules in under_segmented have too few "
                      "class labels to cut on; their TU counts are lower bounds.",
        },
        "modules": {r["module"]: {k: (dict(v) if isinstance(v, collections.Counter)
                                      else v)
                                  for k, v in r.items()} for r in results},
    }
    p = pathlib.Path(a.out)
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(json.dumps(payload, indent=1) + "\n", encoding="utf-8", newline="\n")
    print(f"{payload['meta']['modules']} modules, "
          f"{payload['meta']['total_functions']} functions -> "
          f"{payload['meta']['total_tus']} TUs  ->  {p}")
    print(f"  boundaries: {dict(conf)}   TUs with a class: "
          f"{payload['meta']['tus_with_class']}/{payload['meta']['total_tus']}")
    print(f"  under-segmented (TU count is a lower bound): "
          f"{', '.join(d['module'] for d in suspect[:8])}"
          f"{' ...' if len(suspect) > 8 else ''}")


if __name__ == "__main__":
    main()
