# Runbook: repairing references between functions

**Audience:** an agent or contributor picking this up cold. Follow it top to bottom.
**Scope:** making a reference point at the symbol the ROM actually uses.
**Not in scope:** parameter types -- see `runbook-type-reconstruction.md`.

---

## 1. The problem, in one paragraph

`tools/match.py` compares a compiled function to the ROM word by word, but every
**relocated** word is a wildcard: our object holds a placeholder where the ROM holds a
final address. So on its own the byte gate asks *"is there a call here"* and never *"a call to
what"*. (`match.py --strict-relocs` is now default-on and does check reloc
destinations against `relocs.txt` -- but the corpus was matched before that existed, so
for any file you did not personally re-verify, assume the weaker guarantee.) A file can name a callee that does not exist, or the wrong one, and still match
perfectly. The ROM link would object, but `tools/eligible.py` refuses to enroll a file
whose references do not resolve, so a broken file never reaches the link. The two gates
cover for each other.

**Consequence you must internalise:** a byte match is *not* evidence about references.
Only enrollment plus the ROM link is.

## 2. The authority

Never guess from the name. The name is what a recovery pass wrote down; it is the thing
under suspicion. The authority is dsd's analysis of the real ROM:

```
config/**/relocs.txt     from:0x021111e8 kind:load to:0x021099e4 module:overlay(2)
config/**/symbols.txt    _ZN5Actor5SpawnEjj... kind:function(arm,size=0x4c) addr:0x02010e2c
```

The join is: **compile the file -> read the relocation offsets out of the object ->
add the function's address -> look up `from:` -> take `to:` and `module:` -> ask that
module's `symbols.txt` for the name.**

`tools/resolve_placeholders.py` implements exactly this. Do not reimplement it.

## 2a. What "verified" means, and what it does not

`tools/build_pin.py` is the single answer to *what will the build do with this
file*: the compiler version `rombuild` pins for it (`config/rombuild-versions.txt`,
keyed by file **stem**), the build's flags, a byte compare, and a
reloc-destination check. Every rewrite tool verifies through it, and it **fails
closed** -- an unknown pin or a check that could not run is a failure, not a pass.

Both halves are load-bearing, and each was a real false green:

- **Pinned, not swept.** A match under any of the 25 versions in `match.SWEEP` is
  not a match the build will reproduce. `config/rombuild-versions.txt` names the
  files the build has to compile with something other than the default `2004/b56` --
  8 entries at the time of writing; run `python tools/build_pin.py` for the current
  set. (An earlier revision here claimed "an audit found 79 files"; that number
  appears nowhere else in the tree and nothing supports it. If such an audit
  happened, cite it.)
- **Destinations, not just bytes.** `match.compare` wildcards every relocated
  word, so a byte-only check is blind by construction to the one thing these tools
  change. A transform once passed the byte check and then failed the ROM build by
  a single word -- and that word turned out to be a pre-existing wrong callee the
  transform had merely exposed.

## 3. Preconditions

**Cold start:** `tools/mwccarm/**` and `extracted/**` are git-ignored, so a fresh clone
has neither the 25 compilers nor the extracted ROM, and step 1 fails immediately. See
`notes/rom-build.md` first.

```sh
cd <worktree>
git status --porcelain          # must be clean; a dirty tree poisons the report
python tools/eligible.py        # ~5 min, compiles ~11,100 files
```

`eligible.py` writes `build/rombuild-eligibility.json`, stamped with the commit it
describes. Everything below reads it. **If you change `src/`, it is stale -- re-run it.**
A stale report is the single easiest way to produce confident nonsense; it has already
happened twice.

## 4. The loop

```sh
python tools/resolve_placeholders.py                # report only, writes nothing
python tools/resolve_placeholders.py --apply -j 16  # rewrite + verify + revert on fail
```

Read the outcome table before doing anything else. Each outcome means something
different and only one of them is a fix:

| outcome | meaning | action |
|---|---|---|
| `rewritten` | renamed, and it still reproduces the ROM with correct reloc destinations | keep |
| `partial` | some names in the file are not textual; fixing only the rest leaves it broken | needs a source transform, not a rename |
| `not textual` | the name is compiler-generated (a member call, or double-mangled) so there is no token to rewrite | run `tools/demember_calls.py` -- see 4a |
| `target unnamed in config` | the address resolves, but only to `func_0208xxxx` | that is a `symbols.txt` gap; name it there first |
| `unresolved` | the join did not produce one answer | read the printed reason -- see below |
| `reverted` | the rename stopped it matching | investigate; usually a type clash with a shared header |

`unresolved` covers several distinct reasons; the tool prints which, and they are not
interchangeable:

- `0x... names {ov000: ..., ov004: ...} (residency leaves [...])` -- two overlays both
  define it *and* both could have been in memory. The tool has already applied the
  residency map (`tools/overlay_residency.py`); `residency leaves` is what survived
  it. **Stop**; see `notes/overlay-residency.md` §9 -- there are ten of these left and
  each needs its own argument.
- `no reloc recorded at 0x...` -- the value is not a relocation at all (an immediate).
- `no candidate of [...] defines 0x...` -- a gap in `symbols.txt`, not in the source.
- `resolves to both X and Y` -- one name used for two different targets in one file;
  split it by hand.
