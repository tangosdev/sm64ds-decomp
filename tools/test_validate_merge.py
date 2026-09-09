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

    def test_every_function_in_a_complete_tu_range_is_byte_verified(self):
        functions = {"matched": {
            "ov006:0x02000000": {
                "module": "ov006", "addr": 0x02000000, "size": 4},
            "ov006:0x02000004": {
                "module": "ov006", "addr": 0x02000004, "size": 8},
        }}
        enrollment = {"source": {"ov006:0x02000000-0x0200000c": {
            "module": "ov006", "addr": 0x02000000, "end": 0x0200000c,
        }}}

        split = VM.verification_split(functions, enrollment)
        self.assertEqual(split["stats"]["verifiedFunctions"], 2)
        self.assertEqual(split["stats"]["verifiedBytes"], 12)
        self.assertEqual(split["stats"]["claimedFunctions"], 0)
        self.assertEqual(split["claimed"], {})

    def test_zero_size_alias_does_not_displace_the_function_it_aliases(self):
        # config/arm9/symbols.txt has ten addresses carrying two function records, and
        # in every one the LAST line is a zero-size alias over a real function. Keyed by
        # module:addr, last-wins would hand the alias's name, size and matched flag to
        # the address and make the function invisible.
        config = self.repo / "config" / "arm9" / "symbols.txt"
        config.write_text(
            "Example kind:function(arm,size=0x4) addr:0x02000000\n"
            "ExampleAlias kind:function(arm,size=0x0) addr:0x02000000\n",
            encoding="utf-8")
        head = commit(self.repo, "add alias", "bob")
        snap = VM.function_snapshot(head)
        record = snap["functions"]["arm9:0x02000000"]
        self.assertEqual(record["name"], "Example")
        self.assertEqual(record["size"], 0x4)
        self.assertTrue(record["matched"])
        self.assertEqual(snap["stats"]["matchedFunctions"], 1)

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

    def test_lost_credit_does_not_excuse_a_non_attribution_failure(self):
        # Attribution never blocks a merge, but that must not mask a PR that
        # actually lost matched code alongside its credit.
        (self.repo / "src" / "Example.c").write_text(
            "// NONMATCHING\nint Example(void) { return 0; }\n")
        (self.repo / "attribution.json").write_text(
            '{"overrides": {"src/Example.c": "bob"}}\n', encoding="utf-8")
        commit(self.repo, "unmatch and repin", "maintainer")
        report = VM.build_report(self.base, "HEAD")
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
        # is reported as its own warning and never blocks the merge.
        report = VM.build_report(base, head)
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


class RomDataRatchet(unittest.TestCase):
    """Rows are `(module, symbol)` for an address-free report, `(module, symbol, addr,
    bytes)` for one `romdata_check.summarize` produced with the cartridge anchor."""

    def rows(self, entries):
        out = []
        for entry in entries:
            if len(entry) == 2:
                out.append({"module": entry[0], "symbol": entry[1]})
            else:
                module, symbol, addr, size = entry
                out.append({"module": module, "symbol": symbol,
                            "addr": addr, "bytes": size})
        return out

    def data(self, verified=(), differing=(), verified_bytes=None):
        verified_rows = self.rows(verified)
        body = {
            "verified": len(verified_rows),
            "differs": len(differing),
            "verifiedSymbols": verified_rows,
            "differingSymbols": self.rows(differing),
        }
        body["verifiedBytes"] = (
            sum(row.get("bytes", 4) for row in verified_rows)
            if verified_bytes is None else verified_bytes)
        return body

    def test_equal_count_cannot_hide_a_lost_exact_symbol(self):
        """An address-free report -- a base built before `summarize` carried the
        address -- keeps the original name-only diff, and so keeps calling this a
        loss. The fallback is what makes the anchored path safe to add at all."""
        base = self.data(verified=(("ov006", "_ZTV3Old"),))
        head = self.data(verified=(("ov006", "_ZTV3New"),))
        out = VM.rom_data_regressions(base, head)
        self.assertTrue(any("lost 1 exact symbol" in reason for reason in out))
        self.assertIn("ov006:_ZTV3Old", "; ".join(out))

    def test_equal_count_cannot_hide_a_new_differing_symbol(self):
        base = self.data(differing=(("ov006", "_ZTV3Old"),))
        head = self.data(differing=(("ov006", "_ZTV3New"),))
        out = VM.rom_data_regressions(base, head)
        self.assertTrue(any("gained 1 differing symbol" in reason for reason in out))
        self.assertIn("ov006:_ZTV3New", "; ".join(out))

    def test_verified_byte_loss_is_not_hidden(self):
        base = self.data(verified=(("arm9", "Data"),), verified_bytes=16)
        head = self.data(verified=(("arm9", "Data"),), verified_bytes=12)
        self.assertTrue(any("verified bytes fell" in reason
                            for reason in VM.rom_data_regressions(base, head)))

    def test_omitting_the_head_measurement_is_a_regression(self):
        base = self.data(verified=(("arm9", "Data"),))
        self.assertEqual(VM.rom_data_regressions(base, {}),
                         ["head full-ROM report omitted the ROM-data measurement"])

    # -- the cartridge anchor: a rename is not a deletion ---------------------------
    #
    # `ov081/symbols.txt` carries `_ZTV8Moneybag` and `_ZTV8daGmch_c` at one address,
    # 0x02128c04. Adopting the ROM's own RTTI name retires the coined row, so the head
    # report proves 0x84 bytes at that address under the other name. Nothing was lost,
    # and `verifiedBytes` -- unchanged across the rename -- already said so.

    def test_a_rename_at_the_same_address_and_size_is_not_a_loss(self):
        base = self.data(verified=(("ov081", "_ZTV8Moneybag", 0x02128c04, 0x84),))
        head = self.data(verified=(("ov081", "_ZTV8daGmch_c", 0x02128c04, 0x84),))
        self.assertEqual(VM.rom_data_regressions(base, head), [])

    def test_a_replacement_at_another_address_is_still_a_loss(self):
        base = self.data(verified=(("ov081", "_ZTV8Moneybag", 0x02128c04, 0x84),))
        head = self.data(verified=(("ov081", "_ZTV8daGmch_c", 0x02128d00, 0x84),))
        out = VM.rom_data_regressions(base, head)
        self.assertTrue(any("lost 1 exact symbol" in reason for reason in out))
        self.assertIn("ov081:_ZTV8Moneybag", "; ".join(out))

    def test_two_departures_answered_by_one_arrival_still_lose_one(self):
        """Matching is one-to-one. Two symbols proven at an address cannot both be
        excused by a single replacement standing there."""
        base = self.data(verified=(("ov081", "_ZTV8MoneybgA", 0x02128c04, 0x84),
                                   ("ov081", "_ZTV8MoneybgB", 0x02128c04, 0x84)),
                         verified_bytes=0x84)
        head = self.data(verified=(("ov081", "_ZTV8daGmch_c", 0x02128c04, 0x84),),
                         verified_bytes=0x84)
        out = VM.rom_data_regressions(base, head)
        self.assertTrue(any("lost 1 exact symbol" in reason for reason in out))

    def test_a_replacement_proving_fewer_bytes_is_still_a_loss(self):
        """Same address, less of it proven: the anchor is address AND extent, so a
        shorter arrival cannot stand in for what the base actually verified."""
        base = self.data(verified=(("ov081", "_ZTV8Moneybag", 0x02128c04, 0x84),))
        head = self.data(verified=(("ov081", "_ZTV8daGmch_c", 0x02128c04, 0x7c),))
        self.assertTrue(any("lost 1 exact symbol" in reason
                            for reason in VM.rom_data_regressions(base, head)))

    def test_the_same_address_in_another_module_is_still_a_loss(self):
        """Overlays share address space; only `(module, addr)` names one datum."""
        base = self.data(verified=(("ov081", "_ZTV8Moneybag", 0x02128c04, 0x84),))
        head = self.data(verified=(("ov030", "_ZTV8daGmch_c", 0x02128c04, 0x84),))
        self.assertTrue(any("lost 1 exact symbol" in reason
                            for reason in VM.rom_data_regressions(base, head)))

    def test_an_address_free_head_cannot_excuse_an_anchored_departure(self):
        """A half-upgraded pair of reports falls back rather than guessing."""
        base = self.data(verified=(("ov081", "_ZTV8Moneybag", 0x02128c04, 0x84),),
                         verified_bytes=0x84)
        head = self.data(verified=(("ov081", "_ZTV8daGmch_c"),), verified_bytes=0x84)
        self.assertTrue(any("lost 1 exact symbol" in reason
                            for reason in VM.rom_data_regressions(base, head)))

    def test_a_renamed_differing_symbol_is_not_a_newly_differing_symbol(self):
        """The same cartridge data still wrong at the same address under a new name is
        not new breakage -- the gate must not go red for work that changed nothing."""
        base = self.data(differing=(("ov081", "_ZTV8Moneybag", 0x02128c04, 0x84),))
        head = self.data(differing=(("ov081", "_ZTV8daGmch_c", 0x02128c04, 0x84),))
        self.assertEqual(VM.rom_data_regressions(base, head), [])

    def test_a_genuinely_new_differing_symbol_is_still_reported(self):
        base = self.data(differing=(("ov081", "_ZTV8Moneybag", 0x02128c04, 0x84),))
        head = self.data(differing=(("ov081", "_ZTV8Moneybag", 0x02128c04, 0x84),
                                    ("ov081", "_ZTV5Other", 0x02128e00, 0x40)))
        out = VM.rom_data_regressions(base, head)
        self.assertTrue(any("gained 1 differing symbol" in reason for reason in out))
        self.assertIn("ov081:_ZTV5Other", "; ".join(out))

    def test_a_rename_that_also_breaks_the_data_is_caught_both_ways(self):
        """VERIFIED -> DIFFERS across a rename: the address anchor must not launder
        it. The verified side loses the symbol; the differing side gains one."""
        base = self.data(verified=(("ov081", "_ZTV8Moneybag", 0x02128c04, 0x84),))
        head = self.data(differing=(("ov081", "_ZTV8daGmch_c", 0x02128c04, 0x84),),
                         verified_bytes=0x84)
        out = "; ".join(VM.rom_data_regressions(base, head))
        self.assertIn("lost 1 exact symbol", out)
        self.assertIn("gained 1 differing symbol", out)


