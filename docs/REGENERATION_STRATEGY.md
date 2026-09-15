# Regeneration Strategy

This is the intended contract for the rebuild engine. The current implementation is only a partial implementation of it.

## States

A feature has one model state: `clean`, `dirty`, `suppressed`, `failed`, or `blocked`. A rebuild pass has `not_started`, `running`, `succeeded`, `failed`, or `cancelled`. Generated geometry is cache data and must not be confused with saved parameters.

## Graph semantics

An edge `A -> B` means B requires A's result. Missing dependencies, duplicate edges, and self-edges are diagnostics. Dirty state propagates from a changed node to every reachable dependent. Cycles are rejected before mutation.

## Scheduling

The engine computes a dependency-first topological order. Ties are resolved by stable document order or stable object ID, never unordered-container iteration. A partial rebuild evaluates only dirty nodes and their required upstream nodes.

## Failure policy

A feature failure records a diagnostic containing feature ID, operation, inputs, and cause. The feature becomes `failed`; dependent features become `blocked` unless they have an explicitly valid fallback. The document remains structurally valid. Generated results use either an atomic commit or the last valid result, chosen by the transaction contract.

## Cancellation and transactions

Cancellation is checked between feature evaluations and must leave the document at the pre-rebuild committed state. Rebuild is not itself a user transaction. A document transaction owns model edits, records the before/after state, and publishes one change event on commit.

## Diagnostics and testing

Diagnostics must identify the cycle path, failed feature, blocked dependents, and the last successful feature. Tests must cover stable order, transitive dirtiness, cycles, failure preservation, suppression, cancellation, partial rebuild, and save/reload/rebuild.
