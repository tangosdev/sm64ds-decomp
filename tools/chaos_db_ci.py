"""CI-safe Chaos Viewer data generator: rebuilds chaos-db.json from COMMITTED
data only (no ROM, no local ledger), so GitHub Actions can refresh the
chaos-data branch on every push and the hosted viewer always shows current
modules and percentages.

Derived the same way as progress.py --write-readme:
  universe   config/**/symbols.txt  (name, addr, size per module)
  matched    srcpath resolves a source, it is not marked // NONMATCHING, and the record
             is not in the byte-gate-failure class (policy D -- see tools/bytegate.py)
  near-miss  nearmiss/db.jsonl (committed) -> div badge
  author     git history: the FIRST contributor to land the surviving match for each
             function (see first_matchers) -- credit follows renames and is not stolen by
             a later duplicate submission (login from users.noreply emails, else author name)
  project    tools/chaosviewer.config.json (committed branding/prompt config)

Not derivable without the ROM (left to local regens): disasm/callee detail
chunks, coddog sim/sibling. The details/ directory on the chaos-data branch is
preserved as-is by the workflow.

Usage: python tools/chaos_db_ci.py [--out chaos-db.json]
"""
import argparse
import collections
import json
import pathlib
import sys
import re
import subprocess
import time

REPO = pathlib.Path(__file__).resolve().parent.parent
CONFIG = REPO / "config"
SRC = REPO / "src"
sys.path.insert(0, str(REPO / "tools"))
import asm_policy  # noqa: E402
import srcpath as SP  # noqa: E402
import relocs as RL  # noqa: E402
import rombuild_check as RBC  # noqa: E402
import layout_check as LYC  # noqa: E402
import tiers as TIERS  # noqa: E402
import bytegate as BG  # noqa: E402


def enrolled_addresses():
    """{(module, addr)} for every range a delinks.txt marks ``complete``.

    This is the set the ROM build actually compiles and byte-compares against the
    cartridge.  ``matched`` below is a different and much weaker test -- a source
    resolved through enrollment/filename fallback, with no ``NONMATCHING`` banner or
    ``dcd`` blob and no byte-gate exclusion --
    and the two differ by several hundred functions, because a file can sit in the
    tree, be counted, and be compiled by nothing.  The published percentage was the
    weaker number alone, so both now ride along and the site can say which is which.

    Reads only committed config, so it stays CI-safe: no ROM, no compiler.
    """
    out = set()
    for sym, label in RL.module_universe():
        delinks = sym.parent / "delinks.txt"
        if not delinks.is_file():
            continue
        for rel, addr, _end in RBC.complete_entries(delinks):
            if not rel.startswith("mods/"):
                out.add((label, addr))
    return out


def alias_collision_addresses():
    """{(module, addr)} where a size-0 function record and a SIZED one collide.

    ADDRESSES, not records, so the caller must act on only the `size == 0` side. Both
    records live at the same key and dropping both would be the opposite of the fix: the
    sized primaries here are the bodies that SHOULD be counted once someone matches them,
    and func_01ff8708 is 1,776 bytes of it. bytegate.is_zero_size_alias is the one
    predicate that gets that right; call it rather than re-spelling the test.

    config/arm9/itcm/symbols.txt declares eight bodies twice, once as a sized function and
    once as a zero-size alias at the identical address (_dmul beside func_01ff8708,
    _ll_sdiv beside func_01ffaa34, _s32_div_f beside __aeabi_idiv, _u32_div_f beside
    __aeabi_uidiv, and _dadd, _deq, _ll_udiv, _ull_mod likewise); config/arm9/symbols.txt
    declares two more (__cxa_vec_cleanup beside __destroy_arr, __cxa_vec_ctor beside
    func_020733a8). srcpath resolves src/_dmul.c -- a real HAND-ASM PRIMITIVE match --
    onto the ZERO-SIZE record, so the same 1,776-byte body read as matched at 0 bytes
    under the alias and unmatched at full size under the primary. linkcheck reports those
    NO-SYM (len-mismatch), which is the byte gate declining to compare a real function
    against a zero-length range.

    Those ten records leave the universe here, numerator and denominator both. A second
    name for a function already in the list is not a second function, and the zero-size
    half can never be byte-compared, so leaving it in the denominator (which is what
    happened until 2026-09-06) parks ten records that no amount of decompilation can
    clear. Deriving the set rather than listing it means the drop self-heals: repoint the
    alias at a real size in config and the record returns to the count with no edit here.
    Every zero-size record in the universe today has a sized twin, and a zero-size symbol
    that stands alone is NOT caught by this -- it stays counted, so a genuinely unmatched
    stub cannot be hidden by calling it an alias.

    Committed config only, no ROM and no compiler, so it runs in the workflows that
    publish the count. The other half of the byte-gate-failure class cannot be; see
    tools/bytegate.py.
    """
    return BG.alias_collision_addresses(RL.module_universe)


