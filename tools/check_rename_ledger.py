#!/usr/bin/env python3
"""Check symbols/actor_renames.tsv against each module's own symbols.txt.

The ledger's fourth column asserts, in the present tense, what the symbol at a
given address is called. `tools/cpp_index.py:86` and `tools/cpp_rename.py:51`
read it back as a live address -> symbol map, so a row naming a symbol that
lives at a DIFFERENT address is not stale prose -- it is a false lookup result.

WHERE THE BAD ROWS CAME FROM, and why they cannot be regenerated away.
`tools/actor_names.py` WRITES this file (line 282, on every run, including the
default dry run); it never reads it back. Its `parse_spawnfunc` used to scan a
fixed window past the factory's own body, ran into the NEXT class's D1, and
took that class's vtable -- "shifted every vtable and method name one class
late", as its own docstring now puts it. That defect was fixed on main
(#1418-#1423), but the checked-in ledger was never regenerated, so it is still
the pre-fix output. Regenerating today does NOT repair it: `propose()` refuses
any address that already carries a real name, and by now nearly every one does,
so a fresh run emits 1 row instead of 2,573 (measured 2026-09-02). The file has
stopped being a reproducible artifact and is now maintained in place -- which
is what this check is for.

config/**/symbols.txt is the authority: it is what the ROM build consumes and
what `dsd check symbols` validates. It is not read-only -- `class_rename.py:209`,
`import_symbols.py:166` and `actor_names.py:313 (--apply)` all rewrite it -- but
every one of those writes is applied in order to make the build emit that
symbol, so symbols.txt and the linked ROM agree by construction and the ledger
does not. This check therefore only ever moves the ledger toward symbols.txt.

SCOPE. Only mangled (_ZN...) and vtable (_ZTV...) rows are checked. Coined
source-style spellings (Foo_Spawn, g_profile_BAR) are reconstructions that
deliberately have no symbols.txt counterpart. That is a real limit, not a clean
bill of health: some coined rows carry the same one-class shift (ov009's
DockPole_Spawn / DockPole_SpawnInfo sit on MetalNet's addresses), and this
check will never see them. The summary line prints how many rows went unchecked
so the green is not read as more than it is.
"""
import argparse
import collections
import io
import os
import re
import sys

LEDGER = "symbols/actor_renames.tsv"
SYM_RE = re.compile(r"\s*(\S+)\s+kind:(\S+)\s+addr:(0x[0-9a-f]+)")
WAS_RE = re.compile(r" \(was [^)]*\)$")


def symbols_path(repo, module):
    for rel in ("config/arm9/overlays/%s/symbols.txt" % module,
                "config/%s/symbols.txt" % module):
        p = os.path.join(repo, rel)
        if os.path.exists(p):
            return p
    return None


def load_symbols(repo, module, cache):
    if module in cache:
        return cache[module]
    path = symbols_path(repo, module)
    table = None
    if path:
        table = collections.defaultdict(list)
        with io.open(path, encoding="utf-8") as fh:
            for line in fh:
                m = SYM_RE.match(line)
                if m:
                    table[int(m.group(3), 16)].append(m.group(1))
    cache[module] = table
    return table


def is_cartridge_spelling(vtable_symbol):
    """_ZTV18daObjPowerUpItem_c -> True; _ZTV11PowerFlower -> False.

    The character class has to admit digits: `_ZTV7da1up_c` is a cartridge name
    whose class is `da1up_c`, and `da[A-Z_]` classified it as legacy, which made
    pick() see two legacy candidates at ov002 0x021083c8 and give up on a row it
    could have resolved.
    """
    m = re.match(r"_ZTV(\d+)(.*)$", vtable_symbol)
    if not m:
        return False
    return bool(re.match(r"da[A-Z0-9_]", m.group(2)))


def pick(candidates):
    """Resolve an aliased address to one spelling.

    An address can carry both a legacy coined vtable name and the cartridge RTTI
    one; they are the same class. Prefer the legacy spelling, because the method
    rows this file is being made consistent with use legacy spellings too.
    Choosing the cartridge name here would be a second, separate claim.
    """
    if len(candidates) == 1:
        return candidates[0]
    legacy = [c for c in candidates if not is_cartridge_spelling(c)]
    return legacy[0] if len(legacy) == 1 else None


def split_lines(raw):
    """-> (lines, newline). Tolerates a file with mixed endings.

    Splitting on a single detected terminator merged an LF row into the previous
    CRLF row's last column, so the merged row went unreported and --fix would
    have rewritten the wrong field. Split on LF and strip the CR.
    """
    newline = "\r\n" if "\r\n" in raw else "\n"
    return [ln[:-1] if ln.endswith("\r") else ln for ln in raw.split("\n")], newline


