#!/usr/bin/env python
"""Line up two function-entry traces and name what they disagree about.

EITHER SIDE MAY BE THE PORT OR THE CARTRIDGE. The format is detected from the
file, not declared on the command line:

  PORT       port/hal/fn_trace.cpp, MSVC /Gh. A 24-byte "PTF3" header and then
             three words a record: the callee, the call site, and the caller's
             stack pointer. The older two-word "PTF2" is still read, and is
             marked on sight because its caller column was inferred rather than
             recorded. Read by port/tools/porttrace.py, resolved through that
             build's walk_window.map.
  CARTRIDGE  the instrumented melonDS of lane ROMTRACE. No header at all, eight
             words a record, the file size a multiple of 32. Read by
             port/tools/romtrace_resolve.py, resolved through config/**/
             symbols.txt plus the per-frame overlay fingerprints.

So `port vs port` is the self-check with a known answer and `port vs cartridge`
is the real job, and both go through the same code below.

WHAT THE TWO HALVES RECORD IS NOT THE SAME THING. Four asymmetries, all of them
handled here rather than papered over, and all of them re-printed at the bottom
of every run so nobody reads a table without them:

  1. THE EMULATOR HOOKS BL AND BLX. IT DOES NOT HOOK B OR BX. A ROM function
     that its caller reaches by a tail branch is INVISIBLE to the cartridge
     trace. /Gh hooks the callee's own entry, so the port records that entry
     however it was reached. A function that appears "port only" can therefore
     be a tail-called ROM function rather than a defect, and this is the single
     most likely false positive in the whole comparison.

  2. THE PORT DOES NOT RECORD ARGUMENTS and never will: /Gh fires after the
     arguments are already wherever the calling convention put them, and the
     port's conventions are not the ARM's. Argument values are therefore never
     compared. The caller itself IS recorded on both sides now, so edges are
     real on both, but the cartridge's caller carries an offset into the call
     site and the port's does not, so offsets are dropped: the cartridge's
     `f+0x2c -> g` and the port's `f -> g` are the same edge here.

  3. THE PORT RUNS HOST FUNCTIONS THE CARTRIDGE NEVER HAD: port/hal,
     port/unmatched, port/ntr and the C runtime. --host collapse (the default)
     removes them from the tree and re-parents their ROM descendants onto the
     nearest ROM ancestor, so `romA -> hostX -> romB` compares against the
     cartridge's `romA -> romB`. How many were removed is printed, never hidden.

  4. THE CARTRIDGE TRACE CONTAINS INTERRUPT AND DMA WORK the port emulates
     somewhere else entirely. --irq keep (the default) leaves it in; --irq drop
     removes those calls and everything nested under them. Either way the census
     is printed for both sides, so the reader can see what the other choice
     would have done without re-running.

FRAME WINDOWS ARE NEVER SLID OVER EACH OTHER. Each side reports the window it
covers. When the two differ the per-frame rate is what gets ranked, and the
tool says so; it does not search for an offset that makes the traces agree.
"""

import argparse
import collections
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import porttrace            # noqa: E402  (same directory, deliberately)
import romtrace_resolve     # noqa: E402

REPO = os.path.dirname(os.path.dirname(HERE))

PORT_MAGICS = (porttrace.MAGIC2, porttrace.MAGIC3)
ROM_RECORD_BYTES = 32

# The host functions that mean a hardware register was really written.
# ntr::io_write is the general seam; the geometry ports have their own.
WRITE_SEAMS = ("ntr::io_write", "ntr::gx_write_port", "ntr::gx_write_fifo",
               "ntr::ipc_reg_write")

