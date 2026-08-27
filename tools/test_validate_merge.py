"""PR reporting is revision-scoped so committed R100 moves preserve credit."""
import json
import os
import pathlib
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import validate_merge as VM  # noqa: E402


def git(repo, *args, env=None):
    merged_env = dict(os.environ)
    if env:
        merged_env.update(env)
    return subprocess.run(["git", "-C", str(repo), *args], check=True,
                          capture_output=True, text=True, env=merged_env).stdout.strip()


def commit(repo, message, login):
    git(repo, "add", "-A")
    git(repo, "-c", f"user.name={login}",
        "-c", f"user.email={login}@users.noreply.github.com",
        "commit", "-qm", message)
    return git(repo, "rev-parse", "HEAD")


class ValidateMerge(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = pathlib.Path(self.tmp.name)
        git(self.repo, "init", "-q", ".")
        (self.repo / "src").mkdir()
        config = self.repo / "config" / "arm9"
        config.mkdir(parents=True)
        (config / "symbols.txt").write_text(
            "Example kind:function(arm,size=0x4) addr:0x02000000\n",
            encoding="utf-8")
        (config / "delinks.txt").write_text(
            "    .text start:0x02000000 end:0x02000004 kind:code\n\n"
            "src/Example.c:\n"
            "    complete\n"
            "    .text start:0x02000000 end:0x02000004\n",
            encoding="utf-8")
        (self.repo / "src" / "Example.c").write_text("int Example(void) { return 0; }\n")
        self.base = commit(self.repo, "base", "alice")
        self.base_branch = git(self.repo, "branch", "--show-current")
        self.old_repo = VM.REPO
        VM.REPO = self.repo

    def tearDown(self):
        VM.REPO = self.old_repo
        self.tmp.cleanup()

    def test_committed_perfect_move_preserves_counts_and_credit(self):
        (self.repo / "src" / "nested").mkdir()
        git(self.repo, "mv", "src/Example.c", "src/nested/Example.c")
        config = self.repo / "config" / "arm9" / "delinks.txt"
        config.write_text(config.read_text(encoding="utf-8").replace(
            "src/Example.c:", "src/nested/Example.c:"), encoding="utf-8")
        head = commit(self.repo, "move", "bob")
        report = VM.build_report(self.base, head)
        self.assertEqual(report["status"], "Passed")
        self.assertEqual(report["coverage"]["delta"]["matchedFunctions"], 0)
        self.assertEqual(len(report["diff"]["perfectRenames"]), 1)
        self.assertEqual(report["attribution"]["base"], report["attribution"]["head"])
        self.assertEqual(report["attribution"]["added"], [])
        self.assertEqual(report["attribution"]["changed"], [])
        self.assertEqual(report["attribution"]["lost"], [])
        self.assertEqual(report["attribution"]["head"]["alice"]["functions"], 1)

    def test_nonmatching_is_read_from_requested_revision(self):
        (self.repo / "src" / "Example.c").write_text(
            "// NONMATCHING\nint Example(void) { return 0; }\n")
        head = commit(self.repo, "mark draft", "bob")
        self.assertEqual(VM.function_snapshot(self.base)["stats"]["matchedFunctions"], 1)
        self.assertEqual(VM.function_snapshot(head)["stats"]["matchedFunctions"], 0)

    def test_marker_deep_in_the_leading_comment_block_still_counts_as_draft(self):
        # The rule is the header REGION, not a byte window: the recovery prose above a
        # banner grows without bound and must not push the marker out of sight.
        prose = "".join(f"// recovery note line {i}: tried and rejected\n"
                        for i in range(12))
        (self.repo / "src" / "Example.c").write_text(
            prose + "// NONMATCHING: register allocation differs\n"
            "int Example(void) { return 0; }\n", encoding="utf-8")
        head = commit(self.repo, "draft with a long preamble", "bob")
        self.assertEqual(VM.function_snapshot(head)["stats"]["matchedFunctions"], 0)

    def test_a_stranded_nonmatching_marker_fails_the_gate(self):
        # A marker below the first line of code is invisible to every counter, so the
        # file would publish as MATCHED against its author's own written claim. The
        # gate names the file instead of letting that inversion land.
        (self.repo / "src" / "Example.c").write_text(
            "int Example(void) { return 0; }\n"
            "// NONMATCHING: register allocation differs\n", encoding="utf-8")
        head = commit(self.repo, "stranded marker", "bob")
        # The misread the gate exists for: the counters still see a match.
        self.assertEqual(VM.function_snapshot(head)["stats"]["matchedFunctions"], 1)
        report = VM.build_report(self.base, head)
        self.assertEqual(report["status"], "Failed")
        self.assertEqual(report["asmPolicy"]["strandedMarkers"], ["src/Example.c"])
        self.assertTrue(any("outside the leading comment block" in r
                            for r in report["reasons"]))

    def test_require_merge_commit_rejects_an_uncommitted_merge_shape(self):
        with self.assertRaisesRegex(RuntimeError, "not a committed merge"):
            VM.build_report(self.base, "HEAD", require_merge_commit=True)

    def test_pr_linkcheck_grouped_json_is_flattened(self):
        state = VM._link_state([{
            "file": "src/Example.c",
            "results": [{"sym": "Example", "verdict": "VERIFIED"},
                        {"sym": "Thunk", "verdict": "BLIND-RELOC"}],
        }])
        self.assertEqual(state["checked"], 2)
        self.assertEqual(state["tally"], {"VERIFIED": 1, "BLIND": 1})
        self.assertEqual(state["blocking"], [])

    def test_declared_draft_no_repro_does_not_block(self):
        # pr_linkcheck marks a self-declared NONMATCHING draft worst=DRAFT while
        # its per-slot results still read NO-REPRO (#968). Flattening must keep
        # the downgrade, or every PR that so much as renames a declared draft
        # fails on a non-reproduction the gate already excused.
        state = VM._link_state([{
            "file": "src/_ZN5Model27LoadCompressedTextureToVramEPcjS0_.cpp",
            "worst": "DRAFT",
            "results": [{"sym": "_ZN5Model27LoadCompressedTextureToVramEPcjS0_",
                         "verdict": "NO-REPRO"}],
        }])
        self.assertEqual(state["checked"], 1)
        self.assertEqual(state["tally"], {"DRAFT": 1})
        self.assertEqual(state["blocking"], [])

    def test_draft_downgrade_does_not_shield_wrong_dest(self):
        # The draft excuse covers non-reproduction only. A WRONG verdict on a
        # declared draft still blocks: its call graph must be honest even if
        # its bytes differ.
        state = VM._link_state([{
            "file": "src/Draft.c",
            "worst": "DRAFT",
            "results": [{"sym": "Draft", "verdict": "WRONG"}],
        }])
        self.assertEqual(state["tally"], {"WRONG": 1})
        self.assertEqual(len(state["blocking"]), 1)

    def test_port_refcheck_absent_is_neutral_and_stale_refs_are_named(self):
        # The phase is optional: a worker that never ran it, or a PR that
        # touches no source, must read exactly like a clean run -- no row, no
        # reason. When it does fail, the reason has to name the stale
        # reference, because the PR author never sees the worker's log.
        neutral = VM.build_report(self.base, "HEAD")
        self.assertEqual(neutral["portRefcheck"], {"available": False})
        self.assertNotIn("Port reference check", neutral["reportMarkdown"])

        stale = {"schemaVersion": 1, "ok": False, "checked": 395, "failed": 1,
                 "failures": [{"check": "manifests", "file": "port/slice_gate8.txt",
                               "line": 6, "message": "src/Example.c not found (renamed?)"}]}
        report = VM.build_report(self.base, "HEAD", port_report=stale)
        self.assertEqual(report["status"], "Failed")
        self.assertIn("port/slice_gate8.txt:6: src/Example.c not found (renamed?)",
                      "; ".join(report["reasons"]))
        self.assertIn("| Port reference check | 395 checked; 1 stale |",
                      report["reportMarkdown"])

    def test_port_refcheck_report_without_the_flat_failure_list_still_reads(self):
        # Tolerate a report written by an older port_refcheck that only groups
        # failures per check, so a base/worker version skew cannot make a real
        # failure look like a pass.
        grouped = {"ok": False, "checked": 2, "checks": {
            "cmake-symbols": {"checked": 2, "failures": [
                {"file": "port/CMakeLists.txt", "line": 9,
                 "message": "symbol 'Example' has no src/Example.c"}]}}}
        state = VM._port_state(grouped)
        self.assertFalse(state["passed"])
        self.assertEqual(state["failures"][0]["check"], "cmake-symbols")
        self.assertTrue(VM._port_state({"ok": True, "checked": 2, "failures": []})["passed"])

    def test_committed_test_merge_is_accepted_and_keeps_feature_author_credit(self):
        git(self.repo, "switch", "-q", "-c", "feature")
        (self.repo / "src" / "nested").mkdir()
        git(self.repo, "mv", "src/Example.c", "src/nested/Example.c")
        config = self.repo / "config" / "arm9" / "delinks.txt"
        config.write_text(config.read_text(encoding="utf-8").replace(
            "src/Example.c:", "src/nested/Example.c:"), encoding="utf-8")
        feature = commit(self.repo, "feature move", "bob")
        git(self.repo, "switch", "-q", self.base_branch)
        git(self.repo, "-c", "user.name=maintainer",
            "-c", "user.email=maintainer@users.noreply.github.com",
            "merge", "--no-ff", "-qm", "test merge", "feature")
        report = VM.build_report(
            self.base, "HEAD", require_merge_commit=True, expected_pr_head=feature)
        self.assertTrue(report["committedMerge"])
        self.assertEqual(report["status"], "Passed")
        self.assertEqual(report["attribution"]["head"]["alice"]["functions"], 1)
        with self.assertRaisesRegex(RuntimeError, "second parent"):
            VM.build_report(
                self.base, "HEAD", require_merge_commit=True,
                expected_pr_head=self.base)

    def test_a_credit_change_warns_without_failing(self):
        # Reassignment alone -- both a before and an after contributor -- is never a
        # blocker: a rebase or resolving someone else's merge conflict relabels a
        # function's "last touched by" without losing anything, and this project spends
        # no tokens defending credit. It is still named, in the warning and the table,
        # so a PR author can see which files moved without that costing the merge.
        (self.repo / "attribution.json").write_text(
            '{"overrides": {"src/Example.c": "bob"}}\n', encoding="utf-8")
        commit(self.repo, "pin credit", "maintainer")
        report = VM.build_report(self.base, "HEAD")
        self.assertEqual(report["status"], "Passed")
        self.assertEqual(report["reasons"], [])
        self.assertIn("src/Example.c: alice -> bob", "; ".join(report["warnings"]))
        self.assertEqual(report["attribution"]["changed"][0]["path"], "src/Example.c")
        self.assertIn("| `arm9:0x02000000` | `src/Example.c` | alice | bob |",
                      report["reportMarkdown"])

    def test_attribution_override_is_not_needed_for_a_pure_change(self):
        # The override label exists for a real LOSS. A pure reassignment already passes
        # without it, and passing the label besides changes nothing about that.
        (self.repo / "attribution.json").write_text(
            '{"overrides": {"src/Example.c": "bob"}}\n', encoding="utf-8")
        commit(self.repo, "pin credit", "maintainer")
        report = VM.build_report(self.base, "HEAD", allow_attribution_change=True)
        self.assertEqual(report["status"], "Passed")
        self.assertEqual(report["reasons"], [])
        self.assertIn("src/Example.c: alice -> bob", "; ".join(report["warnings"]))
        self.assertNotIn("(override label)", report["reportMarkdown"])
        self.assertEqual(len(report["attribution"]["changed"]), 1)

    def test_a_wholesale_credit_move_stays_within_the_relay_summary_limit(self):
        # The relay rejects a result whose summary runs past 500 characters, and a
        # rejected result is a job that never reports at all. Naming files is worth
        # length in the warning and the check body; a repin sweep must still fit in the
        # reply that carries the verdict.
        symbols = self.repo / "config" / "arm9" / "symbols.txt"
        text = symbols.read_text(encoding="utf-8")
        overrides = {}
        for i in range(40):
            name = f"ExtremelyLongMangledFunctionName{i:02d}"
            text += f"{name} kind:function(arm,size=0x4) addr:0x{0x02000004 + i * 4:08x}\n"
            (self.repo / "src" / f"{name}.c").write_text(
                f"int {name}(void) {{ return 0; }}\n")
            overrides[f"src/{name}.c"] = "bob"
        symbols.write_text(text, encoding="utf-8")
        base = commit(self.repo, "more functions", "alice")
        (self.repo / "attribution.json").write_text(
            json.dumps({"overrides": overrides}), encoding="utf-8")
        commit(self.repo, "repin them all", "maintainer")

        report = VM.build_report(base, "HEAD")
        self.assertEqual(report["status"], "Passed")
        self.assertEqual(len(report["attribution"]["changed"]), 40)
        self.assertLessEqual(len(report["summary"]), VM.SUMMARY_LIMIT)
        self.assertIn("+37 more", "; ".join(report["warnings"]))
        self.assertIn("+15 more", report["reportMarkdown"])

    def test_override_does_not_excuse_a_non_attribution_failure(self):
        # Scoped to attribution only: a label about credit must not wave through a
        # PR that actually lost matched code.
        (self.repo / "src" / "Example.c").write_text(
            "// NONMATCHING\nint Example(void) { return 0; }\n")
        (self.repo / "attribution.json").write_text(
            '{"overrides": {"src/Example.c": "bob"}}\n', encoding="utf-8")
        commit(self.repo, "unmatch and repin", "maintainer")
        report = VM.build_report(self.base, "HEAD", allow_attribution_change=True)
        self.assertEqual(report["status"], "Failed")
        self.assertIn("lost 1 matched function(s)", report["reasons"])

    def test_identical_base_failure_is_warning_but_changed_failure_blocks(self):
        base_failure = {"status": "error", "failure": {
            "phase": "mwldarm", "returncode": 1, "output": "undefined Existing"}}
        same = VM.build_report(self.base, "HEAD", base_failure, base_failure)
        self.assertEqual(same["status"], "Passed")
        self.assertTrue(same["rom"]["sameBaselineFailure"])
        changed_failure = {"status": "error", "failure": {
            "phase": "mwldarm", "returncode": 1, "output": "undefined New"}}
        changed = VM.build_report(self.base, "HEAD", base_failure, changed_failure)
        self.assertEqual(changed["status"], "Failed")
        self.assertIn("full-ROM validation failed", changed["reasons"])

    def test_function_universe_change_is_not_hidden_as_percentage_progress(self):
        symbols = self.repo / "config" / "arm9" / "symbols.txt"
        symbols.write_text(
            symbols.read_text(encoding="utf-8")
            + "Other kind:function(arm,size=0x4) addr:0x02000004\n",
            encoding="utf-8")
        head = commit(self.repo, "change denominator", "bob")
        report = VM.build_report(self.base, head)
        self.assertEqual(report["status"], "Failed")
        self.assertIn("function/byte coverage denominator changed", report["reasons"])

    def _declare_symbol(self, name):
        """Add ``name`` to the base symbol universe (no source yet) and commit,
        so a later src/ addition changes neither denominator nor matched set."""
        symbols = self.repo / "config" / "arm9" / "symbols.txt"
        symbols.write_text(
            symbols.read_text(encoding="utf-8")
            + f"{name} kind:function(arm,size=0x4) addr:0x02000004\n",
            encoding="utf-8")
        return commit(self.repo, f"declare {name}", "alice")

    def test_unbannered_dcd_transcription_is_demoted_and_fails_the_pr(self):
        # A dcd dump byte-matches vacuously (it IS the ROM words re-spelled), so
        # without a banner it must not count as matched at any revision, and a PR
        # that adds one must fail with a reason that names the file.
        base = self._declare_symbol("Raw")
        (self.repo / "src" / "Raw.c").write_text(
            "asm void Raw(void) {\n    dcd 0xe12fff1e\n}\n", encoding="utf-8")
        head = commit(self.repo, "transcribe Raw", "bob")
        snap = VM.function_snapshot(head)
        self.assertEqual(snap["stats"]["matchedFunctions"], 1)  # Example only
        self.assertFalse(snap["functions"]["arm9:0x02000004"]["matched"])
        report = VM.build_report(base, head)
        self.assertEqual(report["status"], "Failed")
        self.assertIn("src/Raw.c", "; ".join(report["reasons"]))
        self.assertTrue(any("asm-transcription" in r for r in report["reasons"]))
        self.assertEqual(report["asmPolicy"]["transcribed"], ["src/Raw.c"])

    def test_nonmatching_bannered_dcd_stays_on_the_draft_path(self):
        # The same dcd body under a NONMATCHING banner is an honest draft: not
        # matched (the established draft rule), and NOT a transcription failure.
        base = self._declare_symbol("Raw")
        (self.repo / "src" / "Raw.c").write_text(
            "// NONMATCHING\nasm void Raw(void) {\n    dcd 0xe12fff1e\n}\n",
            encoding="utf-8")
        head = commit(self.repo, "draft Raw", "bob")
        snap = VM.function_snapshot(head)
        self.assertEqual(snap["stats"]["matchedFunctions"], 1)
        report = VM.build_report(base, head)
        self.assertEqual(report["status"], "Passed")
        self.assertEqual(report["asmPolicy"]["transcribed"], [])

    def test_hand_asm_primitive_banner_keeps_an_asm_body_matched(self):
        # The other legitimate tier: the original really was assembly, the banner
        # says so, and the function counts as matched with no gate reason.
        base = self._declare_symbol("Prim")
        (self.repo / "src" / "Prim.c").write_text(
            "// HAND-ASM PRIMITIVE: byte-faithful asm-block match (CPSR read).\n"
            "asm void Prim(void) {\n    mrs r0, cpsr\n    bx lr\n}\n",
            encoding="utf-8")
        head = commit(self.repo, "match Prim", "bob")
        snap = VM.function_snapshot(head)
        self.assertEqual(snap["stats"]["matchedFunctions"], 2)
        self.assertTrue(snap["functions"]["arm9:0x02000004"]["matched"])
        report = VM.build_report(base, head)
        self.assertEqual(report["status"], "Passed")
        self.assertEqual(report["asmPolicy"], {"transcribed": [], "unbanneredAsm": [],
                                               "strandedMarkers": []})

    def _claim_without_enrolling(self, name):
        """A symbol with a src/ file and NO `complete` delinks entry: matched by the
        filename test, byte-verified by nothing. The population policy D subtracts."""
        self._declare_symbol(name)
        (self.repo / "src" / f"{name}.c").write_text(
            f"int {name}(void) {{ return 0; }}\n", encoding="utf-8")
        return commit(self.repo, f"claim {name}", "bob")

    def test_withdrawing_an_unverified_claim_warns_instead_of_blocking(self):
        # Bannering a file that compiles to the wrong length is how this repo says
        # "not a match". When every exit from `matched` counted as loss, that edit
        # was the one thing a PR could not do -- so the gate rewarded leaving the
        # false claim in the tree. The file is still there under the same name and
        # was never byte-verified, so nothing was lost; a claim was retracted.
        base = self._claim_without_enrolling("Claimed")
        self.assertTrue(
            VM.function_snapshot(base)["functions"]["arm9:0x02000004"]["matched"])
        (self.repo / "src" / "Claimed.c").write_text(
            "// NONMATCHING -- compiles to 0x14 against the ROM's 0x10.\n"
            "int Claimed(void) { return 0; }\n", encoding="utf-8")
        head = commit(self.repo, "banner Claimed", "bob")

        # Withdrawing also hands the claim's contributor credit back to nobody, which
        # is its own reason and keeps its own label. That is the real shape of such a
        # PR: attribution-override for the credit, and this rule for the count. Assert
        # the two separately so neither can be mistaken for the other.
        unlabelled = VM.build_report(base, head)
        self.assertEqual(unlabelled["status"], "Failed")
        self.assertEqual([r.split(" (")[0] for r in unlabelled["reasons"]],
                         ["contributor attribution was lost"])

        report = VM.build_report(base, head, allow_attribution_change=True)
        self.assertEqual(report["status"], "Passed")
        self.assertEqual(report["coverage"]["delta"]["withdrawnMatchedFunctions"], 1)
        self.assertEqual(report["coverage"]["delta"]["lostMatchedFunctions"], 0)
        # The total is deliberately unchanged, so an existing reader of this field
        # still sees everything that left the set.
        self.assertEqual(report["coverage"]["delta"]["removedMatchedFunctions"], 1)
        self.assertEqual([w["path"] for w in report["matchedWithdrawn"]],
                         ["src/Claimed.c"])
        self.assertTrue(any("withdrawn" in w for w in report["warnings"]))
        self.assertFalse(any("lost" in x and "matched" in x
                             for x in report["reasons"]))
        self.assertIn("Claims withdrawn", VM.render_markdown(report))

    def test_withdrawing_a_byte_verified_match_still_blocks(self):
        # The restriction that makes the rule safe. Example.c carries `complete`, so
        # the ROM build compiles it and compares it to the cartridge -- retracting
        # that claim is a real coverage loss, not a correction.
        (self.repo / "src" / "Example.c").write_text(
            "// NONMATCHING\nint Example(void) { return 0; }\n", encoding="utf-8")
        head = commit(self.repo, "banner a verified match", "bob")
        report = VM.build_report(self.base, head)
        self.assertEqual(report["status"], "Failed")
        self.assertEqual(report["coverage"]["delta"]["withdrawnMatchedFunctions"], 0)
        self.assertEqual(report["coverage"]["delta"]["lostMatchedFunctions"], 1)
        self.assertTrue(any("lost 1 matched function" in x for x in report["reasons"]))

    def test_deleting_a_claimed_source_is_loss_not_withdrawal(self):
        # No banner, no file: the tree really did shed something. `matched` falls the
        # same way it does for a withdrawal, and only the surviving-file test tells
        # the two apart.
        base = self._claim_without_enrolling("Claimed")
        (self.repo / "src" / "Claimed.c").unlink()
        head = commit(self.repo, "delete Claimed", "bob")
        report = VM.build_report(base, head)
        self.assertEqual(report["status"], "Failed")
        self.assertEqual(report["coverage"]["delta"]["withdrawnMatchedFunctions"], 0)
        self.assertTrue(any("lost 1 matched function" in x for x in report["reasons"]))

    def test_replacing_a_claim_with_a_transcription_is_not_a_withdrawal(self):
        # A dcd dump is not a retraction, it is a vacuous match wearing a banner's
        # clothes: the file survives under the same name, so only the transcription
        # test keeps it out of the withdrawn set.
        base = self._claim_without_enrolling("Claimed")
        (self.repo / "src" / "Claimed.c").write_text(
            "asm void Claimed(void) {\n    dcd 0xe12fff1e\n}\n", encoding="utf-8")
        head = commit(self.repo, "transcribe Claimed", "bob")
        report = VM.build_report(base, head)
        self.assertEqual(report["status"], "Failed")
        self.assertEqual(report["coverage"]["delta"]["withdrawnMatchedFunctions"], 0)
        self.assertTrue(any("lost 1 matched function" in x for x in report["reasons"]))

    def test_raw_asm_group_verdict_blocks_and_overrides_vacuous_slots(self):
        # pr_linkcheck stamps RAW-ASM on the group row while the transcription's
        # per-slot results read VERIFIED (vacuously -- the dcd words ARE the ROM
        # bytes). Flattening must carry the override or those slots launder it.
        state = VM._link_state([{
            "file": "src/Raw.c",
            "worst": "RAW-ASM",
            "results": [{"sym": "Raw", "verdict": "VERIFIED"}],
        }])
        self.assertEqual(state["tally"], {"RAW-ASM": 1})
        self.assertEqual(len(state["blocking"]), 1)


class ModuleFidelityDetail(unittest.TestCase):
    """`105/106` on its own is unactionable -- name the module and the function.

    The worker already ships both (`moduleFidelity.results` and `failures`); the
    summary table used to drop them, so a few wrong bytes in three million came
    with no way to find them while every per-file check on the PR stayed green.
    """

    def detail(self, **analysis):
        analysis.setdefault("moduleFidelity", {"results": []})
        analysis.setdefault("failures", [])
        return "\n".join(VM._module_fidelity_detail({"analysis": analysis}))

    def test_exact_build_renders_nothing(self):
        self.assertEqual(self.detail(
            moduleFidelity={"results": [{"module": "ov013", "exact": True}]}), "")

    def test_missing_analysis_renders_nothing(self):
        self.assertEqual(VM._module_fidelity_detail({"available": False}), [])
        self.assertEqual(VM._module_fidelity_detail(None), [])

    def test_names_the_module_and_the_function(self):
        out = self.detail(
            moduleFidelity={"results": [
                {"module": "ov013", "exact": False,
                 "differingBytes": 8, "comparedBytes": 4320},
                {"module": "arm9", "exact": True, "differingBytes": 0},
            ]},
            failures=[{"module": "ov013", "name": "_ZN21ClockPaintingPendulum8BehaviorEv",
                       "addr": 0x021112a8, "size": 0x94, "differingBytes": 8}])
        self.assertIn("`ov013`", out)
        self.assertIn("_ZN21ClockPaintingPendulum8BehaviorEv", out)
        self.assertIn("0x021112a8", out)
        self.assertNotIn("`arm9`", out)          # exact modules stay out of the way

    def test_module_differing_with_no_failing_function_says_so(self):
        # Bytes outside every delink range -- padding, alignment, unreached data.
        # Nothing per-function can ever surface these, so the absence is the news.
        out = self.detail(moduleFidelity={"results": [
            {"module": "arm9", "exact": False,
             "differingBytes": 8, "comparedBytes": 382212}]})
        self.assertIn("No enrolled function range accounts", out)

    def test_a_mismatching_pinned_function_is_flagged_as_a_pin_failure(self):
        out = self.detail(
            moduleFidelity={"results": [{"module": "ov013", "exact": False,
                                         "differingBytes": 8, "comparedBytes": 4320}]},
            failures=[{"module": "ov013", "name": "Behavior",
                       "addr": 0x021112a8, "size": 0x94, "differingBytes": 8}],
            alternateToolchain={"applied": {"Behavior": "1.2/base"}})
        self.assertIn("did not apply its pin", out)
        self.assertIn("1.2/base", out)

    def test_a_failure_with_a_reason_instead_of_a_count_still_renders(self):
        out = self.detail(
            moduleFidelity={"results": [{"module": "ov013", "exact": False,
                                         "differingBytes": 4, "comparedBytes": 4320}]},
            failures=[{"module": "ov013", "name": "Odd", "addr": 0x1, "size": 0x4,
                       "reason": "range outside built or retail module"}])
        self.assertIn("range outside built or retail module", out)


class RomFailureDetailTest(unittest.TestCase):
    """A failed ROM build must reach the author as the compiler said it, not as
    the phase name alone -- "mwccarm failed" is unactionable, and when the box
    and the author disagree it is the only evidence there is."""

    def test_failure_output_is_rendered_with_phase_and_exit_code(self):
        out = "\n".join(VM._rom_failure_detail({"failure": {
            "phase": "mwccarm", "returncode": 1,
            "output": "the file 'Clipper.h' cannot be opened"}}))
        self.assertIn("the file 'Clipper.h' cannot be opened", out)
        self.assertIn("mwccarm failed", out)
        self.assertIn("exit 1", out)
        self.assertIn("<details>", out)

    def test_a_successful_build_renders_nothing(self):
        self.assertEqual(VM._rom_failure_detail({"analysis": {}}), [])
        self.assertEqual(VM._rom_failure_detail(None), [])

    def test_a_failure_that_captured_no_output_renders_nothing(self):
        # Better the bare table row than an empty code fence.
        self.assertEqual(
            VM._rom_failure_detail({"failure": {"phase": "mwldarm", "output": "   "}}), [])
        self.assertEqual(
            VM._rom_failure_detail({"failure": {"phase": "mwldarm"}}), [])

    def test_long_output_keeps_the_tail_where_the_error_is(self):
        noisy = ("compiling something" + chr(10)) * 500 + "FATAL: the real error"
        out = "\n".join(VM._rom_failure_detail(
            {"failure": {"phase": "mwccarm", "returncode": 1, "output": noisy}}))
        self.assertIn("FATAL: the real error", out)
        self.assertIn("trimmed", out)


if __name__ == "__main__":
    unittest.main()
