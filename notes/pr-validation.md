# Full PR validation

The merge gate should answer several different questions without compressing them into
one misleading percentage:

| Metric | Meaning | Gate |
|---|---|---|
| Matched functions and bytes | A verified-looking `src/` file exists for a configured ROM function | Must not regress |
| Coverage denominator | Configured function count and code-byte universe | Must not change silently |
| Source-built functions and bytes | Code actually linked from our translation units instead of a ROM gap object | Must not regress |
| Module fidelity | Linked executable-module bytes equal retail | Stock head must pass, unless base and head have the same recorded pre-existing build failure |
| Contributor lineage | The first matcher still owns a surviving match after moves/renames | Must not change or disappear |
| Relocations | Affected source reproduces bytes and names the correct destinations | No WRONG or NO-REPRO |

`tools/rombuild.py` emits the build/fidelity artifact. `tools/validate_merge.py` compares
two committed revisions, combines the build artifacts with `pr_linkcheck` JSON, and emits
both stable JSON and the Markdown table shown on a PR.

## Private worker sequence

The compiler and ROM remain on the private worker. For each relay job:

1. Check out the requested `baseSha` and run
   `python tools/rombuild.py --profile stock --report-json build/base-rom.json`.
   The worker may cache this artifact by commit SHA.
2. Create a temporary branch at `baseSha`, merge the requested PR head SHA, and
   **commit the merge before measuring**. Attribution follows Git history; a staged but
   uncommitted move has no lineage.
3. Run `python tools/rombuild.py --profile stock --report-json build/head-rom.json`.
4. Run `python tools/pr_linkcheck.py --base <baseSha> --json build/link.json --md build/link.md`.
5. Run:

   ```
   python tools/validate_merge.py --base <baseSha> --head HEAD \
     --require-merge-commit --expected-pr-head <headSha> \
     --base-rom-report build/base-rom.json \
     --head-rom-report build/head-rom.json --link-report build/link.json \
     --out build/validate-report.json --markdown build/validate-report.md
   ```

6. Return `status`, `summary`, `details` (the existing per-file table), and the new
   `reportMarkdown` field to the relay. The public GitHub workflow remains unable to run
   PR code or access ROM material; it only renders the worker's result.

A base failure is not hidden. If base and merge fail in the same phase with the same
failure signature, the report shows a warning and permits a non-regressing PR. If a green
base becomes red, or the failure changes, validation fails.

The merge gate always uses the stock profile. `--profile mods` is a developer tool for
building intentional experiments and is never accepted as reconstruction proof.