# INTERRUPT AND DMA WORK, by name, and the rule is ONE LEVEL DEEP ON PURPOSE.
#
# A call counts as interrupt/DMA work when the callee is named below, or when
# the function it is attributed to is. It is NOT propagated down a whole
# subtree, and that is a measurement rather than a preference: on the cartridge
# side nesting is reconstructed from the stack pointer and the stack is reset at
# every frame boundary, so the first record of a frame always reads as depth
# zero however deep it really was. On the ten-frame title capture 165 of the
# 4,448 interrupt/DMA calls land at that fake depth zero, and a subtree rule
# therefore swallows everything after them: it reports 23,405 of 27,571 calls as
# interrupt work, which is plainly wrong. An interrupt also runs on its own
# stack, so stack-pointer nesting across the interrupt boundary means nothing in
# the first place.
#
# WHAT THE ONE-LEVEL RULE COSTS, measured on that same capture: 4,448 calls by
# callee name, and 91 more whose caller is an interrupt handler and whose callee
# is not (IRQ::VBlankHandler -> OS_WakeupThread and five more like it). Anything
# deeper than one level inside a handler body is NOT identified as interrupt
# work and is compared like any other call. That is the honest floor of a
# name-based rule and it is small.
IRQ_PREFIXES = ("IRQ::", "DMA")

# An address-shaped name: `func_020c4684`, `func_ov007_020c0b78`. The address it
# carries is the only thing that identifies it, and the two halves do not always
# spell the same address the same way.
ADDR_NAME = re.compile(r"^(?:func|data)_(0[0-9a-fA-F]{7})$")
SYM_ROW = re.compile(r"^(\S+)\s+kind:function\(([^)]*)\)\s+addr:(0x[0-9a-fA-F]+)")


def rom_name_index(repo):
    """{ROM address: the best name the decomp has for it}.

    ONE ADDRESS CAN CARRY TWO NAMES AND THE TWO HALVES CAN PICK DIFFERENT ONES.
    The cartridge resolver reads config/**/symbols.txt and prefers a real name
    over an address-shaped placeholder; the port reads its own link map and gets
    whatever that build linked. Measured on the title screen: 0x01ffabe4 is
    `__aeabi_idiv` on the cartridge side and `func_01ffabe4` on the port side,
    509 calls against 379, and without this it reads as one function each side
    never enters instead of one function both sides enter.

    ARM9 AND ITCM ONLY, AND NOT THE OVERLAYS. 103 overlays share 22 base
    addresses, so an overlay address does not name a function: reading every
    config/**/symbols.txt into one address table renamed ov007 rows after
    whichever overlay happened to be scanned last and turned 69 one-sided names
    into 72. An overlay name carries its overlay inside it already and is left
    exactly as it is.
    """
    best = {}
    for path in (os.path.join(repo, "config", "arm9", "symbols.txt"),
                 os.path.join(repo, "config", "arm9", "itcm", "symbols.txt")):
        if not os.path.isfile(path):
            continue
        with open(path, errors="replace") as f:
            for line in f:
                m = SYM_ROW.match(line)
                if not m:
                    continue
                name, addr = m.group(1), int(m.group(3), 16)
                cur = best.get(addr)
                if cur is None or (ADDR_NAME.match(cur)
                                   and not ADDR_NAME.match(name)):
                    best[addr] = name
    return best


def dealias(name, index):
    """An address-shaped name replaced by the decomp's real name for it."""
    m = ADDR_NAME.match(name)
    if not m:
        return name
    return index.get(int(m.group(1), 16), name)


# ------------------------------------------------------------------ loading

def detect_format(path):
    """'port' or 'rom'. Refuses anything else rather than guessing."""
    if not os.path.isfile(path):
        sys.exit("tracediff: no such trace: {}".format(path))
    size = os.path.getsize(path)
    with open(path, "rb") as f:
        head = f.read(4)
    if len(head) < 4:
        sys.exit("tracediff: {} is {} bytes, too short to be a trace"
                 .format(path, size))
    first = struct.unpack("<I", head)[0]
    if first in PORT_MAGICS:
        return "port"
    if size and size % ROM_RECORD_BYTES == 0 and first in (
            romtrace_resolve.TAG_CALL, romtrace_resolve.TAG_FRAME,
            romtrace_resolve.TAG_FINGERPRINT):
        return "rom"
    sys.exit(
        "tracediff: {} is neither format. A port trace starts with PTF2 or "
        "PTF3; a cartridge trace is a multiple of {} bytes and starts with "
        "tag 1, 2 or "
        "3. This one is {} bytes and starts {:#010x}. Refusing to guess."
        .format(path, ROM_RECORD_BYTES, size, first))


