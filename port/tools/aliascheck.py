#!/usr/bin/env python3
"""aliascheck -- an /alternatename is a NAME bridge and never an ABI bridge.

    #pragma comment(linker, "/alternatename:__ZN4Heap10SetDefaultEv="
                            "?SetDefault@Heap@@QAEHXZ")

tells link.exe "if the left name is undefined, resolve it to the right one".
It renames a symbol and says NOTHING about how that symbol is called.  The
LEFT side is what call sites emit: its spelling fixes where they put the
receiver and who pops the arguments.  The RIGHT side is the body that runs.
If the two disagree, the frame contract breaks and the linker never says a
word, because a rename is not a declaration.

THIS IS THE CLASS THAT KEEPS COMING BACK.  It has produced a fix on cons in
every wave that touched the host faces, most recently the six receiver
bridges of 2026-08-16 (Heap::SetDefault, Heap::Destroy, Heap::ResizeToFit,
Heap::_Destroy, the SolidHeapAllocator pair, FaderColor::AdvanceFade, and
the three Enemy base-ctor spawn sites of #1543).  Every one of them was
found by a person reading a fault, never by a tool.

TWO RULES, AND THE FIRST ONE IS THE LOAD-BEARING ONE

  RULE R -- RECEIVER LOCATION.  __thiscall takes the receiver in ECX and
      nowhere else.  Every other convention this tree uses either takes it as
      an ordinary first stack argument or has no receiver at all.  So an alias
      whose two sides disagree about WHERE THE RECEIVER LIVES delivers the
      object to a place the body does not read:

        flat C left, __thiscall right   the caller PUSHES the receiver, the
            body reads ECX.  Whatever ECX happened to hold becomes `this`.
            If the body only stores through it (Heap::SetDefault: `int old =
            G; G = (int)this; return old`) there is no fault at all -- it
            publishes a register nobody set and the boot carries on.  That is
            why this class survives green batteries for weeks.

        __thiscall left, flat C right   the caller puts the receiver in ECX,
            the body reads its first STACK argument as the receiver, which is
            the caller's first pushed argument -- or, when the caller pushed
            nothing, the caller's own return address.  Every declared
            argument shifts by one on top of that.

      RULE R IS DECIDED FROM THE DECORATION ALONE.  No build, no demangler, no
      external tool: the convention is one letter of an MSVC decorated name.
      That is the whole point.  This fires the moment a bad alias is WRITTEN,
      not after a player crashes.

  RULE P -- POP SIZE.  Both sides' callee-pop, from their declared parameter
      lists.  A __thiscall or __stdcall callee pops every declared argument, a
      __cdecl callee pops nothing, a __fastcall callee pops all but the two it
      took in registers.  Reading a parameter LIST out of an MSVC decoration
      needs a demangler, so this rule runs only where `undname` is on PATH
      (it ships beside dumpbin in the VS Build Tools the port builds with).
      WHEN undname IS ABSENT RULE P DOES NOT RUN AND SAYS SO.  It never
      silently reports a green it did not earn.

      Rule P is strictly secondary.  Every alias-form instance the original
      sweep proved -- Player::TryGrab, BlendModelAnim::SetAnim,
      Platform::KillByMegaChar -- is a receiver crossing and rule R catches
      all three without a demangler.  Rule P adds the pairs where both sides
      are decorated and only the argument counts differ.

WHAT COUNTS AS A DIRECTIVE.  This does not walk the tree itself.  It calls
alternatename_guard.collect_directives(), which already decides that question
correctly and had to be taught to: a bare /alternatename line quoted in lane
prose is NOT a linker input, and reading one as input used to fail the build
over a comment (commit ab554f5fd).  On this tree the naive spelling this
checker was originally written with reports 1590 directives where 1573 are
real and 17 are prose, and five of the prose lines are quotations of exactly
the receiver-crossing directives the receiver fixes DELETED.  A checker that
reports a defect out of the comment recording that the defect was fixed is
worse than no checker.

THE BASELINE IS A RATCHET, AND EVERY ROW IN IT IS ADJUDICATED.  Sixteen
receiver crossings are live on cons.  A crossing is a defect only if a caller
really does pass the receiver the way the other side does not read it, so
each of the sixteen was walked to its callers and its body and tagged in
aliascheck_baseline.txt:

    5 OPEN    live defects.  ExpandingHeapAllocator::DeallocateAll (which
              also leaks 8 bytes of stack per call), HeapAllocator::Remove,
              Player::GetHurtState, and the two declared spellings of
              Player::St_EndingFly_Main.  None is fixed here; they are five
              behavioural changes across four owners' files.
    11 INERT  real crossings with a checked reason they cannot misbehave: the
              body never reads the receiver, or the flat name is only
              address-taken into a vtable whose dispatcher puts the receiver
              in ECX anyway, or the directive is defeated by a defined LHS and
              never fires at all.

A crossing NOT in the file fails.  The OPEN rows are reported at every run,
separately from regressions, so a pass can never be read as "the class is
closed" -- only as "the tree did not get worse".  The list may only shrink.

    python port/tools/aliascheck.py [repo-root]     gate (exit 1 on new)
    python port/tools/aliascheck.py --selftest      fixture battery
    python port/tools/aliascheck.py --list          every crossing, with its
                                                    baseline status
    python port/tools/aliascheck.py --json out.json
"""

