"""Reject tool drift before a wrong dsd can generate build artifacts."""
import hashlib
import json
import pathlib
import subprocess
import sys
import tempfile
import unittest
from unittest import mock
from types import SimpleNamespace

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import dsd_pin as PIN  # noqa: E402


class DsdPinTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.executable = pathlib.Path(self.temp.name) / "dsd.exe"
        self.body = b"synthetic executable identity"
        self.executable.write_bytes(self.body)

    def response(self, output="ds-decomp-cli 0.11.0\n", returncode=0, stderr=""):
        return subprocess.CompletedProcess([], returncode, stdout=output, stderr=stderr)

    def assert_actionable(self, error):
        text = str(error)
        self.assertIn("expected ds-decomp-cli 0.11.0", text)
        self.assertIn(str(self.executable), text)
        self.assertIn("actual", text)
        self.assertIn("CONTRIBUTING.md#first-time-setup", text)

    def test_exact_version_records_executable_path_and_content_hash(self):
        with mock.patch.object(PIN.subprocess, "run", return_value=self.response()) as run:
            result = PIN.check(self.executable)
        self.assertEqual(result, {"path": str(self.executable), "version": "0.11.0",
                                  "sha256": hashlib.sha256(self.body).hexdigest()})
        self.assertEqual(run.call_args.args[0], [str(self.executable), "--version"])
        self.assertEqual(run.call_args.kwargs["timeout"], 10)
        self.assertEqual(self.executable.read_bytes(), self.body)

    def test_crlf_version_output_is_accepted(self):
        with mock.patch.object(PIN.subprocess, "run", return_value=self.response("ds-decomp-cli 0.11.0\r\n")):
            self.assertEqual(PIN.check(self.executable)["version"], "0.11.0")

    def test_newer_older_and_prerelease_versions_are_not_the_pin(self):
        for version in ("0.12.0", "0.10.0", "0.11.0-dev"):
            with self.subTest(version=version), mock.patch.object(
                    PIN.subprocess, "run", return_value=self.response(f"ds-decomp-cli {version}\n")):
                with self.assertRaises(PIN.DsdPinError) as caught:
                    PIN.check(self.executable)
                self.assert_actionable(caught.exception)
                self.assertIn(version, str(caught.exception))

    def test_empty_or_unrecognized_output_cannot_pass_by_containing_the_version(self):
        for output in ("", "dsd 0.11.0", "warning\nds-decomp-cli 0.11.0", "ds-decomp-cli 0.11.00"):
            with self.subTest(output=output), mock.patch.object(
                    PIN.subprocess, "run", return_value=self.response(output)):
                with self.assertRaises(PIN.DsdPinError) as caught:
                    PIN.check(self.executable)
                self.assert_actionable(caught.exception)

    def test_nonzero_exit_refuses_even_when_stdout_names_the_pin(self):
        with mock.patch.object(PIN.subprocess, "run", return_value=self.response(returncode=7, stderr="broken tool")):
            with self.assertRaises(PIN.DsdPinError) as caught:
                PIN.check(self.executable)
        self.assert_actionable(caught.exception)
        self.assertIn("exited 7", str(caught.exception))
        self.assertIn("broken tool", str(caught.exception))

    def test_missing_executable_fails_before_starting_a_process(self):
        self.executable.unlink()
        with mock.patch.object(PIN.subprocess, "run") as run:
            with self.assertRaises(PIN.DsdPinError) as caught:
                PIN.check(self.executable)
        run.assert_not_called()
        self.assert_actionable(caught.exception)

    def test_unreadable_executable_is_a_pin_failure(self):
        with mock.patch.object(pathlib.Path, "read_bytes", side_effect=PermissionError("denied")), \
                mock.patch.object(PIN.subprocess, "run") as run:
            with self.assertRaises(PIN.DsdPinError) as caught:
                PIN.check(self.executable)
        run.assert_not_called()
        self.assert_actionable(caught.exception)
        self.assertIn("unreadable", str(caught.exception))

    def test_process_launch_failure_is_actionable(self):
        with mock.patch.object(PIN.subprocess, "run", side_effect=OSError("cannot execute")):
            with self.assertRaises(PIN.DsdPinError) as caught:
                PIN.check(self.executable)
        self.assert_actionable(caught.exception)
        self.assertIn("could not run", str(caught.exception))

    def test_version_query_timeout_is_bounded_and_actionable(self):
        with mock.patch.object(PIN.subprocess, "run",
                               side_effect=subprocess.TimeoutExpired([str(self.executable)], 10)):
            with self.assertRaises(PIN.DsdPinError) as caught:
                PIN.check(self.executable)
        self.assert_actionable(caught.exception)
        self.assertIn("timed out after 10s", str(caught.exception))

    def test_executable_replaced_during_query_cannot_inherit_old_version(self):
        def replace(*args, **kwargs):
            self.executable.write_bytes(b"replacement executable")
            return self.response()

        with mock.patch.object(PIN.subprocess, "run", side_effect=replace):
            with self.assertRaises(PIN.DsdPinError) as caught:
                PIN.check(self.executable)
        self.assert_actionable(caught.exception)
        self.assertIn("changed during", str(caught.exception))

    def test_executable_removed_during_query_cannot_pass(self):
        def remove(*args, **kwargs):
            self.executable.unlink()
            return self.response()

        with mock.patch.object(PIN.subprocess, "run", side_effect=remove):
            with self.assertRaises(PIN.DsdPinError) as caught:
                PIN.check(self.executable)
        self.assert_actionable(caught.exception)
        self.assertIn("became unreadable", str(caught.exception))


class BuildEntrypointPinTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.repo = pathlib.Path(self.temp.name)
        self.build = self.repo / "build"
        self.build.mkdir()
        self.failure = PIN.DsdPinError("fixture: expected pinned dsd, actual newer version")

    def test_rombuild_records_preflight_failure_before_build_input_preparation(self):
        import rombuild as RB
        sentinel = self.build / "existing-output"
        sentinel.write_bytes(b"keep")
        report_path = self.build / "report.json"
        with mock.patch.object(RB, "BUILD", self.build), \
                mock.patch.object(RB.DSP, "check", side_effect=self.failure) as check, \
                mock.patch.object(RB.RP, "prepare_profile") as prepare, \
                mock.patch.object(RB.LAY, "check") as layout, \
                mock.patch.object(RB, "run") as run, \
                mock.patch.object(sys, "argv", ["rombuild.py", "--report-json", str(report_path)]), \
                mock.patch("builtins.print"):
            self.assertEqual(RB.main(), 1)
        check.assert_called_once_with(RB.DSD)
        prepare.assert_not_called()
        layout.assert_not_called()
        run.assert_not_called()
        report = json.loads(report_path.read_text())
        self.assertEqual(report["status"], "error")
        self.assertEqual(report["failure"]["phase"], "preflight")
        self.assertIn(str(self.failure), report["failure"]["output"])
        self.assertEqual(report["phases"], [])
        self.assertEqual(sentinel.read_bytes(), b"keep")

    def test_linkcheck_refuses_before_cleaning_existing_scratch(self):
        import tubuild as TB
        scratch = self.build / "tu" / "_baseline" / "link"
        scratch.mkdir(parents=True)
        sentinel = scratch / "existing-output"
        sentinel.write_bytes(b"keep")
        args = SimpleNamespace(id=None, baseline=True, module="arm9", clean=True)
        with mock.patch.object(TB, "REPO", self.repo), \
                mock.patch.object(TB, "BASELINE_LINK", scratch), \
                mock.patch.object(TB, "load_manifest", return_value={}), \
                mock.patch.object(TB.DSP, "check", side_effect=self.failure) as check, \
                mock.patch.object(TB.shutil, "rmtree") as remove, \
                mock.patch.object(TB.RP, "prepare_profile") as prepare, \
                mock.patch("builtins.print"):
            with self.assertRaisesRegex(SystemExit, "expected pinned dsd"):
                TB.cmd_linkcheck(args)
        check.assert_called_once_with(TB.RB.DSD)
        remove.assert_not_called()
        prepare.assert_not_called()
        self.assertEqual(list(scratch.iterdir()), [sentinel])
        self.assertEqual(sentinel.read_bytes(), b"keep")


if __name__ == "__main__":
    unittest.main()