def load_port_side(path, map_path, root, label, index):
    if not map_path:
        sys.exit("tracediff: {} is a PORT trace, so it needs the "
                 "walk_window.map of the build that wrote it".format(path))
    mapinfo = porttrace.load_map(map_path, root)
    words, hdr = porttrace.read_trace(path)
    frames, unresolved = porttrace.resolve(words, hdr, mapinfo)
    clsof = dict((dealias(n, index), c)
                 for n, c in zip(mapinfo[1], mapinfo[2]))
    out = [(fno, [(dealias(caller, index), dealias(callee, index), cls)
                  for _d, caller, callee, cls in calls])
           for fno, calls in frames]
    if not hdr.get("has_caller"):
        print("NOTE: {} is a PTF2 trace, written before the caller was "
              "recorded. Its caller column comes from the stack pointer and is "
              "wrong wherever one function makes two calls with different "
              "argument counts. Read the per-function table, not the edge "
              "table, off it.".format(path))
    # Every ROM name this build actually carries. A ROM function the cartridge
    # entered and the port did not means something different depending on
    # whether the port HAS the function at all, and this is what tells the two
    # cases apart.
    present = set(dealias(n, index)
                  for n, c in zip(mapinfo[1], mapinfo[2]) if c == "ROM")
    return {"label": label, "kind": "port", "frames": out,
            "truncated": bool(hdr["filled"]),
            "unresolved": sum(unresolved.values()),
            "present": present, "path": path, "clsof": clsof,
            "has_caller": bool(hdr.get("has_caller"))}


class _StrictArgs(object):
    strict = True


def load_rom_side(path, repo, label):
    res = romtrace_resolve.Resolver(repo)
    words = romtrace_resolve.read_records(path)
    frames, unresolved, tag_bad, _n = romtrace_resolve.walk(
        words, res, _StrictArgs())
    if tag_bad:
        sys.exit("tracediff: {} has {} records with an unknown tag. Not "
                 "reading it.".format(path, sum(tag_bad.values())))
    # The emulator records the address of the BL, so the caller is read here as
    # well, and the caller's offset inside it is dropped: the port cannot
    # recover an offset, so comparing one would only ever disagree.
    out = []
    for fno, calls in frames:
        rows = [(c[1], c[3], "???" if c[4] == "???" else "ROM") for c in calls]
        out.append((fno, rows))
    if not res.saw_fingerprints:
        print("NOTE: {} carries NO fingerprint records, so no overlay could be "
              "resolved in it. Re-capture with romtrace's --fingerprint (see "
              "romtrace_resolve.py --print-probes) before reading anything "
              "below as a measurement.".format(path))
    return {"label": label, "kind": "rom", "frames": out,
            "truncated": False, "unresolved": sum(unresolved.values()),
            # The cartridge has every ROM function by construction, so the
            # "is it present at all" question only ever applies to the port,
            # and every name on this side is a ROM name.
            "present": None, "path": path, "clsof": {}, "has_caller": True}


def load_side(path, map_path, root, repo, label, index):
    kind = detect_format(path)
    side = (load_port_side(path, map_path, root, label, index) if kind == "port"
            else load_rom_side(path, repo, label))
    calls = sum(len(c) for _f, c in side["frames"])
    print("{:<10s} {:<10s} {:>5d} frames  {:>12,d} calls  {}".format(
        label, side["kind"], len(side["frames"]), calls,
        "BUFFER FILLED -- TRUNCATED" if side["truncated"] else "buffer ok"))
    if side["truncated"]:
        print("           raise SM64DS_FN_TRACE_MB or narrow the frame window")
    if side["unresolved"]:
        print("           {:,} unresolved call targets".format(
            side["unresolved"]))
    return side


