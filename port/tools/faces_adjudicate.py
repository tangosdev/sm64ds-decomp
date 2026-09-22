#!/usr/bin/env python3
"""Extract every FACE host definition's body so a human can rule on it.

WHAT A FACE IS AND WHY THE RULING MATTERS. linkage.py's FACES bucket is every
symbol where a host file defines the ROM's C name AND the matched TU of that
name is also in the binary. Two very different things produce that shape:

  FORWARDS    the host definition is a one-line bridge that calls the matched
              body. The decompiled code is what runs; the face only carries the
              ROM's C name to it. Not work, and not scaffolding.
  DUPLICATES  the host definition restates the logic itself, and the matched TU
              is in the binary for some unrelated reason. The decompiled code
              is NOT what runs on that path. That is replacement work, and it
              is currently counted inside the published linked percentage.

linkage.py cannot tell them apart -- its own docstring says so -- because a map
records that both definitions linked and nothing about what either one does.
Only reading the host body settles it, and there are 210 of them.

WHAT THIS TOOL DOES AND DOES NOT DO. It does not rule. It finds each face
symbol's definition in its host source, extracts the body, and prints it with a
file:line reference and a SUGGESTED reading, so the ruling is made against the
text rather than against the symbol name. The suggestion is a reading aid with
a deliberately narrow basis, and it is wrong often enough that every row still
has to be read:

  looks-forward   the body is a single statement that names the same class and
                  method as the symbol demangles to, or calls the symbol's own
                  C name on another linkage.
  looks-duplicate the body has more than one statement and calls nothing that
                  matches the symbol.
  read-it         anything else.

THE TRAP THIS TOOL CANNOT SEE, and the reason the suggestion is not a verdict:
a face can forward to the WRONG target and still read as a forward.
_Z14ApproachLinearRsss forwarded to ApproachLinear2 for months -- a sibling 140
lines away differing only in s16 wrap -- and it was one line, it named a
plausible callee, and it shipped with signs spinning forever. See the checklist
at the top of port/hal/method_faces.cpp: target, arity, receiver, each checked
against the callee actually named.

    python port/tools/faces_adjudicate.py [repo-root] [--obj NAME] [--brief]
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import linkage  # noqa: E402


def default_root():
    here = os.path.dirname(os.path.abspath(__file__))
    return os.path.dirname(os.path.dirname(here))


def faces(root, mapfile):
    """{obj: {sym: matched-src}} for linkage.py's FACES bucket, same rules."""
    matched = linkage.matched_index(root)
    syms = linkage.map_symbols(mapfile)
    linked, hits = set(), {}
    for sym, obj in syms:
        stem = os.path.splitext(os.path.splitext(obj)[0])[0]
        if stem in matched:
            linked.add(stem)
        if stem not in matched and sym in matched:
            hits.setdefault(obj, set()).add(sym)
    out = {}
    for obj, s in hits.items():
        src = linkage.host_source_for(root, obj) or os.path.join(
            root, "port", "hal", "cxx_aliases.cpp")
        for sym in s:
            if linkage.abi_reason(src, sym):
                continue          # a documented exception, not a face
            if sym in linked:
                out.setdefault(obj, {})[sym] = (matched[sym], src)
    return out


# Itanium mangling is only decoded far enough to recover class and method, which
# is all the suggestion needs. `_ZN5dCc_c5ClearEv` -> CylinderClsn,
# Clear. Nested names and templates fall through and the row reads `read-it`,
# which is the right default for a name this cannot parse.
NESTED = re.compile(r"^_ZN((?:\d+\w+)+)E?")


FREE = re.compile(r"^_ZN?K?(\d+)(\w+)")


