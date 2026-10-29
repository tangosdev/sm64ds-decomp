#!/usr/bin/env python3
"""The three tiers: how much of the game is MATCHED, LINKED and PORTED.

One percentage cannot describe this project, because "done" means three
different things and they move independently:

  MATCHED    the C compiles to the ROM's exact bytes. Provably the original
             logic. This is the number the README bar and the treemap show.
  LINKED     the function is enrolled in the ROM build: a delinks.txt entry
             marks its range ``complete``, so mwccarm compiles its source and
             the link byte-compares it against the cartridge. A matched
             function served from gap bytes (the ROM's own bytes) is matched
             but NOT linked. Computed live from committed config, no stamp.
  PORTED     the function's translation unit actually reaches the PC port's
             binary, replacing the host stand-in that was there before.

They are deliberately not a funnel: a function can be MATCHED and LINKED while
still being unreadable, and readability work never changes the matched bytes.
PORTED is measured on a port build and can lag the other two.

Bars count functions. MATCHED counts against the dsd function universe.
LINKED counts universe functions covered by an enrolled ``complete`` range.
PORTED counts functions whose translation unit the port build links, read from
config/port_linked_functions.txt. A stamp without that list falls back to
matched translation units and labels the count accordingly.

Usage:
    python tools/tiers.py                 # human-readable table
    python tools/tiers.py --json          # machine-readable, for chaos-db
    python tools/tiers.py --write-readme  # rewrite the block in README.md

MATCHED is read from chaos-db.json when it is present, which is the same file
the README bar and treemap read, so every surface reports one number. Without
it, the tiers that need no ROM are still reported and MATCHED is omitted rather
than guessed.
"""
import collections
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent

SRC = REPO / "src"
README = REPO / "README.md"
README_START = "<!-- tiers:start -->"
README_END = "<!-- tiers:end -->"
PORTED_STAMP = REPO / "config" / "port_linkage.json"
PORTED_FUNCTIONS = REPO / "config" / "port_linked_functions.txt"
FUNCTION_AUTHORS = REPO / "function-authors.json"


# ------------------------------------------------------------------ markers
#
# The `@symbol` marker names a member's linker symbol where present; otherwise
# the filename stem is the same name by construction. The slicer below is kept
# here because tools/tubuild.py reuses it to slice a promoted TU's member
# source at marker boundaries. It is not a readability metric.

SYMBOL = re.compile(r"//\s*@symbol\s+(\S+)")


def _marked_member_fragment(text, symbol):
    """Return one explicitly marked member's source, or None if ambiguous.

    Production TUs put ``// @symbol <linker-name>`` immediately before each
    hand-written member. Slicing at the next marker keeps an unrelated member's
    text from being attributed to this member.
    """
    markers = list(SYMBOL.finditer(text))
    matches = [i for i, marker in enumerate(markers)
               if marker.group(1) == symbol]
    if len(matches) != 1:
        return None
    i = matches[0]
    start = markers[i].end()
    stop = markers[i + 1].start() if i + 1 < len(markers) else len(text)
    return text[start:stop]


# ------------------------------------------------------------------- LINKED
#
# LINKED is computed live from committed config: a universe function counts
# when a delinks.txt entry marks its range ``complete``, i.e. mwccarm compiles
# its source and the ROM link byte-compares it against the cartridge. Ranges
# with no entry (or without ``complete``) are served from ROM gap bytes: any
# function there may be MATCHED but is NOT linked. Same rule as
# chaos_db_ci.enrolled_addresses(), applied at function granularity so every
# member of a multi-function TU counts, not just the range start.


def _delinks_module(path):
    """Module label for a config/**/delinks.txt path, or None if not code."""
    parts = pathlib.Path(path).parts
    try:
        i = parts.index("arm9")
    except ValueError:
        return None
    rest = parts[i + 1:]
    if not rest or rest[0] == "delinks.txt":
        return "arm9"
    if rest[0] in ("itcm", "dtcm") and len(rest) == 2:
        return rest[0]
    if rest[0] == "overlays" and len(rest) == 3:
        return rest[1]
    return None


