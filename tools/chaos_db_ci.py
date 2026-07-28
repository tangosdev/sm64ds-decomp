"""CI-safe Chaos Viewer data generator: rebuilds chaos-db.json from COMMITTED
data only (no ROM, no local ledger), so GitHub Actions can refresh the
chaos-data branch on every push and the hosted viewer always shows current
modules and percentages.

Derived the same way as progress.py --write-readme:
  universe   config/**/symbols.txt  (name, addr, size per module)
  matched    src/<name>.c[pp] exists and is not marked // NONMATCHING
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
import re
import subprocess
import time

REPO = pathlib.Path(__file__).resolve().parent.parent
CONFIG = REPO / "config"
SRC = REPO / "src"

FUNC_RE = re.compile(
    r"^(\S+)\s+kind:function\((?:arm|thumb),size=0x([0-9a-fA-F]+)\).*?addr:0x([0-9a-fA-F]+)")
LOGIN_RE = re.compile(r"^(?:\d+\+)?([^@]+)@users\.noreply\.github\.com$")

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


def module_label(sym_path: pathlib.Path) -> str | None:
    """config/arm9/symbols.txt -> arm9; config/arm9/overlays/ovNNN -> ovNNN.
    itcm/dtcm are skipped to match the viewer's module universe."""
    rel = sym_path.parent.relative_to(CONFIG).as_posix()
    if rel == "arm9":
        return "arm9"
    m = re.fullmatch(r"arm9/overlays/(ov\d+)", rel)
    return m.group(1) if m else None


def _handle_from(name: str, email: str) -> str:
    """git identity -> canonical-ish handle: noreply login, else the email local-part
    (stable across author-name typos, usually equals the GitHub handle), else the name."""
    email = email.strip()
    m = LOGIN_RE.match(email)
    return m.group(1) if m else (email.split("@")[0].lower() or name.strip())


def first_matchers() -> dict[str, str]:
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
         "--format=%x01%an%x02%ae", "--name-status", "--", "src/"],
        cwd=REPO, capture_output=True, text=True, encoding="utf-8", errors="replace").stdout
    origin: dict[str, str] = {}   # live path -> author of the earliest add in its lineage
    handle = None
    for line in out.splitlines():
        if line.startswith("\x01"):
            name, _, email = line[1:].partition("\x02")
            handle = _handle_from(name, email)
        elif handle and line and line[0] in "ADR":
            parts = line.split("\t")
            code = parts[0]
            if code.startswith("A") and len(parts) >= 2:
                origin.setdefault(parts[1].strip(), handle)
            elif code.startswith("D") and len(parts) >= 2:
                origin.pop(parts[1].strip(), None)
            elif code.startswith("R") and len(parts) >= 3:
                old, new = parts[1].strip(), parts[2].strip()
                origin[new] = origin.pop(old, handle)  # carry the matcher's credit forward
    return origin


def match_finishers() -> dict[str, str]:
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
    out = subprocess.run(
        ["git", "log", "--full-history", "--reverse",
         f"--format={REC}%H{FLD}%an{SUB}%ae", "--name-status", "-M", "--", "src/"],
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
        state[want[idx]] = "draft" if b"// NONMATCHING" in data[pos:pos + 200] else "clean"
        pos += size + 1
        idx += 1

    drafted: set[str] = set()
    finishers: dict[str, str] = {}
    for sha, handle, ents in commits:
        for code, paths in ents:                       # an extension change keeps its history
            if code.startswith("R") and len(paths) >= 2 and paths[0] in drafted:
                drafted.add(paths[1])
        dels = [p[0] for c, p in ents if c.startswith("D")]
        adds = [p[0] for c, p in ents if c.startswith("A")]
        for d in dels:                                 # ...whether git called it a rename or not
            if d not in drafted:
                continue
            base = d.rsplit(".", 1)[0]
            for a in adds:
                if a != d and a.rsplit(".", 1)[0] == base:
                    drafted.add(a)
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
    for sym in sorted(CONFIG.rglob("symbols.txt")):
        label = module_label(sym)
        if label is None:
            continue
        for line in sym.read_text(errors="ignore").splitlines():
            m = FUNC_RE.match(line)
            if not m:
                continue
            name, size, addr = m.group(1), int(m.group(2), 16), int(m.group(3), 16)
            src_path = None
            for ext in ("c", "cpp"):
                f = SRC / f"{name}.{ext}"
                if f.is_file():
                    src_path = f"src/{name}.{ext}"
                    break
            head = (SRC / src_path.split("/", 1)[1]).read_text(errors="ignore")[:200] if src_path else ""
            matched = bool(src_path) and "NONMATCHING" not in head
            total_b += size
            rec = {"id": f"{label}:0x{addr:08x}", "module": label, "name": name,
                   "addr": addr, "size": size, "matched": matched}
            if src_path:
                rec["srcPath"] = src_path
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
            else:
                r = nm.get((label, addr))
                if r and r.get("divergences") is not None:
                    rec["div"] = r["divergences"]
            functions.append(rec)

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
            "moduleCount": len({f["module"] for f in functions}),
        },
        "functions": functions,
    }
    out = pathlib.Path(args.out)
    out.write_text(json.dumps(db), encoding="utf-8")
    print(f"wrote {out} ({out.stat().st_size // 1024} KB): "
          f"{matched_n}/{len(functions)} funcs, {matched_b}/{total_b} bytes, "
          f"{db['stats']['moduleCount']} modules, "
          f"{sum(1 for f in functions if 'author' in f)} authored")

    # The single source of truth for the contributor chart: matched-function count per canonical
    # login. Regenerated on every merge (the workflow re-runs this), so "someone's number" is a
    # committed fact, not re-derived from git each time. --contrib-out defaults next to --out.
    tally = collections.Counter(f["author"] for f in functions if f.get("author"))
    contrib = {
        "generatedAt": db["generatedAt"],
        "note": "Matched functions per contributor (canonical GitHub login), credited to whoever "
                "landed each match FIRST (credit follows renames; a later duplicate match does not "
                "steal it). Auto-generated by tools/chaos_db_ci.py from committed matches, git "
                "history, attribution.json aliases (collapse split identities) + overrides. Do not "
                "hand-edit; fix names in attribution.json instead.",
        "totalMatched": matched_n,
        "contributors": [{"login": who, "matched": n} for who, n in tally.most_common()],
    }
    cpath = pathlib.Path(args.contrib_out) if args.contrib_out else out.with_name("contributions.json")
    cpath.write_text(json.dumps(contrib, indent=1), encoding="utf-8")
    print(f"wrote {cpath}: {len(tally)} contributors "
          f"(top: {', '.join(f'{w}={n}' for w, n in tally.most_common(4))})")


if __name__ == "__main__":
    main()
