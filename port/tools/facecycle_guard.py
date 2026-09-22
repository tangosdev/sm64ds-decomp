"""facecycle_guard.py -- fail the link when a face forwards to another face.

WHY THIS GUARD EXISTS (run link100, lane FACEFIX, 2026-09-14).

The port reaches the ROM's own code through FACES. A face is a one-line
forwarder that bridges a name or an ABI: either

  R  the face defines the ROM's flat Itanium name and calls the C++ member
       extern "C" T _ZNxxx(void *self, ...) { ((Cls *)self)->Cls::meth(...); }
  F  the face defines the C++ member a caller's shadow class asks for and calls
     the ROM body under its flat name
       T Cls::meth(...) { return _ZNxxx(this, ...); }

Each shape is correct on its own, and each rests on a premise about the OTHER
side: an R face assumes the member is a real body, an F face assumes the flat
name is a real body. Nothing in the build ever checked either premise. When two
faces are written against each other -- by two people, in two files, months
apart, each looking only at its own side -- the premise fails on both and the
pair calls itself forever. There is no diagnostic: both sides resolve, no
symbol is duplicated, no symbol is unresolved, and every static gate passes. The
program dies of stack exhaustion the first time that code runs, which for most
of these is long after the boot everything else is tested against.

The class was already known in one shape and only in one shape. SYNC6 retired
nineteen F rows from port/faces_sync.txt when a translation unit came back into
the link and the generated forwarder became a SECOND definition. That is the
LOUD version: the linker prints LNK2005 and somebody fixes it. The silent
version is the same premise failing with no duplicate to announce it, and on
2026-09-14 the shipped artifact carried EIGHT of them.

WHAT IT CHECKS. Purely post-link, on the map and the image the linker just
wrote. Every function that is a trivial forwarder (a short straight-line body
whose single outgoing control transfer is a direct call or tail jump) becomes a
node; a cycle among those nodes is a face forwarding to a face. The check needs
no source, no ledger and no rebuild, which is the point: it is blind to WHO
wrote each side, so it catches generated faces, hand-written faces and any
mixture of the two, in either direction.

TWO TRAPS THIS GUARD PAYS FOR, both measured by earlier lanes tonight:

  1. /OPT:ICF MAKES ADDRESS IDENTITY MEANINGLESS. Bodies with identical bytes
     AND identical relocations fold onto one address, and folding is iterative,
     so forwarders fold once their targets do. On 2026-09-14 five names sat on
     00415870 and five more on 005c36d0, spanning three different classes in
     three different object files. A verdict that trusts one label per address
     is wrong roughly as often as it is right, so this guard prints EVERY name
     at every address in a cycle and never names a cycle by one symbol.

  2. A SWEEP THAT STOPS AT THE FIRST UNDECODABLE BYTE LOOKS EXACTLY LIKE A
     SWEEP THAT FOUND NOTHING. capstone's skipdata is on for that reason: data
     between functions must not silently end the scan and turn this guard into
     a rubber stamp.

EXIT CODES: 0 clean (or every cycle allowlisted), 1 a cycle was found, 2 the
guard could not run (missing file, capstone or pefile absent). 2 is a failure
too: a guard that cannot run must not read as a pass.

USAGE
    python port/tools/facecycle_guard.py <walk_window.map> [--exe <path>]
                                         [--allow port/facecycle_allow.txt]
The image defaults to the .exe beside the map.
"""

import argparse
import bisect
import collections
import os
import re
import sys

MAP_LINE = re.compile(
    r"^\s*([0-9a-fA-F]{4}):([0-9a-fA-F]{8})\s+(\S+)\s+([0-9a-fA-F]{8})"
    r"\s+(\S*)\s*(\S*)\s*$")
IMM = re.compile(r"^0x([0-9a-f]+)$")

# A forwarder is short and straight. Twelve instructions is generous: the
# longest real one in the 2026-09-14 image is seven (push ebp / mov ebp,esp /
# two argument pushes / mov ecx / call / add esp / pop ebp / ret). The cap is
# what keeps ordinary mutual recursion in game logic out of the graph.
MAX_FORWARDER_INSNS = 14


def load_map(path):
    """addr -> [(name, objfile)] for every public symbol."""
    names_at = collections.defaultdict(list)
    with open(path, "r", errors="replace") as fh:
        for line in fh:
            m = MAP_LINE.match(line.rstrip("\n"))
            if not m:
                continue
            _seg, _off, name, addr, flags, obj = m.groups()
            names_at[int(addr, 16)].append((name, obj or flags))
    return names_at


