#!/usr/bin/env python
"""Turn an instrumented-melonDS cartridge call trace into named functions.

THE OTHER HALF OF THIS IS THE PC PORT. port/tools/porttrace.py reads the port's
own function-entry trace and resolves it through that build's walk_window.map.
This reads the trace an instrumented melonDS writes while the REAL cartridge
runs, and resolves it through the decomp's own symbol tables. The two print the
same columns on purpose: frame, depth, caller -> callee. port/tools/tracediff.py
consumes both.

CANONICAL NAMES MUST AGREE BETWEEN THE TWO HALVES or the comparison is
worthless, so this spells a name the way porttrace.canonical spells it: no
overlay prefix on the name, and no argument list. `_ZN8SaveData19IsCharacter
UnlockedEj` comes back as `SaveData::IsCharacterUnlocked`, and an address-named
function keeps the address name it already has, `func_ov007_020c0b78`, which
carries its overlay inside it anyway. Where a function lives is reported in its
own column instead, and in the summary.

THE RECORD FORMAT, from C:/tmp/melontrace/src/ROMTrace.h. Eight 32-bit words per
record, raw, NO FILE HEADER. A file whose size is not a multiple of 32 is not
one of these and is refused rather than guessed at.

    [0] tag   1 = CALL, 2 = FRAME, 3 = FINGERPRINT
    CALL         [1] address of the BL/BLX, bit 0 set if the caller was Thumb
                 [2] target address, bit 0 set if the target is Thumb
                 [3..6] r0, r1, r2, r3 at the call
                 [7] r13, the stack pointer, which is where nesting comes from
    FRAME        [1] frame number, rest zero
    FINGERPRINT  [1] address, [2..5] the four words read there

THE THREE TRAPS, and what this does about each.

1. 103 OVERLAYS LOAD AT 22 DISTINCT BASE ADDRESSES, and 52 of them share
   0x021111A0, so an address alone does not name a function. The tracer writes
   one FINGERPRINT record per probed address per frame holding the four words
   actually sitting there. This compares those against the overlay bodies in
   extracted/overlays/ and so knows which overlay is resident. Four all-zero
   words mean nothing is loaded there. A base whose contents match no overlay
   of ours is recorded as unknown and skipped, never guessed at.

   FOUR WORDS AT THE BASE ARE NOT ENOUGH AT 0x021111A0, and this is the part
   the original design did not say. Of the 52 overlays that load there, 33 fall
   into four groups whose first sixteen bytes are byte-identical, and a further
   9 begin with sixteen zero bytes, which is exactly what an EMPTY base looks
   like. A capture that only probes the bases can therefore mislabel 9 real
   overlays as "nothing loaded" and cannot separate 33 more.

   The fix costs nothing and needs no emulator change, because romtrace's
   --fingerprint takes arbitrary addresses: probe 0x02111340 as well, which is
   base + 0x1A0, and all 52 come apart with no zero collisions. `--print-probes`
   prints the full --fingerprint list to use, worked out from the bodies on
   disk. Any probe address that falls inside a candidate overlay's own span is
   used to narrow it, so extra probes need no special handling here.

   The only overlays that stay indistinguishable are the fourteen 32-byte
   all-zero stubs (ov061, ov067, ov068, ov069, ov076, ov082, ov083, ov086,
   ov087, ov088, ov093, ov097, ov099, ov101). An overlay whose whole content is
   zero has nothing to fingerprint and cannot be told from an empty base by any
   probe at all. They hold no code, so nothing is ever resolved against them
   and the tie costs the listing nothing.

   RESOLUTION GRANULARITY IS ONE FRAME. An overlay swapped in and out inside a
   single frame is missed, and calls into it in that frame resolve against
   whichever overlay the fingerprint caught. This is stated, not hidden, and the
   summary counts how often a base was ambiguous or unknown. Where a base stays
   ambiguous, every candidate is consulted and a name is only reported if they
   all agree on it.

2. ITCM FUNCTIONS LIVE AT 0x01FF8000, BELOW THE ARM9 BASE. Every division in
   the game goes through the ITCM aeabi helpers, so a resolver that only knows
   about 0x02000000 and up is blind to all of them. config/arm9/itcm/symbols.txt
   is loaded alongside the main table, and the summary reports the ITCM share
   separately so its absence is visible rather than silent.

3. arm9_dec.bin IS NOT AT THE ARM9 RAM BASE. Not an issue here and NOT corrected
   for: the trace records live addresses as the processor sees them and every
   symbol table in config/ is already absolute. No file offsets are involved.
"""

import argparse
import bisect
import collections
import os
import re
import struct
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))

RECORD_WORDS = 8
RECORD_BYTES = RECORD_WORDS * 4

TAG_CALL = 1
TAG_FRAME = 2
TAG_FINGERPRINT = 3

# name kind:function(arm,size=0x1c) addr:0x020137e0
SYM_ROW = re.compile(
    r"^(\S+)\s+kind:(\w+)\(([^)]*)\)\s+addr:(0x[0-9a-fA-F]+)")
