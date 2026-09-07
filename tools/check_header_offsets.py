"""Confirm every field in a struct header lands on the offset its comment claims.

The generated headers encode layout twice: once as a sequence of declarations and
pads, and once as a `/* 0x0a4 */` comment per field. Retyping a field silently
breaks the agreement between them unless the following pad is shrunk to match, and
nothing else in the build would notice -- a header is not compiled on its own, and
the byte gate cannot see a field no source file happens to read.

This walks the declarations, applies natural alignment, and compares. Used as the
first gate on any header edit; see notes/archive/plan-scalar-markers.md 4.

    python tools/check_header_offsets.py include/dActor_c.h include/dScMgBase_c.h
    python tools/check_header_offsets.py --changed              # vs origin/main
    python tools/check_header_offsets.py --changed main
    python tools/check_header_offsets.py --changed --committed-only   # what CI sees

Running it with no arguments is an error, not a pass -- see _resolve_paths.
An empty --changed work list is an error too, EXCEPT in the one case where it is
honest: the diff contained files and none of them was under include/. That
distinction is computed and printed rather than assumed -- see changed_paths.
"""
import re, subprocess, sys, pathlib
SZ = {"u8":1,"s8":1,"char":1,"u16":2,"s16":2,"short":2,"u32":4,"s32":4,"int":4,
      "unsigned":4,"long":4,"Fix12i":4,"float":4,"u64":8,"s64":8,"double":8}
# Alignment is NOT the same as width once aggregates are in play: a Vector3 is 12
# bytes wide but 4-aligned, and using the width for the padding test would invent
# padding before every one.
#
# Scalars are self-aligned only up to 4. mwccarm 2004/b56 -proc arm946e aligns
# `long long` and `double` to 4, not 8 -- measured, not assumed:
#     struct { char c; long long v; };   /* sizeof == 12, not 16 */
#     struct { char c; double d; };      /* sizeof == 12, not 16 */
# (Asserting 16 instead is rejected by the compiler, so the probe discriminates.)
# This is what lets `s64 unk_004;` sit at 0x004 in include/dBgPi.h (the struct
# include/ClsnResult.h used to hold, before #1643 renamed the collision classes).
ALIGN = {t: min(w, 4) for t, w in SZ.items()}

REPO = pathlib.Path(__file__).resolve().parent.parent
# A simple aggregate body: no nested braces, no methods.
AGG = re.compile(r"(?:typedef\s+)?struct\s+(\w+)\s*\{([^{}]*)\}", re.S)
# `Fix12i x, y, z;` -- one type, several declarators, optional array bounds.
MEMBERS = re.compile(r"^\s*([A-Za-z_]\w*)\s+([^;]+);", re.M)


def learn_aggregates(*paths):
    """Teach SZ/ALIGN the widths of simple project aggregates.

    Without this a `Vector3 pos;` field is unrecognised, and the tool's own rule --
    an unrecognised declaration leaves the running offset short, so every later
    field silently matches at the wrong place -- means it cannot gate ANY header
    that uses one. include/dBgCh_Gnd.h has carried `Vector3 pos;` since it was
    hand-extended and has been unparseable that whole time.

    Deliberately narrow: only brace-free bodies whose members are already-known
    types, so nothing is guessed. Anything else stays unknown and still reports
    UNPARSED rather than being silently mis-sized.
    """
    for path in paths:
        try:
            txt = pathlib.Path(path).read_text(errors="replace")
        except OSError:
            continue
        for name, body in AGG.findall(txt):
            if name in SZ:
                continue
            off = align = 0
            for typ, decls in MEMBERS.findall(body):
                ptr = "*" in decls
                w = 4 if ptr else SZ.get(typ)
                a = 4 if ptr else ALIGN.get(typ)
                if w is None:
                    off = None
                    break
                for d in decls.split(","):
                    arr = re.search(r"\[\s*(0x[0-9a-fA-F]+|\d+)\s*\]", d)
                    if off % a:
                        off += a - (off % a)
                    off += w * (int(arr.group(1), 0) if arr else 1)
                    align = max(align, a)
            if off and align:
                if off % align:
                    off += align - (off % align)   # tail padding
                SZ[name], ALIGN[name] = off, align


learn_aggregates(REPO / "include" / "types.h")

# `void *p;` / `Model* m;` -- any pointer is 4 bytes on this target.
# The leading `struct`/`union`/`class`/`enum` is an elaborated type specifier and
# names the same type as the bare tag: `struct Matrix4x3 mat4x3;` is a Matrix4x3.
# Without it these were the ONLY unparsed declarations left in include/, and an
# unparsed one stops the gate reporting mismatches for the rest of the header --
# so a handful of `struct X y;` lines were suppressing the check on 8 files.
# An unknown tag still fails: the keyword makes the shape parseable, not the size.
# `Particle::SysTracker mSysTracker;` -- a namespace-qualified type. Without the
# `(?:\w+::)*` prefix this failed to match at all (`::` fits nowhere in the
# plain-identifier type group), which is a WORSE failure mode than an
# unrecognised type: the line never reached the "unrecognised declaration"
# path either, just fell through silently mismatched against every regex
# below it. The type is still looked up by its bare final segment (SZ has no
# way to record a qualified name), which is exactly what a
# `Name_size_must_be_0xN` assertion of the same class also has to do.
# `u8 touchIcon_0f4[8][0x24];` -- a 2-D array (dScMgBase_c's __destroy_arr
# element block). One `(?:\[...\])?` group only ever captured the FIRST
# dimension and left `[0x24];` dangling after what should have been the
# terminating `;`, so the whole match failed. Capture every bracket group as
# one blob instead and multiply the dimensions in ARR_DIMS below.
DECL = re.compile(r"^\s*(?:(?:struct|union|class|enum)\s+)?((?:\w+::)*[A-Za-z_]\w*)\s*(\**)\s*(\w+)\s*"
                  r"((?:\[\s*(?:0x[0-9a-fA-F]+|\d+)\s*\])*)\s*;"
                  r"(?:\s*/\*\s*(0x[0-9a-fA-F]+))?")