def _snap(records):
    """A function_snapshot-shaped dict from (name, addr, size, matched) tuples."""
    functions, spans, total, matched_bytes = {}, {}, 0, 0
    for name, addr, size, is_matched in records:
        key = f"arm9:0x{addr:08x}"
        prior = functions.get(key)
        if prior is None or size > prior["size"]:
            functions[key] = {"id": key, "module": "arm9", "addr": addr, "name": name,
                              "size": size, "matched": is_matched,
                              "srcPath": f"src/{name}.c" if is_matched else None}
        if size > 0:
            spans.setdefault("arm9", []).append((addr, addr + size))
        total += size
        if is_matched:
            matched_bytes += size
    matched = {k: r for k, r in functions.items() if r["matched"]}
    return {"functions": functions, "spans": spans, "matched": matched,
            "stats": {"totalFunctions": len(functions), "totalBytes": total,
                      "matchedFunctions": len(matched), "matchedBytes": matched_bytes}}


class Repartition(unittest.TestCase):
    """The denominator carve-out, and the attacks an adversarial review threw at it."""

    def test_split_of_an_unmatched_symbol_is_a_repartition(self):
        # PR #2360's shape: one 0x3c symbol is really a 0x2c routine and a 0x10 one.
        base = _snap([("func_020610fc", 0x020610fc, 0x3c, False)])
        head = _snap([("func_020610fc", 0x020610fc, 0x2c, False),
                      ("func_02061128", 0x02061128, 0x10, False)])
        got = VM.classify_repartition(base, head)
        self.assertIsNotNone(got)
        self.assertEqual(got["kind"], "split")
        self.assertEqual(got["functionDelta"], 1)

    def test_split_that_also_gains_a_match_is_refused(self):
        # The free numerator: a new symbol whose name collides with an existing src
        # stem is resolved as matched by the filename convention. Land the split first.
        base = _snap([("func_020610fc", 0x020610fc, 0x3c, False)])
        head = _snap([("func_020610fc", 0x020610fc, 0x2c, False),
                      ("func_02061128", 0x02061128, 0x10, True)])
        self.assertIsNone(VM.classify_repartition(base, head))

    def test_split_that_grows_a_matched_neighbour_is_refused(self):
        # Attack S. Every count holds -- one matched function before and after, and the
        # covered bytes are identical -- but the matched symbol grew by 4, which raises
        # matchedBytePercent while matchedFunctionPercent falls. Counts cannot see it;
        # pinning each matched record's SIZE can.
        base = _snap([("hit", 0x02000000, 0x10, True), ("miss", 0x02000010, 0x20, False)])
        head = _snap([("hit", 0x02000000, 0x14, True), ("miss", 0x02000014, 0x10, False),
                      ("miss2", 0x02000024, 0xc, False)])
        self.assertIsNone(VM.classify_repartition(base, head))

    def test_split_that_swaps_which_function_is_matched_is_refused(self):
        # Counts again equal, one match out and one in. The matched SET is the test.
        base = _snap([("a", 0x02000000, 0x10, True), ("b", 0x02000010, 0x10, False),
                      ("c", 0x02000020, 0x10, False)])
        head = _snap([("a", 0x02000000, 0x10, False), ("b", 0x02000010, 0x10, True),
                      ("c", 0x02000020, 0x8, False), ("d", 0x02000028, 0x8, False)])
        self.assertIsNone(VM.classify_repartition(base, head))

    def test_new_bytes_are_not_a_repartition(self):
        base = _snap([("a", 0x02000000, 0x10, False)])
        head = _snap([("a", 0x02000000, 0x10, False), ("b", 0x02000010, 0x10, False)])
        self.assertIsNone(VM.classify_repartition(base, head))

    def test_merges_stay_blocked_even_when_the_absorber_is_matched(self):
        # Attack M1, the one that killed the first draft's merge rule. A matched,
        # TEXT-ONLY symbol grows to swallow an unmatched neighbour: matchedBytes rises
        # by exactly the absorbed size, in a symbols.txt-only diff, with nothing
        # compiled and nothing byte-compared. Merges get no arithmetic carve-out.
        base = _snap([("hit", 0x02052550, 0x1c, True),
                      ("miss", 0x0205256c, 0x1c, False)])
        head = _snap([("hit", 0x02052550, 0x38, True)])
        self.assertIsNone(VM.classify_repartition(base, head))

    def test_merging_unmatched_symbols_to_shrink_the_denominator_is_refused(self):
        # And the crude version: a hundred unmatched symbols become one. Covered bytes
        # identical, matched set identical -- and the headline still rises for no work,
        # which is why "every changed range is unmatched" is not a sufficient rule.
        base = _snap([(f"f{i}", 0x02000000 + i * 4, 4, False) for i in range(100)])
        head = _snap([("f0", 0x02000000, 400, False)])
        self.assertIsNone(VM.classify_repartition(base, head))

    def test_case_two_shaped_merge_is_refused_pending_its_own_rule(self):
        # The __destroy_arr shape. Correct on the merits and still not this rule's to
        # allow: it lands as a re-partition PR and then a match PR.
        base = _snap([("__destroy_arr", 0x0207328c, 0x5c, False),
                      ("func_020732e8", 0x020732e8, 0x18, False)])
        head = _snap([("__destroy_arr", 0x0207328c, 0x74, True)])
        self.assertIsNone(VM.classify_repartition(base, head))

    def test_a_stable_denominator_is_not_a_repartition(self):
        base = _snap([("a", 0x02000000, 0x10, False)])
        head = _snap([("a", 0x02000000, 0x10, True)])
        self.assertIsNone(VM.classify_repartition(base, head))

    def test_cross_module_shuffle_is_not_a_repartition(self):
        # totalBytes alone would call this clean. The covered set is compared per
        # module precisely because overlay addresses overlap.
        base = {"functions": {}, "spans": {"arm9": [(0x02000000, 0x02000010)],
                                           "ov000": [(0x02200000, 0x02200010)]},
                "matched": {},
                "stats": {"totalFunctions": 2, "totalBytes": 0x20,
                          "matchedFunctions": 0, "matchedBytes": 0}}
        head = {"functions": {}, "spans": {"arm9": [(0x02000000, 0x02000020)],
                                           "ov000": []},
                "matched": {},
                "stats": {"totalFunctions": 3, "totalBytes": 0x20,
                          "matchedFunctions": 0, "matchedBytes": 0}}
        self.assertIsNone(VM.classify_repartition(base, head))


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


def _enr(ranges):
    """An enrollment_snapshot-shaped dict from (addr, end) tuples, all `complete` src."""
    src = {}
    for addr, end in ranges:
        key = f"arm9:0x{addr:08x}-0x{end:08x}"
        src[key] = {"id": key, "module": "arm9", "addr": addr, "end": end,
                    "size": end - addr, "path": f"src/r{addr:08x}.c", "kind": "src"}
    return {"entries": dict(src), "source": src, "mods": {},
            "stats": {"sourceFunctions": len(src),
                      "sourceBytes": sum(e["size"] for e in src.values()),
                      "modFunctions": 0, "modBytes": 0}}


def _compiled(enrollment):
    """`build_report`'s `compiled_src` for an enrollment whose sources are all real C.

    The production set is the merge's own added/modified sources that carry no asm body
    and no banner excusing one; here it is simply every path `_enr` invented.
    """
    return {e["path"] for e in enrollment["source"].values()}