SIZE_IN = re.compile(r"size=(0x[0-9a-fA-F]+|\d+)")

ADDR_NAME = re.compile(r"^(?:func|data)_(?:ov\d{3}_)?0[0-9a-fA-F]{7}$")


# ------------------------------------------------------------------ naming

def itanium_qual(stem):
    """`_ZN6Player8BehaviorEv` -> `Player::Behavior`, or None.

    Deliberately the same partial parse port/tools/linkage.py does, character
    for character, because the two halves have to agree on the spelling or the
    diff between them is meaningless. Reads the length-prefixed components
    between `_ZN` and its `E`, after any cv-qualifier letters. Needs at least
    two components: a one-component `_ZN...E` is a namespace-scope free function
    with no class to pair against the port's MSVC `?name@Scope@@` row on equal
    terms. Constructors, destructors and operators are NOT handled, on purpose:
    the two manglings do not spell those the same way at all and a wrong pair
    would invent a function that does not exist.
    """
    if not stem.startswith("_ZN"):
        return None
    i = 3
    while i < len(stem) and stem[i] in "KVr":
        i += 1
    parts = []
    while i < len(stem) and stem[i].isdigit():
        j = i
        while j < len(stem) and stem[j].isdigit():
            j += 1
        n = int(stem[i:j])
        if j + n > len(stem):
            return None
        parts.append(stem[j:j + n])
        i = j + n
    if i >= len(stem) or stem[i] != "E" or len(parts) < 2:
        return None
    return "::".join(parts)


def canonical(sym):
    """The name as the port half spells it: no overlay prefix, no arguments."""
    q = itanium_qual(sym)
    return q if q else sym


# ------------------------------------------------------------------ symbols

class Table(object):
    """One sorted [address, address+size) -> name table."""

    def __init__(self, region):
        self.region = region
        self.rows = {}          # addr -> (size, raw name)

    def add(self, addr, size, name):
        old = self.rows.get(addr)
        if old is None:
            self.rows[addr] = (size, name)
            return
        osize, oname = old
        # Several names share one address: an alias, or a zero-sized helper
        # name laid over a func_ address name. Keep the widest span, and prefer
        # the real name over the address-shaped placeholder, because the real
        # name is the one the port's own map will also carry.
        best = oname
        if ADDR_NAME.match(oname) and not ADDR_NAME.match(name):
            best = name
        self.rows[addr] = (max(osize, size), best)

    def finish(self):
        self.addrs = sorted(self.rows)
        self.names = [canonical(self.rows[a][1]) for a in self.addrs]
        self.sizes = [self.rows[a][0] for a in self.addrs]
        self.lo = self.addrs[0] if self.addrs else 0
        self.hi = ((self.addrs[-1] + max(self.sizes[-1], 4))
                   if self.addrs else 0)
        return self

    def lookup(self, addr):
        """(name, offset) or None. A size of 0 is an alias, not a span."""
        k = bisect.bisect_right(self.addrs, addr) - 1
        if k < 0:
            return None
        base = self.addrs[k]
        size = self.sizes[k]
        off = addr - base
        if size and off >= size:
            # Past the end of the last function that starts at or below addr.
            # NO SLACK IS ALLOWED HERE. Attaching an address to the nearest
            # name below it is how a resolver reports a high percentage while
            # naming things wrongly, and it is not needed: with the slack at
            # zero all four captures still resolve every call target, so every
            # name in the listing is an exact hit inside that symbol's own
            # declared size. A size of 0 is an alias, not a span, and matches
            # only its own address.
            return None
        return self.names[k], off


def load_table(path, region):
    """Function symbols from one config/**/symbols.txt."""
    t = Table(region)
    if not os.path.isfile(path):
        return None
    with open(path, errors="replace") as f:
        for line in f:
            m = SYM_ROW.match(line)
            if not m:
                continue
            name, kind, attrs, addr = m.groups()
            if kind != "function":
                continue
            s = SIZE_IN.search(attrs)
            size = int(s.group(1), 0) if s else 0
            t.add(int(addr, 16), size, name)
    if not t.rows:
        return None
    return t.finish()


def load_overlay_yaml(path):
    """[{id, base, code_size, bss_size}], parsed without a YAML dependency.

    The file is a flat list of scalar keys under `overlays:` and has been for
    every dsd version this repo has used. Anything else is refused rather than
    half-read.
    """
    out = []
    cur = None
    started = False
    with open(path, errors="replace") as f:
        for line in f:
            if line.startswith("overlays:"):
                started = True
                continue
            if not started:
                continue
            st = line.strip()
            if st.startswith("- "):
                if cur is not None:
                    out.append(cur)
                cur = {}
                st = st[2:].strip()
            if cur is None or ":" not in st:
                continue
            k, v = st.split(":", 1)
            v = v.strip()
            try:
                cur[k.strip()] = int(v, 0)
            except ValueError:
                cur[k.strip()] = v
    if cur is not None:
        out.append(cur)
    rows = []
    for o in out:
        if "id" not in o or "base_address" not in o:
            sys.exit("romtrace_resolve: {} has an overlay row with no id or "
                     "base_address; this reader will not guess".format(path))
        rows.append(dict(id=o["id"], base=o["base_address"],
                         code=o.get("code_size", 0),
                         bss=o.get("bss_size", 0)))
    return rows