ARR_DIMS = re.compile(r"\[\s*(0x[0-9a-fA-F]+|\d+)\s*\]")
# lines inside a struct body that are legitimately not declarations
IGNORABLE = re.compile(r"^\s*($|/\*|\*|//|\}|#)")


def _code_without_comments_or_strings(line, in_block_comment=False):
    """Return C/C++ code tokens and whether a block comment remains open.

    Brace depth is used only to skip inline method bodies.  Counting raw braces
    makes a `// }` or `/* { */` inside one of those bodies end or extend the skip,
    and the next real body brace is then mistaken for the outer struct's close.
    This deliberately small lexer removes comments and quoted literals while
    preserving every punctuation token the body-depth state needs.
    """
    code = []
    quote = None
    escaped = False
    i = 0
    while i < len(line):
        ch = line[i]
        nxt = line[i + 1] if i + 1 < len(line) else ""
        if in_block_comment:
            if ch == "*" and nxt == "/":
                in_block_comment = False
                i += 2
            else:
                i += 1
            continue
        if quote:
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == quote:
                quote = None
            i += 1
            continue
        if ch == "/" and nxt == "/":
            break
        if ch == "/" and nxt == "*":
            in_block_comment = True
            i += 2
            continue
        if ch in "\"'":
            quote = ch
            i += 1
            continue
        code.append(ch)
        i += 1
    return "".join(code), in_block_comment

# Class sizes come from the tree's own compile-time assertions:
#   typedef char ModelAnim_size_must_be_0x64[...]
# Without these an embedded member is an unknown type, which this checker skips --
# and skipping without advancing the offset makes every later field mismatch.
# rglob, not glob: Matrix4x3 and Matrix3x3 assert their sizes in include/math/,
# so a non-recursive scan missed exactly the two that Model.h and friends embed.
CLASS_SIZES = {}
# ...and WHERE each one came from. The table is keyed by the bare tag, so two
# classes of the same name in different headers are one entry -- see
# _shadowed_by_nested below, which needs the origin to tell them apart.
CLASS_SIZE_SRC = {}
# ...and the same sizes keyed by their QUALIFIED name, read out of the assertion's
# own `sizeof(...)` expression rather than out of its typedef identifier.
#
# The identifier cannot be trusted to spell the qualified name, because the tree's
# convention for one is not mechanical. `dMgPsOpt_c::TouchIcon_c` is asserted as
# `dMgPsOpt_TouchIcon_c_size_must_be_0x24` -- outer tag with its `_c` dropped -- and
# `Particle::SysTracker::Contents` as `Particle_SysTracker_Contents_size_must_be_0x748`.
# No rule derives `dMgPsOpt_TouchIcon_c` from `dMgPsOpt_c` and `TouchIcon_c` that does
# not also have to guess. The sizeof() operand, though, is the real qualified name by
# construction -- the compiler resolves it or the header does not build.
#
# Measured over all 507 headers when this was written: 486 assertions parse in full,
# 26 of them qualified, and the typedef identifier's `_0xN` suffix agrees with the
# expression's own constant in 486 of 486 cases. So this reads the same numbers the
# bare-tag table already reads; it only learns a second, unambiguous key for them.
CLASS_SIZES_QUALIFIED = {}
# `== 6` as well as `== 0x6`: types.h asserts Vector3s in decimal, and it is the one
# assertion in the tree that the full form would otherwise miss.
_SIZE_ASSERT_FULL = re.compile(
    r"typedef\s+char\s+(\w+)_size_must_be_(0x[0-9a-fA-F]+)\s*\[\s*"
    r"sizeof\s*\(\s*(?:struct|class|union)?\s*((?:\w+\s*::\s*)*\w+)\s*\)\s*"
    r"==\s*(0x[0-9a-fA-F]+|\d+)", re.S)
for _h in pathlib.Path(__file__).resolve().parents[1].joinpath("include").rglob("*.h"):
    _txt = _h.read_text(errors="replace")
    for _m in re.finditer(r"(\w+)_size_must_be_(0x[0-9a-fA-F]+)", _txt):
        CLASS_SIZES[_m.group(1)] = int(_m.group(2), 16)
        CLASS_SIZE_SRC[_m.group(1)] = _h
    for _m in _SIZE_ASSERT_FULL.finditer(_txt):
        _expr = re.sub(r"\s+", "", _m.group(3))
        if "::" in _expr:
            CLASS_SIZES_QUALIFIED[_expr] = int(_m.group(2), 16)
            # Only if nothing else claimed it: the bare-tag table above is what the
            # shadow check reads, and a qualified entry must not overwrite the origin
            # of a same-named top-level class.
            CLASS_SIZE_SRC.setdefault(_expr, _h)