def enrollment_of(label, addr, src_path, enrolled, blocks):
    """``enrolled`` / ``unenrolled`` / ``no_block`` for one function.

    ``verified`` above is a boolean, and a boolean cannot tell the two halves of its
    false case apart. They are different facts with different remedies:

      enrolled    a delinks range carries ``complete``: mwccarm compiles this source
                  and the link byte-compares it against the cartridge. Identical to
                  ``verified`` for a matched function, by construction: both read the
                  same enrolled_addresses() set, so the two fields cannot drift.
      unenrolled  a delinks entry names a source for this range but has no ``complete``.
                  The source is in the tree and the build reads ROM bytes instead. This
                  is where a real match that nobody promoted looks exactly like a file
                  that was never going to build.
      no_block    no delinks entry names this range at all. Overwhelmingly deliberate:
                  enroll.py skips thumb functions, addresses that are not 4-aligned,
                  zero-size alias symbols and everything in config/rombuild-exclude.txt,
                  and it writes an intentional divergence under mods/ rather than src/.

    Two existing readers, no third delinks parser: enrolled_addresses() for ``complete``
    (address-keyed, the same set ``verified`` uses) and layout_check.delinks_paths() for
    "is there an entry at all" (path-keyed, the L1/L5 reader).

    The two keyings can disagree for an entry under mods/: a delinks range naming a
    mods/ path can be marked ``complete``, so an address-coverage test calls it
    enrolled, but enrolled_addresses() drops mods/ on purpose, because a deliberate
    divergence must never be counted as a reproduction of the cartridge. Such an entry
    is reported no_block here, which is the honest answer for the src/ path it displaced:
    that file is compiled by nothing. `mods/` is currently empty, so this case does not
    occur in the tree today, but the reader still has to handle it correctly.
    """
    if (label, addr) in enrolled:
        return "enrolled"
    return "unenrolled" if src_path and src_path in blocks else "no_block"


def tier_stats():
    """The CONVERTED and LINKED tiers, flattened for the stats block.

    Flat rather than nested because the two consumers downstream are a shell
    script's python one-liner and a C# record, and neither gains anything from
    nesting. MATCHED is not repeated here; it is already the block above.

    Never raises. This runs inside the generator whose output pays the coin
    ledger, and a readability scan is not worth killing a refresh over -- a run
    that dies here would leave contributions.json unmoved and every match landed
    since the last refresh uncredited. On failure the key is simply absent, and
    the site falls back to showing MATCHED alone.
    """
    try:
        c = TIERS.converted()
        k = TIERS.linked()
    except Exception as e:  # noqa: BLE001 - see docstring
        print(f"  tiers: SKIPPED ({e})")
        return None
    out = {
        "converted": c["converted"],
        "convertedOf": c["functions"],
        "convertedSourceFiles": c["source_files"],
        "convertedPct": c["pct"],
    }
    if k:
        out.update({
            "linked": k["linked"],
            "linkedOf": k["matchedTus"],
            "linkedPct": k["pct"],
            "linkedMeasuredAt": k["measuredAt"],
            "linkedBranch": k["branch"],
            "linkedCommit": k["commit"],
        })
    return out

FUNC_RE = re.compile(
    r"^(\S+)\s+kind:function\((?:arm|thumb),size=0x([0-9a-fA-F]+)\).*?addr:0x([0-9a-fA-F]+)")
LOGIN_RE = re.compile(r"^(?:\d+\+)?([^@]+)@users\.noreply\.github\.com$")

# Match-coin weighting, per module. A match is worth 1 MC by default; a module
# listed here is worth its multiplier instead. This exists to steer effort at
# modules nobody is picking up, not to re-rank past work -- `matched` below stays
# the honest function count and only `coins` carries the weight.
#
# itcm at 10x: it was invisible in every viewer and every count until 2026-08-05,
# so nothing in it was ever chosen by anyone. 25 of its 43 functions are still
# unmatched, including _ZN12MeshCollider10DetectClsnER10SphereClsn at 7112 bytes,
# the largest unmatched function in the game.
COIN_WEIGHTS = {"itcm": 10}