import argparse
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
PORT_DIR = os.path.dirname(HERE)
DEFAULT_ROOT = os.path.dirname(PORT_DIR)
BASELINE = os.path.join(HERE, "aliascheck_baseline.txt")

sys.path.insert(0, HERE)
import alternatename_guard as ag  # noqa: E402  (path set above)
import msvc_undname as mu     # noqa: E402


# --------------------------------------------------------------------------
# MSVC decoration: where does the receiver live, and what does the callee pop?
#
#   ?Name@Class@@ <access> [<this-cv>] <conv> <return> <params...> @Z
#
# access letter, from the MSVC table:
#   C D K L S T          STATIC member    -- no receiver, NO this-cv letter
#   A B E F G H I J      non-static member (private / private virtual /
#   M N O P Q R U V W X  protected / public / virtual / thunk) -- has a
#                        receiver AND a this-cv letter before the convention
#   Y Z                  free function    -- no receiver, no this-cv letter
# convention letter:
#   A B __cdecl    C D __pascal    E F __thiscall    G H __stdcall
#   I J __fastcall    M N __clrcall
#
# So QAE = public(Q) non-const-this(A) __thiscall(E), UAE = public virtual,
# SAX = public STATIC __cdecl returning void, YAX = free __cdecl returning
# void.  The receiver question is decided by two letters and nothing else.
# --------------------------------------------------------------------------
STATIC_ACCESS = set("CDKLST")
MEMBER_ACCESS = set("ABEFGHIJMNOPQRUVWX")
FREE_ACCESS = set("YZ")
CONV = {"A": "__cdecl", "B": "__cdecl", "C": "__pascal", "D": "__pascal",
        "E": "__thiscall", "F": "__thiscall", "G": "__stdcall",
        "H": "__stdcall", "I": "__fastcall", "J": "__fastcall",
        "M": "__clrcall", "N": "__clrcall"}

# Receiver location, and it is a BINARY question: ECX or the stack.
#
#   ECX     a non-static member declared __thiscall or __fastcall, and the
#           extern "C" @name@N fastcall spelling. The object never touches the
#           stack; the caller must have put it in ECX.
#   STACK   everything else. A free function, a STATIC member, a flat extern
#           "C" name, and a non-static member declared __cdecl or __stdcall
#           all take their receiver -- if they have one at all -- as an
#           ordinary first stack argument.
#
# Collapsing "no receiver" into STACK is deliberate rather than sloppy. A flat
# extern "C" name has no IMPLICIT receiver, so whatever receiver it carries is
# a declared argument sitting in the first stack slot: the same physical place
# a __cdecl member's `this` goes. Those two agree and must stay quiet. What
# never agrees is one side reading a register the other side never wrote.
R_ECX = "ecx"
R_STACK = "stack"