# ------------------------------------------------------------------ shaping

def is_irq(name):
    return any(name.startswith(p) for p in IRQ_PREFIXES)


def nearest_rom(stack, through_irq=False):
    """The deepest surviving ROM ancestor, or None.

    This is what re-parents a ROM callee over the host frames between it and
    its ROM caller. It also means that under --host keep a host callee is still
    reported as an edge out of its nearest ROM ancestor rather than out of
    another host function, which keeps the caller column comparable between the
    two policies.

    through_irq looks past an ancestor that --irq drop removed. The interrupt
    test needs that, so that whether a call counts as interrupt work does not
    depend on whether interrupt work is being dropped: the census has to be the
    same number under both policies or it is not a census.
    """
    for name, cls, suppressed in reversed(stack):
        if cls != "ROM":
            continue
        if suppressed is None or (through_irq and suppressed == "irq"):
            return name
    return None


def shape(side, host_mode, irq_mode):
    """Walk each frame's (caller, callee, class) rows back into a tree and tally
    it, applying the host and interrupt policies on the way.

    BOTH HALVES NOW RECORD THE CALLER, so the tree is rebuilt by unwinding to
    the named caller and nothing here depends on a stack pointer. The emulator
    has always had the caller, since it hooks the BL itself; the port gained it
    when the /Gh hook started recording the call site instead of the caller's
    esp. One walk serves both and neither side gets a rule the other does not.
    """
    edges = collections.Counter()
    counts = collections.Counter()
    writes = collections.Counter()
    rom = set()
    dropped_host = 0
    dropped_irq = 0
    irq_calls = 0
    unresolved_rows = 0

    clsof = side.get("clsof") or {}
    romside = side["kind"] == "rom"

    for _fno, rows in side["frames"]:
        stack = []            # [(name, cls, suppressed)]
        for caller, callee, cls in rows:
            # Unwind to the recorded caller. If it is not on the stack it was
            # already running when the window opened, so it becomes this
            # branch's root.
            idx = -1
            for j in range(len(stack) - 1, -1, -1):
                if stack[j][0] == caller:
                    idx = j
                    break
            if idx >= 0:
                del stack[idx + 1:]
            else:
                ccls = "ROM" if romside else clsof.get(caller, "HOST")
                csup = None
                if ccls != "ROM" and host_mode == "collapse":
                    csup = "host"
                stack = [(caller, ccls, csup)]

            if cls == "???":
                unresolved_rows += 1

            suppressed = None
            if cls != "ROM":
                if callee in WRITE_SEAMS:
                    # The write seams are counted BEFORE they are collapsed:
                    # that count is the whole point of the port-versus-port arm.
                    caller = nearest_rom(stack)
                    if caller:
                        writes[caller] += 1
                if host_mode == "collapse":
                    suppressed = "host"
                    dropped_host += 1

            # The interrupt census is taken over the calls that actually take
            # part in the comparison, so a host function removed a line earlier
            # is not also counted as interrupt work. One level, and computed
            # past a dropped interrupt ancestor, so the census is the same
            # number under --irq keep and --irq drop.
            if suppressed is None:
                in_irq = is_irq(callee) or is_irq(
                    nearest_rom(stack, through_irq=True) or "")
                if in_irq:
                    irq_calls += 1
                    if irq_mode == "drop":
                        suppressed = "irq"
                        dropped_irq += 1

            if suppressed is None:
                caller = nearest_rom(stack) or "(root)"
                edges[(caller, callee)] += 1
                counts[callee] += 1
                if cls == "ROM":
                    rom.add(callee)

            stack.append((callee, cls, suppressed))

    return {"edges": edges, "counts": counts, "writes": writes, "rom": rom,
            "dropped_host": dropped_host, "dropped_irq": dropped_irq,
            "irq_calls": irq_calls, "unresolved_rows": unresolved_rows,
            "frames": len(side["frames"])}


# ------------------------------------------------------------------ report

