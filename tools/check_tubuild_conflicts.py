#!/usr/bin/env python3
"""Reconcile a TU's recorded conflict warnings against the conflicts still in its source.

`tubuild.py create` unions the legacy files that make up a translation unit. When two
of them disagree -- same macro, same extern, same shadow declaration, different text --
it refuses to pick a winner. It does two things instead, and they are meant to stay in
step:

  * `assemble_shadow_source` writes the losing text into the generated source inside a
    `/* TUBUILD CONFLICT -- ... NOT applied: */` comment, so a human can see it;
  * `build_manifest_entry` freezes a matching `tubuild create warning: CONFLICT: ...`
    line into the manifest entry's `notes`.

Nothing ever revisits that note. A human who reconciles the conflict deletes the marker
from the source, and the note stays behind claiming an open conflict that no longer
exists. Measured on main when this check was written: of the 29 manifest entries that
carry either a note or a marker, 8 had every conflict resolved with all their notes
still standing, 4 were partially resolved, and 1 (ov006/MgWhichWiggler) had the
opposite drift -- a hand-written entry whose notes never carried the warning list, over
a source that still holds an unresolved conflict nobody has recorded.

That last shape is the one that matters. A conflict marker is a *deferred decision*:
two legacy files disagreed and the generated source kept one arbitrarily. If it is
neither recorded nor resolved, the TU is carrying an unreviewed guess.

So this check enforces a two-way correspondence rather than a count:

  note (plain)      <-> marker present     ok, an open conflict, still recorded
  note (RESOLVED)   <-> no marker          ok, reconciled and the history kept
  note (plain)      <-> no marker          STALE: resolved, note not updated
  note (RESOLVED)   <-> marker present     REOPENED: marker came back
  no note           <-> marker present     UNRECORDED: a deferred decision off the books

with one refinement on top, because a STALE and an UNRECORDED can be the same record
seen twice. Both halves name the legacy function the losing text came from. Renaming a
symbol rewrites that name inside the source marker -- it is comment text a tree-wide
substitution reaches -- and leaves the manifest note, which is prose, spelling the old
name. The pair then fails to match and is reported as two unrelated problems on a
conflict nobody touched. So before reporting either, notes and markers that differ ONLY
in how they spell the legacy function are paired up and reported once:

  note(f) + marker(g), f and g one address    RENAME DESYNC: rewrite the note's name

That is not an exemption -- the check still fails, and the marker still stands for an
open conflict that must not be deleted. It is the difference between telling someone
their record is gone and telling them a name moved. The two spellings must resolve to
the same address in the same module, so notes and markers about genuinely different
functions never collapse together.

Resolving a conflict is therefore a two-line edit: delete the marker from the source,
and rewrite its note from

    tubuild create warning: CONFLICT: macro 'FOO' differs between ...

to

    tubuild create warning (RESOLVED): CONFLICT: macro 'FOO' differs between ...

Rewriting rather than deleting is deliberate. The note is the only surviving record
that two legacy files ever disagreed about that name; a future reader who finds the
reconciled text wants to know it was a judgement call, not a transcription.

Pairing is exact, not by count. Both sides carry the conflict kind, the key, and the
name of the legacy file's function the losing text came from, so the reconciliation key
is `(kind, key, legacy function)` and there is no ambiguity even when one TU has fifty
conflicts over the same handful of names -- ov062/Koopa does (49 of them).

Usage:
    python tools/check_tubuild_conflicts.py             # every manifest entry
    python tools/check_tubuild_conflicts.py ov006/dScMgCard_c ...
    python tools/check_tubuild_conflicts.py --list      # per-conflict inventory
"""
import argparse
import ast
import collections
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import tu_manifest  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent
MANIFEST = REPO / "config" / "tu_manifest.d"