def demangle_parts(sym):
    m = NESTED.match(sym)
    if not m:
        # A FREE FUNCTION, `_Z14ApproachLinearRsss`, has no _ZN nesting. Left
        # unhandled it returns no parts, the suggestion has no target to look
        # for, and the two ApproachLinear faces -- the pair whose wrong-target
        # bug is the cautionary tale at the top of method_faces.cpp -- land in
        # the unread pile for the wrong reason.
        f = FREE.match(sym)
        if f:
            n = int(f.group(1))
            return [f.group(2)[:n]]
        return []
    parts, rest = [], m.group(1)
    while rest:
        d = re.match(r"(\d+)", rest)
        if not d:
            break
        n = int(d.group(1))
        start = d.end()
        parts.append(rest[start:start + n])
        rest = rest[start + n:]
    return parts


def strip_comments(text):
    """Blank out comments, preserving every byte offset and line number.

    THE READING THIS EXISTS TO PREVENT. hal/player_bridges.cpp documents the
    retirement of a stub by quoting it:

        This was `void _ZN11ShadowModel12InitCylinderEv(void *) {}`, the stub
        that kept the matched body out of the image; it is now the ordinary
        C-name-to-method bridge

    and a plain text search finds that empty body first, several lines above
    the real forward. The tool then reports the face as an empty stub -- which
    is precisely the DUPLICATES ruling, delivered with a file:line that points
    at prose about the opposite being true. Offsets are preserved rather than
    the comments deleted so the line number stays the real one.
    """
    out = list(text)
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c in "\"'":
            q = c
            i += 1
            while i < n and text[i] != q:
                i += 2 if text[i] == "\\" else 1
            i += 1
        elif text.startswith("//", i):
            while i < n and text[i] != "\n":
                out[i] = " "
                i += 1
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            for k in range(i, j):
                if out[k] != "\n":
                    out[k] = " "
            i = j
        else:
            i += 1
    return "".join(out)


def find_definition(text, sym):
    """(line number, body text) for sym's definition in a host source."""
    # A definition is the symbol name followed by an argument list and a brace,
    # possibly with the brace on the next line. Declarations end in `;` and are
    # skipped by requiring the brace.
    for m in re.finditer(r"\b" + re.escape(sym) + r"\s*\(", text):
        i = text.find(")", m.end() - 1)
        if i < 0:
            continue
        depth = 1
        i = m.end()
        while i < len(text) and depth:
            if text[i] == "(":
                depth += 1
            elif text[i] == ")":
                depth -= 1
            i += 1
        j = i
        while j < len(text) and text[j] in " \t\r\n":
            j += 1
        if j >= len(text) or text[j] != "{":
            continue                      # a declaration or a call site
        depth = 0
        k = j
        while k < len(text):
            if text[k] == "{":
                depth += 1
            elif text[k] == "}":
                depth -= 1
                if depth == 0:
                    break
            k += 1
        return text[:m.start()].count("\n") + 1, text[m.start():k + 1]
    return None, None


# A macro invocation at line start that carries the symbol as an argument:
# `HUD_FACE(_ZN3HUD15RenderCoinCountEv, RenderCoinCount)`.
MACRO_USE = re.compile(r"^([A-Z][A-Z0-9_]{2,})\s*\(([^)]*)\)", re.MULTILINE)


def find_macro_use(text, sym):
    """(line, macro name, macro definition line) when a macro defines sym.

    find_definition looks for `sym(` and a brace, which is every face in the
    tree except the five the HUD_FACE macro generates. For those the symbol is
    an ARGUMENT, never a declarator, so the search finds nothing and the row
    lands in the ledger citing `file:None` -- a location that cannot be checked
    and breaks the format the file promises. The macro invocation is the honest
    site: it is where the definition is written, even though the braces live in
    the #define.
    """
    for m in MACRO_USE.finditer(text):
        args = [a.strip() for a in m.group(2).split(",")]
        if sym not in args:
            continue
        name = m.group(1)
        line = text[:m.start()].count("\n") + 1
        d = re.search(r"^#define\s+" + re.escape(name) + r"\b", text,
                      re.MULTILINE)
        dline = text[:d.start()].count("\n") + 1 if d else None
        return line, name, dline
    return None, None, None