def audit(repo):
    """-> (lines, newline, findings, missing, stats).

    findings: (lineno, module, addr_text, claimed, correction, candidates)
    """
    cache = {}
    path = os.path.join(repo, LEDGER)
    with io.open(path, "rb") as fh:
        raw = fh.read().decode("utf-8")
    lines, newline = split_lines(raw)
    findings = []
    stats = collections.Counter()
    missing = set()
    for i, line in enumerate(lines, 1):
        parts = line.split("\t")
        if len(parts) < 4:
            continue
        if i == 1 and parts[0] == "module":
            continue
        module, addr_text, claimed = parts[0], parts[1], parts[3]
        if not (claimed.startswith("_ZN") or claimed.startswith("_ZTV")):
            stats["unchecked"] += 1
            continue
        try:
            addr = int(addr_text, 16)
        except ValueError:
            stats["unchecked"] += 1
            continue
        table = load_symbols(repo, module, cache)
        if table is None:
            missing.add(module)
            stats["no_symbols_txt"] += 1
            continue
        stats["checked"] += 1
        names = table.get(addr, [])
        if claimed in names:
            continue
        findings.append((i, module, addr_text, claimed, pick(names), names))
    return lines, newline, findings, sorted(missing), stats


def summarize(stats, missing):
    print("  checked %d mangled/vtable row(s); %d coined row(s) are out of scope "
          "and unchecked" % (stats["checked"], stats["unchecked"]))
    if missing:
        print("  WARNING: %d row(s) skipped -- no symbols.txt for module(s): %s"
              % (stats["no_symbols_txt"], ", ".join(missing)))


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--repo", default=".")
    ap.add_argument("--fix", action="store_true",
                    help="rewrite rows whose address resolves to exactly one class")
    args = ap.parse_args()

    lines, newline, findings, missing, stats = audit(args.repo)
    if not findings:
        print("check_rename_ledger: every mangled/vtable row agrees with its "
              "module's symbols.txt")
        summarize(stats, missing)
        return 1 if missing else 0

    unresolved = [f for f in findings if f[4] is None]
    print("check_rename_ledger: %d row(s) name a symbol that does not live at the "
          "given address" % len(findings))
    summarize(stats, missing)
    per = collections.Counter(f[1] for f in findings)
    print("  by module: " + ", ".join("%s=%d" % kv for kv in per.most_common()))
    for lineno, module, addr, claimed, fix, names in findings:
        print("  %s:%d  %s %s" % (LEDGER, lineno, module, addr))
        print("      ledger says : %s" % claimed)
        print("      symbols.txt : %s" % ("/".join(names) or "<nothing at this address>"))
    if unresolved:
        print("  %d row(s) could not be resolved automatically." % len(unresolved))

    if not args.fix:
        return 1

    fixed = 0
    withdrawn = 0
    for lineno, _module, _addr, claimed, fix, _names in findings:
        if fix is None:
            continue
        idx = lineno - 1
        parts = lines[idx].split("\t")
        parts[3] = fix
        if len(parts) > 4:
            # A vtable row's "alloc=0xNNN" is the instance size, and it tracked
            # the NAME rather than the address: 31 of the 32 withdrawn figures
            # that are checkable against an include/*.h size assert match the
            # row's PRE-correction class exactly. Once the name is corrected the
            # figure is evidence about a class that does not live at this
            # address, so withdraw it rather than let it re-attach to the true
            # owner. "alloc=?" is already this file's marker for unknown.
            if "alloc=0x" in parts[4]:
                parts[4] = re.sub(r"alloc=0x[0-9a-fA-F]+", "alloc=?", parts[4])
                withdrawn += 1
            # Keep the retired claim. Five of these names -- AmbientSoundEffects,
            # Seaweed, VirtualDoor, daObjC1_Trap_c and daObjPushblock_c -- have
            # no other row in this file, and AmbientSoundEffects exists nowhere
            # authoritative in the tree at all, so overwriting the column
            # silently is the one way this correction could lose something.
            parts[4] = WAS_RE.sub("", parts[4]) + " (was %s)" % claimed
        lines[idx] = "\t".join(parts)
        fixed += 1
    with io.open(os.path.join(args.repo, LEDGER), "w", encoding="utf-8", newline="") as fh:
        fh.write(newline.join(lines))
    print("  rewrote %d row(s), withdrew %d stale alloc= figure(s); "
          "%d left for a human" % (fixed, withdrawn, len(unresolved)))
    return 1 if unresolved else 0


if __name__ == "__main__":
    sys.exit(main())