def linked(authors_path=None, config_root=None):
    """LINKED, live: enrolled functions over the function universe.

    Returns ``linked`` / ``of`` / ``pct`` / ``unit`` (always ``"functions"``).
    The universe is the function-authors.json keys (``module:0xaddr``, the same
    keys the port list uses); a function counts when its address falls inside
    a ``complete`` .text/.init range of its module's delinks.txt. Ranges claimed
    by a deliberate-divergence path never count (see the mods rule in
    enroll.py). None when function-authors.json is absent.
    """
    import rombuild_check as RBC  # noqa: E402 - canonical delinks reader

    authors = pathlib.Path(authors_path) if authors_path else FUNCTION_AUTHORS
    if not authors.is_file():
        return None
    try:
        universe = set(json.loads(authors.read_text(encoding="utf-8"))["functions"])
    except (OSError, ValueError, KeyError):
        return None
    if not universe:
        return None
    root = pathlib.Path(config_root) if config_root else REPO / "config"
    covered = collections.defaultdict(list)
    for dl in sorted(root.rglob("delinks.txt")):
        mod = _delinks_module(dl)
        if mod is None:
            continue
        try:
            entries = RBC.complete_entries(dl)
        except OSError:
            continue
        for rel, start, end in entries:
            if str(rel).startswith("mods/"):
                continue
            covered[mod].append((start, end))
    got = sum(
        1 for key in universe
        if any(st <= int(key.rsplit(":", 1)[1], 16) < en
               for st, en in covered.get(key.rsplit(":", 1)[0], ()))
    )
    return {"linked": got, "of": len(universe), "unit": "functions",
            "pct": round(100.0 * got / len(universe), 2)}


# ------------------------------------------------------------------- PORTED
#
# PORTED cannot be computed here and must not pretend otherwise. port/tools/
# linkage.py reads build/port/walk_window.map, which only exists after an MSVC
# build of the port, and the port branches are not merged to main. So the number
# is STAMPED: measured by hand, recorded with the branch and commit it came from,
# and refreshed when someone runs a port build.
#
# A stamp with no provenance is worse than no number, so a stamp missing its
# branch/commit/date is treated as absent.


def ported_functions(stamp=None, list_path=None):
    """``(keys, commit)`` from the per-function list, or ``(None, reason)``.

    config/port_linked_functions.txt is written by tools/port_linked_functions.py
    from a port build. It is believed only while its ``# commit:`` line equals the
    commit of the usable stamp: two port builds must never be mixed, and a list
    without the stamp's provenance is a number nobody can trace. ``keys`` is the
    set of ``module:0xaddr`` strings; ``commit`` is the list's commit on success.
    """
    p = pathlib.Path(list_path or PORTED_FUNCTIONS)
    if not p.is_file():
        return None, "no per-function list"
    if stamp is None:
        stamp = ported_stamp()
    if not stamp:
        return None, "no usable stamp"
    keys, commit, universe = set(), None, None
    for line in p.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if line.startswith("# commit:"):
            commit = line.split(":", 1)[1].strip()
        elif line.startswith("# universe:"):
            try:
                universe = int(line.split(":", 1)[1])
            except ValueError:
                universe = None
        elif line and not line.startswith("#"):
            keys.add(line)
    if not commit or commit != stamp["commit"]:
        return None, f"list commit {commit!r}, stamp commit {stamp['commit']!r}"
    return keys, universe


