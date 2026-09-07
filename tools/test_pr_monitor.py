import datetime as dt
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from pr_monitor import classify  # noqa: E402


NOW = dt.datetime(2026, 8, 23, 12, tzinfo=dt.timezone.utc)


def pr(**overrides):
    base = {
        "number": 1, "title": "test", "isDraft": False,
        "mergeStateStatus": "CLEAN", "createdAt": "2026-08-23T11:00:00Z",
        "statusCheckRollup": [{"name": "validate", "status": "COMPLETED",
                                "conclusion": "SUCCESS"}],
    }
    base.update(overrides)
    return base


def test_conflict_wins_over_passing_checks():
    assert classify(pr(mergeStateStatus="DIRTY"), NOW, 15, set())[0] == "conflict"


def test_failed_check_needs_repair():
    bad = [{"name": "references", "status": "COMPLETED", "conclusion": "FAILURE"}]
    assert classify(pr(statusCheckRollup=bad), NOW, 15, set())[0] == "failed"


def test_reviewable_requires_age_clean_merge_and_completed_checks():
    assert classify(pr(), NOW, 15, set())[0] == "reviewable"
    assert classify(pr(createdAt="2026-08-23T11:50:00Z"), NOW, 15, set())[0] == "cooldown"
    waiting = [{"name": "validate", "status": "IN_PROGRESS", "conclusion": ""}]
    assert classify(pr(statusCheckRollup=waiting), NOW, 15, set())[0] == "waiting"


def test_skipped_and_drafts_are_never_review_candidates():
    assert classify(pr(number=1450), NOW, 15, {1450})[0] == "skipped"
    assert classify(pr(isDraft=True), NOW, 15, set())[0] == "draft"