def decode(sym):
    """(kind, convention, receiver) for one decorated name.

    kind is 'data', 'func' or 'unknown'.  Data aliases (?name@@3..., the
    ??_7 vftables, ??_R RTTI) have no frame contract and are skipped.
    """
    if not sym:
        return "unknown", None, R_STACK
    if sym.startswith("??_7") or sym.startswith("??_R") or \
            sym.startswith("??_8") or sym.startswith("??_9"):
        return "data", None, R_STACK          # vftable / RTTI / vbtable
    if sym.startswith("?"):
        if "@@" not in sym:
            return "unknown", None, R_STACK
        body = sym.split("@@", 1)[1]
        # a template or nested scope can leave more @-runs; the encoding
        # letter is the first character after the LAST @@ run that opens the
        # signature, and for every name in this tree that is this one.
        while body[:1] == "@":
            body = body[1:]
        if not body:
            return "unknown", None, R_STACK
        if body[0].isdigit():
            return "data", None, R_STACK      # ?name@@3<type><cv> etc.
        acc = body[0]
        if acc in STATIC_ACCESS:
            conv = CONV.get(body[1:2])
            return "func", conv, R_STACK
        if acc in FREE_ACCESS:
            conv = CONV.get(body[1:2])
            return "func", conv, R_STACK
        if acc in MEMBER_ACCESS:
            conv = CONV.get(body[2:3])       # skip the this-cv letter
            if conv in ("__thiscall", "__fastcall"):
                return "func", conv, R_ECX
            return "func", conv, R_STACK
        return "unknown", None, R_STACK
    if sym.startswith("@"):
        return "func", "__fastcall", R_ECX   # extern "C" @name@N
    # A flat name: extern "C" __cdecl, or a C global. Which of the two is not
    # decidable from the spelling, so the caller filters on the OTHER side
    # being a function before it draws a conclusion.
    return "flat", "__cdecl", R_STACK


def crossing(lhs, rhs):
    """The receiver verdict for one directive, or None if it is not a
    function pair at all."""
    lk, lc, lr = decode(lhs)
    rk, rc, rr = decode(rhs)
    if lk == "data" or rk == "data":
        return None
    if lk == "unknown" or rk == "unknown":
        return None
    # One side flat and the other a DATA symbol is the common data-bridge
    # shape; both sides flat tells us nothing (no decoration to read). Only a
    # pair with at least one decoded FUNCTION carries a readable contract.
    if lk == "flat" and rk == "flat":
        return None
    if lr == rr:
        return None
    return dict(lhs=lhs, rhs=rhs, lhs_conv=lc, rhs_conv=rc,
                lhs_recv=lr, rhs_recv=rr,
                direction=("DROPPED" if rr == R_ECX else "SHIFTED"))


# --------------------------------------------------------------------------
# rule P: pop size, which needs a real parameter list
# --------------------------------------------------------------------------
CONV_POP = {"__cdecl": lambda n: 0, "__pascal": lambda n: 4 * n,
            "__stdcall": lambda n: 4 * n, "__thiscall": lambda n: 4 * n,
            "__fastcall": lambda n: 4 * max(0, n - 2)}
WIDE = ("double", "__int64", "long long")


def split_params(s):
    depth, cur, out = 0, "", []
    for ch in s:
        if ch in "(<":
            depth += 1
        elif ch in ")>":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur)
            cur = ""
        else:
            cur += ch
    out.append(cur)
    return [x.strip() for x in out if x.strip() and x.strip() != "void"]


def pop_of(sig):
    """(pop, why) from a demangled signature, or (None, why)."""
    if not sig or "(" not in sig:
        return None, "no parameter list"
    conv = next((c for c in CONV_POP if c in sig), None)
    if conv is None:
        return None, "no convention in signature"
    i, j = sig.find("("), sig.rfind(")")
    params = split_params(sig[i + 1:j])
    if any(w in p for p in params for w in WIDE):
        return None, "wide parameter, size is not 4"
    return CONV_POP[conv](len(params)), "%s/%d params" % (conv, len(params))


# Demangling lives in msvc_undname.py, and it lives there because THIS file
# got the invocation wrong. It piped the name list into undname's stdin;
# undname takes FILENAMES, answered with its usage banner and exit 1, and this
# helper read that non-zero exit as "the tool is missing". Rule P therefore
# reported NOT RUN on every machine including ones where undname is installed
# and works. Lane ABR1 caught it in review. One caller, one shape, one
# selftest that pins the file form and pins that stdin is refused.
def undname(names):
    """{decorated: demangled}, or {} if undname could not be used."""
    return mu.demangle(names, quiet=True)


# --------------------------------------------------------------------------
# baseline
# --------------------------------------------------------------------------
def load_baseline(path=BASELINE):
    """{directive: (tag, note)} where tag is OPEN, INERT or UNADJUDICATED.

    A row's adjudication rides in its trailing comment, so the file stays a
    plain list a human can read and the loader stays the strip-at-# shape the
    tree's other baselines use.  UNADJUDICATED is a real state and is
    reported: a crossing that got into the file without anybody deciding what
    it does is the failure mode a baseline invites.
    """
    rows = {}
    if not os.path.isfile(path):
        return rows
    with open(path, "r", encoding="utf-8") as f:
        for ln in f:
            key, _, comment = ln.partition("#")
            key = key.strip()
            if not key:
                continue
            comment = comment.strip()
            tag = "UNADJUDICATED"
            note = comment
            for t in ("OPEN", "INERT"):
                if comment.startswith(t + ":") or comment == t:
                    tag = t
                    note = comment[len(t):].lstrip(": ").strip()
                    break
            rows[key] = (tag, note)
    return rows


