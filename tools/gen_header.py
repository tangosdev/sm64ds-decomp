"""Differential between the committed struct headers and the evidence for them.

241 headers in include/ say "AUTO-GENERATED from matched-function evidence". No
generator has ever existed in this repo (notes/plan-gen-header.md 1), so nothing has
ever checked them. This does.

It does not write headers. Phase 1 of the plan is a worklist, not a code change:
every declared field is placed in exactly one bucket, and every disagreement is
printed with its provenance so a person can read it.

    python tools/evidence_history.py            # pass 1, generates
    python tools/evidence_hierarchy.py          # pass 2, reconciles
    python tools/evidence_rom.py                # pass 3, verifies
    python tools/gen_header.py --report

Passes are optional; whatever is present is used, and the report says which ran.
Missing passes are stated, never silently treated as "no evidence" -- a pass that
did not run and a pass that found nothing must not look alike.
"""
import argparse
import collections
import json
import pathlib
import re
import sys

BANNER = "AUTO-GENERATED from matched-function evidence"

# a declared field line: `    u8  mScaleX;            /* 0x080 */`
FIELD = re.compile(r"^\s*([A-Za-z_][\w ]*?)\s*(\*?)\s+(\w+)\s*(\[[^\]]*\])?\s*;"
                   r"\s*/\*\s*(0x[0-9a-fA-F]+)")

DECLARED_WIDTH = {"u8": 1, "s8": 1, "char": 1, "signed char": 1, "unsigned char": 1,
                  "u16": 2, "s16": 2, "short": 2, "unsigned short": 2,
                  "u32": 4, "s32": 4, "int": 4, "unsigned int": 4, "long": 4,
                  "Fix12i": 4, "float": 4, "void": 4,
                  "s64": 8, "u64": 8, "long long": 8, "double": 8}
SIGNED_TYPES = {"s8", "s16", "s32", "int", "short", "signed char", "long", "Fix12i"}
UNSIGNED_TYPES = {"u8", "u16", "u32", "unsigned char", "unsigned short", "unsigned int"}


def declared_width(typ, ptr, arr):
    if ptr:
        return 4
    t = typ.strip()
    if arr:                      # `u8 pad_000[0x8]` -- an array, width is per element
        return DECLARED_WIDTH.get(t)
    return DECLARED_WIDTH.get(t)


PAD_LINE = re.compile(r"^\s*u8\s+pad_[0-9a-fA-F]+\s*\[\s*(0x[0-9a-fA-F]+|\d+)\s*\]")
END_OF_FIELDS = re.compile(r"^\s*(#\s*ifdef|#\s*endif|\}|/\* methods)")
SKIPPABLE = re.compile(r"^\s*($|/\*|\*|//)")


def next_meaningful(lines, i, cur):
    """The next line that is not blank and not inside a comment.

    Checking only the immediately-following line missed every marker whose pad is
    separated from it by an explanatory comment. `include/RaycastGround.h` documents
    its own idiom across three lines, and the continuation lines start with prose --
    not `*` -- so matching comment-looking prefixes is not enough. Track whether the
    field's own trailing `/*` ever closed.
    """
    in_comment = cur.count("/*") > cur.count("*/")
    while i < len(lines):
        ln = lines[i]
        if in_comment:
            if "*/" in ln:
                in_comment = False
                rest = ln.split("*/", 1)[1]
                if rest.strip():
                    return rest
            i += 1
            continue
        if SKIPPABLE.match(ln):
            if "/*" in ln and "*/" not in ln:
                in_comment = True
            i += 1
            continue
        return ln
    return ""