def load_allow(path):
    """A reviewed exception is one line naming any ONE symbol in the cycle.

    Kept deliberately weak: an exception is meant to be rare and argued in a
    comment beside it, not a maintained list.
    """
    allow = set()
    if path and os.path.exists(path):
        with open(path, "r", errors="replace") as fh:
            for line in fh:
                line = line.split("#", 1)[0].strip()
                if line:
                    allow.add(line)
    return allow


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("map")
    ap.add_argument("--exe")
    ap.add_argument("--allow")
    ap.add_argument("--verbose", action="store_true")
    args = ap.parse_args(argv)

    try:
        import pefile
        from capstone import Cs, CS_ARCH_X86, CS_MODE_32
    except ImportError as exc:                       # exit 2, never a pass
        print("facecycle_guard: CANNOT RUN (%s). A guard that cannot run is "
              "not a pass." % exc)
        return 2

    map_path = args.map
    exe_path = args.exe or os.path.splitext(map_path)[0] + ".exe"
    for p in (map_path, exe_path):
        if not os.path.exists(p):
            print("facecycle_guard: CANNOT RUN, missing %s" % p)
            return 2

    names_at = load_map(map_path)
    allow = load_allow(args.allow)

    pe = pefile.PE(exe_path, fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    code = [(base + s.VirtualAddress,
             base + s.VirtualAddress + max(s.Misc_VirtualSize, s.SizeOfRawData))
            for s in pe.sections if s.Characteristics & 0x20000000]

    def in_code(a):
        return any(lo <= a < hi for lo, hi in code)

    starts = sorted(a for a in names_at if in_code(a))
    if not starts:
        print("facecycle_guard: CANNOT RUN, no code symbols in %s" % map_path)
        return 2

    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = False
    md.skipdata = True                                # trap 2

    def owner(a):
        i = bisect.bisect_right(starts, a) - 1
        return starts[i] if i >= 0 else None

    def next_start(a):
        i = bisect.bisect_right(starts, a)
        return starts[i] if i < len(starts) else a + 0x400

    graph = {}
    bodies = {}
    trivial = set()
    for a in starts:
        end = min(next_start(a), a + 0x1000)
        off = a - base
        if off < 0 or off + (end - a) > len(img):
            graph[a] = []
            continue
        insns, targets, conditional, terminated = [], [], 0, False
        for ins in md.disasm(img[off:off + (end - a)], a):
            insns.append((ins.address, ins.mnemonic, ins.op_str))
            mn = ins.mnemonic
            if mn == "call":
                m = IMM.match(ins.op_str)
                if m:
                    targets.append(int(m.group(1), 16))
            elif mn == "jmp":
                m = IMM.match(ins.op_str)
                if m:
                    t = int(m.group(1), 16)
                    if not (a <= t < end):            # a tail call, not a loop
                        targets.append(t)
                terminated = True
                break
            elif mn.startswith("j"):
                conditional += 1
            elif mn.startswith("ret"):
                terminated = True
                break
            if len(insns) >= 400:
                break
        bodies[a] = insns
        graph[a] = [owner(t) for t in targets
                    if in_code(t) and owner(t) is not None]
        if (terminated and conditional == 0
                and len(insns) <= MAX_FORWARDER_INSNS and len(targets) == 1):
            trivial.add(a)

    # Cycles whose every node is a trivial forwarder.
    found = {}
    for s in trivial:
        stack = [(s, [s])]
        while stack:
            cur, path = stack.pop()
            for t in graph.get(cur, ()):
                if t == s and len(path) >= 2:
                    found.setdefault(tuple(sorted(set(path))), list(path))
                elif t in trivial and t not in path and len(path) < 6:
                    stack.append((t, path + [t]))

    live = {}
    for key, path in found.items():
        every = [n for a in path for (n, _o) in names_at[a]]
        if any(n in allow for n in every):
            continue
        live[key] = path

    print("facecycle_guard: %d code symbols, %d trivial forwarders, "
          "%d cycles (%d allowlisted)"
          % (len(starts), len(trivial), len(found), len(found) - len(live)))

    if not live:
        print("facecycle_guard: OK -- no face forwards to another face")
        return 0

    print("")
    print("facecycle_guard: FAIL -- %d face/forwarder recursion cycle(s). Each "
          "one is an" % len(live))
    print("unbounded call with no exit: the process dies of stack exhaustion "
          "the first")
    print("time that code runs. Fix the side whose premise is false, which is "
          "the side")
    print("that does NOT have a real body behind it; look the member and the "
          "flat name")
    print("up in the map and see which one a src/ object defines.")
    for key in sorted(live, key=lambda k: k[0]):
        path = live[key]
        print("")
        print("  CYCLE " + " -> ".join("%08x" % a for a in path + [path[0]]))
        for a in path:
            print("    %08x" % a)
            for (n, o) in names_at[a]:               # trap 1: EVERY name
                print("        %-60s [%s]" % (n, o))
            for (ia, mn, op) in bodies.get(a, [])[:MAX_FORWARDER_INSNS]:
                print("          %08x  %s %s" % (ia, mn, op))
    return 1


if __name__ == "__main__":
    sys.exit(main())