def directive_key(lhs, rhs):
    return "%s=%s" % (lhs, rhs)


# --------------------------------------------------------------------------
# selftest: every arm has a break that only that arm catches
# --------------------------------------------------------------------------
FIXTURES = [
    # (lhs, rhs, expected verdict)  verdict: None | 'DROPPED' | 'SHIFTED'
    # -- the six receiver bridges landed 2026-08-16, in their BROKEN form.
    #    Each of these is the directive the fix deleted. The checker must see
    #    every one of them; that is what makes it a regression net rather
    #    than a description of today.
    ("__ZN4Heap10SetDefaultEv", "?SetDefault@Heap@@QAEHXZ", "DROPPED"),
    ("__ZN4Heap7DestroyEv", "?Destroy@Heap@@QAEXXZ", "DROPPED"),
    ("__ZN4Heap11ResizeToFitEv", "?ResizeToFit@Heap@@QAEIXZ", "DROPPED"),
    ("__ZN4Heap8_DestroyEv", "?_Destroy@Heap@@QAEXXZ", "DROPPED"),
    ("__ZN10FaderColor11AdvanceFadeEv", "?AdvanceFade@FaderColor@@QAEHXZ",
     "DROPPED"),
    ("__ZN5Enemy16InitEnemyStandardEjRK7Vector3",
     "?InitEnemyStandard@dEnemyBase_c@@QAEXIABUVector3@@@Z", "DROPPED"),
    # -- the three the original prove.py pinned, all still catchable
    ("__ZN6Player7TryGrabER8dActor_c", "?TryGrab@Player@@QAE_NAAUActor@@@Z",
     "DROPPED"),
    ("__ZN14BlendModelAnim7SetAnimER8SharedFilePtr",
     "?SetAnim@BlendModelAnim@@QAEXAAUSharedFilePtr@@@Z", "DROPPED"),
    ("__ZN8Platform15KillByMegaCharER6Player",
     "?KillByMegaChar@dBgActor_c@@QAEXAAUPlayer@@@Z", "DROPPED"),
    # -- the OTHER direction: thiscall left, flat right (SolidHeapAllocator)
    ("?Allocate@SolidHeapAllocator@@QAEPAXIH@Z",
     "__ZN18SolidHeapAllocator8AllocateEji", "SHIFTED"),
    ("?MemoryLeft@ExpandingHeapAllocator@@QAEIXZ",
     "__ZN22ExpandingHeapAllocator10MemoryLeftEv", "SHIFTED"),
    # -- and the shapes that must stay QUIET, one per way of being quiet
    ("__ZN5Sound16LoadInitialGroupEi", "?LoadInitialGroup@Sound@@SAXH@Z",
     None),                       # STATIC member: no receiver either side
    ("__ZN2GX12SetBankForBGEt", "?SetBankForBG@GX@@YAXG@Z", None),
    ("?ChangeState@Player@@QAEHAAUState@1@@Z",
     "?ChangeState@PlayerChangeStateFace@@QAEHAAUState@1@@Z", None),
    ("?IDENTITY_MATRIX4X3@@3UM48@@A", "_data_02082128", None),   # data bridge
    ("__ZTV9FaderWipe", "??_7HalFaderWipe@@6B@", None),     # vftable bridge
    ("_port_crash_dir_get", "_port_crash_dir_get_stub", None),  # flat/flat
    # -- a virtual thiscall is still a thiscall (the UAE forms)
    ("__ZN6Player13OnYoshiTryEatEv", "?OnYoshiTryEat@Player@@UAEHXZ",
     "DROPPED"),
    # -- a member declared __cdecl keeps its receiver on the STACK, so
    #    bridging it from a flat C name is NOT a crossing
    ("__ZN4Heap10SetDefaultEv", "?SetDefault@Heap@@QAAHXZ", None),
    # -- but bridging that same __cdecl member FROM a thiscall spelling is
    ("?SetDefault@Heap@@QAEHXZ", "?SetDefault@Heap@@QAAHXZ", "SHIFTED"),
]