def load_headers(root):
    """The 241 bannered headers -> {class: {offset: FieldInfo}}.

    `placeholder` marks the generator's idiom for "an object of unknown type starts
    here": a lone `u8 mModel; /* 0x0d4 */` followed by `u8 pad_0d5[0x4f]`, where the
    pad reserves the object's real extent. The `u8` is a marker, NOT a claim that the
    field is one byte wide -- so a wider access at that offset agrees with it.

    Two forms exist. The common one is marker+pad. The trailing one has no pad because
    the unknown object runs to the end of the struct; it is the same idiom and must be
    read the same way. `span` is 1+pad when known, None when the object runs to the
    end -- a span of 4 or less means the "object" is really a scalar and the marker is
    just a missing retype.
    """
    out = {}
    for h in sorted((root / "include").glob("*.h")):
        txt = h.read_text(errors="replace")
        if BANNER not in txt:
            continue
        lines = txt.splitlines()
        fields = {}
        for i, line in enumerate(lines, 1):
            m = FIELD.match(line)
            if not m:
                continue
            typ, ptr, name, arr, off = m.groups()
            if name.startswith("pad_"):
                continue
            placeholder, span = False, None
            if typ.strip() == "u8" and not ptr and not arr:
                nxt = next_meaningful(lines, i, line)
                pm = PAD_LINE.match(nxt)
                if pm:
                    placeholder, span = True, 1 + int(pm.group(1), 0)
                elif END_OF_FIELDS.match(nxt):
                    placeholder = True          # trailing marker, extent unknown
            fields[int(off, 16)] = (typ.strip(), bool(ptr), arr, name,
                                    f"include/{h.name}:{i}", placeholder, span)
        out[h.stem] = fields
    return out


def load_pass(root, name):
    p = root / "build" / f"evidence_{name}.json"
    if not p.exists():
        return None
    try:
        return json.loads(p.read_text(encoding="utf-8"))
    except Exception as e:
        print(f"  !! evidence_{name}.json unreadable: {e}", file=sys.stderr)
        return None


def widths_for(passes, cls, off):
    """Union of observed widths per pass -> {pass_name: {width: count}}."""
    hexoff = hex(off)
    out = {}
    for pname, data in passes.items():
        if not data:
            continue
        e = data.get("classes", {}).get(cls, {}).get(hexoff)
        if e and e.get("widths"):
            out[pname] = {int(k): v for k, v in e["widths"].items()}
    return out