# Bump whenever COIN_WEIGHTS changes, or whenever the DEFINITION of a matched
# function changes. The backend credits the DELTA between a published career total
# and what it last paid, so a formula change would otherwise read as thousands of
# new matches and pay a retroactive windfall (and fire a wall of Discord
# milestones). The backend rebases silently when this number moves: it banks the new
# totals as already-credited without touching balances, so a change only ever affects
# matches landed AFTER it.
#
# 2 -> 3: policy D. `matched` now excludes the byte-gate-failure class, which takes 58
# coins back off five contributors (ruspecial -43, tangosdev -8, lunavyqo -4,
# andrewboudreau -2, mitch030504 -1). Without the bump the backend would read those as
# negative deltas and claw the balances back; with it, the smaller totals are banked as
# the new baseline and nobody's balance moves.
COIN_FORMULA = 3

# A NONMATCHING file that reproduces the ROM and has no match left to chase - the banner
# tags which kind. These are NOT pending work, so the viewers paint them apart from real
# near-miss drafts instead of lumping everything unmatched together.
NOMATCH_RE = re.compile(r"NONMATCHING \((ASM-PRIMITIVE|NOT-C-EXPRESSIBLE)\)")
NOMATCH_REASONS = {
    "ASM-PRIMITIVE": (
        "asm-primitive",
        "Nintendo shipped this as an assembly primitive. There is no original C to "
        "recover, so writing C for it would invent a source that never existed.",
    ),
    "NOT-C-EXPRESSIBLE": (
        "not-c-expressible",
        "A bare epilogue or mid-frame exit stub the symbol table split out, not a real "
        "function. No standalone C construct produces it.",
    ),
}


def no_match_needed(head: str) -> dict[str, str] | None:
    """Bucket + hover explanation for a file that needs no match, else None."""
    m = NOMATCH_RE.search(head)
    if not m:
        return None
    bucket, reason = NOMATCH_REASONS[m.group(1)]
    return {"bucket": bucket, "reason": reason}


# An asm-bodied file is countable only under an explicit banner: HAND-ASM PRIMITIVE
# (policy-matched -- the original really was assembly) or NONMATCHING (draft / hatch).
# A dcd blob with neither is a transcription: it byte-matches vacuously because it IS
# the ROM words re-spelled, so counting it as matched puts a lie in the progress bar
# (PR #1072 landed 8 of these as +8 matches / +8,208 bytes / credit). "transcribed"
# is demoted from matched; mnemonic asm without a banner is a policy gray zone
# (embedded hatches inside real C, e.g. CP15 intrinsics) and is only WARNED about.
# The detector is asm_policy.classify, shared with validate_merge and pr_linkcheck.


def _handle_from(name: str, email: str) -> str:
    """git identity -> canonical-ish handle: noreply login, else the email local-part
    (stable across author-name typos, usually equals the GitHub handle), else the name."""
    email = email.strip()
    m = LOGIN_RE.match(email)
    return m.group(1) if m else (email.split("@")[0].lower() or name.strip())