# --- the note side, from tubuild.build_manifest_entry + _merge_field ----------------
#
# f"tubuild create warning: {w}" over
# f"CONFLICT: {kind_label} {key!r} differs between the legacy file that used to hold "
# f"{seen[key][1]} and the one that held {name}; kept the first, the other is "
# f"commented out for review"
#
# `key!r` is a plain string repr for macros and externs, and a (kind, name) TUPLE repr
# for local declarations, because _merge_field keys those on `(item[0], item[1])`.
NOTE_RE = re.compile(
    r"^tubuild create warning(?P<resolved> \(RESOLVED\))?: CONFLICT: "
    r"(?P<kind>macro|extern declaration|local declaration) "
    r"(?P<key>.+?) differs between the legacy file that used to hold "
    r"(?P<first>\S+) and the one that held (?P<second>[^;]+); kept the first")

# --- the marker side, from tubuild.assemble_shadow_source ---------------------------
MARK_DECL_BODY = re.compile(
    r"/\* TUBUILD CONFLICT -- alternate body of (?P<dkind>\w+) (?P<dname>'[^']*'|\"[^\"]*\"), "
    r"from the legacy file for (?P<fn>\S+), NOT applied:")
MARK_MACRO = re.compile(
    r"/\* TUBUILD CONFLICT -- alternate #define of (?P<key>\S+), from the legacy "
    r"file for (?P<fn>\S+), NOT applied:")
MARK_EXTERN = re.compile(
    r"/\* TUBUILD CONFLICT -- alternate declaration of (?P<key>\S+), from the "
    r"legacy file for (?P<fn>\S+), NOT applied:")

# Any TUBUILD CONFLICT comment at all. A marker this script cannot parse is a failure,
# not a pass: an unrecognised shape means tubuild's emission changed and the pairing
# below silently stopped covering it.
SYMBOL_RE = re.compile(r"^(\S+)\s+kind:function\([^)]*\)\s+addr:(0x[0-9a-fA-F]+)")
MARK_ANY = re.compile(r"/\* TUBUILD CONFLICT --")


def _times(n):
    """`n` occurrences, spelled so a count of one reads naturally."""
    return "once" if n == 1 else "%d times" % n


def _unrepr(text):
    """`'FOO'` -> `FOO`; `('struct', 'C')` -> `struct C`. Falls back to the raw text so
    an unparsable key still compares equal to itself on both sides."""
    try:
        value = ast.literal_eval(text.strip())
    except (ValueError, SyntaxError):
        return text.strip()
    if isinstance(value, tuple):
        return " ".join(str(v) for v in value)
    return str(value)


def parse_notes(entry):
    """-> [(key3, resolved, note_text)] for every CONFLICT note on the entry."""
    out = []
    for note in entry.get("notes", []):
        m = NOTE_RE.match(note.strip())
        if m:
            key3 = (m.group("kind"), _unrepr(m.group("key")), m.group("second").strip())
            out.append((key3, bool(m.group("resolved")), note))
        elif "CONFLICT:" in note and "tubuild create warning" in note:
            # Recognisably a conflict warning whose text this script cannot read. Do
            # not drop it silently -- report it as unparsed and fail.
            out.append((None, False, note))
    return out


def parse_markers(text):
    """-> ([key3], unparsed_marker_count) for every TUBUILD CONFLICT marker in a source."""
    keys = []
    for m in MARK_DECL_BODY.finditer(text):
        keys.append(("local declaration",
                     "%s %s" % (m.group("dkind"), _unrepr(m.group("dname"))),
                     m.group("fn")))
    for m in MARK_MACRO.finditer(text):
        keys.append(("macro", m.group("key"), m.group("fn")))
    for m in MARK_EXTERN.finditer(text):
        keys.append(("extern declaration", m.group("key"), m.group("fn")))
    return keys, len(MARK_ANY.findall(text)) - len(keys)


AUTO_NAME_RE = re.compile(r"^func_(?:(ov\d+)_)?([0-9a-fA-F]{8})$")

_SITES_CACHE = {}


def _symbol_sites(repo):
    """-> {name: {(module, address)}} for every function in config/arm9/**/symbols.txt."""
    key = str(repo.resolve())
    if key not in _SITES_CACHE:
        sites = {}
        for sf in sorted(repo.glob("config/arm9/**/symbols.txt")):
            module = sf.parent.name
            for line in sf.read_text(errors="replace").splitlines():
                m = SYMBOL_RE.match(line)
                if m:
                    sites.setdefault(m.group(1), set()).add(
                        (module, int(m.group(2), 16)))
        _SITES_CACHE[key] = sites
    return _SITES_CACHE[key]