class MergeEvidence(unittest.TestCase):
    """A merge raises the headline, so it lands only on evidence the ROM build supplies.

    The evidence has two halves and both are tested here: a newly `complete` delinks
    range that covers every removed address, AND a source behind that range which is real
    compiled code. The second half is not redundant -- a `dcd` transcription reproduces
    vacuously, so the range alone proves nothing.

    The live case: `__destroy_arr` is declared 0x5c in symbols.txt while the ROM's own
    `.exceptix` record gives 0x74. The trailing 0x18 is carried as a separate symbol and
    is not a function at all -- it is the catch handler.
    """

    DESTROY, CATCH = 0x0207328c, 0x020732e8

    def _junk(self):
        """A hundred four-byte symbols folded into one 400-byte one. No work at all."""
        return (_snap([(f"func_{0x02100000 + i * 4:08x}", 0x02100000 + i * 4, 4, False)
                       for i in range(100)]),
                _snap([("func_02100000", 0x02100000, 400, False)]))

    def test_the_live_case_lands(self):
        base = _snap([("__destroy_arr", self.DESTROY, 0x5c, False),
                      ("func_020732e8", self.CATCH, 0x18, False)])
        head = _snap([("__destroy_arr", self.DESTROY, 0x74, True)])
        he = _enr([(self.DESTROY, self.DESTROY + 0x74)])
        got = VM.classify_merge(base, head, _enr([]), he, _compiled(he))
        self.assertIsNotNone(got)
        self.assertEqual(got["kind"], "merge")
        self.assertEqual(got["functionDelta"], -1)
        self.assertEqual(got["sourceByteDelta"], 0x74)
        self.assertEqual(got["removed"], [f"arm9:0x{self.CATCH:08x}"])

    def test_without_new_delinks_coverage_it_is_refused(self):
        # The whole rule. Identical symbol-table move, no build behind it.
        base = _snap([("__destroy_arr", self.DESTROY, 0x5c, False),
                      ("func_020732e8", self.CATCH, 0x18, False)])
        head = _snap([("__destroy_arr", self.DESTROY, 0x74, True)])
        self.assertIsNone(VM.classify_merge(base, head, _enr([]), _enr([]), set()))

    def test_a_hundred_junk_symbols_merged_into_one_is_refused(self):
        base, head = self._junk()
        self.assertIsNone(VM.classify_merge(base, head, _enr([]), _enr([]), set()))

    def test_a_hundred_junk_symbols_cannot_borrow_an_unrelated_new_range(self):
        # The evidence has to cover the merged addresses, not merely exist.
        base, head = self._junk()
        he = _enr([(0x02300000, 0x02300190)])
        self.assertIsNone(VM.classify_merge(base, head, _enr([]), he, _compiled(he)))

    def test_a_transcribed_source_is_not_evidence_even_covering_the_merge(self):
        # THE COVERING-RANGE ATTACK, and the reason the source classification is an
        # argument at all. Same hundred junk symbols, but now the new `complete` range
        # covers exactly the addresses the fold removes and sourceBytes rises by exactly
        # its extent -- every arithmetic condition the rule checks is satisfied. What
        # backs it is a `dcd` transcription under a HAND-ASM PRIMITIVE banner, so it
        # byte-compares against retail by construction (the words ARE the ROM's words),
        # module fidelity passes, and `asm_policy.classify` returns None because the
        # banner excuses it. Measured before the `compiled` clause: this classified as
        # `merge`, functionDelta -99, sourceByteDelta 400.
        base, head = self._junk()
        he = _enr([(0x02100000, 0x02100000 + 400)])
        self.assertIsNone(VM.classify_merge(base, head, _enr([]), he, set()))

    def test_a_covering_range_from_compiled_source_still_lands(self):
        # The mirror, and the thing the rule must not break: the identical fold, backed
        # by a range whose source is real compiled C. #2429 exists to let this through.
        base = _snap([("__destroy_arr", self.DESTROY, 0x5c, False),
                      ("func_020732e8", self.CATCH, 0x18, False)])
        head = _snap([("__destroy_arr", self.DESTROY, 0x74, True)])
        he = _enr([(self.DESTROY, self.DESTROY + 0x74)])
        # Named explicitly rather than through _compiled, so the test still reads as a
        # statement about one file if the helper ever changes.
        got = VM.classify_merge(base, head, _enr([]), he,
                                {f"src/r{self.DESTROY:08x}.c"})
        self.assertIsNotNone(got)
        self.assertEqual(got["kind"], "merge")
        self.assertEqual(got["sourceByteDelta"], 0x74)

    def test_absorbing_a_matched_function_is_refused(self):
        # A merge may swallow an ASM stub or a severed epilogue. Never a match.
        base = _snap([("__destroy_arr", self.DESTROY, 0x5c, False),
                      ("real_function", self.CATCH, 0x18, True)])
        head = _snap([("__destroy_arr", self.DESTROY, 0x74, True)])
        he = _enr([(self.DESTROY, self.DESTROY + 0x74)])
        self.assertIsNone(VM.classify_merge(base, head, _enr([]), he, _compiled(he)))

    def test_source_bytes_must_rise_by_exactly_the_new_extent(self):
        # An unrelated range leaving under cover of the merge holds the delta short.
        he = _enr([(self.DESTROY, self.DESTROY + 0x74)])
        he["stats"]["sourceBytes"] = 0x40
        base = _snap([("__destroy_arr", self.DESTROY, 0x5c, False),
                      ("func_020732e8", self.CATCH, 0x18, False)])
        head = _snap([("__destroy_arr", self.DESTROY, 0x74, True)])
        self.assertIsNone(VM.classify_merge(base, head, _enr([]), he, _compiled(he)))

    def test_a_range_already_complete_in_base_is_not_evidence(self):
        # Absorbing into a range the build was already compiling proves nothing new.
        enr = _enr([(self.DESTROY, self.DESTROY + 0x74)])
        base = _snap([("__destroy_arr", self.DESTROY, 0x5c, False),
                      ("func_020732e8", self.CATCH, 0x18, False)])
        head = _snap([("__destroy_arr", self.DESTROY, 0x74, True)])
        self.assertIsNone(VM.classify_merge(base, head, enr, enr, _compiled(enr)))

    def test_total_bytes_must_not_move(self):
        base = _snap([("__destroy_arr", self.DESTROY, 0x5c, False),
                      ("func_020732e8", self.CATCH, 0x18, False)])
        head = _snap([("__destroy_arr", self.DESTROY, 0x80, True)])
        he = _enr([(self.DESTROY, self.DESTROY + 0x80)])
        self.assertIsNone(VM.classify_merge(base, head, _enr([]), he, _compiled(he)))

    def test_a_split_is_not_a_merge(self):
        base = _snap([("func_020610fc", 0x020610fc, 0x3c, False)])
        head = _snap([("func_020610fc", 0x020610fc, 0x2c, False),
                      ("func_02061128", 0x02061128, 0x10, False)])
        self.assertIsNone(VM.classify_merge(base, head, _enr([]), _enr([]), set()))

    def test_missing_enrollment_snapshots_refuse_rather_than_crash(self):
        base = _snap([("__destroy_arr", self.DESTROY, 0x5c, False),
                      ("func_020732e8", self.CATCH, 0x18, False)])
        head = _snap([("__destroy_arr", self.DESTROY, 0x74, True)])
        self.assertIsNone(VM.classify_merge(base, head, None, None, set()))


class MergeArithmetic(unittest.TestCase):
    """`sourceBytes` must rise by the NEW coverage, which is not the new extent.

    A range the base was already compiling, lying wholly inside one of the merge's new
    ranges, did not become covered here: its bytes moved from one `complete` entry to
    another and were in `sourceBytes` all along. Comparing against the raw extent made
    an honest fold over an already-enrolled neighbour fail for arithmetic (+80 against
    an extent of 84) with nothing wrong.

    A base range that leaves from OUTSIDE every new range is the opposite: real lost
    coverage, and it is deliberately not subtracted, so it still shortens the delta and
    the comparison still refuses it. That clause is the anti-smuggling one, and the
    second test holds it in place.
    """

    BODY, EPILOGUE = 0x02071644, 0x02071694
    ELSEWHERE = 0x02050000

    def test_bytes_the_base_already_compiled_are_not_new_coverage(self):
        base = _snap([("func_02071644", self.BODY, 0x50, False),
                      ("func_02071694", self.EPILOGUE, 0x4, False)])
        head = _snap([("func_02071644", self.BODY, 0x54, True)])
        be = _enr([(self.EPILOGUE, self.EPILOGUE + 0x4)])
        he = _enr([(self.BODY, self.BODY + 0x54)])
        # sourceBytes: 4 -> 84, a rise of 80 against a new extent of 84.
        self.assertEqual(he["stats"]["sourceBytes"] - be["stats"]["sourceBytes"], 0x50)
        got = VM.classify_merge(base, head, be, he, _compiled(he))
        self.assertIsNotNone(got)
        self.assertEqual(got["newExtent"], 0x54)
        self.assertEqual(got["absorbedBytes"], 0x4)
        self.assertEqual(got["sourceByteDelta"], 0x50)

    def test_an_unrelated_range_leaving_under_cover_of_the_merge_still_fails(self):
        # The outside-a-range clause, unchanged: this range is nowhere near the merge,
        # so its bytes are not subtracted and the arithmetic catches the loss.
        base = _snap([("func_02071644", self.BODY, 0x50, False),
                      ("func_02071694", self.EPILOGUE, 0x4, False)])
        head = _snap([("func_02071644", self.BODY, 0x54, True)])
        be = _enr([(self.EPILOGUE, self.EPILOGUE + 0x4),
                   (self.ELSEWHERE, self.ELSEWHERE + 0x40)])
        he = _enr([(self.BODY, self.BODY + 0x54)])
        self.assertIsNone(VM.classify_merge(base, head, be, he, _compiled(he)))