def load_overlays(repo):
    """Overlay metadata, head fingerprints and per-overlay symbol tables."""
    ypath = os.path.join(repo, "extracted", "dsd", "arm9_overlays",
                         "overlays.yaml")
    if not os.path.isfile(ypath):
        sys.exit("romtrace_resolve: no {}. The extracted ROM is a gitignored "
                 "input; wire it in before running this.".format(ypath))
    rows = load_overlay_yaml(ypath)

    bodies = os.path.join(repo, "extracted", "overlays")
    missing = []
    for o in rows:
        # extracted/overlays/, never extracted/dsd/arm9_overlays/: the dsd
        # copies are shifted.
        p = os.path.join(bodies, "overlay_{:04d}.bin".format(o["id"]))
        if not os.path.isfile(p):
            missing.append(o["id"])
            o["body"] = None
        else:
            with open(p, "rb") as f:
                o["body"] = f.read()
        o["table"] = load_table(
            os.path.join(repo, "config", "arm9", "overlays",
                         "ov{:03d}".format(o["id"]), "symbols.txt"),
            "ov{:03d}".format(o["id"]))
    if missing:
        sys.exit("romtrace_resolve: no overlay body for {}. Overlay residency "
                 "cannot be decided without them and this will not guess."
                 .format(", ".join("ov{:03d}".format(i) for i in missing)))

    by_base = collections.defaultdict(list)
    for o in rows:
        by_base[o["base"]].append(o)
    return rows, by_base


ZERO4 = (0, 0, 0, 0)


def words_at(o, addr):
    """The four words of overlay o's body that would sit at addr, or None."""
    off = addr - o["base"]
    if off < 0 or off + 16 > o["code"] or off + 16 > len(o["body"]):
        return None
    return struct.unpack("<4I", o["body"][off:off + 16])


def recommend_probes(overlays, by_base):
    """(addresses, notes): the --fingerprint list a capture should use.

    Every base, plus one extra address per base where the four words at the
    base do not separate the overlays that load there. The extra is the lowest
    16-byte-aligned offset inside every candidate's own code at which all of
    them differ and none of them is all-zero, so it distinguishes them from
    each other AND from an empty base.

    An overlay whose entire body is zero cannot be separated from an empty base
    by any probe, and is reported as such rather than worked around.
    """
    addrs = []
    notes = []
    for base in sorted(by_base):
        addrs.append(base)
        cands = by_base[base]
        stubs = [o for o in cands
                 if o["body"] is not None and not any(o["body"])]
        real = [o for o in cands if o not in stubs]
        if stubs:
            notes.append(
                "0x{:08x}: ov{} {} all zero bytes, so {} indistinguishable "
                "from an empty base by any probe. {} no code."
                .format(base,
                        ", ov".join("{:03d}".format(o["id"]) for o in stubs),
                        "is" if len(stubs) == 1 else "are",
                        "it is" if len(stubs) == 1 else "they are",
                        "It holds" if len(stubs) == 1 else "They hold"))
        if len(real) < 2:
            continue
        heads = [words_at(o, base) for o in real]
        if len(set(heads)) == len(real) and ZERO4 not in heads:
            continue
        limit = min(o["code"] for o in real)
        pick = None
        for off in range(0, limit - 15, 16):
            seen = [words_at(o, base + off) for o in real]
            if None in seen:
                break
            if len(set(seen)) == len(real) and ZERO4 not in seen:
                pick = off
                break
        if pick is None:
            notes.append(
                "0x{:08x}: no single extra probe separates the {} overlays "
                "here within their shared 0x{:x} bytes. Residency at this base "
                "stays ambiguous and a name is only reported where the "
                "candidates agree.".format(base, len(real), limit))
        else:
            addrs.append(base + pick)
            notes.append(
                "0x{:08x}: the four words at the base do NOT separate the {} "
                "overlays here; 0x{:08x} (base + 0x{:x}) does."
                .format(base, len(real), base + pick, pick))
    return addrs, notes


# ------------------------------------------------------------------ resolver