SZ.update(CLASS_SIZES)


def _qualified_size(outer, typ):
    """Size of `typ` as written inside `outer`, from a QUALIFIED size assertion.

    Returns None when no assertion names this nesting, which leaves the caller on
    its existing bare-tag path -- this only ever adds an answer where there was
    none, it never overrides one.

    Two forms are accepted:

      exact   `dMgPsOpt_c::TouchIcon_c` for `TouchIcon_c` written inside dMgPsOpt_c
      nested  `dPa_c::level_c::callback_c` for `callback_c` written inside dPa_c,
              where the type is declared in an intermediate scope the field walk
              does not track

    The second form must be UNIQUE to answer. Two different `Outer::*::Inner`
    entries with the same size are still two different classes, and picking either
    would be the exact guess this function exists to avoid -- see
    _shadowed_by_nested, which is the same failure one level up.
    """
    if not outer:
        return None
    exact = CLASS_SIZES_QUALIFIED.get(outer + "::" + typ)
    if exact is not None:
        return exact
    prefix = outer + "::"
    hits = {v for k, v in CLASS_SIZES_QUALIFIED.items()
            if k.startswith(prefix) and k.rsplit("::", 1)[-1] == typ}
    return hits.pop() if len(hits) == 1 else None

# A derived class's own fields start at the base's DATA SIZE, not its sizeof.
#
# The Itanium ABI lets a derived class place members in a non-POD base's TAIL
# PADDING, and this tree's classes do it: Platform's last field ends at 0x31e
# and its size rounds to 0x320, so DonutBlock's s16 lands at 0x31e -- confirmed
# by the bytes, since DonutBlock::Behavior reads this+0x31e and reproduces the
# ROM. Starting at sizeof reported every field of such a class as mismatched
# and exited 1.
#
# Starting at the data size is right for BOTH cases, because the walk below
# already aligns each field: a class whose first own field is a 4-byte value
# still gets 0x31e rounded up to 0x320. So this is not a special case for tail
# padding, it is the correct start offset that sizeof was approximating.
#
# The data size is read off the LAST COMMENTED FIELD -- the ROM evidence -- not
# recomputed, so this cannot drift from the walk below. A class with no
# commented fields falls back to sizeof.
DATA_SIZE = {}
for _h in pathlib.Path(__file__).resolve().parents[1].joinpath("include").rglob("*.h"):
    _txt = _h.read_text(errors="replace")
    for _cm in re.finditer(r"^\s*struct (\w+)\s*(?::\s*(?:public\s+)?\w+\s*)?\{",
                           _txt, re.M):
        _cls = _cm.group(1)
        _body = _txt[_cm.end():]
        _end = _body.find("\n};")
        if _end != -1:
            _body = _body[:_end]
        _last = None
        for _fm in re.finditer(r"^\s*(?:(?:struct|union|class|enum)\s+)?([A-Za-z_]\w*)"
                               r"\s*(\**)\s*(\w+)\s*(?:\[\s*(0x[0-9a-fA-F]+|\d+)\s*\])?\s*;"
                               r"\s*/\*\s*(0x[0-9a-fA-F]+)", _body, re.M):
            _ty, _star, _nm, _arr, _off = _fm.groups()
            _w = 4 if _star else SZ.get(_ty)
            if _w is None:
                _last = None
                break
            _n = int(_arr, 0) if _arr else 1
            _last = int(_off, 16) + _w * _n
        if _last is not None and _cls not in DATA_SIZE:
            DATA_SIZE[_cls] = _last

KNOWN = REPO / "config" / "header-offset-known-issues.txt"


def _known_issues():
    """Header paths this gate already tolerates. See the file's own header comment.

    Waived rather than skipped silently: each one still prints, so the debt stays
    visible in the log and the list can only shrink.
    """
    if not KNOWN.is_file():
        return set()
    return {line.split("#", 1)[0].strip().replace("\\", "/")
            for line in KNOWN.read_text(encoding="utf-8").splitlines()
            if line.split("#", 1)[0].strip()}


HEADER_SUFFIXES = (".h", ".hpp")


def _git_lines(args, repo=None):
    """(lines, error). A git failure is an error string, never a traceback.

    Split on newlines, not on whitespace: `git diff --name-only` quotes and escapes a
    path containing a space, but the old `proc.stdout.split()` tore any such path into
    two nonexistent ones, and a nonexistent path here reads as "no header changed".
    """
    proc = subprocess.run(["git", *args], cwd=str(repo or REPO),
                          capture_output=True, text=True)
    if proc.returncode != 0:
        return None, f"git {' '.join(args)} failed: {proc.stderr.strip()}"
    return [ln.strip() for ln in proc.stdout.splitlines() if ln.strip()], None