def first_matchers(rev="HEAD") -> dict[str, str]:
    """{'src/name.ext': handle} crediting each currently-tracked file to the FIRST
    contributor to land the match it descends from. Credit belongs to whoever matched a
    function first; a later duplicate submission of the same function does not steal it.

    We replay the whole src/ history oldest-first, tracking one 'origin author' per live
    path, and follow git's own add / delete / rename classification (-M):
      * add     -- starts a lineage: the adder owns it (setdefault, so re-processing is safe)
      * rename  -- CARRIES the origin author to the new path. This is why a maintainer's
                   mass symbol-rename (func_ovNN_ADDR -> the real _ZN...Ev name) keeps the
                   original matcher's credit instead of handing it to the renamer.
      * delete  -- ENDS the lineage. A later add at that path is a fresh match and credits
                   the new author -- this is the false-match case: a wrong match that was
                   deleted ('fix the false matches') and later redone correctly by someone
                   else must credit the person who actually landed the surviving match.
    The distinction between rename and delete+add is exactly git's content-similarity call,
    which is what separates 'same match, new name' from 'the first attempt was wrong.'"""
    # diff.renameLimit=0 lifts the exhaustive-rename cap: the actor-symbol pass renamed
    # ~2200 files in one commit, far over git's default limit, so without this those renames
    # degrade to delete+add and the mass-renamer wrongly inherits every matcher's credit.
    out = subprocess.run(
        ["git", "-c", "diff.renameLimit=0", "log", "--reverse", "--diff-filter=ADR", "-M",
         "--format=%x01%an%x02%ae", "--name-status", rev, "--", "src/"],
        cwd=REPO, capture_output=True, text=True, encoding="utf-8", errors="replace").stdout
    origin: dict[str, str] = {}   # live path -> author of the earliest add in its lineage
    handle = None
    adds: list[str] = []
    dels: list[str] = []

    def flush(who):
        """Apply one commit's adds/deletes, pairing a same-stem delete+add as a rename.

        Promoting a matched function to a real C++ method changes its extension
        (src/F.c -> src/F.cpp) and rewrites enough of the body that git's similarity
        check falls under the rename threshold, so it arrives here as delete+add. That
        is still the SAME function, so ending the lineage would hand the original
        matcher's credit to whoever did the promotion (it moved 85 matches off five
        contributors before this pairing existed). Pairing is deliberately limited to a
        delete and an add of the same stem IN ONE COMMIT, which cannot be confused with
        the false-match case the delete rule exists for: that is a delete in one commit
        and a corrected add in a later one.
        """
        by_stem = {d.rsplit(".", 1)[0]: d for d in dels}
        paired = set()
        for new in adds:
            old = by_stem.get(new.rsplit(".", 1)[0])
            if old is not None and old != new:
                origin[new] = origin.pop(old, who)   # same function, new extension
                paired.add(old)
                paired.add(new)
        for a in adds:
            if a not in paired:
                origin.setdefault(a, who)
        for d in dels:
            if d not in paired:
                origin.pop(d, None)
        adds.clear()
        dels.clear()

    for line in out.splitlines():
        if line.startswith("\x01"):
            if handle:
                flush(handle)
            name, _, email = line[1:].partition("\x02")
            handle = _handle_from(name, email)
        elif handle and line and line[0] in "ADR":
            parts = line.split("\t")
            code = parts[0]
            if code.startswith("A") and len(parts) >= 2:
                adds.append(parts[1].strip())
            elif code.startswith("D") and len(parts) >= 2:
                dels.append(parts[1].strip())
            elif code.startswith("R") and len(parts) >= 3:
                old, new = parts[1].strip(), parts[2].strip()
                origin[new] = origin.pop(old, handle)  # carry the matcher's credit forward
    if handle:
        flush(handle)
    return origin