- `unparsed module spec` -- a `module:` form the tool does not know; report it.

Report-only mode additionally prints `resolvable` for anything it *would* rewrite, and
`compile failed` / `function not in object` if it could not get that far.

**Never pass `--allow-generic` to make a number go up.** It trades a wrong name for an
anonymous one and hides the config gap that caused it.

## 4a. The `not textual` bucket has its own tool

A file often declares an ad-hoc local class purely so it can spell a call:

    struct Actor { short ReflectAngle(int, int, short); };
    ((Actor*)self)->ReflectAngle(a, b, c);

The compiler builds the mangled name from that declaration, so the name never
appears in the source and no rename can reach it. Correcting the declaration is
*not* the fix either: the ROM's parameter is `Fix12<int>`, a class, and mwccarm
passes a by-value class differently **at the call site** --

    take_i(h, 0x800)   ->  mov r1,#0x800
    take_f(h, v)       ->  ldr r1,[pc,#..] ; ldm r1,{r1}

-- so the true signature breaks the caller's bytes.

`tools/demember_calls.py` rewrites the call to name the ROM symbol directly as an
`extern "C"` free function, keeping the *original* declaration's types verbatim.
Those types are wrong about the ROM and right about the codegen, and sub-word
types are not interchangeable with `int`: a `short` parameter truncates where an
`int` sign-extends.

```sh
python tools/demember_calls.py                # report only
python tools/demember_calls.py --apply -j 16
```

## 5. Verification, in order

```sh
python tools/eligible.py                                   # refresh the report
python tools/enroll.py --complete-list build/eligible-names.txt
python tools/rombuild.py                                   # the real gate
python tools/prepush_attribution.py                        # credit must not move
python tools/check_references.py                           # must not regress
```

The only line that matters in the build output:

```
module fidelity: 106/106 exact, 100.000000% of compared bytes
ROM-build analysis: PASS
```

Anything less than **106/106 and PASS** means a reference now points somewhere wrong.
That is the gate doing its job -- read the named function, do not retry blindly.

## 6. Reporting: the evidence split is mandatory

Two different strengths of evidence, and conflating them is the most common way these
changes get oversold:

- **enrolled** -> the ROM link compared the resolved symbol against the real ROM word
- **byte-verified only** -> still blocked for another reason; byte matching is blind to
  relocation targets, i.e. blind to precisely what you changed

Count both and state both. Never write "0 reverted" as though it were proof.

```sh
python - <<'PY'
import json, pathlib, subprocess, collections, sys
sys.path.insert(0,"tools"); import eligible as E
rs = {r["file"].replace("\\","/"): r["reason"] for r in E.load_report()[0]}
ch = subprocess.run(["git","diff","--name-only","origin/main","--","src"],
                    capture_output=True, text=True).stdout.split()
c = collections.Counter("enrolled (link-verified)" if rs.get(f) is None
                        else "byte-verified only" for f in ch)
print(len(ch), "changed:", dict(c))
PY
```

## 7. Shipping

- Validator caps **200 in-scope files** across `src include config mods
  attribution.json`. Enforced server-side, so nothing in-repo will warn you -- count
  before pushing and split if over.
- Cut every batch **from `main`**, never stacked on the previous one -- a PR diff is
  measured against main, so stacking defeats the cap.
- `config/**/delinks.txt` is generated. If it conflicts, regenerate -- take `main`'s
  copy, then re-run `python tools/eligible.py && python tools/enroll.py
  --complete-list build/eligible-names.txt`. Never hand-merge.
- The pre-push hook (installed via `core.hooksPath` -> `tools/hooks/`, not copied
  into `.git/hooks/`) runs `port_refcheck.py` and the reference ratchet on every
  push, plus link-verification and attribution when the target is `main`.
  A rename in `src/` can strand a
  hand-written bridge in `port/`, which nothing else catches.

## 8. Definition of done

- [ ] `ROM-build analysis: PASS`, module fidelity 106/106 exact
- [ ] attribution 0 changed, 0 lost
- [ ] `check_references.py` does not regress; run `--update` to bank progress
- [ ] enrolled vs byte-only split stated in the PR body
- [ ] in-scope file count under 200
- [ ] no `--allow-generic`, no hand-edited delinks

## 9. Known dead ends

- **Renaming a member call.** Still true that a rename cannot reach it -- but this is
  no longer a dead end; see 4a and `tools/demember_calls.py`.
- **Expect the gate to find bugs that are not yours.** Twice now a tool's output
  failed the ROM build and the tool was innocent: enrolling the file was simply the
  first time anything looked at its callees. Read the named function before assuming
  your change caused it.
- **~~169 references remain genuinely ambiguous.~~** Was true; the overlay-residency
  data existed after all, in the game's own loader. 127 of the 137 real cases are
  settled -- `notes/overlay-residency.md`. Ten are left and they are listed there.
  The lesson worth keeping: "tooling is finished" meant "we asked the symbol table
  and it had no more to say", which is not the same thing.
- **`is_misnamed()` returns `True` unconditionally.** That is deliberate: the name is
  never evidence, so there is nothing to gate on. Narrowing it is how this cost five
  separate sweeps.