def changed_paths(base, committed_only=False, repo=None, head="HEAD"):
    """What this branch changed, in the three buckets the empty-list decision needs.

    Returns ``(buckets, error)``. ``buckets`` is a dict of sorted, de-duplicated
    repo-relative paths::

        total     every added/modified file, ANY directory -- the proof the diff
                  machinery ran at all
        include   the subset under include/, ANY extension
        headers   the subset of those that is a .h/.hpp -- the actual work list
        worktree  the subset of `headers` that is NOT in the commits (see below)
        dropped   include/ paths the extension filter discarded

    THREE SOURCES, unioned:

    1. ``{base}...HEAD`` -- the commits. This was the only source the tool had, and it
       is why a local pre-commit run reported a pass on work it had never opened: a
       header edited and not yet committed is not in this diff, so `--changed` printed
       "no include/ header added or modified" and exited 0 over a dirty tree.
    2. ``HEAD`` vs the working tree -- staged and unstaged edits both. Empty on a CI
       checkout, and the entire work list when a developer runs this before committing.
    3. untracked files -- a brand-new header that has never been `git add`ed is in
       neither of the above, and a brand-new header is precisely the one whose offsets
       have never been checked by anything.

    ``committed_only=True`` uses source 1 alone, which is exactly what CI sees; the
    other two are still computed so the caller can say out loud that they were ignored.

    ``head`` is for pointing this at history other than the current branch -- the tests
    aim it at real merge commits and assert the headers those pull requests edited still
    come back. Sources 2 and 3 are relative to the working tree by definition, so they
    are only consulted when ``head`` is the working tree's own HEAD.

    Renames arrive as an add because ``-M`` is deliberately not passed -- a header that
    moved still has to have its offsets agree.
    """
    commits, err = _git_lines(["diff", "--name-only", "--diff-filter=AM",
                               f"{base}...{head}"], repo)
    if err:
        return None, err
    if head == "HEAD":
        dirty, err = _git_lines(["diff", "--name-only", "--diff-filter=AM", "HEAD"], repo)
        if err:
            return None, err
        untracked, err = _git_lines(["ls-files", "--others", "--exclude-standard"], repo)
        if err:
            return None, err
    else:
        dirty = untracked = []

    local = set(dirty) | set(untracked)
    total = set(commits) if committed_only else set(commits) | local
    inc = {p for p in total if p.startswith("include/")}
    heads = {p for p in inc if p.endswith(HEADER_SUFFIXES)}
    return {
        "total": sorted(total),
        "include": sorted(inc),
        "headers": sorted(heads),
        "dropped": sorted(inc - heads),
        # Reported even under --committed-only, where it is what is being IGNORED.
        "worktree": sorted(p for p in local
                           if p.startswith("include/")
                           and p.endswith(HEADER_SUFFIXES)
                           and p not in set(commits)),
    }, None


def _resolve_changed(base, committed_only=False, repo=None):
    """(paths, exit_code). ``paths`` is None when the caller should exit immediately.

    AN EMPTY WORK LIST IS NOT AUTOMATICALLY A PASS, AND NOT AUTOMATICALLY A FAILURE.
    The old code exited 0 on any empty diff, which collapsed four different situations
    into one green line. They are not the same and this pulls them apart:

    * The diff named files and none of them is under include/. This is the ONE honest
      empty: most pull requests touch no header, the gate has genuinely nothing to do,
      and saying so is not a hollow pass because the count of files it DID see is
      printed as evidence the diff resolved to something.
    * The diff named nothing at all. On a pull request that cannot happen -- a pull
      request always changes a file -- so this is a base ref that resolved to the wrong
      commit (an unfetched `origin/main`, a shallow clone with no merge base, HEAD equal
      to base). The old code called it a pass. It is the failure mode that makes every
      other guarantee in this file worthless, so it now fails.
    * include/ changed but the .h/.hpp filter discarded every one of those paths. The
      filter existed to skip `include/`'s non-headers; when it eats the whole work list
      it is no longer skipping noise, it is skipping the job.
    * The headers are only in the working tree. Included by default now; under
      --committed-only they are reported loudly rather than silently dropped.
    """
    root = pathlib.Path(repo) if repo else REPO
    buckets, err = changed_paths(base, committed_only, root)
    if buckets is None:
        print(f"check_header_offsets: {err}", file=sys.stderr)
        return None, 1

    if committed_only and buckets["worktree"]:
        # Not folded in, so say so at the top of the log where it cannot be missed.
        print(f"check_header_offsets: WARNING -- --committed-only, so "
              f"{len(buckets['worktree'])} uncommitted header(s) are NOT being checked:")
        for p in buckets["worktree"]:
            print(f"    {p}")

    if not buckets["total"]:
        print(f"check_header_offsets: the diff against {base} is EMPTY -- no file added "
              f"or modified anywhere in the tree, not merely no header.", file=sys.stderr)
        print(f"check_header_offsets: that is not 'a change that touches no header', it "
              f"is a base ref that resolved to nothing. Check that {base} exists and is "
              f"fetched (a shallow clone has no merge base), or pass the header paths "
              f"explicitly.", file=sys.stderr)
        return None, 1

    if buckets["dropped"] and not buckets["headers"]:
        print(f"check_header_offsets: {len(buckets['dropped'])} include/ path(s) changed "
              f"and NONE is a {'/'.join(HEADER_SUFFIXES)} -- the extension filter "
              f"discarded this gate's entire work list:", file=sys.stderr)
        for p in buckets["dropped"]:
            print(f"    {p}", file=sys.stderr)
        return None, 1

    if not buckets["headers"]:
        # The honest empty. The evidence is the count, and it is printed.
        print(f"check_header_offsets: {len(buckets['total'])} file(s) added or modified "
              f"vs {base}, 0 of them under include/ -- nothing for this gate to check.")
        return None, 0

    if buckets["dropped"]:
        print(f"check_header_offsets: ignoring {len(buckets['dropped'])} non-header "
              f"include/ path(s): {', '.join(buckets['dropped'])}")
    if buckets["worktree"] and not committed_only:
        print(f"check_header_offsets: {len(buckets['worktree'])} of these are "
              f"UNCOMMITTED working-tree changes: {', '.join(buckets['worktree'])}")
    print(f"check_header_offsets: {len(buckets['headers'])} changed header(s) vs {base}")

    # A path the commit range names can have been deleted in the working tree since.
    # Dropping it silently would shrink the work list back towards the empty pass this
    # whole function exists to refuse, so it is named.
    live = [p for p in buckets["headers"] if (root / p).is_file()]
    gone = [p for p in buckets["headers"] if p not in live]
    if gone:
        print(f"check_header_offsets: {len(gone)} changed header(s) no longer on disk, "
              f"skipping: {', '.join(gone)}")
    if not live:
        print("check_header_offsets: every changed header has since been deleted -- "
              "nothing left to check, and that is not a pass", file=sys.stderr)
        return None, 1
    return live, 0