def suggest(sym, body):
    """A reading aid. Never a verdict -- see the module docstring."""
    if body is None:
        return "read-it", "no definition found by the extractor"
    inner = body[body.find("{") + 1:body.rfind("}")]
    stmts = [s.strip() for s in inner.split(";") if s.strip()]
    parts = demangle_parts(sym)
    cls, meth = (parts[0], parts[-1]) if len(parts) >= 2 else ("", "")
    # THREE SPELLINGS OF THE SAME FORWARD, and the first cut of this only
    # accepted one of them. `Class::Method(...)` is the qualified call; a face
    # far more often writes `((Class *)self)->Method(...)`, which is the same
    # call and read as "names no matching callee" put 40-odd clean forwards in
    # the pile a human had to work through. The third is a face that reaches
    # its own C name on the other linkage.
    #
    # The class name is required in both method spellings, not just the method
    # name, because dropping it is what makes the wrong-target trap invisible:
    # a bare `\bMethod\b` search says yes to a sibling of the same name on a
    # different class, which is the sharper edge of the same mistake that let
    # ApproachLinear forward to ApproachLinear2.
    qualified = (cls and meth and re.search(
        r"\b%s\s*::\s*%s\b" % (re.escape(cls), re.escape(meth)), inner))
    through_cast = (cls and meth and re.search(
        r"\(\s*(?:const\s+)?%s\s*\*\s*\)[^;]*?->\s*%s\s*\("
        % (re.escape(cls), re.escape(meth)), inner))
    free_fn = (not cls and meth and re.search(
        r"\b%s\s*\(" % re.escape(meth), inner))
    names_target = bool(qualified or through_cast or free_fn
                        or re.search(r"\b" + re.escape(sym) + r"\b", inner))
    if len(stmts) <= 1 and names_target:
        return "looks-forward", "one statement naming %s::%s" % (cls, meth)
    if names_target:
        return "read-it", "%d statements, does name the target" % len(stmts)
    if len(stmts) > 1:
        return "looks-duplicate", "%d statements, names no matching callee" \
            % len(stmts)
    return "read-it", "%d statement(s), names no matching callee" % len(stmts)


