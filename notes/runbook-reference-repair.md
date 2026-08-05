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
config/**/symbols.txt    _ZN5Actor5SpawnEjj... kind:function(arm,size=0x54) addr:0x...
```

The join is: **compile the file -> read the relocation offsets out of the object ->
add the function's address -> look up `from:` -> take `to:` and `module:` -> ask that
module's `symbols.txt` for the name.**

`tools/resolve_placeholders.py` implements exactly this. Do not reimplement it.

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
| `not textual` | the name is compiler-generated (a member call, or double-mangled) so there is no token to rewrite | needs a source transform |
| `target unnamed in config` | the address resolves, but only to `func_0208xxxx` | that is a `symbols.txt` gap; name it there first |
| `unresolved` | the join did not produce one answer | read the printed reason -- see below |
| `reverted` | the rename stopped it matching | investigate; usually a type clash with a shared header |

`unresolved` covers several distinct reasons; the tool prints which, and they are not
interchangeable:

- `0x... names {ov000: ..., ov004: ...}` -- two overlays both define it. **Stop**; see
  `overlay-ambiguous-references.md`.
- `no reloc recorded at 0x...` -- the value is not a relocation at all (an immediate).
- `no candidate of [...] defines 0x...` -- a gap in `symbols.txt`, not in the source.
- `resolves to both X and Y` -- one name used for two different targets in one file;
  split it by hand.
- `unparsed module spec` -- a `module:` form the tool does not know; report it.

Report-only mode additionally prints `resolvable` for anything it *would* rewrite, and
`compile failed` / `function not in object` if it could not get that far.

**Never pass `--allow-generic` to make a number go up.** It trades a wrong name for an
anonymous one and hides the config gap that caused it.

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
- The pre-push hook runs `port_refcheck.py`. A rename in `src/` can strand a
  hand-written bridge in `port/`, which nothing else catches.

## 8. Definition of done

- [ ] `ROM-build analysis: PASS`, module fidelity 106/106 exact
- [ ] attribution 0 changed, 0 lost
- [ ] `check_references.py` does not regress; run `--update` to bank progress
- [ ] enrolled vs byte-only split stated in the PR body
- [ ] in-scope file count under 200
- [ ] no `--allow-generic`, no hand-edited delinks

## 9. Known dead ends

- **Renaming a member call.** `anim->SetAnim(...)` generates its mangled name from the
  declaration; there is no token to rewrite. It needs the call rewritten to an
  `extern "C"` mangled free call with scalar args -- the dominant pattern in the corpus,
  though I have no exact count and any figure you see quoted for it is unverified. This
  is the `not textual` bucket.
- **169 references remain genuinely ambiguous.** Two overlays define the same address.
  Tooling is finished; these need overlay-residency data or an emulator trace.
- **`is_misnamed()` returns `True` unconditionally.** That is deliberate: the name is
  never evidence, so there is nothing to gate on. Narrowing it is how this cost five
  separate sweeps.