def _resolve_paths(argv, repo=None):
    """The headers to check, or (None, code) having said why.

    This tool fed ``sys.argv[1:]`` straight into the loop below, so running it bare
    checked nothing, printed nothing, and exited 0 -- a clean pass over zero files.
    Nothing in `.github/workflows/` or `tools/hooks/` invoked it, so that silent
    vacuous pass was, in practice, its only behaviour. Refusing an empty list is what
    makes wiring it into CI mean anything.

    ``--changed [base]`` is the CI form: the added and modified headers of this branch,
    working tree included unless ``--committed-only`` is given.
    """
    argv = list(argv)
    committed_only = False
    if "--committed-only" in argv:
        argv.remove("--committed-only")
        committed_only = True
    if argv and argv[0] == "--changed":
        base = argv[1] if len(argv) > 1 else "origin/main"
        return _resolve_changed(base, committed_only, repo)
    if committed_only:
        print("check_header_offsets: --committed-only only means anything with --changed",
              file=sys.stderr)
        return None, 1
    if not argv:
        print("usage: check_header_offsets.py <header>... | "
              "--changed [base] [--committed-only]\n"
              "refusing to run with no headers: an empty check is not a pass",
              file=sys.stderr)
        return None, 1
    # A named header that is not there was a bare FileNotFoundError traceback out of
    # the walk below -- loud, but it never said WHICH argument was wrong, and this
    # file's own usage line named two headers that renames had since deleted
    # (include/Enemy.h in #1574, include/ClsnResult.h in #1643).
    root = pathlib.Path(repo) if repo else REPO
    missing = [a for a in argv
               if not pathlib.Path(a).is_file() and not (root / a).is_file()]
    if missing:
        print(f"check_header_offsets: {len(missing)} named header(s) do not exist: "
              f"{', '.join(missing)}", file=sys.stderr)
        return None, 1
    return argv, 0


def _nested_names(all_lines):
    """Tags defined as a NESTED type somewhere in this file.

    Indentation is the test, which is what the field walk below already uses to
    tell a nested definition from the outer one, so the two agree by construction.
    """
    out = set()
    for line in all_lines:
        m = re.match(r"^[ \t]+(?:struct|union|class) (\w+)\s*(?::\s*(?:public\s+)?\w+\s*)?\{", line)
        if m:
            out.add(m.group(1))
    return out


def _shadowed_by_nested(typ, nested_names, fp):
    """True when SZ[typ] is about a DIFFERENT class than the one `typ` names here.

    CLASS_SIZES is keyed by the bare tag -- it has no way to record a qualified
    name -- so a class nested inside the struct under test silently borrows the
    size of any unrelated top-level class that happens to share its tag.

    Measured, one instance in the tree and it is not a small one:
    `fBase_c::Manager` is SceneNode(0x14) + 2 * fLiNdBaPr_c(0x10) = 0x34, but
    `Manager_size_must_be_0x3c` in include/Particle__Manager.h describes
    Particle::Manager. The checker took 0x3c and reported the two fields after it
    8 bytes late -- against a header whose own `fBase_c_size_must_be_0x50`
    assertion proves the comments right and the checker wrong. This was invisible
    while fBase_c.h was skipped wholesale; unskipping it is what exposed it.

    Refuse rather than guess. A local size is derivable in principle, but a nested
    body carries inline method bodies that the narrow learner above will not walk,
    and a WRONG size here is worse than an honest UNPARSED: it shifts every later
    field and each one still "matches" if the comments were written from the same
    wrong model.
    """
    if typ not in nested_names or typ not in CLASS_SIZE_SRC:
        return False
    # Compare RESOLVED paths. CLASS_SIZE_SRC is built by rglob from an absolute repo
    # root while `fp` arrives from the command line, so `include/dScEntry_c.h` and
    # `C:/...
    # /include/dScEntry_c.h` are the same file and unequal as Path objects. Comparing
    # them raw made a type asserted in its OWN header -- dScEntry_c::icon_c, whose
    # `icon_c_size_must_be_0x24` sits nine lines below the struct -- look shadowed,
    # turning a header that checked 9 fields clean into an UNPARSED failure.
    try:
        return CLASS_SIZE_SRC[typ].resolve() != pathlib.Path(fp).resolve()
    except OSError:
        return CLASS_SIZE_SRC[typ] != fp


