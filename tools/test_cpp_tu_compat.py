"""Synthetic compatibility contract for consolidating functions into one C++ TU."""
import contextlib
import io
import json
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import cpp_tu_compat as CTC  # noqa: E402


class CppTuCompatibility(unittest.TestCase):
    def setUp(self):
        self.report = CTC.audit()
        self.rows = {row["surface"]: row for row in self.report["surfaces"]}

    def test_contract_covers_every_path_sensitive_surface(self):
        self.assertEqual(set(self.rows), {
            "srcpath", "enroll", "eligible", "rombuild", "validate_merge",
            "tiers", "langmode_audit", "attribution", "port_refcheck",
        })

    def test_current_readiness_is_explicit_and_behavior_backed(self):
        self.assertEqual(
            {name: row["status"] for name, row in self.rows.items()},
            {
                "srcpath": "ready",
                "enroll": "ready",
                "eligible": "gap",
                "rombuild": "ready",
                "validate_merge": "ready",
                "tiers": "gap",
                "langmode_audit": "gap",
                "attribution": "gap",
                "port_refcheck": "blocked",
            })
        self.assertFalse(self.report["productionCompatible"])
        self.assertFalse(self.report["allSurfacesReady"])
        self.assertEqual(self.report["blockers"], [
            "port_refcheck",
        ])
        self.assertEqual(self.report["policyAndMetricGaps"], [
            "eligible", "tiers", "langmode_audit", "attribution",
        ])

        self.assertEqual(self.rows["srcpath"]["evidence"]["symbolsForSource"],
                         list(CTC.SYMBOLS))
        self.assertEqual(self.rows["enroll"]["evidence"]["roundTripEntries"], 1)
        self.assertEqual(self.rows["enroll"]["evidence"]["roundTripCompleteMarks"], 1)
        self.assertEqual(self.rows["eligible"]["evidence"]["isolatedSymbols"],
                         list(CTC.SYMBOLS))
        self.assertEqual(len(self.rows["rombuild"]["evidence"]["enrolledSymbolMap"]), 1)
        self.assertEqual(
            self.rows["rombuild"]["evidence"]["enrolledSymbolMap"][CTC.SOURCE],
            list(CTC.SYMBOLS))
        self.assertEqual(self.rows["rombuild"]["evidence"]["symbolsKeptByIsolation"],
                         [list(CTC.SYMBOLS)])
        self.assertEqual(self.rows["validate_merge"]["evidence"]["sourceFunctions"], 2)
        self.assertEqual(self.rows["validate_merge"]["evidence"]["completeRanges"], 1)
        self.assertEqual(self.rows["tiers"]["evidence"]["legacyFiles"], 2)
        self.assertEqual(self.rows["tiers"]["evidence"]["mergedFiles"], 1)
        self.assertEqual(self.rows["tiers"]["evidence"]["legacyConverted"], 2)
        self.assertEqual(self.rows["tiers"]["evidence"]["mergedConverted"], 1)
        self.assertEqual(
            self.rows["langmode_audit"]["evidence"]["mangledDefinitionsCounted"], 0)
        self.assertEqual(
            set(self.rows["attribution"]["evidence"]["resolvedByFunction"].values()),
            {"alice"})
        self.assertEqual(self.rows["port_refcheck"]["evidence"]["manifestFailures"], 0)
        self.assertEqual(self.rows["port_refcheck"]["evidence"]["cmakeSymbolFailures"], 2)

    def test_default_is_an_informational_command_and_strict_mode_is_a_gate(self):
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            self.assertEqual(CTC.main([]), 0)
        self.assertIn("C++ two-function-TU compatibility: BLOCKED", out.getvalue())

        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            self.assertEqual(CTC.main(["--require-ready"]), 1)

    def test_json_output_is_machine_readable(self):
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            self.assertEqual(CTC.main(["--json"]), 0)
        payload = json.loads(out.getvalue())
        self.assertEqual(payload["schemaVersion"], 1)
        self.assertEqual(payload["fixture"]["symbols"], list(CTC.SYMBOLS))


if __name__ == "__main__":
    unittest.main()
