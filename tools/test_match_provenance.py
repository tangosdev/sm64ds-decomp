#!/usr/bin/env python3
"""Tests for match provenance validation, slugify, repo discovery, ledger."""
from __future__ import annotations

import json
import os
import pathlib
import subprocess
import sys
import tempfile
import unittest

TOOLS = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS))

from match_provenance import (  # noqa: E402
    ProvenanceError,
    append_ledger_row,
    configure,
    is_agent_credit,
    is_repo_root,
    load_ledger,
    make_id,
    normalize_provenance,
    provenance_from_cli_args,
    resolve_credit_author,
    resolve_repo,
    slugify_token,
    validate_match_provenance,
)


class ValidateProvenance(unittest.TestCase):
    def test_human_ok(self):
        self.assertIsNone(validate_match_provenance({"kind": "human"}))
        self.assertIsNone(
            validate_match_provenance(
                {"kind": "human", "by": "lunavyqo", "note": "hand"}
            )
        )

    def test_human_bad_field(self):
        err = validate_match_provenance({"kind": "human", "model": "x"})
        self.assertIsNotNone(err)

    def test_ai_complete_ok(self):
        self.assertIsNone(
            validate_match_provenance(
                {
                    "kind": "ai",
                    "model": "claude-opus-4",
                    "reasoning": "high",
                    "harness": "cursor-agent",
                }
            )
        )

    def test_ai_missing_reasoning(self):
        err = validate_match_provenance(
            {
                "kind": "ai",
                "model": "claude-opus-4",
                "reasoning": "",
                "harness": "fanout-v3",
            }
        )
        self.assertIsNotNone(err)
        self.assertIn("reasoning", err)

    def test_slugify_display_names(self):
        self.assertEqual(slugify_token("Grok 4.5", field="model"), "grok-4.5")
        self.assertEqual(slugify_token("Grok Build", field="harness"), "grok-build")

    def test_normalize_accepts_display_names(self):
        p = normalize_provenance(
            {
                "kind": "ai",
                "model": "Grok 4.5",
                "reasoning": "High",
                "harness": "Grok Build",
            }
        )
        self.assertEqual(p["model"], "grok-4.5")
        self.assertEqual(p["reasoning"], "high")
        self.assertEqual(p["harness"], "grok-build")

    def test_cli_display_names(self):
        p = provenance_from_cli_args(
            kind="ai",
            model="Grok 4.5",
            reasoning="high",
            harness="Grok Build",
        )
        self.assertEqual(p["model"], "grok-4.5")
        self.assertEqual(p["harness"], "grok-build")
        self.assertNotIn("by", p)

    def test_cli_ai_incomplete_raises(self):
        with self.assertRaises(ProvenanceError):
            provenance_from_cli_args(kind="ai", model="m", reasoning=None, harness="h")

    def test_cli_human_ok(self):
        p = provenance_from_cli_args(kind="human", note="hand")
        self.assertEqual(p["kind"], "human")
        self.assertEqual(p.get("note"), "hand")
        self.assertNotIn("by", p)

    def test_normalize_strips_legacy_by(self):
        p = normalize_provenance(
            {
                "kind": "ai",
                "model": "x",
                "reasoning": "high",
                "harness": "h",
                "by": "should-not-stay",
            }
        )
        self.assertNotIn("by", p)


class RepoDiscovery(unittest.TestCase):
    def test_is_repo_root_real(self):
        # tools/ is inside the real tree when tests run in-repo
        root = TOOLS.parent
        self.assertTrue(is_repo_root(root))

    def test_resolve_from_explicit(self):
        root = TOOLS.parent
        self.assertEqual(resolve_repo(root), root.resolve())

    def test_resolve_from_env(self):
        root = TOOLS.parent.resolve()
        old = os.environ.get("DECOMP_ROOT")
        os.environ["DECOMP_ROOT"] = str(root)
        try:
            # Force walk to use env even if cwd is weird: pass no explicit
            # but env is checked after explicit
            self.assertEqual(resolve_repo(None), root)
        finally:
            if old is None:
                os.environ.pop("DECOMP_ROOT", None)
            else:
                os.environ["DECOMP_ROOT"] = old

    def test_configure_sets_ledger_under_repo(self):
        root = configure(TOOLS.parent)
        from match_provenance import ledger_path

        self.assertEqual(ledger_path(), root / "config" / "match_provenance.jsonl")

    def test_temp_copy_needs_repo(self):
        """Script under /tmp is not a root; --repo must be used."""
        with tempfile.TemporaryDirectory() as td:
            fake = pathlib.Path(td)
            self.assertFalse(is_repo_root(fake))
            with self.assertRaises(SystemExit) as cm:
                resolve_repo(fake)
            self.assertIn("not a decomp root", str(cm.exception))