class MergeEvidenceThroughBuildReport(unittest.TestCase):
    """The same rule driven through `build_report` on a real tree, not hand-built dicts.

    Every test above calls `classify_merge` directly and invents the `compiled` set, so
    none of them touches the wiring: `compiled_src` is keyed on the paths
    `diff_snapshot` reports, while the ranges are keyed on the paths written in
    `delinks.txt`. If those two spellings ever stopped agreeing, the source clause would
    reject EVERY merge and the carve-out would be dead -- and the suite above would
    still be green, because it never asks a real tree for either one.

    So both halves are asserted here from one committed base: the transcription is
    refused, and the compiled source is still allowed. A fix that only refuses is not a
    fix, and this is the test that can tell the difference.
    """

    DESTROY, CATCH = 0x0207328c, 0x020732e8

    # Real C++: the shape #2429 exists for -- the catch handler reunited with the body
    # it belongs to, compiled and byte-compared by the ROM build.
    COMPILED = (
        "void __destroy_arr(void *base, int size, int n, void (*dtor)(void *)) {\n"
        "    char *p = (char *)base + size * n;\n"
        "    while (n--) { p -= size; dtor(p); }\n"
        "}\n")
    # The attack: the ROM's own words re-spelled. It byte-compares against retail by
    # construction, so module fidelity passes, and `asm_policy.classify` returns None
    # because the banner excuses the `dcd` body.
    TRANSCRIBED = ("// HAND-ASM PRIMITIVE\n"
                   "asm void __destroy_arr(void) {\n"
                   "    dcd 0xe92d4070\n"
                   "    dcd 0xe1a04000\n"
                   "    dcd 0xe8bd8070\n"
                   "}\n")

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = pathlib.Path(self.tmp.name)
        git(self.repo, "init", "-q", ".")
        (self.repo / "src").mkdir()
        self.config = self.repo / "config" / "arm9"
        self.config.mkdir(parents=True)
        # An unrelated `complete` range in both revisions, so the merge's own range is
        # the only NEW one and `sourceBytes` has something to rise from.
        (self.config / "symbols.txt").write_text(
            "Anchor kind:function(arm,size=0x4) addr:0x02000000\n"
            f"__destroy_arr kind:function(arm,size=0x5c) addr:0x{self.DESTROY:08x}\n"
            f"func_{self.CATCH:08x} kind:function(arm,size=0x18) "
            f"addr:0x{self.CATCH:08x}\n",
            encoding="utf-8")
        (self.config / "delinks.txt").write_text(
            "    .text start:0x02000000 end:0x02100000 kind:code\n\n"
            "src/Anchor.c:\n    complete\n    .text start:0x02000000 end:0x02000004\n",
            encoding="utf-8")
        (self.repo / "src" / "Anchor.c").write_text("int Anchor(void) { return 0; }\n")
        # Both halves unmatched in base -- a merge may absorb an ASM stub, never a match.
        (self.repo / "src" / "__destroy_arr.c").write_text(
            "// NONMATCHING\nvoid __destroy_arr(void) {}\n")
        (self.repo / "src" / f"func_{self.CATCH:08x}.c").write_text(
            "// NONMATCHING\nvoid stub(void) {}\n")
        self.base = commit(self.repo, "base", "alice")
        self.old_repo = VM.REPO
        VM.REPO = self.repo

    def tearDown(self):
        VM.REPO = self.old_repo
        self.tmp.cleanup()

    def _fold(self, source):
        """Commit the fold: 0x5c + 0x18 become one 0x74 record, enrolled `complete`."""
        (self.config / "symbols.txt").write_text(
            "Anchor kind:function(arm,size=0x4) addr:0x02000000\n"
            f"__destroy_arr kind:function(arm,size=0x74) addr:0x{self.DESTROY:08x}\n",
            encoding="utf-8")
        (self.config / "delinks.txt").write_text(
            "    .text start:0x02000000 end:0x02100000 kind:code\n\n"
            "src/Anchor.c:\n    complete\n    .text start:0x02000000 end:0x02000004\n\n"
            "src/__destroy_arr.c:\n    complete\n"
            f"    .text start:0x{self.DESTROY:08x} "
            f"end:0x{self.DESTROY + 0x74:08x}\n",
            encoding="utf-8")
        (self.repo / "src" / "__destroy_arr.c").write_text(source)
        os.remove(self.repo / "src" / f"func_{self.CATCH:08x}.c")
        return VM.build_report(self.base, commit(self.repo, "fold", "bob"))

    def test_a_transcribed_source_cannot_carry_the_merge_through_the_report(self):
        # Measured on 660900ee3 against this exact tree: reasons [], repartition
        # {'kind': 'merge', 'functionDelta': -1, 'sourceByteDelta': 116}.
        report = self._fold(self.TRANSCRIBED)
        self.assertIsNone(report["repartition"])
        self.assertIn("function/byte coverage denominator changed", report["reasons"])

    def test_a_compiled_source_still_carries_the_merge_through_the_report(self):
        # The delinks spelling `src/__destroy_arr.c` and the diff spelling of the same
        # file have to agree for this to pass at all -- that agreement is the point.
        report = self._fold(self.COMPILED)
        self.assertEqual(report["reasons"], [])
        self.assertIsNotNone(report["repartition"])
        self.assertEqual(report["repartition"]["kind"], "merge")
        self.assertEqual(report["repartition"]["functionDelta"], -1)
        self.assertEqual(report["repartition"]["sourceByteDelta"], 0x74)
        self.assertEqual(report["repartition"]["removed"],
                         [f"arm9:0x{self.CATCH:08x}"])


if __name__ == "__main__":
    unittest.main()