# HAND RULINGS. Every symbol here was read individually because the mechanical
# reading could not settle it, and the ruling below is the human one. The
# reading aid never overrides these.
#
# The recurring reason a face lands here is the SHADOW-CLASS IDIOM: the host
# declares a local one-method class and aliases its mangling onto the real
# one, so the body names `StarMarkerFace::SpawnRedCoinStarIfNecessary` and the
# linker sends it to `StarMarker::SpawnRedCoinStarIfNecessary`. Nothing in the
# text says the two are the same function; the /alternatename pragma beside it
# does, and that is the evidence cited.
HAND = {
    "_Z14ApproachLinearRsss": ("FORWARDS", "calls ApproachLinear, the s16 "
        "overload, NOT the ApproachLinear2 sibling this face once had"),
    "_Z15ApproachLinear2Rsss": ("FORWARDS", "calls ApproachLinear2, its own "
        "target"),
    "_Z14ApproachLinearRiii": ("FORWARDS", "calls ApproachLinear, the int "
        "overload"),
    "_ZNK7PathPtr8NumNodesEv": ("FORWARDS", "qualified PathPtr::NumNodes"),
    "_ZNK9Animation13GetFrameCountEv": ("FORWARDS", "Animation::GetFrameCount "
        "through a const cast"),
    "_ZN11ShadowModel12InitCylinderEv": ("FORWARDS", "null-vtable guard then "
        "ShadowModel::InitCylinder"),
    "_ZN9Animation8LoadFileER13SharedFilePtr": ("FORWARDS", "null-fileptr "
        "guard then Animation::LoadFile"),
    "_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii": ("FORWARDS", "0xffff "
        "terminator guard then OAM::Render; the guard turns a runaway walk "
        "off the end of main RAM into one skipped sprite"),
    "_ZNK5dBgPi9GetClsnIDEv": ("FORWARDS", "qualified "
        "ClsnResult::GetClsnID"),
    "_ZNK10dBgCh_Actr13GetWallResultEv": ("FORWARDS",
        "WithMeshClsn::GetWallResult through a const cast"),
    "_ZNK10dBgCh_Actr14GetFloorResultEv": ("FORWARDS",
        "WithMeshClsn::GetFloorResult through a const cast"),
    "_ZNK10dBgCh_Actr12TouchesWaterEv": ("FORWARDS", "qualified "
        "WithMeshClsn::TouchesWater"),
    "_ZN5Model8LoadFileER13SharedFilePtr": ("FORWARDS", "PORT_TRACE_SETFILE "
        "tracing around Model::LoadFile"),
    "_ZN9ModelBase7SetFileEP8BMD_Fileii": ("FORWARDS", "PORT_TRACE_SETFILE "
        "tracing around ModelBase::SetFile"),
    # The four HUD_FACE rows cite the INVOCATION line, which is where the
    # definition is written; the braces are in the #define at line 283. They
    # used to cite `sub_actors.cpp:None`, a location nobody can check.
    "_ZN3HUD15RenderCoinCountEv": ("FORWARDS", "HUD_FACE macro (defined at "
        "sub_actors.cpp:283), expands to ((HUD *)s)->HUD::RenderCoinCount()"),
    "_ZN3HUD15RenderLifeCountEv": ("FORWARDS", "HUD_FACE macro (defined at "
        "sub_actors.cpp:283), expands to ((HUD *)s)->HUD::RenderLifeCount()"),
    "_ZN3HUD15RenderTimeTimerEv": ("FORWARDS", "HUD_FACE macro (defined at "
        "sub_actors.cpp:283), expands to ((HUD *)s)->HUD::RenderTimeTimer()"),
    "_ZN3HUD17RenderHealthMeterEv": ("FORWARDS", "HUD_FACE macro (defined at "
        "sub_actors.cpp:283), expands to "
        "((HUD *)s)->HUD::RenderHealthMeter()"),
    "_ZNK9Animation12WillHitFrameEi": ("FORWARDS", "two hops: hal_anim_willhit "
        "at player_bridges.cpp:763 calls Animation::WillHitFrame"),
    "_ZN5dPa_c7level_c20checkYoganCallback_c14SpawnParticlesERN8Particle6SystemE":
        ("FORWARDS", "nested-namespace method through a cast"),
    "_ZN5dPa_c7level_c23cleanParticleCallback_c8OnUpdateERN8Particle6SystemEb":
        ("FORWARDS", "nested-namespace method through a cast"),
    "_ZN5dPa_c7level_c26checkWaterRippleCallback_c8OnUpdateERN8Particle6SystemEb":
        ("FORWARDS", "nested-namespace method through a cast"),
    "_ZNK7dBgW_Kc13GetUnkOctreeYEv": ("FORWARDS", "qualified "
        "MeshCollider::GetUnkOctreeY"),
    "_ZNK7dBgW_Kc16GetOctreeOriginYEv": ("FORWARDS", "qualified "
        "MeshCollider::GetOctreeOriginY"),
    "_ZNK7PathPtr5LoopsEv": ("FORWARDS", "qualified PathPtr::Loops"),
    "_ZN2GX7LoadTexEPKvjj": ("FORWARDS", "optional texture log then "
        "GX::LoadTex"),
    "_ZN11CommonModel6RenderEPK7Vector3": ("FORWARDS", "qualified "
        "CommonModel::Render, returns 1 for a caller that ignores it"),
    "_ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE": ("FORWARDS",
        "both branches call Platform::UpdateKillByMegaChar; the probe only "
        "counts"),
    "_ZN10StarMarker27SpawnRedCoinStarIfNecessaryEv": ("FORWARDS",
        "shadow-class idiom, alias pragma at actor_classes_bob_world.cpp:311"),
    "_ZN5Model6RenderEPK7Vector3": ("FORWARDS",
        "shadow-class idiom, alias pragma at actor_faces_bob.cpp:172"),
    "_ZN6Player14InitMetalWarioEv": ("FORWARDS",
        "shadow-class idiom, alias pragma at Cap_StateDispatch.cpp:67"),
    # WAS DUPLICATES, FIXED IN THE SAME WAVE. It restated the matched body --
    # DeathTable_SetBit(*(short *)(self + 0xce)) against the matched TU's
    # DeathTable_SetBit(deathTableId) with deathTableId declared at 0xce -- and
    # is now the ordinary bridge. The ledger keeps the original row and records
    # the fix in a dated section, because it is append-only; this entry tracks
    # the code, which is what a re-run has to agree with.
    "_ZN8dActor_c17TrackInDeathTableEv": ("FORWARDS",
        "qualified Actor::TrackInDeathTable; was the bucket's one DUPLICATES "
        "and was replaced in run linkw wave C"),
}


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    want_obj = None
    if "--obj" in sys.argv:
        want_obj = sys.argv[sys.argv.index("--obj") + 1]
    brief = "--brief" in sys.argv
    ledger = "--ledger" in sys.argv
    root = os.path.abspath(args[0]) if args else default_root()
    mapfile = os.path.join(root, "build", "port", "walk_window.map")

    fc = faces(root, mapfile)
    total = sum(len(v) for v in fc.values())
    if not ledger:
        print("root: %s" % root)
        print("faces: %d symbols across %d host objects" % (total, len(fc)))
        print()

    cache = {}
    counts = {}
    for obj in sorted(fc, key=lambda o: -len(fc[o])):
        if want_obj and obj != want_obj:
            continue
        print("== %s (%d) ==" % (obj, len(fc[obj])))
        for sym in sorted(fc[obj]):
            tu, src = fc[obj][sym]
            if src not in cache:
                with open(src, errors="replace") as f:
                    cache[src] = strip_comments(f.read())
            line, body = find_definition(cache[src], sym)
            macro = None
            if line is None:
                line, macro, dline = find_macro_use(cache[src], sym)
                if macro:
                    macro = "%s macro, defined at line %s" % (macro, dline)
            tag, why = suggest(sym, body)
            if macro and sym not in HAND:
                why = macro
            if sym in HAND:
                tag, why = HAND[sym]
            elif ledger and tag == "looks-forward":
                # In the ledger the mechanical reading is stated as the ruling
                # it is: the body is ONE statement whose only call names the
                # same class and the same method the symbol demangles to. That
                # is a forward by construction, and the wrong-target shape the
                # class name guards against cannot hide inside it. Every row
                # was still read; the rule is what makes 178 of them readable
                # in one pass instead of one at a time.
                tag = "FORWARDS"
                why = "one statement calling %s" % why.split("naming ")[-1]
            counts[tag] = counts.get(tag, 0) + 1
            rel = src.replace("\\", "/").split("/port/")[-1]
            if ledger:
                print("%-62s %-11s %s:%s -- %s"
                      % (sym, tag, rel, line, why))
                continue
            print("  %-15s %-52s %s:%s" % (tag, sym, rel, line))
            if not brief:
                print("      why: %s" % why)
                if body:
                    one = " ".join(body.split())
                    print("      %s" % (one[:300]))
        if not ledger:
            print()
    if ledger:
        return
    print("SUGGESTIONS (a reading aid, not verdicts):")
    for k in sorted(counts, key=lambda x: -counts[x]):
        print("  %-16s %4d" % (k, counts[k]))


if __name__ == "__main__":
    main()