class LedgerAndAtlas(unittest.TestCase):
    def test_append_and_load(self):
        with tempfile.TemporaryDirectory() as td:
            path = pathlib.Path(td) / "prov.jsonl"
            append_ledger_row(
                module="arm9",
                addr=0x02001B44,
                name="func_02001b44",
                provenance={"kind": "human"},
                author="tester",
                src_path="src/arm9/func_02001b44.c",
                path=path,
            )
            append_ledger_row(
                module="arm9",
                addr=0x020092AC,
                name="func_020092ac",
                provenance={
                    "kind": "ai",
                    "model": "Grok 4.5",
                    "reasoning": "high",
                    "harness": "Grok Build",
                },
                author="lunavyqo",
                src_path="src/arm9/func_020092ac.c",
                path=path,
            )
            led = load_ledger(path)
            self.assertIn(make_id("arm9", 0x02001B44), led)
            self.assertEqual(
                led[make_id("arm9", 0x02001B44)]["matchProvenance"]["kind"], "human"
            )
            self.assertEqual(led[make_id("arm9", 0x02001B44)]["author"], "tester")
            ai = led[make_id("arm9", 0x020092AC)]["matchProvenance"]
            self.assertEqual(ai["kind"], "ai")
            self.assertEqual(ai["model"], "grok-4.5")
            self.assertEqual(ai["harness"], "grok-build")
            self.assertNotIn("by", ai)
            self.assertEqual(led[make_id("arm9", 0x020092AC)]["author"], "lunavyqo")

    def test_load_rejects_incomplete_ai(self):
        with tempfile.TemporaryDirectory() as td:
            path = pathlib.Path(td) / "bad.jsonl"
            path.write_text(
                json.dumps(
                    {
                        "id": "arm9:0x02000000",
                        "matchProvenance": {
                            "kind": "ai",
                            "model": "x",
                            "reasoning": "high",
                            # harness missing
                        },
                    }
                )
                + "\n",
                encoding="utf-8",
            )
            with self.assertRaises(ProvenanceError):
                load_ledger(path)

    def test_bank_cli_slugifies_and_accepts_display_names(self):
        bank = TOOLS / "bank.py"
        repo = TOOLS.parent
        # Prefer a real banked sample if present
        candidates = list((repo / "src" / "arm9").glob("func_*.c"))
        if not candidates:
            self.skipTest("no src samples")
        src = candidates[0]
        r = subprocess.run(
            [
                sys.executable,
                str(bank),
                "--repo",
                str(repo),
                "--src",
                str(src.relative_to(repo)),
                "--kind",
                "ai",
                "--model",
                "Grok 4.5",
                "--reasoning",
                "high",
                "--harness",
                "Grok Build",
                "--no-verify",
                "--session-scope",
                "focused",
                "--batch-size",
                "1",
                "--dry-run",
            ],
            cwd="/tmp",  # not the repo — forces --repo path
            capture_output=True,
            text=True,
        )
        self.assertEqual(r.returncode, 0, r.stderr + r.stdout)
        self.assertIn("grok-4.5", r.stdout)
        self.assertIn("grok-build", r.stdout)
        self.assertIn("REPO =", r.stdout)
        # Who is git credit, not agent — dry-run should not force author=grok
        self.assertNotIn("author='grok'", r.stdout)

    def test_bank_cli_rejects_incomplete_ai(self):
        bank = TOOLS / "bank.py"
        repo = TOOLS.parent
        candidates = list((repo / "src" / "arm9").glob("func_*.c"))
        if not candidates:
            self.skipTest("no src samples")
        src = candidates[0]
        r = subprocess.run(
            [
                sys.executable,
                str(bank),
                "--repo",
                str(repo),
                "--src",
                str(src.relative_to(repo)),
                "--kind",
                "ai",
                "--model",
                "claude-opus-4",
                "--reasoning",
                "high",
                # harness intentionally missing
                "--no-verify",
                "--session-scope",
                "focused",
                "--batch-size",
                "1",
            ],
            cwd=repo,
            capture_output=True,
            text=True,
        )
        self.assertEqual(r.returncode, 2)
        self.assertIn("provenance", (r.stderr + r.stdout).lower())

    def test_agent_names_are_not_credit(self):
        self.assertTrue(is_agent_credit("grok"))
        self.assertTrue(is_agent_credit("grok-4.5"))
        self.assertTrue(is_agent_credit("claude-opus-4"))
        self.assertFalse(is_agent_credit("lunavyqo"))
        # explicit grok must not win over a real path with git history
        who = resolve_credit_author(
            "src/arm9/func_020092ac.c",
            explicit="grok",
            root=TOOLS.parent,
        )
        self.assertNotEqual(who, "grok")
        if who:
            self.assertFalse(is_agent_credit(who))

if __name__ == "__main__":
    unittest.main()