def _root_offset(all_lines, open_lineno):
    """Offset of the first field of a ROOT struct whose body opens at `open_lineno`.

    4 when the struct is polymorphic and leaves its vptr implicit, 0 otherwise.

    Scoped to THIS struct's body on purpose. A twin header carries a flat C view of
    the same class below an `#else`, and that half routinely spells `void *vtable;`
    even when the C++ half above it does not -- Animation.h is exactly that shape. A
    file-wide search for either marker reads the wrong half and silently shifts every
    field of the right one by 4.
    """
    virtual = vtable_field = False
    for line in all_lines[open_lineno:]:
        if re.match(r"^\};", line):
            break
        if re.match(r"^\s*virtual\s", line):
            virtual = True
        elif re.match(r"^\s*[A-Za-z_][^;]*[ *]vtable\s*;", line):
            vtable_field = True
    return 4 if (virtual and not vtable_field) else 0


def main(argv, repo=None):
    """Check every header `argv` resolves to. Exit code is the gate's verdict."""
    root = pathlib.Path(repo) if repo else REPO
    paths, code = _resolve_paths(argv, root)
    if paths is None:
        return code
    rc = 0
    WAIVED = _known_issues()
    for path in paths:
        if pathlib.PurePath(path).as_posix() in WAIVED:
            print(f"{path}: WAIVED -- config/header-offset-known-issues.txt")
            continue
        # Resolve against the repo root, not the process cwd: `--changed` yields
        # repo-relative paths and the gate must not depend on where it was run
        # from. An explicit argument that resolves from cwd still wins.
        fp = pathlib.Path(path)
        if not fp.is_absolute() and not fp.is_file():
            fp = root / path
        txt = fp.read_text(errors="replace")
        off, bad, n, skipped, pending = 0, 0, 0, [], []
        trusted = True
        started = in_comment = unmodelled = nested = skip_other = False
        skip_body = 0
        await_body = body_comment = False
        await_body_line = None
        unknown_base = derived_from = None
        expected = fp.stem
        all_lines = txt.splitlines()
        nested_names = _nested_names(all_lines)
        for lineno, line in enumerate(all_lines, 1):
            if not started:
                # A struct-with-body BEFORE the file's own class is a helper type
                # (ActorBase_SceneNode in fBase_c.h, KCL_Tri in dBgW_Kc.h,
                # Particle::SysTracker's namespace-nested body in Stage.h), not the
                # struct this file is named for. Without this check the FIRST
                # struct-with-body wins regardless of name, and the tool silently
                # checks the helper instead of the class the header exists to
                # verify -- confirmed tree-wide: 13 headers had this shape, most
                # already reporting a false "0 unparsed" pass on the wrong struct.
                if skip_other:
                    if re.match(r"^\s*\};", line):
                        skip_other = False
                    continue
                # `struct X {` or `struct X : Base {`. Without the second form this
                # tool never started on a derived struct at all, and reported
                # "0 commented fields ... struct spans 0x0" -- which reads exactly
                # like a pass. Every class this repo reconstructs from here on is a
                # derived struct, so that silence covered the growing majority.
                m0 = re.match(r"^\s*struct (\w+)\s*(?::\s*(?:public\s+)?(\w+)\s*)?\{", line)
                if m0:
                    if m0.group(1) != expected:
                        # `struct BMA_File { u16 numFrames; };` -- opens AND closes on
                        # the same line. Setting skip_other here and never checking
                        # THIS line for the closing brace left it permanently stuck:
                        # the real target struct's own opening line was then read
                        # while still "skipping", so it was silently swallowed whole
                        # -- MaterialChanger.h and TextureTransformer.h went from an
                        # honest UNPARSED failure to a hollow "0 fields, 0 unparsed"
                        # pass, checking nothing at all. Only skip forward if this
                        # line's own body does NOT already close it.
                        if not re.search(r"\};", line):
                            skip_other = True
                        continue
                    started = True
                    base = m0.group(2)
                    if base is None:
                        # A ROOT class has no base sub-object, but it is not therefore
                        # at offset 0: a POLYMORPHIC root places one implicit 4-byte
                        # vptr at 0x00. That is not a guess -- the tree already writes
                        # the model down by hand, verbatim in Animation.h and dCc_c.h:
                        #
                        #   /* 0x00 is the vptr, placed implicitly by the first
                        #      virtual declaration. */
                        #
                        # ...UNLESS the struct declares the vptr as a REAL field. The
                        # flat port-side headers do exactly that -- ArrowSignRight.h
                        # opens `void *vtable;  /* 0x000 */` -- and there the comments
                        # already count it, so starting at 4 double-counts and every
                        # field after it reads 4 bytes late. Ten-plus headers in this
                        # tree declare a literal vtable field, so this is a real fork,
                        # not a special case for one file.
                        off = _root_offset(all_lines, lineno)
                    if base is not None:
                        # A derived struct's own fields do NOT start at 0 -- the base
                        # sub-object is there. Guessing 0 would mismatch every field,
                        # so refuse unless the base states its own size.
                        if base not in SZ:
                            unknown_base = base
                            break
                        # Data size, not sizeof -- see DATA_SIZE above. Each field is
                        # aligned below, so a base whose padding is unused still puts
                        # the first own field exactly where sizeof would have.
                        off = DATA_SIZE.get(base, SZ[base])
                        derived_from = base
                continue
            if in_comment:
                if "*/" in line:
                    in_comment = False
                continue
            # An inline method BODY (`virtual ~dScMgBase_c() { ...; }`), being
            # skipped over by the methods-declared-first branch below. Track brace
            # depth across lines the same way `nested`/`in_comment` already do,
            # rather than assuming a method is always one line -- an unbalanced
            # line here would otherwise fall through to DECL parsing and get
            # reported UNPARSED against the tool's own skipped-method text.
            if await_body or skip_body:
                body_code, body_comment = _code_without_comments_or_strings(
                    line, body_comment)
                saw_open = "{" in body_code
                depth = body_code.count("{") - body_code.count("}")
                if await_body:
                    if saw_open:
                        await_body = False
                        await_body_line = None
                        skip_body = max(depth, 0)
                        continue
                    if ";" in body_code:
                        # A split declaration, not an inline definition.  A real
                        # field on this line must still reach DECL; otherwise the
                        # pending-method state would create another zero-field pass.
                        await_body = body_comment = False
                        layout_line = DECL.match(line) or re.match(r"^\s*\};", line)
                        if layout_line:
                            skipped.append(await_body_line)
                        await_body_line = None
                        if not layout_line:
                            continue
                    else:
                        # Blank/comment and signature-continuation lines are both
                        # harmless while waiting for the Allman opening brace.
                        continue
                else:
                    skip_body = max(skip_body + depth, 0)
                    continue
            # A NESTED type definition -- `struct State { ... };` inside Player -- is a
            # type, not a field: it occupies no space and advances no offset. Consume it
            # whole, before the checks below can misread it.
            #
            # Two of those checks would, and both fail silently. Its closing `};` matches
            # the outer-struct terminator on the next line, and a pointer-to-member
            # declaration (`int (Player::*mInit)();`) matches the member-function pattern
            # that ends the field list. Either way the walk stopped at the nested type,
            # and for Player.h that meant "0 commented fields ... struct spans 0xd0" --
            # the base's size, no fields checked, exit status 0. A header with 177
            # checkable fields reported exactly like a clean pass.
            if nested:
                if re.match(r"^\s*\};", line):
                    nested = False
                continue
            # A nested FORWARD declaration -- `struct State;` inside Camera -- is a type
            # too, and occupies no space either. It shows up when a class handles its
            # nested type ONLY by pointer: `Camera::ChangeState` is mangled `PNS_5StateE`,
            # which is what forces State to be nested, while nothing in the tree
            # dereferences one, so there is no layout to write down.
            #
            # DECL wants two identifiers (`struct X y;`), so a bare tag matched nothing
            # and fell through to UNPARSED -- and per the note above, ONE unparsed line
            # suppresses the mismatch check for the whole header. That is the same
            # silent-no-op shape this gate has already been bitten by twice, arriving a
            # third way; here it at least exits non-zero rather than reporting a pass.
            if re.match(r"^\s*(?:struct|union|class)\s+\w+\s*;\s*$", line):
                continue
            if re.match(r"^\s*struct \w+\s*(?::\s*(?:public\s+)?\w+\s*)?\{", line):
                nested = True
                continue
            if re.match(r"^\s*(#|\}|/\* methods)", line):
                break
            if IGNORABLE.match(line):
                if "/*" in line and "*/" not in line:
                    in_comment = True
                continue
            # A polymorphic C++ struct carries an implicit vptr at offset 0 that no
            # declaration mentions, so the running offset cannot be derived from the text.
            # Say the struct is unmodelled rather than emit a mismatch per field.
            #
            # `~Name(...)` (a bare, non-virtual destructor declaration -- Particle::
            # SysTracker in include/Stage.h is the first instance) starts with `~`,
            # which the type-name alternative below never matches (`~` is not in
            # `[A-Za-z_]`), so without this alternative it fell through to UNPARSED.
            if re.match(r"^\s*(virtual\b|~\w+\s*\([^;]*\)\s*;|[A-Za-z_][\w:<>, &*]*\([^;]*\)\s*(const)?\s*;)", line):
                # ...unless we started from a base whose size is asserted. A derived
                # class places no vptr of its own -- it inherits the base's, and the
                # base's asserted size already counts it. The running offset is sound,
                # so the member functions merely end the field list.
                # A derived class can ALSO declare its destructor/overrides FIRST
                # (dScene_c.h's KEY FUNCTION convention -- "the destructor is declared
                # first, which is safe for a derived class"). Before any real field
                # has been seen, a method line doesn't end the list, it's still the
                # header's front matter -- without this, dScMgBase_c.h (destructor +
                # 8 overrides, THEN ~30 fields) reported "0 commented fields ...
                # struct spans 0x50", the exact same silent-no-op shape as every
                # other bug this file documents, just arrived at from the opposite
                # direction. Once a field HAS been seen, a method line ends the
                # list as before -- that's the generated-header convention
                # (Stage.h: fields, then methods).
                if n == 0:
                    body_code, body_comment = _code_without_comments_or_strings(line)
                    saw_open = "{" in body_code
                    depth = body_code.count("{") - body_code.count("}")
                    if saw_open:
                        skip_body = max(depth, 0)
                    elif ";" not in body_code:
                        # `virtual ~X()` followed by an Allman `{` is an inline
                        # body too.  Raw depth on the signature is zero, so the old
                        # walk treated that `{` as a field and its `}` as the end of
                        # the outer struct, checking none of the fields below it.
                        await_body = True
                        await_body_line = f"{lineno}: {line.strip()}"
                    continue
                break
            # A declaration can carry a trailing comment that runs onto later lines:
            #     u8 unk_010;   /* 0x010 - first byte of the 0x28-byte ClsnResult
            #                              the hit is written into ... */
            # Those continuation lines are prose, not declarations. Without this they
            # were reported UNPARSED and the header failed the gate on its own comments.
            if line.count("/*") > line.count("*/"):
                in_comment = True
            m = DECL.match(line)
            typ = m.group(1).rsplit("::", 1)[-1] if m else None
            # A qualified assertion is checked FIRST and settles the question. It
            # names one class unambiguously, so neither the bare-tag lookup nor the
            # shadow refusal below has anything left to decide. `TouchIcon_c
            # mIcons[8];` in dMgPsOpt_c.h is the live case: the bare tag
            # `TouchIcon_c` is asserted nowhere, so the field came back UNPARSED and
            # the header's whole walk stopped at `struct spans 0x8`. With the
            # qualified size it walks to 0x128, which is what the header's own
            # `dMgPsOpt_c_size_must_be_0x128` says -- an independent check that the
            # size read here is the right one.
            qsize = _qualified_size(expected, typ) if (m and not m.group(2)) else None
            if qsize is not None:
                w = qsize
            elif m and not m.group(2) and _shadowed_by_nested(typ, nested_names, fp):
                w = None
            else:
                w = 4 if (m and m.group(2)) else SZ.get(typ)
            if w is None:
                # An unrecognised declaration is NOT harmless: skipping it leaves the
                # running offset short, so every later field silently "matches" at the
                # wrong place. Say so rather than quietly carrying on.
                skipped.append(f"{lineno}: {line.strip()}")
                # ...and stop claiming MISMATCH from here on. The running offset is now
                # known-wrong, so every later comparison is against a meaningless number:
                # `struct CylinderClsn base;` is unparseable, which made the very next
                # field report "comment 0x30, computed 0x000". Those are artifacts of the
                # skip, not defects in the header. UNPARSED already fails the gate, so
                # nothing is being hidden -- the difference is that what it prints is true.
                trusted = False
                continue
            _, _, name, arr, decl = m.groups()
            # Alignment is the strictest MEMBER alignment, never the type's size: a
            # 100-byte ModelAnim aligns to 4, not to 100. Aligning to size padded 0xd4
            # out to 0x12c and made every later field in the header mismatch.
            #
            # Prefer a computed alignment when we have one: learn_aggregates knows a
            # Vector3 is 4-aligned because its members are. Fall back to min(w, 4) for
            # a type known only by its `_size_must_be_` assertion -- the size alone
            # cannot give the alignment, and no member of any struct here exceeds 4.
            a = 4 if (m and m.group(2)) else ALIGN.get(typ, min(w, 4))
            if off % a:                       # compiler would insert padding here
                off += a - (off % a)
            if decl is not None and trusted:
                n += 1
                if int(decl, 16) != off:
                    # Buffered, not printed: a polymorphic struct declares its virtuals
                    # AFTER its fields, so we only learn the layout is unmodelled once
                    # every field has already been compared against an offset that is
                    # short by the implicit vptr. Printing as we went emitted a screen of
                    # MISMATCHes for a header the tool then correctly skipped.
                    pending.append(f"  MISMATCH {path}:{lineno} {name}: comment {decl}, "
                                   f"computed 0x{off:03x}")
                    bad += 1
            arr_n = 1
            for d in ARR_DIMS.findall(arr):
                arr_n *= int(d, 0)
            off += w * arr_n
        if await_body and await_body_line:
            # EOF before either `{` or a terminating `;` is malformed input.  It
            # must not collapse back to the same zero-field pass this state fixes.
            skipped.append(await_body_line)
        if unknown_base:
            # Not a pass and not a failure: a statement of what is missing. Adding
            # `typedef char X_size_must_be_0xN[sizeof(X) == 0xN ? 1 : -1];` to the
            # base's header makes this header checkable AND makes the base's own size
            # a claim the compiler enforces.
            print(f"{path}: skipped -- derives from {unknown_base}, whose size is asserted "
                  f"nowhere (add {unknown_base}_size_must_be_0x.. to its header)")
            continue
        if unmodelled:
            # not a failure: this gate is for the generated flat headers, and a
            # hand-written polymorphic one has layout the text does not determine
            print(f"{path}: skipped -- polymorphic C++ struct, implicit vptr not modelled")
            continue
        for msg in pending:
            print(msg)
        for s in skipped:
            print(f"  UNPARSED {path}:{s}")
        print(f"{path}: {n} commented fields, {bad} mismatched, "
              f"{len(skipped)} unparsed, struct spans 0x{off:x}")
        rc |= bool(bad or skipped)
    return rc


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