class RelocIndex(unittest.TestCase):
    """`_reloc_index` reads the module of the DESTINATION, from every file, and says
    when its own input is not fit to be relied on.

    The index answers exactly one question -- "does anything point at these bytes" --
    and it is asked in order to PERMIT something. An index that skipped unreadable
    input would answer "nothing" for every address in the cartridge, so every way the
    input can be defective is a defect here rather than a line to skip.
    """

    ARM9_ROWS = ("from:0x02004808 kind:arm_call to:0x020049f0 module:main\n"
                 "from:0x0203b5bc kind:arm_call to:0x01ffa4bc module:none\n"
                 "from:0x02005efc kind:arm_call to:0x020ab110 module:overlay(1)\n")

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.repo = pathlib.Path(self.tmp.name)
        git(self.repo, "init", "-q", ".")
        self.cfg = self.repo / "config" / "arm9"
        (self.cfg / "overlays" / "ov001").mkdir(parents=True)
        (self.cfg / "itcm").mkdir(parents=True)
        self.write("symbols.txt",
                   "Anchor kind:function(arm,size=0x4) addr:0x02000000\n")
        self.write("overlays/ov001/symbols.txt",
                   "OvOne kind:function(arm,size=0x4) addr:0x020ab110\n")
        self.write("itcm/symbols.txt",
                   "ItcmOne kind:function(arm,size=0x4) addr:0x01ff8000\n")
        self.write("relocs.txt", self.ARM9_ROWS)
        # A call from an overlay INTO the main binary. The arm9 file never sees it.
        self.write("overlays/ov001/relocs.txt",
                   "from:0x020aa42c kind:arm_call to:0x0201a9ec module:main\n"
                   "from:0x020aa438 kind:load to:0x020ff028 module:overlays(2,7)\n")
        # And one from ITCM, in a directory `_module_from_relocs` never recognised.
        self.write("itcm/relocs.txt",
                   "from:0x01ff8100 kind:arm_call to:0x0201b000 module:main\n")
        self.old_repo = VM.REPO
        VM.REPO = self.repo
        for cache in (VM._RELOC_CACHE, VM._ENROLMENT_CACHE,
                      VM._SYMBOL_BASE_CACHE):
            cache.clear()
            self.addCleanup(cache.clear)
        self.addCleanup(setattr, VM, "REPO", self.old_repo)

    def write(self, rel, text):
        (self.cfg / rel).write_text(text, encoding="utf-8", newline="\n")

    def image(self, module, words):
        """The module's cartridge image, based at its lowest declared symbol."""
        self.write(f"overlays/{module}/symbols.txt",
                   "data_020ab100 kind:data(any) addr:0x020ab100\n")
        d = self.repo / "extracted" / "overlays"
        d.mkdir(parents=True, exist_ok=True)
        image = bytearray()
        for word in words:
            image += word.to_bytes(4, "little")
        (d / f"overlay_{int(module[2:]):04d}.bin").write_bytes(bytes(image))

    def index(self):
        VM._RELOC_CACHE.clear()
        VM._SYMBOL_BASE_CACHE.clear()
        return VM._reloc_index(commit(self.repo, "relocs", "tester"))

    def test_every_module_file_supplies_destinations_and_the_index_is_valid(self):
        index = self.index()
        self.assertEqual(index["defects"], [])
        dests = index["dests"]
        # 0x0201b000 is the ITCM file's row: the whole point of reading every file.
        self.assertEqual(dests["arm9"], [0x020049f0, 0x0201a9ec, 0x0201b000])
        self.assertEqual(dests["ov001"], [0x020ab110])
        self.assertEqual(dests["ov002"], [0x020ff028])
        self.assertEqual(dests["ov007"], [0x020ff028])
        # A documented token naming no module counts for every module, not for none.
        self.assertEqual(dests["*"], [0x01ffa4bc])
        self.assertEqual(
            VM._incoming_relocations(dests, "arm9", 0x0201b000, 0x0201b004), 1)
        self.assertEqual(
            VM._incoming_relocations(dests, "arm9", 0x0201affc, 0x0201b000), 0)
        self.assertEqual(
            VM._incoming_relocations(dests, "ov001", 0x01ffa4bc, 0x01ffa4c0), 1)

    def test_a_link_time_constant_row_is_documented_and_carries_no_destination(self):
        # The one row in the whole tree that names no address.
        self.write("relocs.txt",
                   self.ARM9_ROWS
                   + "from:0x02072fc8 kind:link_time_const(ARM9_CTOR_START)\n")
        index = self.index()
        self.assertEqual(index["defects"], [])
        self.assertEqual(index["dests"]["arm9"], [0x020049f0, 0x0201a9ec, 0x0201b000])

    def test_a_missing_file_for_a_module_that_declares_functions_is_a_defect(self):
        os.remove(self.cfg / "overlays" / "ov001" / "relocs.txt")
        defects = self.index()["defects"]
        self.assertEqual(len(defects), 1)
        self.assertIn("config/arm9/overlays/ov001/relocs.txt is missing", defects[0])

    def test_an_empty_file_for_a_module_that_declares_functions_is_a_defect(self):
        self.write("relocs.txt", "")
        defects = self.index()["defects"]
        self.assertEqual(len(defects), 1)
        self.assertIn("config/arm9/relocs.txt is empty", defects[0])

    def empty_module(self, sections, name="ov061"):
        """A module with no functions, an empty `relocs.txt`, and that inventory."""
        (self.cfg / "overlays" / name).mkdir(parents=True, exist_ok=True)
        self.write(f"overlays/{name}/symbols.txt", "")
        self.write(f"overlays/{name}/relocs.txt", "")
        self.write(f"overlays/{name}/delinks.txt", sections)

    def test_an_empty_file_for_a_module_with_nothing_but_bss_is_fine(self):
        # 15 of these at cd3a7eb59. ov061 verbatim: a zero-length `.ctor` and a
        # `.bss`, which occupies no cartridge bytes and can hold no pointer.
        self.empty_module(
            "    .ctor       start:0x02115ec0 end:0x02115ec0 kind:rodata align:4\n"
            "    .bss        start:0x02115ee0 end:0x02115ee0 kind:bss align:32\n")
        self.assertEqual(self.index()["defects"], [])

    def test_an_empty_file_beside_a_module_with_no_inventory_fails_closed(self):
        # No `delinks.txt` at all: nothing says whether the module holds data.
        (self.cfg / "overlays" / "ov061").mkdir(parents=True)
        self.write("overlays/ov061/symbols.txt", "")
        self.write("overlays/ov061/relocs.txt", "")
        defects = self.index()["defects"]
        self.assertEqual(len(defects), 1)
        self.assertIn("ov061 has no section inventory", defects[0])

    def test_an_empty_file_beside_unreadable_initialised_data_fails_closed(self):
        # Four bytes of real `.data` and no image in this checkout to read them
        # from. Unavailable evidence is a defect, never a pass.
        self.empty_module(
            "    .data       start:0x020ab100 end:0x020ab104 kind:data align:4\n")
        defects = self.index()["defects"]
        self.assertEqual(len(defects), 1)
        self.assertIn("the module image is unavailable", defects[0])

    def test_an_empty_file_beside_a_word_naming_an_address_is_a_defect(self):
        self.empty_module(
            "    .data       start:0x020ab100 end:0x020ab104 kind:data align:4\n")
        self.image("ov061", [0x02071694])
        defects = self.index()["defects"]
        self.assertEqual(len(defects), 1)
        self.assertIn("holds a nonzero word 0x02071694 at 0x020ab100", defects[0])

    def test_an_empty_file_beside_zeroed_initialised_data_is_fine(self):
        # `config/arm9/dtcm` is this case on the real tree: 0x20 of `.data`, all
        # zeroes, and an empty relocation file that is complete evidence.
        self.empty_module(
            "    .data       start:0x020ab100 end:0x020ab108 kind:data align:4\n")
        self.image("ov061", [0, 0])
        self.assertEqual(self.index()["defects"], [])

    def test_a_malformed_row_is_a_defect(self):
        self.write("relocs.txt", self.ARM9_ROWS + "garbage\n")
        defects = self.index()["defects"]
        self.assertEqual(len(defects), 1)
        self.assertIn("config/arm9/relocs.txt:4 is not a relocation row", defects[0])

    def test_a_truncated_last_row_is_a_defect(self):
        self.write("relocs.txt", self.ARM9_ROWS + "from:0x0203b5c0 kind:arm_call to:0x0207\n")
        defects = self.index()["defects"]
        self.assertEqual(len(defects), 1)
        self.assertIn("config/arm9/relocs.txt:4 is not a relocation row", defects[0])

    def test_a_leading_byte_order_mark_is_a_defect(self):
        self.write("relocs.txt", "﻿" + self.ARM9_ROWS)
        defects = self.index()["defects"]
        self.assertEqual(len(defects), 1)
        self.assertIn("config/arm9/relocs.txt:1 is not a relocation row", defects[0])

    def test_leading_whitespace_is_a_defect(self):
        self.write("relocs.txt", " " + self.ARM9_ROWS)
        defects = self.index()["defects"]
        self.assertEqual(len(defects), 1)
        self.assertIn("config/arm9/relocs.txt:1 is not a relocation row", defects[0])

    def test_an_undocumented_destination_token_is_a_defect(self):
        self.write("relocs.txt",
                   self.ARM9_ROWS
                   + "from:0x0203b5c0 kind:arm_call to:0x02071694 module:elsewhere\n")
        index = self.index()
        self.assertEqual(len(index["defects"]), 1)
        self.assertIn("undocumented destination module 'elsewhere'",
                      index["defects"][0])
        # And the address it named is in nobody's list, which is exactly why the row
        # cannot simply be skipped.
        self.assertEqual(
            VM._incoming_relocations(index["dests"], "arm9", 0x02071694, 0x02071698), 0)

    def test_the_evidence_over_two_revisions_unions_addresses_and_defects(self):
        base = commit(self.repo, "base", "tester")
        self.write("relocs.txt", self.ARM9_ROWS + "garbage\n")
        head = commit(self.repo, "head", "tester")
        VM._RELOC_CACHE.clear()
        evidence = VM._reloc_evidence(base, head)
        self.assertFalse(evidence["valid"])
        self.assertEqual(len(evidence["defects"]), 1)
        # A destination present in either revision counts: a PR cannot earn "nothing
        # points here" by deleting the row that says something does.
        self.assertEqual(
            VM._incoming_relocations(evidence["dests"], "arm9",
                                     0x020049f0, 0x020049f4), 2)


def _relocs(*addrs, module="arm9"):
    """A `reloc_dests` index holding exactly these destinations, all valid."""
    return {module: sorted(addrs)}


def _rom(words=None, default=None):
    """A `rom_word` reader over a dict of {(module, addr): word}."""
    def read(module, addr):
        return (words or {}).get((module, addr), default)
    read.missing = set()
    return read


BX_LR = 0xE12FFF1E
MOV_PC_LR = 0xE1A0F00E
POP_R4_PC = 0xE8BD8010
LDMIA_SP_PC = 0xE8BD9FFF


class ReturnEncodings(unittest.TestCase):
    """The accepted set, spelled out. The three shapes evidence_rom recognises."""

    def test_the_accepted_encodings(self):
        for word in (BX_LR, MOV_PC_LR, POP_R4_PC, LDMIA_SP_PC):
            self.assertTrue(VM._is_return_word(word), hex(word))

    def test_everything_else_is_refused(self):
        for word, why in ((0x012FFF1E, "bxeq lr, a conditional return"),
                          (0xE12FFF13, "bx r3, not lr"),
                          (0xE8BD0010, "pop {r4}, no pc in the list"),
                          (0xE8FD8010, "ldmia sp!, {r4, pc}^, restores CPSR"),
                          (0xE92D4010, "stmdb sp!, {r4, lr}, a store"),
                          (0xEAFFFFFE, "b self, an infinite loop"),
                          (0xE3A00000, "mov r0, #0"),
                          (0xE49DF004, "ldr pc, [sp], #4, deliberately outside the set"),
                          (0x00000000, "padding")):
            self.assertFalse(VM._is_return_word(word), why)