POP_FIXTURES = [
    # (signature, expected pop, note)
    ("public: int __thiscall Heap::SetDefault(void)", 0, "thiscall, no args"),
    ("public: void __thiscall Platform::KillByMegaChar(struct Player &)", 4,
     "thiscall, one arg"),
    ("void __cdecl LoadDoorObjects(struct ObjSubTable &,int,unsigned int)", 0,
     "cdecl pops nothing"),
    ("void __stdcall f(int,int)", 8, "stdcall pops both"),
    ("void __fastcall f(int,int,int)", 4, "fastcall pops all but two"),
    ("public: void __thiscall C::f(double)", None, "wide parameter refused"),
]


def selftest():
    bad = 0
    print("aliascheck --selftest")
    print("\n  RULE R (receiver location), %d fixtures" % len(FIXTURES))
    for lhs, rhs, want in FIXTURES:
        got = crossing(lhs, rhs)
        gotv = got["direction"] if got else None
        ok = gotv == want
        bad += 0 if ok else 1
        print("    %-4s want %-8s got %-8s  %s" %
              ("ok" if ok else "FAIL", want or "quiet", gotv or "quiet",
               lhs[:52]))
    print("\n  RULE P (pop size), %d fixtures" % len(POP_FIXTURES))
    for sig, want, note in POP_FIXTURES:
        got, why = pop_of(sig)
        ok = got == want
        bad += 0 if ok else 1
        print("    %-4s want %-4s got %-4s  %s" %
              ("ok" if ok else "FAIL", want, got, note))
    # The scanner arm: prose must not be a directive. This is the drift that
    # broke the original checker on this tree, so it is pinned here.
    print("\n  SCANNER (prose is not a linker input)")
    d, q = ag.collect_directives(PORT_DIR)
    ok = len(d) > 1000 and len(q) > 0
    bad += 0 if ok else 1
    print("    %-4s %d real directives, %d quoted-in-prose"
          % ("ok" if ok else "FAIL", len(d), len(q)))

    print("\n%s" % ("SELFTEST PASSED" if not bad
                    else "SELFTEST FAILED (%d)" % bad))
    return 1 if bad else 0


# --------------------------------------------------------------------------
def scan(root):
    port_dir = os.path.join(root, "port")
    directives, quoted = ag.collect_directives(port_dir)
    rows = []
    for lhs, rhs, rel, ln in directives:
        c = crossing(lhs, rhs)
        if c is None:
            continue
        c.update(file=rel, line=ln, key=directive_key(lhs, rhs))
        rows.append(c)
    return directives, quoted, rows