class Resolver(object):
    def __init__(self, repo):
        self.arm9 = load_table(
            os.path.join(repo, "config", "arm9", "symbols.txt"), "arm9")
        if self.arm9 is None:
            sys.exit("romtrace_resolve: no config/arm9/symbols.txt under {}"
                     .format(repo))
        # TRAP 2. Without this every division in the game is nameless.
        self.itcm = load_table(
            os.path.join(repo, "config", "arm9", "itcm", "symbols.txt"), "itcm")
        if self.itcm is None:
            sys.exit("romtrace_resolve: no config/arm9/itcm/symbols.txt. Every "
                     "division in the game goes through ITCM; resolving "
                     "without it would silently lose them.")
        self.overlays, self.by_base = load_overlays(repo)

        self.by_id = {o["id"]: o for o in self.overlays}

        # (definite ids, ambiguous id groups). Empty until a FINGERPRINT
        # record arrives, so a trace with none resolves no overlay at all
        # rather than carrying residency over from nowhere.
        self.resident = ((), ())
        self._chain_cache = {}
        self.fp_empty = collections.Counter()
        self.fp_unknown = collections.Counter()
        self.fp_covered = collections.Counter()     # base -> covering overlay
        self.fp_ambiguous = {}                      # base -> {ids: frames}
        self.fp_zero_or_overlay = {}                # base -> {ids: frames}
        self.residency_frames = collections.Counter()   # id -> frames resident
        self.saw_fingerprints = False
        self.ambig_calls = 0

    # -- overlay residency ------------------------------------------------

    def set_residency(self, fps):
        """fps: {address: (w0,w1,w2,w3)} as read this frame.

        A probe address is used against every overlay whose own span contains
        it, so the base probes and any extra discriminating probes are handled
        by the same rule and no probe needs to be declared in advance.
        """
        self.saw_fingerprints = True
        definite = []
        ambiguous = []
        unknown = []
        for base in sorted(self.by_base):
            if base not in fps:
                continue
            cands = self.by_base[base]
            hits = []
            for o in cands:
                ok = True
                for addr, words in fps.items():
                    have = words_at(o, addr)
                    if have is None:
                        continue        # this probe is outside o's own code
                    if have != words:
                        ok = False
                        break
                if ok:
                    hits.append(o)
            # "Nothing loaded" is every probe inside this base reading zero.
            empty_possible = all(
                words == ZERO4 for addr, words in fps.items()
                if base <= addr < base + max(o["code"] for o in cands))

            if hits and empty_possible:
                # The overlays that are all zeros where we looked. Cannot be
                # told from an empty base, so this does not guess: it records
                # the tie and treats the base as empty.
                key = tuple(o["id"] for o in hits)
                self.fp_zero_or_overlay.setdefault(base, collections.Counter())
                self.fp_zero_or_overlay[base][key] += 1
                self.fp_empty[base] += 1
            elif len(hits) == 1:
                definite.append(hits[0]["id"])
            elif len(hits) > 1:
                key = tuple(o["id"] for o in hits)
                self.fp_ambiguous.setdefault(base, collections.Counter())
                self.fp_ambiguous[base][key] += 1
                ambiguous.append(key)
            elif empty_possible:
                self.fp_empty[base] += 1
            else:
                unknown.append(base)

        # A base can read as unknown simply because a LARGER overlay resident
        # at a lower base is lying across it. Check that before calling it
        # unknown, or the summary carries a warning that is really just two
        # overlays overlapping.
        for base in unknown:
            words = fps[base]
            cover = None
            for i in definite:
                have = words_at(self.by_id[i], base)
                if have is not None and have == words:
                    cover = i
                    break
            if cover is not None:
                self.fp_covered[(base, cover)] += 1
            else:
                self.fp_unknown[base] += 1

        self.resident = (tuple(sorted(set(definite))),
                         tuple(sorted(set(ambiguous))))
        for i in self.resident[0]:
            self.residency_frames[i] += 1

    def _chain(self):
        key = self.resident
        c = self._chain_cache.get(key)
        if c is None:
            sure = [self.itcm, self.arm9]
            for i in key[0]:
                t = self.by_id[i]["table"]
                if t is not None:
                    sure.append(t)
            maybe = []
            for group in key[1]:
                os_ = [self.by_id[i] for i in group]
                span = (os_[0]["base"], max(o["code"] for o in os_))
                maybe.append((span, [o["table"] for o in os_
                                     if o["table"] is not None]))
            c = (sure, maybe)
            self._chain_cache[key] = c
        return c

    # -- address -> name --------------------------------------------------

    def resolve(self, addr):
        """(name, offset, region). region is '???' when nothing owns it."""
        sure, maybe = self._chain()
        for t in sure:
            if addr < t.lo or addr >= t.hi:
                continue
            hit = t.lookup(addr)
            if hit:
                return hit[0], hit[1], t.region
        for (base, span), tables in maybe:
            if not (base <= addr < base + span):
                continue
            answers = set()
            for t in tables:
                hit = t.lookup(addr) if t.lo <= addr < t.hi else None
                if hit:
                    answers.add(hit)
            if len(answers) == 1:
                name, off = answers.pop()
                return name, off, "ov?"
            if answers:
                # Two resident candidates for this base name this address
                # differently. Refusing is the whole point.
                self.ambig_calls += 1
                return "ambiguous_{:08x}".format(addr), 0, "???"
        return "unk_{:08x}".format(addr), 0, "???"


# ------------------------------------------------------------------ reading