class AbsorbedEpilogue(unittest.TestCase):
    """The matched-loss exception, in `classify_merge` itself.

    A record may leave `matched` only when all four hold: it is the exact TAIL of a new
    `complete` range, it is one four-byte instruction whose ROM word is an accepted
    return, that range is backed by compiled source, and a VALID relocation index names
    nothing in its bytes. Each test below removes exactly one of them.
    """

    BODY, EPILOGUE, NEXT = 0x02071644, 0x02071694, 0x02071698

    def _merge(self, epilogue_size=0x4):
        addr = self.EPILOGUE
        base = _snap([("func_02071644", self.BODY, addr - self.BODY, False),
                      ("func_02071694", addr, epilogue_size, True)])
        head = _snap([("func_02071644", self.BODY,
                       addr + epilogue_size - self.BODY, True)])
        be = _enr([(addr, addr + epilogue_size)])
        he = _enr([(self.BODY, addr + epilogue_size)])
        return base, head, be, he

    def test_a_ranges_own_tail_return_may_be_absorbed(self):
        base, head, be, he = self._merge()
        got = VM.classify_merge(base, head, be, he, _compiled(he), _relocs(self.BODY),
                                _rom({("arm9", self.EPILOGUE): BX_LR}))
        self.assertIsNotNone(got)
        self.assertEqual(got["absorbed"], [f"arm9:0x{self.EPILOGUE:08x}"])
        self.assertEqual(got["functionDelta"], -1)
        self.assertEqual(got["sourceByteDelta"], 0x50)

    def test_every_accepted_return_encoding_lands(self):
        for word in (BX_LR, MOV_PC_LR, POP_R4_PC, LDMIA_SP_PC):
            base, head, be, he = self._merge()
            got = VM.classify_merge(base, head, be, he, _compiled(he),
                                    _relocs(self.BODY),
                                    _rom({("arm9", self.EPILOGUE): word}))
            self.assertIsNotNone(got, hex(word))

    def test_a_record_the_rom_calls_is_still_refused(self):
        # (d). The same diff with one relocation naming those four bytes.
        base, head, be, he = self._merge()
        self.assertIsNone(VM.classify_merge(
            base, head, be, he, _compiled(he), _relocs(self.BODY, self.EPILOGUE),
            _rom({("arm9", self.EPILOGUE): BX_LR})))

    def test_a_load_of_the_address_counts_as_a_caller(self):
        # A `kind:load` destination is a reference like any other.
        base, head, be, he = self._merge()
        self.assertIsNone(VM.classify_merge(
            base, head, be, he, _compiled(he), _relocs(self.EPILOGUE),
            _rom({("arm9", self.EPILOGUE): BX_LR})))

    def test_without_a_validated_relocation_index_nothing_matched_may_leave(self):
        # (d) again: None is what build_report passes when the index has a defect.
        base, head, be, he = self._merge()
        self.assertIsNone(VM.classify_merge(base, head, be, he, _compiled(he), None,
                                            _rom({("arm9", self.EPILOGUE): BX_LR})))

    def test_without_the_rom_image_nothing_matched_may_leave(self):
        # (b) cannot be checked, so it is not assumed.
        base, head, be, he = self._merge()
        self.assertIsNone(VM.classify_merge(base, head, be, he, _compiled(he),
                                            _relocs(self.BODY), None))
        self.assertIsNone(VM.classify_merge(base, head, be, he, _compiled(he),
                                            _relocs(self.BODY), _rom()))

    def test_a_four_byte_record_that_is_not_a_return_is_refused(self):
        # (b). Byte coverage plus an empty index says nothing about what the bytes ARE.
        base, head, be, he = self._merge()
        self.assertIsNone(VM.classify_merge(
            base, head, be, he, _compiled(he), _relocs(self.BODY),
            _rom({("arm9", self.EPILOGUE): 0xE3A00000})))

    def test_a_conditional_return_is_refused(self):
        # (b). bxeq lr leaves a live fall-through, so the bytes after it are reached.
        base, head, be, he = self._merge()
        self.assertIsNone(VM.classify_merge(
            base, head, be, he, _compiled(he), _relocs(self.BODY),
            _rom({("arm9", self.EPILOGUE): 0x012FFF1E})))

    def test_an_arbitrary_sized_matched_record_at_the_tail_is_refused(self):
        # (b). The 0x100-byte record: at the exact tail of a new compiled range with a
        # clean index, and still refused, because one instruction is the whole claim.
        base, head, be, he = self._merge(epilogue_size=0x100)
        self.assertIsNone(VM.classify_merge(
            base, head, be, he, _compiled(he), _relocs(self.BODY),
            _rom({("arm9", self.EPILOGUE): BX_LR})))

    def test_a_record_inside_the_range_but_not_at_its_tail_is_refused(self):
        # (a). A named callback the merge happens to cover is not an epilogue. Placed
        # mid-range: a record at the exact HEAD of a range cannot reach this clause at
        # all, because head still carries a record at that address -- see the
        # survives-but-loses-matched test below.
        base = _snap([("func_02071644", self.BODY, 0x20, False),
                      ("AutoloadCallback", self.BODY + 0x20, 0x4, True),
                      ("func_02071668", self.BODY + 0x24, 0x30, False)])
        head = _snap([("func_02071644", self.BODY, 0x54, True)])
        be = _enr([(self.BODY + 0x20, self.BODY + 0x24)])
        he = _enr([(self.BODY, self.BODY + 0x54)])
        self.assertIsNone(VM.classify_merge(
            base, head, be, he, _compiled(he), _relocs(self.BODY),
            _rom({("arm9", self.BODY + 0x20): BX_LR})))

    def test_absorption_still_needs_a_new_compiled_range_over_the_bytes(self):
        # (c) and the merge rule's own evidence: no new range at all.
        base, head, be, he = self._merge()
        self.assertIsNone(VM.classify_merge(base, head, be, be, _compiled(be),
                                            _relocs(self.BODY),
                                            _rom({("arm9", self.EPILOGUE): BX_LR})))

    def test_a_transcription_cannot_absorb_a_matched_record_either(self):
        # (c). The covering range reproduces vacuously, so it proves nothing about the
        # instruction it claims to have recovered.
        base, head, be, he = self._merge()
        self.assertIsNone(VM.classify_merge(base, head, be, he, set(),
                                            _relocs(self.BODY),
                                            _rom({("arm9", self.EPILOGUE): BX_LR})))

    def test_a_record_that_survives_but_loses_matched_is_not_absorption(self):
        # Still present in head, so a banner went on or its source left. That is a
        # withdrawal or a loss and this rule must not decide it.
        base = _snap([("func_02071644", self.BODY, 0x50, True),
                      ("func_02071694", self.EPILOGUE, 0x4, True)])
        head = _snap([("func_02071644", self.BODY, 0x50, False),
                      ("func_02071694", self.EPILOGUE, 0x4, False),
                      ("func_02071698", self.NEXT, 0x4, False)])
        he = _enr([(self.BODY, self.BODY + 0x50)])
        self.assertIsNone(VM.classify_merge(base, head, _enr([]), he, _compiled(he),
                                            _relocs(), _rom({}, BX_LR)))


