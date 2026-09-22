# Class migration: agent entry point

The operational workflow is consolidated in [agents/PIPELINE.md](agents/PIPELINE.md).
Use [agents/LAUNCH.md](agents/LAUNCH.md) for launch/resume prompts and
[agents/queue-v2.md](agents/queue-v2.md) for current ownership and handoff commands.
Legacy Redis locks and v1 claim/release recipes are not the active protocol.

Choose the technical reference for the assigned work:

- [Real C++ migration](real-cpp-migration-runbook.md): source forms, lifetime,
  shared-header consumers, and byte/relocation proof for a method or class slice.
- [TU promotion](tu-promotion-conventions.md): consolidate the evidenced compiler
  input, enroll it in production `src/`, retire absorbed sources, and carry the
  reviewed result through its authorized publication/integration endpoint.
- [Class-form compiler observations](cpp-class-form.md):
  measured source forms to test, including destructor and vtable emission.
- [Partitioned experiments](agent-partitioned-tu-workflow.md): only when the
  candidate needs explicit output partitioning or ownership investigation.

Resume existing branches, candidates and evidence. A promotion assignment does not
require a preliminary folder move, a per-function backport PR, or a shadow-only PR.
Use temporary staging only for the concrete reasons in the promotion workflow;
record a measured blocker and continuation owner if production cannot be completed.