def rate(n, frames):
    return (float(n) / frames) if frames else 0.0


def ranked(ta, tb, key, fa, fb, per_frame):
    """[(score, key, na, nb)] over one Counter field, worst disagreement first.

    Ties break on the key's text so two runs of the same pair print the same
    order. Without that a table reordered by dictionary iteration reads as a
    change when nothing changed.
    """
    rows = []
    A, B = ta[key], tb[key]
    for k in set(A) | set(B):
        na, nb = A[k], B[k]
        if na == nb:
            continue
        score = (abs(rate(na, fa) - rate(nb, fb)) if per_frame
                 else float(abs(na - nb)))
        if score <= 0.0:
            continue
        rows.append((score, str(k), k, na, nb))
    rows.sort(key=lambda r: (-r[0], r[1]))
    return [(s, k, na, nb) for s, _t, k, na, nb in rows]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--a", required=True, help="reference trace (.bin)")
    ap.add_argument("--a-map", default=None, help="only if --a is a port trace")
    ap.add_argument("--b", required=True, help="candidate trace (.bin)")
    ap.add_argument("--b-map", default=None, help="only if --b is a port trace")
    ap.add_argument("--root", default=REPO)
    ap.add_argument("--repo", default=REPO,
                    help="the decomp tree the cartridge side resolves against")
    ap.add_argument("--top", type=int, default=25)
    ap.add_argument("--host", choices=("collapse", "keep"), default="collapse",
                    help="collapse: drop port-only host functions and re-parent "
                         "their ROM descendants (default). keep: leave them in, "
                         "which makes every host call an edge the cartridge "
                         "cannot have.")
    ap.add_argument("--irq", choices=("keep", "drop"), default="keep",
                    help="keep: leave interrupt and DMA work in (default). "
                         "drop: remove those calls and their subtrees. The "
                         "census is printed either way.")
    ap.add_argument("--expect", default=None,
                    help="a ROM function this diff MUST name in the hardware-"
                         "write table. Makes the tool usable as a self-check "
                         "with a known answer; exits non-zero if it is missing.")
    ap.add_argument("--expect-only-b", default=None,
                    help="a ROM function this diff MUST report as entered by B "
                         "and never by A. The same self-check for the table the "
                         "cartridge comparison actually reads.")
    ap.add_argument("--expect-agree", action="store_true",
                    help="require that NOTHING disagrees. For proving a change "
                         "that must not move the trace, such as excluding the "
                         "software renderer from the hook.")
    a = ap.parse_args()

    index = rom_name_index(a.repo)

    print("=== the two traces ===")
    A = load_side(a.a, a.a_map, a.root, a.repo, "A (ref)", index)
    B = load_side(a.b, a.b_map, a.root, a.repo, "B (cand)", index)

    fa = [f for f, _c in A["frames"]]
    fb = [f for f, _c in B["frames"]]
    print("\nframe windows: A {}..{} ({} frames)   B {}..{} ({} frames)".format(
        min(fa) if fa else "-", max(fa) if fa else "-", len(fa),
        min(fb) if fb else "-", max(fb) if fb else "-", len(fb)))
    per_frame = len(fa) != len(fb)
    if per_frame:
        print("THE WINDOWS ARE DIFFERENT LENGTHS, so raw totals are not")
        print("comparable and every table below is ranked by CALLS PER FRAME.")
        print("The raw counts are printed beside them. The traces are NOT slid")
        print("over each other to make them agree.")

    ta = shape(A, a.host, a.irq)
    tb = shape(B, a.host, a.irq)
    nfa, nfb = ta["frames"], tb["frames"]

    print("\n=== what was filtered, and from which side ===")
    print("  {:<28s} {:>12s} {:>12s}".format("", A["label"], B["label"]))
    for text, ka, kb in (
            ("host calls removed", ta["dropped_host"], tb["dropped_host"]),
            ("interrupt/DMA calls seen", ta["irq_calls"], tb["irq_calls"]),
            ("interrupt/DMA removed", ta["dropped_irq"], tb["dropped_irq"]),
            ("unresolved rows", ta["unresolved_rows"], tb["unresolved_rows"]),
            ("calls compared", sum(ta["counts"].values()),
             sum(tb["counts"].values()))):
        print("  {:<28s} {:>12,d} {:>12,d}".format(text, ka, kb))
    print("  policy: --host {}  --irq {}".format(a.host, a.irq))

    print("\n=== ROM functions entered by one side only ===")
    only_a = sorted(ta["rom"] - tb["rom"])
    only_b = sorted(tb["rom"] - ta["rom"])
    both = ta["rom"] & tb["rom"]
    print("  both sides        {:>5d}".format(len(both)))
    print("  {} only       {:>5d}".format(A["label"], len(only_a)))
    print("  {} only      {:>5d}".format(B["label"], len(only_b)))

    # The cross-reference that turns a name into a verdict. Only a port knows
    # whether it HAS a function, so this only prints when a port is involved.
    port_is_b = B["kind"] == "port"
    port_is_a = A["kind"] == "port"
    mixed = A["kind"] != B["kind"]

    if only_a:
        print("\n  -- entered by {} and never by {} --".format(
            A["label"], B["label"]))
        absent = []
        for n in only_a[:max(a.top, 60)]:
            tag = ""
            if port_is_b:
                if n in B["present"]:
                    tag = "   [the port build HAS it and never enters it]"
                else:
                    tag = "   [not in the port build at all]"
                    absent.append(n)
            print("    {:>8,d}  {}{}".format(ta["counts"][n], n, tag))
        if len(only_a) > max(a.top, 60):
            print("    ... and {} more".format(len(only_a) - max(a.top, 60)))
        if port_is_b:
            absent_all = [n for n in only_a if n not in B["present"]]
            print("    {} of {} are not in the port build at all; the other {} "
                  "are built and never entered."
                  .format(len(absent_all), len(only_a),
                          len(only_a) - len(absent_all)))

    if only_b:
        print("\n  -- entered by {} and never by {} --".format(
            B["label"], A["label"]))
        for n in only_b[:max(a.top, 60)]:
            tag = ""
            if mixed and port_is_b:
                tag = "   [a tail-branch entry is invisible to the cartridge]"
            if port_is_a and n not in A["present"]:
                tag = "   [not in the port build at all]"
            print("    {:>8,d}  {}{}".format(tb["counts"][n], n, tag))
        if len(only_b) > max(a.top, 60):
            print("    ... and {} more".format(len(only_b) - max(a.top, 60)))

    print("\n=== per-function entry counts that disagree ===")
    rows = ranked(ta, tb, "counts", nfa, nfb, per_frame)
    shared = [r for r in rows if r[1] in both]
    if not shared:
        print("  every function entered by both sides is entered the same")
        print("  number of times. That is a suspicious result, not a good one:")
        print("  point this tool at a build whose fault is already known")
        print("  before believing it.")
    else:
        print("  {:>9s} {:>9s}  {:>8s} {:>8s}  {}".format(
            A["label"][:9], B["label"][:9], "per frm", "per frm", "function"))
        for _s, name, na, nb in shared[:a.top]:
            print("  {:>9,d} {:>9,d}  {:>8.2f} {:>8.2f}  {}".format(
                na, nb, rate(na, nfa), rate(nb, nfb), name))
        if len(shared) > a.top:
            print("  ... and {} more".format(len(shared) - a.top))

    print("\n=== caller -> callee edges that disagree most ===")
    erows = ranked(ta, tb, "edges", nfa, nfb, per_frame)
    if not erows:
        print("  no edge disagrees")
    else:
        print("  {:>9s} {:>9s}  {:>8s} {:>8s}  {}".format(
            A["label"][:9], B["label"][:9], "per frm", "per frm", "edge"))
        for _s, (caller, callee), na, nb in erows[:a.top]:
            flag = ""
            if na and not nb:
                flag = "   <-- {} ONLY".format(A["label"])
            elif nb and not na:
                flag = "   <-- {} ONLY".format(B["label"])
            print("  {:>9,d} {:>9,d}  {:>8.2f} {:>8.2f}  {} -> {}{}".format(
                na, nb, rate(na, nfa), rate(nb, nfb), caller, callee, flag))
        if len(erows) > a.top:
            print("  ... and {} more".format(len(erows) - a.top))

    print("\n=== hardware writes per ROM caller ===")
    print("Only meaningful PORT vs PORT: the cartridge writes its registers")
    print("with a store, not a call, so its column is empty by construction.")
    wrows = ranked(ta, tb, "writes", nfa, nfb, False)
    if not wrows:
        print("  no caller changed its hardware-write count")
    else:
        print("  {:>9s} {:>9s}  {}".format(A["label"][:9], B["label"][:9],
                                           "ROM caller"))
        for _s, c, na, nb in wrows[:a.top]:
            flag = ""
            if nb == 0 and na > 0:
                flag = "   <-- WRITES VANISHED"
            elif na == 0 and nb > 0:
                flag = "   <-- WRITES APPEARED"
            print("  {:>9,d} {:>9,d}  {}{}".format(na, nb, c, flag))
        if len(wrows) > a.top:
            print("  ... and {} more".format(len(wrows) - a.top))

    print("\n=== read every table above against these ===")
    if mixed:
        print("  1. THE EMULATOR HOOKS BL AND BLX AND NOTHING ELSE. A ROM")
        print("     function its caller reaches by a tail branch never appears")
        print("     in the cartridge trace, and the port records it anyway.")
        print("     That is the most likely reason for a port-only name.")
        print("  2. Both sides record the caller, so edges are real on both,")
        print("     but the offset inside the caller is dropped and argument")
        print("     values are never compared at all.")
        print("  3. --host {}: {:,} host calls removed from the port side."
              .format(a.host, tb["dropped_host"] + ta["dropped_host"]))
        print("  4. --irq {}: {:,} interrupt/DMA calls on {}, {:,} on {}."
              .format(a.irq, ta["irq_calls"], A["label"], tb["irq_calls"],
                      B["label"]))
    else:
        print("  Both sides are {} traces, so the format asymmetries do not"
              .format(A["kind"]))
        print("  apply. --host {} --irq {} were still applied to both."
              .format(a.host, a.irq))

    rc = 0
    if a.expect or a.expect_only_b or a.expect_agree:
        print("\n=== self-check ===")
    if a.expect:
        named = [c for _s, c, _na, _nb in wrows]
        if a.expect in named:
            print("PASS: {} is named, at rank {} of {} in the hardware-write "
                  "table".format(a.expect, named.index(a.expect) + 1,
                                 len(named)))
        else:
            print("FAIL: {} is NOT named in the hardware-write table. The tool "
                  "did not find a fault whose answer was already known, so it "
                  "is not to be trusted on one that is not.".format(a.expect))
            rc = 1
    if a.expect_only_b:
        if a.expect_only_b in only_b:
            print("PASS: {} is reported as entered by {} only"
                  .format(a.expect_only_b, B["label"]))
        else:
            print("FAIL: {} is NOT reported as {}-only. The one-side-only "
                  "table did not find an answer that was already known."
                  .format(a.expect_only_b, B["label"]))
            rc = 1
    if a.expect_agree:
        n = len(only_a) + len(only_b) + len(shared) + len(erows)
        if n == 0:
            print("PASS: the two traces agree on every function and every edge")
        else:
            print("FAIL: {} disagreements ({} A-only, {} B-only, {} counts, {} "
                  "edges), and this run required none."
                  .format(n, len(only_a), len(only_b), len(shared), len(erows)))
            rc = 1

    if A["truncated"] or B["truncated"]:
        print("\nWARNING: a trace was truncated, so nothing above is safe to "
              "believe. Re-capture before using this.")
        rc = rc or 2
    return rc


if __name__ == "__main__":
    sys.exit(main())