class AbsorbedEpilogueThroughBuildReport(unittest.TestCase):
    """The matched-loss exception driven through `build_report`, on a real tree.

    Wiring only a real tree can check. `lost N matched function(s)` is raised OUTSIDE
    `classify_merge`, from a loop that had two buckets, so a classification the merge
    rule allowed would still have failed the report. The relocation index is read from
    every `relocs.txt` at both revisions. The ROM word is read from the extracted image
    on disk, the way the byte gate reads it. None of that is exercised by a hand-built
    dict, and every one of the fail-open paths below returned `Passed`, `reasons: []`
    and one absorbed record before this rule validated its own evidence.

    The live shape: `func_02071644` declared 0x50, its trailing `bx lr` carried as
    `func_02071694` and matched with `void f(void) {}`. Recovering the real 0x54 C
    means that four-byte record leaves.
    """

    ANCHOR, BODY, EPILOGUE = 0x02004000, 0x02071644, 0x02071694

    # Two calls into the body and one into Anchor. NOTHING into the four bytes at the
    # body's tail -- that absence is one of the four things the exception needs.
    VALID_RELOCS = ("from:0x02071354 kind:arm_call to:0x02071644 module:main\n"
                    "from:0x020714f8 kind:arm_call to:0x02071644 module:main\n"
                    "from:0x02071614 kind:arm_call to:0x02004000 module:main\n")

    RECOVERED = "\n".join([
        "void func_02071644(unsigned char *p, int len) {",
        "    for (;;) { if (*p < 9) { (*p)++; return; } *p-- = 0; }",
        "}",
        ""])

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.repo = pathlib.Path(self.tmp.name)
        git(self.repo, "init", "-q", ".")
        (self.repo / "src").mkdir()
        (self.repo / ".gitignore").write_text("extracted/\n", encoding="utf-8")
        self.config = self.repo / "config" / "arm9"
        self.config.mkdir(parents=True)
        self.old_repo = VM.REPO
        VM.REPO = self.repo
        self.addCleanup(setattr, VM, "REPO", self.old_repo)
        for cache in (VM._RELOC_CACHE, VM._ENROLMENT_CACHE,
                      VM._SYMBOL_BASE_CACHE):
            cache.clear()
            self.addCleanup(cache.clear)

    def write(self, rel, text):
        path = self.repo / rel
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8", newline="\n")

    def write_rom(self, word):
        offset = self.EPILOGUE - VM.ARM9_IMAGE_BASE
        image = bytearray(offset + 4)
        image[offset:offset + 4] = word.to_bytes(4, "little")
        (self.repo / "extracted").mkdir(exist_ok=True)
        (self.repo / "extracted" / "arm9_dec.bin").write_bytes(bytes(image))

    def delinks(self, entry, start, end):
        return "\n".join([
            "    .text start:0x02000000 end:0x02100000 kind:code",
            "",
            "src/Anchor.c:",
            "    complete",
            f"    .text start:0x{self.ANCHOR:08x} end:0x{self.ANCHOR + 4:08x}",
            "",
            f"{entry}:",
            "    complete",
            f"    .text start:0x{start:08x} end:0x{end:08x}",
            ""])

    def write_overlay_image(self, number, words):
        d = self.repo / "extracted" / "overlays"
        d.mkdir(parents=True, exist_ok=True)
        image = bytearray()
        for word in words:
            image += word.to_bytes(4, "little")
        (d / f"overlay_{number:04d}.bin").write_bytes(bytes(image))

    def data_only_overlay(self, word):
        """A module declaring NO function, holding four bytes of real .data, with an
        empty `relocs.txt`. Its one data word is the retired ARM9 address."""
        self.write("config/arm9/overlays/ov001/symbols.txt",
                   "data_020ab100 kind:data(any) addr:0x020ab100\n")
        self.write("config/arm9/overlays/ov001/delinks.txt", "\n".join([
            "    .data       start:0x020ab100 end:0x020ab104 kind:data align:4",
            "    .bss        start:0x020ab104 end:0x020ab104 kind:bss align:32",
            ""]))
        self.write("config/arm9/overlays/ov001/relocs.txt", "")
        self.write_overlay_image(1, [word])

    def absorb(self, relocs=VALID_RELOCS, extra_reloc=None, epilogue_word=0xE12FFF1E,
               epilogue_size=0x4, rom=True, orphan_module=False, data_only=None):
        """Commit a base and the fold on top of it, and report on the pair.

        `relocs` is the content of `config/arm9/relocs.txt` in BOTH revisions; None
        leaves the file out of both. `extra_reloc` appends one row naming an address,
        head only. `orphan_module` adds an overlay that declares a function and has no
        `relocs.txt` at all -- an incomplete module inventory. `data_only`, when given,
        adds a data-only ov001 whose single data word is that value and whose
        `relocs.txt` is empty in both snapshots.
        """
        nxt = self.EPILOGUE + epilogue_size
        self.write("config/arm9/symbols.txt", "\n".join([
            f"Anchor kind:function(arm,size=0x4) addr:0x{self.ANCHOR:08x}",
            f"func_02071644 kind:function(arm,size=0x50) addr:0x{self.BODY:08x}",
            f"func_02071694 kind:function(arm,size=0x{epilogue_size:x}) "
            f"addr:0x{self.EPILOGUE:08x}",
            f"func_{nxt:08x} kind:function(arm,size=0x70) addr:0x{nxt:08x}",
            ""]))
        self.write("config/arm9/delinks.txt",
                   self.delinks("src/func_02071694.c", self.EPILOGUE, nxt))
        if relocs is not None:
            self.write("config/arm9/relocs.txt", relocs)
        if orphan_module:
            self.write("config/arm9/overlays/ov001/symbols.txt",
                       "OvOne kind:function(arm,size=0x4) addr:0x020ab110\n")
        if data_only is not None:
            self.data_only_overlay(data_only)
        self.write("src/Anchor.c", "int Anchor(void) { return 0; }\n")
        self.write("src/func_02071644.c",
                   "// NONMATCHING: hand-written asm, does NOT count as matched.\n"
                   "void func_02071644(void) {}\n")
        # The vacuous match: an empty body over a severed tail. Counts today.
        self.write("src/func_02071694.c", "void func_02071694(void)\n{\n}\n")
        self.write(f"src/func_{nxt:08x}.c", f"int func_{nxt:08x}(void) {{ return 1; }}\n")
        base = commit(self.repo, "base", "alice")

        self.write("config/arm9/symbols.txt", "\n".join([
            f"Anchor kind:function(arm,size=0x4) addr:0x{self.ANCHOR:08x}",
            f"func_02071644 kind:function(arm,size=0x{nxt - self.BODY:x}) "
            f"addr:0x{self.BODY:08x}",
            f"func_{nxt:08x} kind:function(arm,size=0x70) addr:0x{nxt:08x}",
            ""]))
        self.write("config/arm9/delinks.txt",
                   self.delinks("src/func_02071644.c", self.BODY, nxt))
        if extra_reloc is not None:
            with open(self.config / "relocs.txt", "a", encoding="utf-8",
                      newline="\n") as fh:
                fh.write(f"from:0x02071620 kind:arm_call to:0x{extra_reloc:08x} "
                         f"module:main\n")
        self.write("src/func_02071644.c", self.RECOVERED)
        os.remove(self.repo / "src" / "func_02071694.c")
        if rom:
            self.write_rom(epilogue_word)
        head = commit(self.repo, "absorb", "bob")
        VM._RELOC_CACHE.clear()
        VM._ENROLMENT_CACHE.clear()
        VM._SYMBOL_BASE_CACHE.clear()
        return VM.build_report(base, head)

    def assertRefused(self, report, defect=None):
        self.assertIsNone(report["repartition"])
        self.assertIn("lost 1 matched function(s)", report["reasons"])
        self.assertEqual(report["coverage"]["delta"]["absorbedMatchedFunctions"], 0)
        self.assertEqual(report["matchedAbsorbed"], [])
        if defect is not None:
            named = [r for r in report["reasons"]
                     if r.startswith("relocation evidence invalid")]
            self.assertEqual(len(named), 1, report["reasons"])
            self.assertIn(defect, named[0])
            self.assertFalse(report["relocationEvidence"]["valid"])

    def test_the_report_allows_the_absorption_and_says_so(self):
        report = self.absorb()
        self.assertEqual(report["reasons"], [])
        self.assertTrue(report["relocationEvidence"]["valid"])
        self.assertEqual(report["coverage"]["delta"]["lostMatchedFunctions"], 0)
        self.assertEqual(report["coverage"]["delta"]["absorbedMatchedFunctions"], 1)
        self.assertEqual([a["name"] for a in report["matchedAbsorbed"]],
                         ["func_02071694"])
        rp = report["repartition"]
        self.assertEqual(rp["kind"], "merge")
        self.assertEqual(rp["functionDelta"], -1)
        # 84 newly compiled bytes, 4 of which the base already compiled.
        self.assertEqual(rp["newExtent"], 0x54)
        self.assertEqual(rp["absorbedBytes"], 0x4)
        self.assertEqual(rp["sourceByteDelta"], 0x50)
        self.assertTrue(any("absorbed by the merge" in w for w in report["warnings"]))
        self.assertIn("| Claims absorbed by the merge |", VM.render_markdown(report))

    def test_one_call_into_the_absorbed_bytes_fails_the_report(self):
        # The same diff, byte for byte, with a single relocation pointing into the four
        # bytes. Now it is an ordinary function someone matched and it may not leave.
        report = self.absorb(extra_reloc=self.EPILOGUE)
        self.assertRefused(report)
        self.assertTrue(report["relocationEvidence"]["valid"])

    def test_relocation_files_missing_from_both_snapshots_fail_closed(self):
        self.assertRefused(self.absorb(relocs=None),
                           "config/arm9/relocs.txt is missing")

    def test_an_empty_relocation_file_fails_closed(self):
        self.assertRefused(self.absorb(relocs=""),
                           "config/arm9/relocs.txt is empty")

    def test_a_malformed_relocation_row_fails_closed(self):
        self.assertRefused(self.absorb(relocs=self.VALID_RELOCS + "not a row\n"),
                           "config/arm9/relocs.txt:4 is not a relocation row")

    def test_a_truncated_relocation_row_fails_closed(self):
        self.assertRefused(
            self.absorb(relocs=self.VALID_RELOCS
                        + "from:0x02071620 kind:arm_call to:0x0207\n"),
            "config/arm9/relocs.txt:4 is not a relocation row")

    def test_a_leading_byte_order_mark_fails_closed(self):
        self.assertRefused(self.absorb(relocs="﻿" + self.VALID_RELOCS),
                           "config/arm9/relocs.txt:1 is not a relocation row")

    def test_leading_whitespace_fails_closed(self):
        self.assertRefused(self.absorb(relocs=" " + self.VALID_RELOCS),
                           "config/arm9/relocs.txt:1 is not a relocation row")

    def test_an_undocumented_destination_module_fails_closed(self):
        self.assertRefused(
            self.absorb(relocs=self.VALID_RELOCS
                        + "from:0x02071620 kind:arm_call to:0x02071694 module:elsewhere\n"),
            "undocumented destination module 'elsewhere'")

    def test_a_data_only_module_with_an_empty_relocation_file_fails_closed(self):
        # No function anywhere in ov001, so the old inventory rule exempted its empty
        # `relocs.txt` outright -- and the four bytes of .data it does hold are the
        # retired ARM9 address. A count of functions cannot see that.
        self.assertRefused(self.absorb(data_only=self.EPILOGUE),
                           "config/arm9/overlays/ov001/relocs.txt is empty")

    def test_a_data_only_module_whose_data_is_zeroed_is_still_legitimate(self):
        # The other half of the same rule: an empty relocation file beside data that
        # holds no address at all is complete evidence, and must stay supported.
        report = self.absorb(data_only=0)
        self.assertEqual(report["reasons"], [])
        self.assertTrue(report["relocationEvidence"]["valid"])
        self.assertEqual(report["coverage"]["delta"]["absorbedMatchedFunctions"], 1)

    def test_an_incomplete_module_inventory_fails_closed(self):
        # A module that declares a function and has no relocation file at all. Every
        # reference it holds is invisible, including references into arm9.
        self.assertRefused(self.absorb(orphan_module=True),
                           "config/arm9/overlays/ov001/relocs.txt is missing")

    def test_a_four_byte_record_that_is_not_a_return_fails_the_report(self):
        # Valid evidence, clean index, exact tail -- and the ROM word is `mov r0, #0`.
        report = self.absorb(epilogue_word=0xE3A00000)
        self.assertRefused(report)
        self.assertTrue(report["relocationEvidence"]["valid"])
        self.assertEqual([r for r in report["reasons"]
                          if r.startswith("relocation evidence invalid")], [])

    def test_an_arbitrary_sized_record_at_the_tail_fails_the_report(self):
        # 0x100 bytes at the exact tail of the new range, with a `bx lr` at its first
        # word and nothing pointing at it. One instruction is the whole claim.
        self.assertRefused(self.absorb(epilogue_size=0x100))

    def test_without_the_rom_image_the_report_fails_closed(self):
        report = self.absorb(rom=False)
        self.assertRefused(report)
        self.assertIn("ROM image unavailable, so nothing matched may leave: arm9",
                      report["reasons"])