def match_finishers(rev="HEAD") -> dict[str, str]:
    """{'src/name.ext': handle} crediting whoever FIRST turned a NONMATCHING draft into a
    real byte-match -- the person who actually matched the function.

    first_matchers() credits whoever added a path first, which is wrong whenever that first
    version was an unmatched draft: the file only becomes countable once someone removes the
    banner, so the drafter would collect credit for a match somebody else made. A bulk import
    of hundreds of drafts made that a large-scale misattribution rather than a rare one.

    A finish is identified by content, not by commit message: the path carried the
    "// NONMATCHING" banner at some earlier point and does not at this commit. The FIRST such
    transition wins, so re-touching an already-matched file never transfers credit.

    This walks full history with per-path state rather than filtering diffs, because every
    diff-shaped approach misses cases:
      - `git log -- <path>` applies history simplification and silently prunes commits that
        arrived through a merge. A match landing on a side branch that forked before the draft
        existed is recorded as an ADD of an unbannered file, invisible to a MODIFY scan.
      - the -G pickaxe lists only files whose own diff contains the pattern, so the newly added
        byte-matching file -- banner-free by definition -- is never reported at all.
    Draft state also has to follow the file across an extension change, which git records as a
    rename when the edit is small and as a delete + add of the same base name when it is not.

    Cost is one full-history log plus a single batched `git cat-file` for the blobs, so the
    whole scan takes a couple of seconds rather than one subprocess per blob."""
    REC, FLD, SUB = "\x01", "\x02", "\x03"
    # diff.renameLimit=0 for the same reason first_matchers sets it, and it matters more
    # here: the stem pairing below compares the whole path minus its extension, so it
    # rescues src/F.c -> src/F.cpp but NOT a directory move, whose stem differs. Without
    # the lifted cap a bulk relocation degrades to delete+add and nothing carries credit.
    out = subprocess.run(
        ["git", "-c", "diff.renameLimit=0", "log", "--full-history", "--reverse",
         f"--format={REC}%H{FLD}%an{SUB}%ae", "--name-status", "-M", rev, "--", "src/"],
        cwd=REPO, capture_output=True, text=True, encoding="utf-8", errors="replace").stdout

    commits: list[tuple[str, str, list]] = []
    sha = handle = None
    ents: list = []
    for line in out.splitlines():
        if line.startswith(REC):
            if sha:
                commits.append((sha, handle, ents))
            sha, _, rest = line[1:].partition(FLD)
            name, _, email = rest.partition(SUB)
            handle, ents = _handle_from(name, email), []
            continue
        if not sha or not line.strip():
            continue
        parts = line.split("\t")
        if len(parts) >= 2:
            ents.append((parts[0], [x.strip() for x in parts[1:]]))
    if sha:
        commits.append((sha, handle, ents))

    def target(code: str, paths: list[str]) -> str:
        return paths[1] if (code.startswith("R") and len(paths) >= 2) else paths[0]

    want = sorted({(s, target(c, p)) for s, _, es in commits for c, p in es
                   if not c.startswith("D")})
    proc = subprocess.Popen(["git", "cat-file", "--batch"], cwd=REPO,
                            stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    query = "".join(f"{s}:{p}" + "\n" for s, p in want).encode()
    data, _ = proc.communicate(query)

    state: dict[tuple[str, str], str | None] = {}
    pos = idx = 0
    while pos < len(data) and idx < len(want):
        nl = data.find(b"\n", pos)
        if nl < 0:
            break
        header = data[pos:nl].decode("utf-8", "replace")
        pos = nl + 1
        if header.endswith("missing"):
            state[want[idx]] = None
            idx += 1
            continue
        try:
            size = int(header.rsplit(" ", 1)[1])
        except (IndexError, ValueError):
            break
        # Clamp to THIS blob. `data` is one concatenated cat-file batch response, so a
        # fixed-width read runs past any blob shorter than that and into the next one's
        # header and content. A clean blob followed by a drafted one was therefore
        # misread as drafted and never recorded a finisher -- and since `want` is
        # sorted by commit SHA, which side of that boundary a blob landed on varied
        # between runs. Same history, different credit.
        #
        # The draft test is asm_policy.has_draft_banner -- the same rule the live count
        # uses -- not a fixed head window: src/func_ov091_021339fc.c carried its marker
        # at byte 246 for months, so a 200-byte read judged every drafted state of that
        # file "clean" and handed the finisher's credit to the drafter.
        blob = data[pos:pos + size].decode("utf-8", "replace")
        state[want[idx]] = "draft" if asm_policy.has_draft_banner(blob) else "clean"
        pos += size + 1
        idx += 1

    drafted: set[str] = set()
    finishers: dict[str, str] = {}
    for sha, handle, ents in commits:
        for code, paths in ents:                       # an extension change keeps its history
            if code.startswith("R") and len(paths) >= 2:
                if paths[0] in drafted:
                    drafted.add(paths[1])
                # ...and so does the finish. Carrying `drafted` alone was a credit leak: the
                # renamed path arrived still marked drafted, with a clean blob and no finisher
                # entry, so the clause below read it as a fresh finish and handed the match to
                # whoever moved the file. Mirrors first_matchers' origin.pop(old, handle).
                if paths[0] in finishers:
                    finishers[paths[1]] = finishers.pop(paths[0])
        dels = [p[0] for c, p in ents if c.startswith("D")]
        adds = [p[0] for c, p in ents if c.startswith("A")]
        for d in dels:                                 # ...whether git called it a rename or not
            if d not in drafted:
                continue
            base = d.rsplit(".", 1)[0]
            for a in adds:
                if a != d and a.rsplit(".", 1)[0] == base:
                    drafted.add(a)
                    if d in finishers:
                        finishers[a] = finishers.pop(d)
        for code, paths in ents:
            if code.startswith("D"):
                continue                               # a delete never clears the draft history
            new = target(code, paths)
            blob = state.get((sha, new))
            if blob == "draft":
                drafted.add(new)
            elif blob == "clean" and new in drafted and new not in finishers:
                finishers[new] = handle
    return finishers


def attribution_overrides() -> dict[str, str]:
    """Manual {'src/name.c': github_login} for matches the git-add author gets wrong -- e.g. a
    contributor's work that landed via a maintainer's consolidating PR/squash, which records the
    maintainer (not the matcher) as the commit author. Applied with HIGHEST priority. Lives in
    attribution.json at the repo root: {"overrides": {"src/x.c": "login", ...}}."""
    p = REPO / "attribution.json"
    if not p.is_file():
        return {}
    try:
        data = json.loads(p.read_text(encoding="utf-8"))
        ov = data.get("overrides", {}) if isinstance(data, dict) else {}
        return {k: v for k, v in ov.items()
                if isinstance(k, str) and k.startswith("src/") and isinstance(v, str) and v}
    except Exception as e:
        print(f"  (attribution.json skipped: {e})")
        return {}


def identity_aliases() -> dict[str, str]:
    """{'git_handle': 'github_login'} to collapse one person's many git identities into a single
    login -- git records vary the author email/name per machine and per squash, so the same human
    splits into several buckets (e.g. 'alexsobolew7' and 'ruspecial' are one person). Keys are the
    lowercased handles src_authors() produces; values are the canonical GitHub login. From
    attribution.json: {"aliases": {"alexsobolew7": "ruspecial", ...}}."""
    p = REPO / "attribution.json"
    if not p.is_file():
        return {}
    try:
        data = json.loads(p.read_text(encoding="utf-8"))
        al = data.get("aliases", {}) if isinstance(data, dict) else {}
        return {str(k).lower(): v for k, v in al.items() if isinstance(v, str) and v}
    except Exception:
        return {}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default="chaos-db.json")
    ap.add_argument("--contrib-out", default=None,
                    help="path for contributions.json (default: next to --out)")
    ap.add_argument("--fail-on-transcribed", action="store_true",
                    help="exit 1 if any unbannered dcd transcription is in the tree")
    args = ap.parse_args()

    nm = {}
    nm_path = REPO / "nearmiss" / "db.jsonl"
    if nm_path.is_file():
        for l in nm_path.read_text(encoding="utf-8", errors="ignore").splitlines():
            if l.strip():
                try:
                    r = json.loads(l)
                    a = r["addr"]
                    nm[(r["module"], int(a, 0) if isinstance(a, str) else a)] = r
                except Exception:
                    continue

    finishers = match_finishers()        # src path -> who turned the draft into a real match
    firstmatch = first_matchers()        # src path -> first contributor to land the match
    overrides = attribution_overrides()  # manual fixes, highest priority
    aliases = identity_aliases()         # collapse one person's split git identities -> one login
    def canon(login):                    # apply the alias map (idempotent)
        return aliases.get(login.lower(), login)

    functions = []
    total_b = matched_b = matched_n = 0
    verified_b = verified_n = 0
    enrolled = enrolled_addresses()
    # Every src path any delinks.txt names, promoted or not. Read once: delinks_paths
    # walks all 106 delinks files, and calling it per function would walk them 11,396
    # times.
    blocks = set(LYC.delinks_paths())
    # The byte-gate-failure class, both halves, read once for the same reason. See the
    # `matched` conjunct below and tools/bytegate.py.
    alias_addrs = alias_collision_addresses()
    wont_build = BG.excluded_paths()
    bytegate_n = collections.Counter()
    enrollment_n = collections.Counter()
    alias_dropped = 0
    transcribed_files, unbannered_files = set(), set()
    # Every module, itcm included. relocs.module_universe is the one definition of
    # what "every module" means, and it fails loudly rather than skipping a new one.
    for sym, label in RL.module_universe():
        for line in sym.read_text(errors="ignore").splitlines():
            m = FUNC_RE.match(line)
            if not m:
                continue
            name, size, addr = m.group(1), int(m.group(2), 16), int(m.group(3), 16)
            # A zero-size record sharing an address with a sized one is a second NAME
            # for the function already on that address, not a second function, so it
            # never becomes a record at all -- it is out of the numerator and out of
            # the denominator alike. It used to be published as an unmatched record,
            # which put ten rows in every total that nothing could ever clear, gave the
            # treemap ten zero-area rectangles, and handed two records the same `id`.
            # See alias_collision_addresses for the ten and for how the drop self-heals.
            if BG.is_zero_size_alias(label, addr, size, alias_addrs):
                alias_dropped += 1
                continue
            f = SP.path_for(name)
            src_path = f.relative_to(REPO).as_posix() if f else None
            text = f.read_text(errors="ignore") if f else ""
            # The settled tag lives in the banner, and banners drift downward as the
            # recovery prose above them grows -- so the leading comment block, never a
            # fixed byte window.
            head = asm_policy.header_region(text)
            cls = asm_policy.classify(text) if src_path else None
            # Policy D (Tango's ruling on audit/enrollment_report.md section 6): a file
            # that exists is not evidence if the byte gate cannot get a verdict out of
            # it. linkcheck reports 22 of the 251 matched-but-unverified functions as
            # NO-SYM -- 18 that no compiler in the sweep will build and 4 zero-size alias
            # records counting a body their sized twin reports unmatched -- and those 22
            # stop counting here. Everything that REPRODUCES the cartridge keeps its
            # matched status, including the 185 unenrolled rows that byte-match but are
            # not promoted, which is why the report's options B and C were rejected: they
            # would have deleted matches that are demonstrably correct. `verified` is
            # unchanged and still published beside this, and none of the 22 was verified,
            # so that number does not move. The alias half of the class is handled above,
            # by never becoming a record; what is left here is the manifest half.
            #
            # The gate is applied only to records the OLD test would have counted, so
            # that it is credited with what it actually removed rather than with every
            # record the class happens to describe.
            countable = (bool(src_path) and not asm_policy.has_draft_banner(text)
                         and cls != "transcribed")
            bytegate_fail = countable and src_path is not None and src_path in wont_build
            matched = countable and not bytegate_fail
            total_b += size
            rec = {"id": f"{label}:0x{addr:08x}", "module": label, "name": name,
                   "addr": addr, "size": size, "matched": matched}
            if bytegate_fail:
                # Recorded on the record, not just subtracted from a total. A reader who
                # wonders why a src/ file exists for an unmatched function gets the
                # answer here instead of having to re-run the gate.
                rec["byteGate"] = "will-not-build"
                bytegate_n[rec["byteGate"]] += 1
            if src_path:
                rec["srcPath"] = src_path
                if cls == "transcribed":
                    rec["transcribed"] = True
                    transcribed_files.add(src_path)
                elif cls == "unbannered-asm":
                    unbannered_files.add(src_path)
                nomatch = no_match_needed(head)
                if nomatch:
                    rec["noMatch"] = nomatch
                if matched:
                    # Priority: manual override > whoever FINISHED the match (turned the
                    # NONMATCHING draft byte-identical) > whoever first added the file.
                    a = (overrides.get(src_path) or finishers.get(src_path)
                         or firstmatch.get(src_path))
                    if a:
                        rec["author"] = canon(a)
            if matched:
                matched_b += size
                matched_n += 1
                # Byte-verified is the subset the ROM build proves: enrolled, compiled,
                # linked into its module and compared to the cartridge. The rest are
                # matched on the strength of a source claim and are filled at link time by
                # a gap object holding the ROM's own bytes.
                if (label, addr) in enrolled:
                    rec["verified"] = True
                    verified_b += size
                    verified_n += 1
            else:
                r = nm.get((label, addr))
                if r and r.get("divergences") is not None:
                    rec["div"] = r["divergences"]
            # Appended last, deliberately. Every field above keeps the position it had
            # before this existed, so the whole diff against a previous chaos-db is one
            # inserted key per record and a reviewer can see at a glance that nothing
            # else moved. Nothing here reads it back, and `matched` is untouched: this
            # annotation reports the tree, it does not redefine the count.
            rec["enrollment"] = enrollment_of(label, addr, src_path, enrolled, blocks)
            enrollment_n[(rec["enrollment"], matched)] += 1
            functions.append(rec)

    if transcribed_files:
        print(f"TRANSCRIBED: {len(transcribed_files)} unbannered dcd transcription(s) "
              f"-- NOT counted as matched:")
        for p in sorted(transcribed_files):
            print(f"  {p}")
    if unbannered_files:
        print(f"unbannered-asm: {len(unbannered_files)} asm-bodied file(s) with no "
              f"HAND-ASM PRIMITIVE or NONMATCHING banner -- policy review needed "
              f"(banner or reclassify):")
        for p in sorted(unbannered_files):
            print(f"  {p}")

    project = None
    pc = REPO / "tools" / "chaosviewer.config.json"
    if pc.is_file():
        project = json.loads(pc.read_text(encoding="utf-8"))

    db = {
        "generatedAt": time.strftime("%Y-%m-%d %H:%M", time.gmtime()) + " UTC",
        "project": project,
        "stats": {
            "totalFunctions": len(functions),
            "matchedFunctions": matched_n,
            "totalBytes": total_b,
            "matchedBytes": matched_b,
            # The subset of `matched` the cartridge actually settles. `matched` is kept
            # unchanged -- it is what contributor credit is computed from, and every
            # existing consumer reads it -- but shipping it alone overstated coverage,
            # so the measured figure travels beside it. See enrolled_addresses.
            "verifiedFunctions": verified_n,
            "verifiedBytes": verified_b,
            "moduleCount": len({f["module"] for f in functions}),
            # The other two tiers ride along here so every consumer reads ONE file.
            # romstats-sync.sh on the VPS fetches this db and nothing else, and the
            # backend stores what it is told rather than walking any repository, so a
            # tier that is not in this block does not reach the site at all. Both are
            # computed from committed source with no ROM and no build, which is what
            # keeps this generator CI-safe.
            "tiers": tier_stats(),
        },
        "functions": functions,
    }
    out = pathlib.Path(args.out)
    out.write_text(json.dumps(db), encoding="utf-8")
    print(f"wrote {out} ({out.stat().st_size // 1024} KB): "
          f"{matched_n}/{len(functions)} funcs, {matched_b}/{total_b} bytes, "
          f"{db['stats']['moduleCount']} modules, "
          f"{sum(1 for f in functions if 'author' in f)} authored")
    # Both numbers in the log, always. The gap between them is the number of functions
    # counted on the strength of a source claim that no build compiles, and it is only
    # visible if the smaller figure is printed next to the larger one.
    print(f"  byte-verified: {verified_n}/{len(functions)} funcs, "
          f"{verified_b}/{total_b} bytes "
          f"({100.0 * verified_b / total_b:.2f}% vs {100.0 * matched_b / total_b:.2f}% "
          f"matched); {matched_n - verified_n} matched function(s) are compiled by "
          f"nothing")
    # ...and WHICH KIND of nothing, because the two halves have different remedies. A
    # matched/unenrolled function has a delinks entry waiting for a `complete`; a
    # matched/no_block one is almost always deliberate (thumb, alias, exclude list) and
    # promoting it is not on the table. Printed so the split lands in the CI log next to
    # the number it explains. See audit/enrollment_report.md.
    print("  enrollment: " + ", ".join(
        f"{k[0]}{'/matched' if k[1] else ''}={n}"
        for k, n in sorted(enrollment_n.items(), key=lambda kv: (kv[0][0], not kv[0][1]))))
    # The policy-D subtraction, in the log next to the number it explains. A silent
    # exclusion is the same mistake the enrollment split was added to fix.
    print("  byte-gate failures (NOT counted matched): " + (", ".join(
        f"{k}={n}" for k, n in sorted(bytegate_n.items())) or "none"))
    # And the records that never became records. This one changes the DENOMINATOR, so it
    # is the last thing that may move quietly: if this number drifts, the published rate
    # drifted with it and the log is where anyone would look.
    print(f"  zero-size aliases dropped from the universe: {alias_dropped} "
          f"(second names for functions already counted at the same address)")
    # A stale manifest row means someone edited one of the will-not-build files without
    # re-running the gate, so its exclusion has lapsed and that function is being counted
    # again. Permissive by design -- the count falls back to the old behaviour rather than
    # guessing -- but it must never be silent, and tools/test_bytegate.py fails on it.
    for s in BG.stale_rows():
        print(f"  WARNING: bytegate row {s['problem']} ({s['src']}); its exclusion has "
              f"lapsed and the function is counted matched again. Re-run "
              f"`python tools/bytegate.py --recheck`.")
    # Per-module counts in the log, so a module that stops being emitted shows up in
    # the CI diff as a line that vanished. The silent version of this cost itcm its
    # entire visibility; a number that goes to zero is at least readable.
    per_mod = collections.Counter(f["module"] for f in functions)
    print("  modules: " + ", ".join(f"{m}={n}" for m, n in sorted(per_mod.items())))

    # The single source of truth for the contributor chart: matched-function count per canonical
    # login. Regenerated on every merge (the workflow re-runs this), so "someone's number" is a
    # committed fact, not re-derived from git each time. --contrib-out defaults next to --out.
    tally = collections.Counter(f["author"] for f in functions if f.get("author"))
    # Weighted match coins. Kept as a SEPARATE field from `matched`, which stays the
    # plain honest count the contributor chart and the progress numbers read.
    coins = collections.Counter()
    for f in functions:
        who = f.get("author")
        if who:
            coins[who] += COIN_WEIGHTS.get(f["module"], 1)
    contrib = {
        "generatedAt": db["generatedAt"],
        "note": "Matched functions per contributor (canonical GitHub login), credited to whoever "
                "landed each match FIRST (credit follows renames; a later duplicate match does not "
                "steal it). Auto-generated by tools/chaos_db_ci.py from committed matches, git "
                "history, attribution.json aliases (collapse split identities) + overrides. Do not "
                "hand-edit; fix names in attribution.json instead. `matched` is the honest function "
                "count; `coins` is the same set weighted by COIN_WEIGHTS (match coins), which is "
                "what the MC ledger spends. They differ only for weighted modules.",
        "totalMatched": matched_n,
        "coinFormula": COIN_FORMULA,
        "coinWeights": COIN_WEIGHTS,
        "contributors": [{"login": who, "matched": n, "coins": coins[who]}
                         for who, n in tally.most_common()],
    }
    cpath = pathlib.Path(args.contrib_out) if args.contrib_out else out.with_name("contributions.json")
    cpath.write_text(json.dumps(contrib, indent=1), encoding="utf-8")
    print(f"wrote {cpath}: {len(tally)} contributors "
          f"(top: {', '.join(f'{w}={n}' for w, n in tally.most_common(4))})")

    if args.fail_on_transcribed and transcribed_files:
        print(f"FAILED: {len(transcribed_files)} unbannered dcd transcription(s) in the "
              f"tree (see the TRANSCRIBED list above)")
        sys.exit(1)


if __name__ == "__main__":
    main()