def address_only_for(passes, cls, off):
    """Passes that saw this offset's address taken but never a sized access.

    A member object handed to a method by address proves the field EXISTS at that
    offset while saying nothing about its width. Reading only `widths` counted 643
    such fields as "unbacked" -- as though nothing had ever reached them.
    """
    hexoff = hex(off)
    return [p for p, data in passes.items()
            if data and data.get("classes", {}).get(cls, {}).get(hexoff, {}).get("address_only")]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=".")
    ap.add_argument("--report", action="store_true")
    ap.add_argument("--cls", help="restrict to one class")
    ap.add_argument("--show", type=int, default=25, help="findings to print per bucket")
    ap.add_argument("-o", "--out", default="build/gen_header_report.json")
    args = ap.parse_args()

    root = pathlib.Path(args.root).resolve()
    headers = load_headers(root)
    # A base class whose own header is bannered has never been verified either, so a
    # derived/base disagreement there is two guesses conflicting -- not evidence. Only
    # the de-bannered, hand-reconstructed headers (Actor, ActorBase, Fader) are
    # authorities. Keep the two apart or the weak half gets actioned as if it were strong.
    bannered = set(headers)
    if args.cls:
        headers = {k: v for k, v in headers.items() if k == args.cls}

    passes = {n: load_pass(root, n) for n in ("history", "hierarchy", "rom")}
    ran = [n for n, d in passes.items() if d]
    missing = [n for n, d in passes.items() if not d]

    # Hierarchy conflicts are keyed (class, offset), but one field can conflict with
    # more than one ancestor -- 16 do, and every one of those mixes a de-bannered
    # reference ancestor with a bannered (itself unverified) one. Last-write-wins would
    # grade those by whichever happened to come last, so prefer the reference ancestor:
    # a conflict against a verified base is the stronger claim and must not be masked.
    hier = passes.get("hierarchy") or {}
    conflicts, sign_conflicts = {}, {}
    for c in hier.get("conflicts", []):
        kind = c.get("kind")
        if kind not in ("width", "sign"):
            continue
        target = conflicts if kind == "width" else sign_conflicts
        key = (c["cls"], int(c["offset"], 16))
        prev = target.get(key)
        if prev is None or (prev["base_cls"] in bannered and c["base_cls"] not in bannered):
            target[key] = c

    # Ancestor lookup, over ALL headers -- the reference (de-bannered) ones like
    # Actor.h are exactly the ancestors that matter and are not in `headers`.
    chain_of = {k: v.get("base") for k, v in hier.get("hierarchy", {}).items()}
    any_fields_cache = {}

    def any_fields(name):
        if name not in any_fields_cache:
            p = root / "include" / f"{name}.h"
            got = {}
            if p.exists():
                for ln in p.read_text(errors="replace").splitlines():
                    g = FIELD.match(ln)
                    if g and not g.group(3).startswith("pad_"):
                        typ, ptr, _nm, arr, off = g.groups()
                        w = (4 if ptr else DECLARED_WIDTH.get(typ.strip(), 1)) \
                            * (int(arr.strip("[]"), 0) if arr else 1)
                        got[int(off, 16)] = (typ.strip(), w)
            any_fields_cache[name] = got
        return any_fields_cache[name]

    def ancestors(name, _seen=None):
        _seen = _seen or set()
        cur = chain_of.get(name)
        while cur and cur not in _seen:
            _seen.add(cur)
            yield cur
            cur = chain_of.get(cur)

    buckets = collections.Counter()
    findings = collections.defaultdict(list)
    per_class = {}

    for cls, fields in sorted(headers.items()):
        pc = collections.Counter()

        def ancestor_type(off, _cls=cls):
            """Does any ancestor already account for this offset?

            Matched by EXTENT, not by exact start: an offset landing inside an
            ancestor's `u8 sceneNode[0x14]` is that field's interior, not a discovery.
            Exact-offset matching filed those as novel.
            """
            for a in ancestors(_cls):
                af = any_fields(a)
                hit = af.get(off)
                if hit is not None:
                    return a, hit[0]          # (type, width) -> type
                for ao, (atyp, aw) in af.items():
                    if ao < off < ao + aw:
                        return a, atyp
            return None

        # A class the hierarchy pass could not place has NO ancestors to consult, so
        # "no ancestor declares this" is unasked, not answered. Counted separately --
        # reporting an unasked question as a zero is how #1129 shipped a false claim.
        hierarchy_blind = cls not in chain_of
        for off, (typ, ptr, arr, name, where, placeholder, span) in sorted(fields.items()):
            dw = declared_width(typ, ptr, arr)
            obs = widths_for(passes, cls, off)
            allw = set().union(*obs.values()) if obs else set()
            addr_only = address_only_for(passes, cls, off)

            key = (cls, off)
            if key in conflicts:
                base_cls = conflicts[key].get("base_cls")
                bucket = ("base-conflict (weak: base unverified)" if base_cls in bannered
                          else "base-conflict")
                findings[bucket].append({
                    "cls": cls, "offset": hex(off), "field": name, "declared": typ,
                    "where": where, "base": conflicts[key].get("base_cls"),
                    "base_type": conflicts[key].get("base", {}).get("type"),
                    "base_where": conflicts[key].get("base", {}).get("file"),
                    "derived_is_marker": placeholder,
                    "rom_widths": sorted(obs.get("rom", {})),
                })
            elif key in sign_conflicts:
                # 4-byte signedness is not settled by a load (plan 6), so these are
                # reported for adjudication, never auto-applied -- but dropping them
                # silently filed them under `confirmed`, which is worse.
                sc = sign_conflicts[key]
                bucket = ("base-sign-conflict (weak: base unverified)"
                          if sc.get("base_cls") in bannered else "base-sign-conflict")
                findings[bucket].append({
                    "cls": cls, "offset": hex(off), "field": name, "declared": typ,
                    "where": where, "base": sc.get("base_cls"),
                    "base_type": sc.get("base", {}).get("type"),
                    "rom_signed": (passes.get("rom") or {}).get("classes", {})
                                  .get(cls, {}).get(hex(off), {}).get("signed"),
                })
            elif not obs:
                # "no width observed" is not "nothing reached it": an address-taken
                # member object proves the field exists and leaves only its size open.
                if addr_only:
                    bucket = "offset-corroborated (width unverified)"
                else:
                    # A field adopted from a de-bannered ancestor is not unreached --
                    # its evidence is the ancestor's, and the passes look per class, so
                    # the derived class need never touch it. Counting those as unreached
                    # overstates uncertainty in exactly the way this report exists to
                    # prevent: adopting 546 verified fields would read as a 14x regression.
                    at = ancestor_type(off)
                    bucket = ("adopted from a verified ancestor"
                              if at and at[0] not in bannered and at[1] == typ
                              else "unreached by any pass")
            elif dw is None:
                bucket = "unknown-type"
                findings[bucket].append({"cls": cls, "offset": hex(off),
                                         "field": name, "declared": typ, "where": where})
            elif (dw and any(w > dw for v in obs.values() for w in v)
                  and len({w for v in obs.values() for w in v}) > 1):
                # A pass observed something WIDER than the declaration, which reads
                # past the field -- that is a real contradiction. A NARROWER
                # observation is not: it is a partial access, and for a declared s64 it
                # is the only thing ARM can emit, having no 64-bit register. Timer and
                # SphereClsn are exactly that and are corroborated, not contradicted.
                #
                # `dw in allw` unions the passes, so the one that happens to agree wins
                # and the disagreement disappears --
                # Timer 0x0 was "confirmed" as s32 because the ROM sees the 4-byte
                # halves of an s64 (ARM has no 64-bit register, so it cannot see
                # otherwise) while history reads the real s64 from source. Plan 4 sends
                # conflicting evidence to a person; it must not be resolved by union.
                bucket = "cross-pass width disagreement"
                findings[bucket].append({
                    "cls": cls, "offset": hex(off), "field": name, "declared": typ,
                    "observed": {k: sorted(v) for k, v in obs.items()}, "where": where})
            elif dw in allw:
                bucket = "confirmed"
                # signedness: only a sub-word LOAD proves it, and only ldrs* proves signed
                romev = (passes.get("rom") or {}).get("classes", {}).get(cls, {}).get(hex(off))
                if romev and dw < 4:
                    s = romev.get("signed", {}).get("s", 0)
                    if s and typ in UNSIGNED_TYPES:
                        bucket = "signedness-contradicted"
                        findings[bucket].append({
                            "cls": cls, "offset": hex(off), "field": name,
                            "declared": typ, "rom": "signed load (ldrs*)",
                            "where": where, "provenance": romev.get("provenance", [])[:2]})
            elif placeholder:
                # A lone u8 + pad means "object here, size unknown" -- a wider access
                # agrees with it, so it is never a contradiction. But when the whole
                # span is 4 bytes or less there is no room for an object: the marker is
                # standing in for a scalar, and the observed width is its real type.
                bucket = ("marker: scalar-sized, retype" if span is not None and span <= 4
                          else "marker: object, extent unknown")
                findings[bucket].append({
                    "cls": cls, "offset": hex(off), "field": name, "span": span,
                    "observed": {k: sorted(v) for k, v in obs.items()}, "where": where})
            else:
                bucket = "width-contradicted"
                findings[bucket].append({
                    "cls": cls, "offset": hex(off), "field": name, "declared": typ,
                    "declared_width": dw, "observed": {k: sorted(v) for k, v in obs.items()},
                    "where": where})
            buckets[bucket] += 1
            pc[bucket] += 1

        # Offsets evidence reaches that the header does not declare. A bare count of
        # these badly overstates what has been discovered: most are the vtable slot,
        # the interior of a field already declared, or an inherited field this flat
        # header pads over. Split them so "new" means new.
        seen = set()
        for pname, data in passes.items():
            if data:
                seen |= {int(o, 16) for o in data.get("classes", {}).get(cls, {})}

        # What each declared field actually covers. A trailing marker (span None) has
        # no pad because its object runs to the end of the struct -- load_headers says
        # so explicitly -- so giving it one byte here contradicted this file's own
        # semantics and pushed ~76 interior offsets into "novel".
        extents = []
        for o, (typ, ptr, arr, name, where, ph, span) in fields.items():
            if ph:
                extents.append((o, o + span if span else float("inf")))
            else:
                w = declared_width(typ, ptr, arr) or 1
                n = int(arr.strip("[]"), 0) if arr else 1
                extents.append((o, o + w * n))

        for off in sorted(seen - set(fields)):
            if off == 0:
                b = "new: vtable slot"          # every bannered header pads 0x0
            elif any(a < off < z for a, z in extents):
                b = "new: interior to a declared field"
            elif ancestor_type(off) is not None:
                b = "new: declared by an ancestor"
            elif hierarchy_blind:
                b = "new: unresolved (class not placed in hierarchy)"
            elif not widths_for(passes, cls, off):
                b = "new: address-only"
            else:
                b = "new: novel field"
                findings[b].append({
                    "cls": cls, "offset": hex(off),
                    "observed": {k: sorted(v) for k, v in widths_for(passes, cls, off).items()}})
            buckets[b] += 1
            pc[b] = pc.get(b, 0) + 1
        per_class[cls] = dict(pc)

    total = sum(v for k, v in buckets.items() if not k.startswith("new"))
    report = {"passes_run": ran, "passes_missing": missing,
              "buckets": dict(buckets), "total_declared_fields": total,
              "per_class": per_class, "findings": {k: v for k, v in findings.items()}}
    dest = root / args.out
    dest.parent.mkdir(parents=True, exist_ok=True)
    dest.write_text(json.dumps(report, indent=1), encoding="utf-8")

    print(f"passes run     : {', '.join(ran) if ran else 'NONE'}")
    if missing:
        print(f"passes MISSING : {', '.join(missing)}   <- results below are partial")
    print(f"headers        : {len(headers)}   declared fields: {total}")
    print()
    order = [
        "confirmed",
        "width-contradicted",
        "signedness-contradicted",
        "base-conflict",
        "base-conflict (weak: base unverified)",
        "base-sign-conflict",
        "base-sign-conflict (weak: base unverified)",
        "cross-pass width disagreement",
        "marker: scalar-sized, retype",
        "marker: object, extent unknown",
        "adopted from a verified ancestor",
        "offset-corroborated (width unverified)",
        "unreached by any pass",
        "unknown-type",
        "new: novel field",
        "new: unresolved (class not placed in hierarchy)",
        "new: declared by an ancestor",
        "new: interior to a declared field",
        "new: vtable slot",
        "new: address-only",
    ]
    for b in order:
        n = buckets.get(b, 0)
        if b.startswith("new") and n:
            print(f"  {b:<38} {n:>6}")
        elif not b.startswith("new") and (n or b in ("confirmed", "unreached by any pass")):
            print(f"  {b:<38} {n:>6}  ({100*n/max(1,total):5.1f}%)")


    for b in ("base-conflict", "base-conflict (weak: base unverified)", "width-contradicted", "signedness-contradicted", "cross-pass width disagreement", "marker: scalar-sized, retype"):
        f = findings.get(b)
        if not f:
            continue
        print(f"\n--- {b} ({len(f)}) ---")
        for x in f[:args.show]:
            if b.startswith("base-conflict"):
                print(f"  {x['cls']}.{x['field']} @{x['offset']}: {x['declared']} "
                      f"vs {x['base']}.{x['base_type']}  [{x['where']}]")
            elif b == "width-contradicted":
                print(f"  {x['cls']}.{x['field']} @{x['offset']}: declared {x['declared']} "
                      f"({x['declared_width']}B), observed {x['observed']}  [{x['where']}]")
            elif b == "cross-pass width disagreement":
                print(f"  {x['cls']}.{x['field']} @{x['offset']}: declared {x['declared']}, "
                      f"passes disagree {x['observed']}  [{x['where']}]")
            elif b.startswith("marker:"):
                print(f"  {x['cls']}.{x['field']} @{x['offset']}: u8 marker spanning "
                      f"{x['span']}B, observed {x['observed']}  [{x['where']}]")
            else:
                print(f"  {x['cls']}.{x['field']} @{x['offset']}: declared {x['declared']}, "
                      f"ROM uses a signed load  [{x['where']}]")
        if len(f) > args.show:
            print(f"  ... {len(f) - args.show} more in {args.out}")
    print(f"\nwrote {args.out}")


if __name__ == "__main__":
    main()