def _sites_of(name, table):
    """Every (module, address) a spelling of a function can denote.

    A retired auto-generated name is no longer in symbols.txt -- being removed from
    it is exactly what renaming a symbol does -- so symbols.txt alone can never pair
    the two halves of a rename. But an auto name encodes its own address, so decode
    it and the old spelling still resolves.
    """
    out = set(table.get(name, ()))
    m = AUTO_NAME_RE.match(name)
    if m:
        out.add((m.group(1) or "arm9", int(m.group(2), 16)))
    return out


def _same_function(a, b, table):
    """True when two different spellings denote one address in one module."""
    return a != b and bool(_sites_of(a, table) & _sites_of(b, table))


def _pair_renames(stale, unrecorded, repo):
    """Match notes against markers that differ ONLY in how they spell the legacy function.

    Renaming a symbol rewrites the marker comment in the generated source, because the
    marker names the legacy function in text a tree-wide substitution reaches. The note
    in config/tu_manifest.d/ is prose the same substitution does not touch. The pair
    then falls apart into a STALE NOTE and an UNRECORDED marker that are really one
    record described twice -- two misleading findings in place of one true one, on a
    conflict nobody actually changed.

    This does not excuse anything: the pair is still reported and the check still fails.
    It is reported accurately, and the fix is one word rather than a deletion. Pairing
    requires the two spellings to resolve to the SAME ADDRESS IN THE SAME MODULE, so a
    note and a marker about genuinely different functions never collapse together.
    """
    table = _symbol_sites(repo)
    renames = []
    for key3 in sorted(stale):
        for other in sorted(unrecorded):
            if other[0] != key3[0] or other[1] != key3[1]:
                continue
            if not _same_function(other[2], key3[2], table):
                continue
            n = min(stale[key3], unrecorded[other])
            if n:
                stale[key3] -= n
                unrecorded[other] -= n
                renames.append((key3, other, n))
    stale += collections.Counter()      # drop the zeroed entries
    unrecorded += collections.Counter()
    return renames