def read_records(path):
    """Every record in the file, as a flat tuple of words.

    There is NO file header. The only structural check available is that the
    size divides by 32, and a file that fails it is refused: a reader that
    silently truncates a misaligned file would produce a listing that looks
    exactly like a good one.
    """
    if not os.path.isfile(path):
        sys.exit("romtrace_resolve: no trace at {}".format(path))
    size = os.path.getsize(path)
    if size == 0:
        sys.exit("romtrace_resolve: {} is empty".format(path))
    if size % RECORD_BYTES:
        sys.exit("romtrace_resolve: {} is {} bytes, which is not a whole "
                 "number of {}-byte records. This is not a romtrace .bin, or "
                 "it was truncated mid-write. Refusing to read it."
                 .format(path, size, RECORD_BYTES))
    with open(path, "rb") as f:
        blob = f.read()
    return struct.unpack("<{}I".format(size // 4), blob)


def selftest(res, trace_path):
    """Break a known-good trace four ways and require each break to show.

    A CHECK THAT HAS ONLY EVER PASSED PROVES NOTHING. A resolver that names
    nothing looks exactly like one that names everything if the only thing read
    is the exit code, and a fingerprint scheme that is quietly doing no work
    looks exactly like one that is doing it perfectly as long as the overlay
    happens to be the first table consulted. Each case below is a mutation of a
    real capture whose correct answer is known in advance.

    Returns the number of cases that did NOT behave as required.
    """
    blob = open(trace_path, "rb").read()
    fails = []

    class Args(object):
        strict = True

    def run(mutated, strict=True):
        a = Args()
        a.strict = strict
        r = Resolver.__new__(Resolver)
        r.__dict__.update(res.__dict__)
        r.resident = ((), ())
        r._chain_cache = {}
        r.fp_empty = collections.Counter()
        r.fp_unknown = collections.Counter()
        r.fp_covered = collections.Counter()
        r.fp_ambiguous = {}
        r.fp_zero_or_overlay = {}
        r.residency_frames = collections.Counter()
        r.saw_fingerprints = False
        r.ambig_calls = 0
        words = struct.unpack("<{}I".format(len(mutated) // 4), mutated)
        frames, unresolved, tag_bad, n = walk(words, r, a)
        return r, frames, unresolved, tag_bad, n

    print("=== selftest on {} ===".format(trace_path))

    # Baseline. Everything below is measured against this.
    _r, _f, unres0, tagbad0, n0 = run(blob)
    bad0 = sum(unres0.values())
    print("  baseline            : {} calls, {} unresolved".format(n0, bad0))
    if bad0 or tagbad0 or not n0:
        fails.append("the baseline capture is not clean, so nothing below "
                     "means anything")

    # 1. A file that is not a whole number of records must be REFUSED, not
    #    silently truncated to the last whole one.
    with tempfile.TemporaryDirectory() as td:
        p = os.path.join(td, "short.bin")
        with open(p, "wb") as f:
            f.write(blob[:-8])
        rc = os.system('"{}" "{}" "{}" --out-dir "{}" >{} 2>&1'.format(
            sys.executable, os.path.abspath(__file__), p,
            os.path.join(td, "o"), os.devnull))
        ok = rc != 0
        print("  misaligned file     : {}".format(
            "REFUSED, correct" if ok else "ACCEPTED, WRONG"))
        if not ok:
            fails.append("a file that is not a multiple of 32 bytes was read")

    # 2. A record with a tag that is not CALL, FRAME or FINGERPRINT must stop
    #    the run under --strict.
    mut = bytearray(blob)
    struct.pack_into("<I", mut, 0, 0xDEADBEEF)
    try:
        run(bytes(mut), strict=True)
        print("  bad tag, strict     : ACCEPTED, WRONG")
        fails.append("an unknown tag did not stop a --strict run")
    except SystemExit:
        print("  bad tag, strict     : REFUSED, correct")

    # 3. A call target pointed somewhere no symbol lives must come back
    #    unresolved, not attached to the nearest name below it.
    mut = bytearray(blob)
    hit = None
    for off in range(0, len(mut), RECORD_BYTES):
        if struct.unpack_from("<I", mut, off)[0] == TAG_CALL:
            hit = off
            break
    if hit is None:
        fails.append("no CALL record to corrupt")
    else:
        struct.pack_into("<I", mut, hit + 8, 0x02FF0000)
        _r, _f, unres, _t, _n = run(bytes(mut))
        ok = unres.get(0x02FF0000) == 1
        print("  target into nowhere : {}".format(
            "unresolved, correct" if ok else "NAMED ANYWAY, WRONG"))
        if not ok:
            fails.append("a call target in unmapped space was given a name")

    # 4. THE ONE THAT MATTERS. Corrupt every overlay fingerprint. Residency
    #    then cannot be decided, so every call into overlay space must become
    #    unresolved. If the number does NOT move, the fingerprints were never
    #    doing the work and the overlay names were coming from somewhere else.
    mut = bytearray(blob)
    nfp = 0
    for off in range(0, len(mut), RECORD_BYTES):
        if struct.unpack_from("<I", mut, off)[0] == TAG_FINGERPRINT:
            struct.pack_into("<I", mut, off + 8, 0xA5A5A5A5)
            nfp += 1
    if not nfp:
        print("  wrecked fingerprints: SKIPPED, this capture has none")
    else:
        _r, _f, unres, _t, n = run(bytes(mut))
        bad = sum(unres.values())
        ok = bad > bad0
        print("  wrecked fingerprints: {} unresolved, was {} -> {}".format(
            bad, bad0,
            "the overlay names really do come from them"
            if ok else "NOTHING CHANGED, WRONG"))
        if not ok:
            fails.append("wrecking every fingerprint changed nothing, so "
                         "overlay residency is not being decided by them")

    print("=== selftest: {} ===".format(
        "all cases behaved as required" if not fails
        else "{} FAILURES".format(len(fails))))
    for m in fails:
        print("  FAIL: {}".format(m))
    return len(fails)


def walk(words, res, args):
    """[(frame_no, [call, ...])], plus counters. A call is a tuple:
    (depth, caller, caller_off, callee, region, thumb_caller, thumb_callee,
     r0, r1, r2, r3)
    """
    frames = []
    cur = None
    stack = []
    fps = {}
    unresolved = collections.Counter()
    tag_bad = collections.Counter()
    ncalls = 0

    def open_frame(n):
        f = (n, [])
        frames.append(f)
        return f

    for i in range(0, len(words), RECORD_WORDS):
        r = words[i:i + RECORD_WORDS]
        tag = r[0]

        if tag == TAG_FRAME:
            if fps:
                res.set_residency(fps)
            fps = {}
            cur = open_frame(r[1])
            stack = []
            continue

        if tag == TAG_FINGERPRINT:
            fps[r[1]] = (r[2], r[3], r[4], r[5])
            continue

        if tag != TAG_CALL:
            tag_bad[tag] += 1
            if args.strict:
                sys.exit("romtrace_resolve: record {} has tag {} (0x{:08x}), "
                         "which is not CALL, FRAME or FINGERPRINT. The file is "
                         "not the format this reads, or it is corrupt. Rerun "
                         "with --no-strict to count them and continue."
                         .format(i // RECORD_WORDS, tag, tag))
            continue

        if cur is None:
            cur = open_frame(0)
        if fps:
            res.set_residency(fps)
            fps = {}

        frm, to = r[1], r[2]
        sp = r[7]
        caller, coff, _creg = res.resolve(frm & ~1)
        callee, eoff, ereg = res.resolve(to & ~1)
        if ereg == "???":
            unresolved[to & ~1] += 1
        # The callee address is a call TARGET, so a non-zero offset into a
        # function means the symbol table does not have that entry point.
        if eoff:
            callee = "{}+0x{:x}".format(callee, eoff)

        while stack and stack[-1] <= sp:
            stack.pop()
        depth = len(stack)
        stack.append(sp)

        cur[1].append((depth, caller, coff, callee, ereg,
                       frm & 1, to & 1, r[3], r[4], r[5], r[6]))
        ncalls += 1

    if fps:
        res.set_residency(fps)

    return frames, unresolved, tag_bad, ncalls


# ------------------------------------------------------------------ output

def write_named(frames, path, max_lines, show_args):
    n = 0
    with open(path, "w") as f:
        f.write("# named call trace of the REAL CARTRIDGE, from an "
                "instrumented melonDS\n")
        f.write("# columns: frame  depth  caller+off -> callee   [region]\n")
        f.write("# depth comes from the stack pointer at the call, the same "
                "way\n# port/tools/porttrace.py derives it, so the two "
                "indentations line up.\n")
        f.write("# names carry no overlay prefix and no argument list, so they "
                "are\n# spelled exactly as the port half spells them.\n")
        for fno, calls in frames:
            f.write("\n=== frame {} ===\n".format(fno))
            for (depth, caller, coff, callee, reg, tc, te,
                 r0, r1, r2, r3) in calls:
                if max_lines and n >= max_lines:
                    f.write("... truncated at --max-lines. The summary and the "
                            "per-frame report are complete.\n")
                    return
                extra = ""
                if show_args:
                    extra = ("   r0={:08x} r1={:08x} r2={:08x} r3={:08x}"
                             .format(r0, r1, r2, r3))
                f.write("{:5d} {:3d} {}{}+0x{:x} -> {}   [{}]{}\n"
                        .format(fno, depth, "  " * min(depth, 24), caller,
                                coff, callee, reg, extra))
                n += 1


def write_frames(frames, path):
    seen = set()
    with open(path, "w") as f:
        f.write("# per frame: how many calls, and which functions are called\n")
        f.write("# HERE FOR THE FIRST TIME in this capture.\n")
        f.write("#\n")
        f.write("# This is the useful one. A function appearing for the first "
                "time is\n# the game starting to do something new, which is "
                "where a screen gets built.\n\n")
        for fno, calls in frames:
            new = []
            for c in calls:
                callee, reg = c[3], c[4]
                if callee not in seen:
                    seen.add(callee)
                    new.append((callee, reg, c[7], c[8], c[9], c[10]))
            f.write("frame {:8d}   {} calls   {} new\n"
                    .format(fno, len(calls), len(new)))
            for callee, reg, r0, r1, r2, r3 in new:
                f.write("        NEW  {:<46s} [{}]  r0={:08x} r1={:08x} "
                        "r2={:08x} r3={:08x}\n"
                        .format(callee, reg, r0, r1, r2, r3))


def write_summary(frames, res, unresolved, tag_bad, path, trace_path, size):
    counts = collections.Counter()
    edges = collections.Counter()
    byregion = collections.Counter()
    thumb = 0
    for _fno, calls in frames:
        for (_d, caller, coff, callee, reg, tc, te, _a, _b, _c, _e) in calls:
            counts[callee] += 1
            edges[("{}+0x{:x}".format(caller, coff), callee)] += 1
            byregion[reg] += 1
            thumb += te
    total = sum(counts.values())
    named = total - byregion["???"]

    with open(path, "w") as f:
        f.write("trace             : {}\n".format(trace_path))
        f.write("bytes             : {} ({} records)\n"
                .format(size, size // RECORD_BYTES))
        f.write("frames            : {}\n".format(len(frames)))
        f.write("calls             : {}\n".format(total))
        f.write("NAMED             : {} of {} ({:.4f}%)\n"
                .format(named, total,
                        100.0 * named / total if total else 0.0))
        f.write("thumb targets     : {}\n".format(thumb))
        f.write("distinct callees  : {}\n".format(len(counts)))
        f.write("distinct edges    : {}\n".format(len(edges)))
        f.write("\n--- where the callees live ---\n")
        for reg, n in sorted(byregion.items(),
                             key=lambda kv: (-kv[1], kv[0])):
            f.write("  {:<8s} {:>9d}  ({:.2f}%)\n"
                    .format(reg, n, 100.0 * n / total if total else 0.0))
        if not byregion.get("itcm"):
            f.write("  NOTE: no ITCM call was seen. Either the capture really "
                    "made none, or\n        romtrace was run with --lo above "
                    "0x01ff8000, which makes every\n        division in the "
                    "game invisible. Check the capture before\n        "
                    "believing an empty ITCM row.\n")

        f.write("\n--- overlay residency, decided per frame from the "
                "fingerprints ---\n")
        f.write("An overlay swapped in and out inside a single frame is "
                "MISSED. This is\nresolved at one-frame granularity and cannot "
                "be finer than the trace is.\n\n")
        if not res.saw_fingerprints:
            f.write("  THIS TRACE CARRIES NO FINGERPRINT RECORDS AT ALL.\n"
                    "  Overlay residency is undecidable without them, so every "
                    "call into\n  overlay space is unresolved and the NAMED "
                    "figure above is a floor,\n  not a measurement of this "
                    "resolver. Re-capture with romtrace's\n  --fingerprint; "
                    "`romtrace_resolve.py --print-probes` prints the exact\n"
                    "  argument to pass.\n\n")
        if res.residency_frames:
            for i, n in sorted(res.residency_frames.items()):
                o = res.by_id[i]
                f.write("  ov{:03d}  base 0x{:08x}  size 0x{:x}  resident in "
                        "{} frames\n".format(i, o["base"], o["code"], n))
        else:
            f.write("  none\n")
        if res.fp_empty:
            f.write("\n  bases holding nothing (every probe read zero), "
                    "skipped:\n")
            for base, n in sorted(res.fp_empty.items()):
                f.write("    0x{:08x}  {} frames\n".format(base, n))
        if res.fp_zero_or_overlay:
            f.write("\n  bases read as empty that could ALSO be an overlay "
                    "which is zero\n  everywhere the probes looked. Treated as "
                    "empty, not guessed at.\n  Add a discriminating probe "
                    "(--print-probes) to settle these:\n")
            for base, groups in sorted(res.fp_zero_or_overlay.items()):
                for ids, n in sorted(groups.items()):
                    f.write("    0x{:08x}  {} frames  could be ov{}\n".format(
                        base, n,
                        ", ov".join("{:03d}".format(i) for i in ids)))
        if res.fp_covered:
            f.write("\n  bases lying UNDER a larger overlay resident at a "
                    "lower base. Not a\n  problem and not a missing overlay: "
                    "the words read there belong to\n  the covering overlay's "
                    "own body.\n")
            for (base, cover), n in sorted(res.fp_covered.items()):
                f.write("    0x{:08x}  {} frames  covered by ov{:03d}\n"
                        .format(base, n, cover))
        if res.fp_unknown:
            f.write("\n  bases holding something that is NOT one of our "
                    "overlays and is not\n  covered by a resident one, skipped "
                    "rather than guessed at:\n")
            for base, n in sorted(res.fp_unknown.items()):
                f.write("    0x{:08x}  {} frames\n".format(base, n))
        if res.fp_ambiguous:
            f.write("\n  bases where SEVERAL overlays match every probe, so "
                    "residency could\n  not be narrowed to one. All candidates "
                    "were consulted and a name is\n  reported only where they "
                    "agree; {} calls disagreed and are counted\n  unresolved. "
                    "Add a discriminating probe (--print-probes).\n"
                    .format(res.ambig_calls))
            for base, groups in sorted(res.fp_ambiguous.items()):
                for ids, n in sorted(groups.items()):
                    f.write("    0x{:08x}  {} frames  one of ov{}\n".format(
                        base, n,
                        ", ov".join("{:03d}".format(i) for i in ids)))

        if tag_bad:
            f.write("\n--- records with an unknown tag ---\n")
            for tag, n in tag_bad.most_common(20):
                f.write("  tag 0x{:08x}  {} records\n".format(tag, n))
        if unresolved:
            f.write("\n--- call targets that resolved to NO symbol ---\n")
            f.write("{} calls across {} distinct addresses\n"
                    .format(sum(unresolved.values()), len(unresolved)))
            for va, n in unresolved.most_common(40):
                f.write("  {:8d}  0x{:08x}\n".format(n, va))

        f.write("\n--- most called ---\n")
        for name, n in counts.most_common(60):
            f.write("{:8d}  {}\n".format(n, name))
        f.write("\n--- busiest edges ---\n")
        for (caller, callee), n in edges.most_common(60):
            f.write("{:8d}  {} -> {}\n".format(n, caller, callee))


# ------------------------------------------------------------------ main

def main():
    ap = argparse.ArgumentParser(
        description="resolve an instrumented-melonDS cartridge call trace "
                    "against the decomp's own symbol tables")
    ap.add_argument("trace", nargs="?", help="the .bin romtrace.exe wrote")
    ap.add_argument("--out-dir")
    ap.add_argument("--selftest", action="store_true",
                    help="break the given trace four ways and require each "
                         "break to show, then exit. Run this before believing "
                         "a percentage.")
    ap.add_argument("--print-probes", action="store_true",
                    help="print the --fingerprint argument a capture should "
                         "use, worked out from the overlay bodies on disk, and "
                         "exit. Every base, plus an extra address wherever the "
                         "words at the base do not separate the overlays that "
                         "load there.")
    ap.add_argument("--root", default=REPO,
                    help="repo root holding config/ and extracted/")
    ap.add_argument("--max-lines", type=int, default=400000,
                    help="cap on trace_named.txt only; the summary and the "
                         "per-frame report are always complete")
    ap.add_argument("--args", dest="show_args", action="store_true",
                    help="print r0-r3 on every line of trace_named.txt")
    ap.add_argument("--strict", dest="strict", action="store_true",
                    default=True,
                    help="stop on the first record with an unknown tag "
                         "(the default)")
    ap.add_argument("--no-strict", dest="strict", action="store_false",
                    help="count unknown-tag records and carry on")
    ap.add_argument("--require-named", type=float, default=None,
                    help="exit non-zero unless at least this percentage of "
                         "call targets resolved to a name. A resolver that "
                         "names nothing looks exactly like one that names "
                         "everything if you only read the exit code.")
    a = ap.parse_args()

    res = Resolver(a.root)

    if a.print_probes:
        addrs, notes = recommend_probes(res.overlays, res.by_base)
        print("--fingerprint " + ",".join("0x{:08x}".format(x)
                                          for x in sorted(addrs)))
        print("")
        for n in notes:
            print("  " + n)
        return 0

    if a.selftest:
        if not a.trace:
            ap.error("--selftest needs a known-good trace to break")
        return 1 if selftest(res, a.trace) else 0

    if not a.trace or not a.out_dir:
        ap.error("a trace and --out-dir are required unless --print-probes")

    words = read_records(a.trace)
    frames, unresolved, tag_bad, ncalls = walk(words, res, a)

    if not os.path.isdir(a.out_dir):
        os.makedirs(a.out_dir)
    write_named(frames, os.path.join(a.out_dir, "trace_named.txt"),
                a.max_lines, a.show_args)
    write_frames(frames, os.path.join(a.out_dir, "trace_frames.txt"))
    write_summary(frames, res, unresolved, tag_bad,
                  os.path.join(a.out_dir, "trace_summary.txt"),
                  a.trace, os.path.getsize(a.trace))

    bad = sum(unresolved.values())
    named = ncalls - bad
    pct = 100.0 * named / ncalls if ncalls else 0.0
    print("romtrace_resolve: {} frames, {} calls, {} named ({:.4f}%), "
          "{} unresolved -> {}"
          .format(len(frames), ncalls, named, pct, bad, a.out_dir))
    if tag_bad:
        print("romtrace_resolve: {} records carried an unknown tag"
              .format(sum(tag_bad.values())))
    if not res.saw_fingerprints:
        print("romtrace_resolve: THIS TRACE HAS NO FINGERPRINT RECORDS. "
              "Overlay residency\n  cannot be decided, so every call into "
              "overlay space is unresolved and the\n  percentage above says "
              "nothing about this resolver. Re-capture with\n  romtrace's "
              "--fingerprint; --print-probes prints the argument to pass.")
    if res.ambig_calls:
        print("romtrace_resolve: {} calls landed at a base where two resident "
              "candidates\n  disagreed about the name, and are counted "
              "unresolved rather than guessed."
              .format(res.ambig_calls))

    if a.require_named is not None and pct < a.require_named:
        print("romtrace_resolve: FAIL, {:.4f}% named is below the {:.4f}% "
              "this run required".format(pct, a.require_named))
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