class DataLeadingOverlayImageBase(unittest.TestCase):
    """The overlay word reader, on an overlay whose first symbol is DATA.

    `modules._overlay_base` -- the derivation this reader claims to follow -- takes the
    lowest address of ANY symbol the module declares. Reading the base off the first
    FUNCTION address instead shifts every read by the distance between them, so a
    module with four bytes of data in front of its first function answers each query
    with the PREVIOUS word. Put a `bx lr` in front of a record whose own word is
    `mov r0, #0` and the guard reads a return that is not there.

    The fixture is the same absorption shape as `AbsorbedEpilogueThroughBuildReport`,
    moved into `ov001` and given a leading data symbol.
    """

    DATA, BODY, EPILOGUE, NEXT = 0x020ab100, 0x020ab104, 0x020ab154, 0x020ab158
    END = 0x020ab1c8
    ANCHOR = 0x02004000

    RELOCS_MAIN = "from:0x02004000 kind:arm_call to:0x02004000 module:main\n"
    # Two calls into the overlay body, none into the four bytes at its tail.
    RELOCS_OV = ("from:0x020ab160 kind:arm_call to:0x020ab104 module:overlay(1)\n"
                 "from:0x020ab164 kind:arm_call to:0x020ab158 module:overlay(1)\n")

    RECOVERED = "\n".join([
        "void func_020ab104(unsigned char *p, int len) {",
        "    for (;;) { if (*p < 9) { (*p)++; return; } *p-- = 0; }",
        "}",
        ""])

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.repo = pathlib.Path(self.tmp.name)
        git(self.repo, "init", "-q", ".")
        (self.repo / "src").mkdir()
        (self.repo / ".gitignore").write_text("extracted/\n", encoding="utf-8")
        self.old_repo = VM.REPO
        VM.REPO = self.repo
        self.addCleanup(setattr, VM, "REPO", self.old_repo)
        for cache in (VM._RELOC_CACHE, VM._ENROLMENT_CACHE,
                      VM._SYMBOL_BASE_CACHE):
            cache.clear()
            self.addCleanup(cache.clear)

    def write(self, rel, text):
        path = self.repo / rel
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8", newline="\n")

    def write_image(self, record_word):
        """The overlay's image, based at its LOWEST SYMBOL -- the data one.

        `bx lr` sits at 0x020ab150, one word in front of the record, which is where a
        reader based on the first function lands when asked for 0x020ab154.
        """
        image = bytearray(self.END - self.DATA)
        image[0x50:0x54] = (0xE12FFF1E).to_bytes(4, "little")
        image[self.EPILOGUE - self.DATA:self.EPILOGUE - self.DATA + 4] = \
            record_word.to_bytes(4, "little")
        d = self.repo / "extracted" / "overlays"
        d.mkdir(parents=True, exist_ok=True)
        (d / "overlay_0001.bin").write_bytes(bytes(image))

    def ov_delinks(self, entry, start, end):
        return "\n".join([
            "    .text       start:0x020ab100 end:0x020ab1c8 kind:code align:4",
            "",
            f"{entry}:",
            "    complete",
            f"    .text start:0x{start:08x} end:0x{end:08x}",
            ""])

    def absorb(self, record_word=0xE3A00000):
        self.write("config/arm9/symbols.txt",
                   f"Anchor kind:function(arm,size=0x4) addr:0x{self.ANCHOR:08x}\n")
        self.write("config/arm9/delinks.txt", "\n".join([
            "    .text start:0x02004000 end:0x02004004 kind:code",
            "",
            "src/Anchor.c:",
            "    complete",
            f"    .text start:0x{self.ANCHOR:08x} end:0x{self.ANCHOR + 4:08x}",
            ""]))
        self.write("config/arm9/relocs.txt", self.RELOCS_MAIN)
        self.write("src/Anchor.c", "int Anchor(void) { return 0; }\n")

        self.write("config/arm9/overlays/ov001/symbols.txt", "\n".join([
            f"data_{self.DATA:08x} kind:data(any) addr:0x{self.DATA:08x}",
            f"func_{self.BODY:08x} kind:function(arm,size=0x50) "
            f"addr:0x{self.BODY:08x}",
            f"func_{self.EPILOGUE:08x} kind:function(arm,size=0x4) "
            f"addr:0x{self.EPILOGUE:08x}",
            f"func_{self.NEXT:08x} kind:function(arm,size=0x70) "
            f"addr:0x{self.NEXT:08x}",
            ""]))
        self.write("config/arm9/overlays/ov001/delinks.txt",
                   self.ov_delinks(f"src/func_{self.EPILOGUE:08x}.c",
                                   self.EPILOGUE, self.NEXT))
        self.write("config/arm9/overlays/ov001/relocs.txt", self.RELOCS_OV)
        self.write(f"src/func_{self.BODY:08x}.c",
                   "// NONMATCHING: hand-written asm, does NOT count as matched.\n"
                   f"void func_{self.BODY:08x}(void) {{}}\n")
        self.write(f"src/func_{self.EPILOGUE:08x}.c",
                   f"void func_{self.EPILOGUE:08x}(void)\n{{\n}}\n")
        self.write(f"src/func_{self.NEXT:08x}.c",
                   f"int func_{self.NEXT:08x}(void) {{ return 1; }}\n")
        base = commit(self.repo, "base", "alice")

        self.write("config/arm9/overlays/ov001/symbols.txt", "\n".join([
            f"data_{self.DATA:08x} kind:data(any) addr:0x{self.DATA:08x}",
            f"func_{self.BODY:08x} kind:function(arm,size=0x54) "
            f"addr:0x{self.BODY:08x}",
            f"func_{self.NEXT:08x} kind:function(arm,size=0x70) "
            f"addr:0x{self.NEXT:08x}",
            ""]))
        self.write("config/arm9/overlays/ov001/delinks.txt",
                   self.ov_delinks(f"src/func_{self.BODY:08x}.c",
                                   self.BODY, self.NEXT))
        self.write(f"src/func_{self.BODY:08x}.c", self.RECOVERED)
        os.remove(self.repo / "src" / f"func_{self.EPILOGUE:08x}.c")
        self.write_image(record_word)
        head = commit(self.repo, "absorb", "bob")
        VM._RELOC_CACHE.clear()
        VM._ENROLMENT_CACHE.clear()
        VM._SYMBOL_BASE_CACHE.clear()
        self.base, self.head = base, head
        return VM.build_report(base, head)

    def test_the_reader_answers_from_the_lowest_symbol_not_the_first_function(self):
        self.absorb()
        read = VM._rom_word_reader(self.head)
        self.assertEqual(read("ov001", self.EPILOGUE), 0xE3A00000)
        self.assertEqual(read("ov001", self.EPILOGUE - 4), 0xE12FFF1E)

    def test_a_non_return_tail_in_a_data_leading_overlay_is_refused(self):
        report = self.absorb()
        self.assertIsNone(report["repartition"])
        self.assertIn("lost 1 matched function(s)", report["reasons"])
        self.assertEqual(report["coverage"]["delta"]["absorbedMatchedFunctions"], 0)
        self.assertTrue(report["relocationEvidence"]["valid"])

    def test_the_same_overlay_fixture_with_a_real_tail_return_still_lands(self):
        # The positive control for the two above: identical tree, and the record's own
        # word IS the return. Without it, the refusal above could come from anything in
        # the fixture rather than from the word the reader now reads.
        report = self.absorb(record_word=0xE12FFF1E)
        self.assertEqual(report["reasons"], [])
        self.assertEqual(report["coverage"]["delta"]["absorbedMatchedFunctions"], 1)
        self.assertEqual([a["name"] for a in report["matchedAbsorbed"]],
                         [f"func_{self.EPILOGUE:08x}"])