def check_entry(entry, repo):
    """-> (problems, stats) for one manifest entry."""
    problems = []
    notes = parse_notes(entry)
    unparsed_notes = [n for k, _r, n in notes if k is None]
    for note in unparsed_notes:
        problems.append(("UNPARSED NOTE",
                         "this is a tubuild conflict warning but does not match the "
                         "text tubuild emits; the check cannot pair it: %r" % note))

    src = repo / entry["source"]
    if not src.is_file():
        if notes:
            problems.append(("MISSING SOURCE",
                             "entry carries %d conflict note(s) but %s does not exist"
                             % (len(notes), entry["source"])))
        return problems, (len(notes), 0)

    markers, unparsed_markers = parse_markers(src.read_text(errors="replace"))
    if unparsed_markers:
        problems.append(("UNPARSED MARKER",
                         "%s holds %d TUBUILD CONFLICT comment(s) this check cannot "
                         "read; tubuild's emission has changed and the pairing no "
                         "longer covers them" % (entry["source"], unparsed_markers)))

    # Multisets, not sets. The same (kind, key, legacy function) can legitimately
    # occur twice -- one legacy file supplying two items that both lose to the same
    # kept text -- and tubuild emits a note and a marker for each. Pairing by set
    # would let 2 notes satisfy 1 marker and hide half the drift.
    open_notes = collections.Counter(k for k, r, _n in notes if k is not None and not r)
    done_notes = collections.Counter(k for k, r, _n in notes if k is not None and r)
    live = collections.Counter(markers)

    stale = open_notes - live
    unrecorded = live - open_notes - done_notes
    for note_key, mark_key, n in _pair_renames(stale, unrecorded, repo):
        problems.append(("RENAME DESYNC",
                         "%s %s is recorded %s against the legacy file for %s, but %s "
                         "spells that same function %s. They are one address in one "
                         "module, so this is a rename the note did not follow, not a "
                         "resolved conflict and not an unrecorded one. Rewrite the "
                         "note's function name to %s; do NOT delete the marker -- the "
                         "conflict it defers is still open"
                         % (note_key[0], note_key[1], _times(n), note_key[2],
                            entry["source"], mark_key[2], mark_key[2])))

    for key3, n in sorted(stale.items()):
        problems.append(("STALE NOTE",
                         "%s %s (from the legacy file for %s) is recorded as an open "
                         "conflict %s, but %s does not carry a matching marker. If it "
                         "was reconciled, rewrite the note as 'tubuild create warning "
                         "(RESOLVED): CONFLICT: ...'"
                         % (key3[0], key3[1], key3[2], _times(n), entry["source"])))
    for key3, n in sorted((done_notes & live).items()):
        problems.append(("REOPENED",
                         "%s %s (from the legacy file for %s) is noted RESOLVED, but "
                         "%s carries its marker again %s"
                         % (key3[0], key3[1], key3[2], entry["source"], _times(n))))
    for key3, n in sorted(unrecorded.items()):
        problems.append(("UNRECORDED",
                         "%s holds a conflict marker %s for %s %s (from the legacy "
                         "file for %s) that no manifest note records. Two legacy files "
                         "disagreed here and the generated source kept one of them "
                         "arbitrarily; that decision is unreviewed"
                         % (entry["source"], _times(n), key3[0], key3[1], key3[2])))

    return problems, (sum(open_notes.values()), sum(live.values()))


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("tu_id", nargs="*",
                    help="manifest entry ids to check (default: all)")
    ap.add_argument("--list", action="store_true",
                    help="print every entry that carries a note or a marker")
    ap.add_argument("--repo", default=str(REPO),
                    help="repository root (default: this script's parent)")
    args = ap.parse_args(argv)

    repo = pathlib.Path(args.repo)
    entries = tu_manifest.load(repo / "config" / "tu_manifest.d")["entries"]
    if args.tu_id:
        wanted = set(args.tu_id)
        known = {e["id"] for e in entries}
        missing = sorted(wanted - known)
        if missing:
            print("no such manifest entry: %s" % ", ".join(missing), file=sys.stderr)
            return 2
        entries = [e for e in entries if e["id"] in wanted]

    failures = 0
    rows = []
    sources_read = 0
    for entry in sorted(entries, key=lambda e: e["id"]):
        if (repo / entry["source"]).is_file():
            sources_read += 1
        problems, (n_notes, n_marks) = check_entry(entry, repo)
        if n_notes or n_marks:
            rows.append((entry["id"], n_notes, n_marks, len(problems)))
        for label, detail in problems:
            print("%s: %s: %s" % (entry["id"], label, detail))
            failures += 1

    # AN EMPTY CHECK IS NOT A PASS. Half of this check lives in the manifest and half
    # in the generated sources, so it has two ways to inspect nothing and still report
    # clean: a manifest that resolves to no entries, and entries whose sources are all
    # absent. Both are what a relocated manifest directory or a partial checkout looks
    # like, and both would otherwise print a reassuring "checked 0" and exit 0. Zero
    # ROWS is deliberately NOT in this list -- a tree with no conflict records left is
    # the goal state, not a failure, and a gate that goes red on success gets switched
    # off.
    if not entries or not sources_read:
        print("checked nothing: %d manifest entries, %d of their sources present. "
              "This check reads notes in config/tu_manifest.d and markers in the "
              "sources those entries name, so with neither in reach it has no evidence "
              "and will not report a pass."
              % (len(entries), sources_read), file=sys.stderr)
        return 2

    if args.list:
        print("\n%-46s %6s %8s %9s" % ("tu id", "open", "markers", "problems"))
        for tid, n, m, p in rows:
            print("%-46s %6d %8d %9d" % (tid, n, m, p))

    print("\nchecked %d manifest entries (%d sources read); %d carry a conflict note "
          "or marker" % (len(entries), sources_read, len(rows)))
    if failures:
        print("%d unreconciled conflict record(s)" % failures)
        return 1
    print("every recorded conflict matches the source, and every conflict in a "
          "source is recorded")
    return 0


if __name__ == "__main__":
    sys.exit(main())