def ported_stamp(stamp_path=None):
    """The stamped PORTED measurement (translation units), or None if unusable."""
    p = pathlib.Path(stamp_path or PORTED_STAMP)
    if not p.is_file():
        return None
    try:
        d = json.loads(p.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return None
    if not all(d.get(k) for k in ("measuredAt", "branch", "commit")):
        return None
    tot = d.get("matchedTus") or 0
    got = d.get("linkedTus") or 0
    if not tot:
        return None
    return {
        "ported": got,
        "portedTus": got,
        "matchedTus": tot,
        "pct": round(100.0 * got / tot, 2),
        "measuredAt": d["measuredAt"],
        "branch": d["branch"],
        "commit": d["commit"],
        "stale": bool(d.get("stale", False)),
    }


def ported(stamp_path=None, list_path=None):
    """PORTED, per function when the stamped build has a per-function list.

    Returns ``ported`` / ``of`` / ``pct`` / ``unit`` (``"functions"`` or, for a
    stamp with no usable list, ``"units"``) plus the stamp's translation-unit
    figures as extra detail: ``linkedTus`` and ``matchedTus``. ``of`` for the
    function count is the list's own ``# universe:`` line, the function universe
    of the measured tree; chaos_db_ci overrides it with the published record
    count so both bars share one denominator. None when there is no usable
    stamp at all.
    """
    stamp = ported_stamp(stamp_path)
    if not stamp:
        return None
    out = {"ported": stamp["ported"], "portedTus": stamp["portedTus"],
           "matchedTus": stamp["matchedTus"], "unit": "units",
           "of": stamp["matchedTus"], "pct": stamp["pct"],
           "measuredAt": stamp["measuredAt"], "branch": stamp["branch"],
           "commit": stamp["commit"], "stale": stamp["stale"]}
    keys, universe = ported_functions(
        stamp, list_path or (pathlib.Path(stamp_path).with_name("port_linked_functions.txt")
                             if stamp_path else None))
    if keys is not None and universe:
        out.update(ported=len(keys), of=universe, unit="functions",
                   pct=round(100.0 * len(keys) / universe, 2))
    return out


# ------------------------------------------------------------------ MATCHED


def matched(db_path=None):
    """MATCHED from chaos-db.json, the file the bar and treemap also read.

    Returns None when the db is absent rather than recomputing it: a second
    implementation of the matched rule is a second source of truth, and this
    repo has already paid for one of those (the itcm undercount).
    """
    p = pathlib.Path(db_path) if db_path else (REPO / "chaos-db.json")
    if not p.is_file():
        return None
    try:
        db = json.loads(p.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return None
    st = db.get("stats") or {}
    tot, got = st.get("totalFunctions"), st.get("matchedFunctions")
    if not tot:
        return None
    return {
        "matched": got,
        "functions": tot,
        "pct": round(100.0 * got / tot, 2),
        "matchedBytes": st.get("matchedBytes"),
        "totalBytes": st.get("totalBytes"),
        "bytePct": (round(100.0 * st["matchedBytes"] / st["totalBytes"], 2)
                    if st.get("totalBytes") else None),
        # The assembly subset, read from the same db for the same reason MATCHED is:
        # recomputing it here would be a second source of truth. Absent from a db
        # generated before the field existed, and every consumer treats that as
        # "do not print the line" rather than as zero.
        "handAsm": st.get("handAsmFunctions"),
        "handAsmBytes": st.get("handAsmBytes"),
    }


# ------------------------------------------------------------------- output


def collect(db_path=None):
    return {"matched": matched(db_path), "linked": linked(), "ported": ported()}


# Joining preserved README rows. Spelled as a constant because a newline escape
# written inline here has been mangled by an editing shell before, and a generator
# that will not import silently stops refreshing the bar.
NEWLINE = chr(10)


def bar(done, tot, width=30):
    filled = round(done / tot * width) if tot else 0
    if done and filled == 0:
        filled = 1
    return "█" * filled + "░" * (width - filled)


def matched_caption(m):
    """The indented line under MATCHED naming its assembly subset, or None.

    Generated rather than written into the README by hand, because it is a number
    and every number in that block goes stale the moment someone matches something.
    Indented so ``write_readme``'s preserved-MATCHED search, which looks for a line
    STARTING with "MATCHED", cannot mistake it for the row itself.
    """
    if not m or not m.get("handAsm"):
        return None
    return (f"           of which {m['handAsm']:,} are byte-exact assembly "
            f"(hand-written in the original, not C)")


def _ported_noun(k):
    """What PORTED counted: functions, or (an older stamp) matched translation units."""
    return "functions" if k.get("unit") == "functions" else "matched TUs"


def bar_block(t, preserved_matched=None):
    """The single fenced block shown under the README "## Progress" heading."""
    rows = []
    m, k, p = t["matched"], t["linked"], t.get("ported")
    if m:
        rows.append(f"MATCHED    {bar(m['matched'], m['functions'])}  "
                    f"{m['pct']:4.1f}%   {m['matched']:,} / {m['functions']:,} functions")
        caption = matched_caption(m)
        if caption:
            rows.append(caption)
    elif preserved_matched:
        # A worktree may have the ROM inputs needed for LINKED but not
        # chaos-db.json, which supplies MATCHED. Updating the live rows must not
        # silently erase the last independently generated MATCHED measurement.
        rows.append(preserved_matched)
    if k:
        rows.append(f"LINKED     {bar(k['linked'], k['of'])}  "
                    f"{k['pct']:4.1f}%   {k['linked']:,} / {k['of']:,} functions")
    if p:
        rows.append(f"PORTED     {bar(p['ported'], p['of'])}  "
                    f"{p['pct']:4.1f}%   {p['ported']:,} / {p['of']:,} {_ported_noun(p)}")
    return "```\n" + "\n".join(rows) + "\n```"


def write_readme(t):
    # Byte round-trip: pathlib text writes translate newlines on Windows, which
    # would reformat the whole file. The block edit must be the only diff.
    text = README.read_bytes().decode("utf-8")
    start = text.index(README_START) + len(README_START)
    end = text.index(README_END)
    current = text[start:end]
    # The MATCHED row AND its caption: a worktree with no chaos-db.json preserves
    # whatever the last real generator wrote, and dropping the caption while keeping
    # the row would leave the block claiming more C than it measured.
    lines = current.splitlines()
    keep = []
    for i, line in enumerate(lines):
        if line.strip().startswith("MATCHED"):
            keep.append(line.strip())
            if i + 1 < len(lines) and lines[i + 1].strip().startswith("of which"):
                keep.append(lines[i + 1].rstrip())
            break
    preserved_matched = NEWLINE.join(keep) if keep else None
    new_text = text[:start] + "\n" + bar_block(t, preserved_matched) + "\n" + text[end:]
    if new_text != text:
        README.write_bytes(new_text.encode("utf-8"))
        return True
    return False


def report(t):
    m, k, p = t["matched"], t["linked"], t.get("ported")
    out = []
    if m:
        out.append(f"MATCHED    {m['matched']:,} / {m['functions']:,} functions  "
                   f"{m['pct']:.1f}%   ({m['bytePct']:.1f}% by code bytes)")
        if m.get("handAsm"):
            out.append(f"           of which {m['handAsm']:,} functions "
                       f"({m['handAsmBytes']:,} bytes) are byte-exact assembly, "
                       f"hand-written in the original game and reproduced as assembly")
    else:
        out.append("MATCHED    (no chaos-db.json; run tools/chaos_db_ci.py first)")
    if k:
        out.append(f"LINKED     {k['linked']:,} / {k['of']:,} functions  "
                   f"{k['pct']:.1f}%   (live: enrolled in the ROM build)")
    else:
        out.append("LINKED     (no function-authors.json)")
    if p:
        flag = "  STALE" if p["stale"] else ""
        out.append(f"PORTED     {p['ported']:,} / {p['of']:,} {_ported_noun(p)}  "
                   f"{p['pct']:.1f}%   (stamped {p['measuredAt']} from {p['branch']}"
                   f" @ {p['commit']}){flag}")
    else:
        out.append("PORTED     (no usable stamp in config/port_linkage.json)")
    return "\n".join(out)


def main():
    db = None
    if "--from-db" in sys.argv:
        i = sys.argv.index("--from-db")
        if i + 1 < len(sys.argv):
            db = sys.argv[i + 1]
    t = collect(db)

    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except Exception:
        pass

    if "--json" in sys.argv:
        print(json.dumps(t, indent=2))
        return
    if "--write-readme" in sys.argv:
        print(f"README.md {'updated' if write_readme(t) else 'already up to date'}")
        return
    if "--bar" in sys.argv:
        print(bar_block(t))
        return
    print(report(t))


if __name__ == "__main__":
    sys.exit(main())