def main(argv):
    ap = argparse.ArgumentParser(add_help=True, description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("root", nargs="?", default=DEFAULT_ROOT)
    ap.add_argument("--selftest", action="store_true")
    ap.add_argument("--list", action="store_true",
                    help="print every crossing with its baseline status")
    ap.add_argument("--json", metavar="PATH")
    ap.add_argument("--baseline", default=BASELINE)
    args = ap.parse_args(argv[1:])

    if args.selftest:
        return selftest()

    root = os.path.abspath(args.root)
    directives, quoted, rows = scan(root)

    # A checker that scanned nothing has not passed. The port carries over
    # 1500 directives; a scan that finds a handful means the walk broke.
    if len(directives) < 100:
        print("aliascheck: REFUSED -- only %d /alternatename directives found "
              "under %s/port. The port carries well over a thousand; a scan "
              "this small means the walk is broken, not that the tree is "
              "clean." % (len(directives), root))
        return 2

    baseline = load_baseline(args.baseline)
    new = [r for r in rows if r["key"] not in baseline]
    for r in rows:
        r["baseline"] = baseline.get(r["key"], ("NEW", ""))[0]
        r["baseline_note"] = baseline.get(r["key"], ("NEW", ""))[1]
    open_rows = [r for r in rows if r["baseline"] == "OPEN"]
    inert = [r for r in rows if r["baseline"] == "INERT"]
    unadj = [r for r in rows if r["baseline"] == "UNADJUDICATED"]
    stale = sorted(set(baseline) - {r["key"] for r in rows})

    print("aliascheck -- /alternatename receiver and pop contracts")
    print("  %d directives read as LINKER INPUT, %d quoted in prose "
          "(not measured)" % (len(directives), len(quoted)))
    print("  %d receiver crossings: %d NEW, %d KNOWN-OPEN (live defects, "
          "unfixed), %d INERT (adjudicated harmless), %d UNADJUDICATED"
          % (len(rows), len(new), len(open_rows), len(inert), len(unadj)))

    if open_rows:
        print("\n--- KNOWN-OPEN: live receiver defects, already on cons, NOT "
              "regressions from this change ---")
        for r in sorted(open_rows, key=lambda r: (r["file"], r["line"])):
            print("  OPEN  %-8s %s:%d  %s"
                  % (r["direction"], r["file"], r["line"],
                     r["baseline_note"]))
            print("      L %s" % r["lhs"])
            print("      R %s" % r["rhs"])

    if unadj:
        print("\n--- UNADJUDICATED: in the baseline with no OPEN/INERT tag. "
              "Decide what each one does. ---")
        for r in unadj:
            print("  %s  %s:%d" % (r["key"], r["file"], r["line"]))

    if new or args.list:
        print("\n--- %s ---"
              % ("NEW receiver crossings, NOT in the baseline" if new
                 else "every receiver crossing (--list)"))
        for r in sorted(rows, key=lambda r: (r["baseline"] != "NEW",
                                             r["file"], r["line"])):
            if not args.list and r["baseline"] != "NEW":
                continue
            tag = ("NEW  <<<<<<" if r["baseline"] == "NEW"
                   else "%-11s" % r["baseline"])
            print("  %s  %s  %s:%d" % (tag, r["direction"], r["file"],
                                       r["line"]))
            print("      L %s   receiver %s (%s)"
                  % (r["lhs"], r["lhs_recv"], r["lhs_conv"]))
            print("      R %s   receiver %s (%s)"
                  % (r["rhs"], r["rhs_recv"], r["rhs_conv"]))

    if stale:
        print("\n--- baseline rows no longer present (RATCHET: delete them) ---")
        for k in stale:
            print("  %s" % k)

    # ---- rule P ------------------------------------------------------------
    pop_bad = []
    names = sorted({s for lhs, rhs, _, _ in directives for s in (lhs, rhs)
                    if s.startswith("?")})
    sigs = undname(names)
    if not sigs:
        print("\nRULE P NOT RUN: undname is not on PATH (it ships beside "
              "dumpbin in the VS Build Tools). Rule R above is complete "
              "without it; rule P is the secondary check and its absence is "
              "reported rather than passed over.")
    else:
        for lhs, rhs, rel, ln in directives:
            lk, _, _ = decode(lhs)
            rk, _, _ = decode(rhs)
            if lk != "func" or rk != "func":
                continue          # both sides must be decoded functions
            lp, lwhy = pop_of(sigs.get(lhs, ""))
            rp, rwhy = pop_of(sigs.get(rhs, ""))
            if lp is None or rp is None or lp == rp:
                continue
            key = directive_key(lhs, rhs)
            row = dict(lhs=lhs, rhs=rhs, file=rel, line=ln, lhs_pop=lp,
                       rhs_pop=rp, key=key, kind="POP")
            rows.append(row)
            if key not in baseline:
                pop_bad.append(row)
        print("\n  RULE P ran over %d demangled names; %d pop mismatches "
              "outside the baseline" % (len(sigs), len(pop_bad)))
        for r in pop_bad:
            print("    POP MISMATCH  %s:%d" % (r["file"], r["line"]))
            print("      L %s  pops %d" % (r["lhs"], r["lhs_pop"]))
            print("      R %s  pops %d" % (r["rhs"], r["rhs_pop"]))

    if args.json:
        import json
        with open(args.json, "w", encoding="utf-8") as f:
            json.dump(rows, f, indent=1)

    print()
    if new or pop_bad:
        print("ALIASCHECK FAILED: %d new receiver crossing(s), %d pop "
              "mismatch(es)." % (len(new), len(pop_bad)))
        print("An /alternatename cannot bridge a calling convention. Write a "
              "face that takes the receiver as its first argument and makes "
              "the qualified call, the shape hal/stage_slot0.cpp's LoadFog "
              "face and the six 2026-08-16 receiver bridges all use.")
        return 1
    print("ALIASCHECK PASSED: no NEW receiver crossing, no new pop mismatch.")
    if open_rows:
        print("  %d KNOWN-OPEN live defect(s) remain, frozen in %s with their "
              "evidence. A pass here means the tree did not get WORSE; it does "
              "not mean the class is closed."
              % (len(open_rows), os.path.basename(BASELINE)))
    if inert:
        print("  %d further crossing(s) adjudicated INERT." % len(inert))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
