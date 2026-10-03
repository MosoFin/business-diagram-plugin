# Scenario visuals

Read this **before** choosing a diagram type or writing JSON. It is the map from a real question to the picture in `examples/` and the visual treatment that picture ships. Tiny 3–4 node samples are not scenarios. Do not copy a spec's company, amounts, or IDs; copy type, density, and the visual fields below.

`node bin/mosofin.mjs guide "<question>" --json` may break a tie on **type**. For preset, logo/box, and motion, this file wins. `recipes/scenarios.mjs` `presentation` is wrong in the three rows marked under Disagreements.

## How to apply a match

A matched row is an explicit visual request. Author its fields; do not also invent a different preset.

- **Preset.** `classic` means omit `meta.visual_preset` (the viewer opens classic in both light and dark). Write `signal-flow` or `blueprint` only when the row names them. No shipped example uses `editorial`; set that only when the user asks for a publication or launch-note look.
- **Node.** `box` means omit `meta.node_style`. `logo` means `meta.node_style: "logo"`: brand mark large, label beneath. A node with no mark stays an outlined box. Never invent a logo. Logo is for "which product is this", not for every architecture.
- **Motion.** `trace` means `meta.animation: "trace"` (finite Live/Still; a still frame must still be complete). `static` means omit `animation`. Trace follows the main path, the message order, or the state rail. It does not add facts.
- **Quality.** `showcase` on every delivered picture. The checkout delta pair is the only exception: those files are comparator inputs and omit `quality_profile`. Do not imitate that omission.
- **Layout.** Use the signature shape (lanes, stages, segments, boundaries, roof). At most five `views` when the picture is a walk. Do not set `engineering_profile` except on deployment ownership.
- **No match.** Omit preset, animation, and `node_style`. Keep `quality_profile: "showcase"`. Choose the type from the question, then the authoring contract.

Presets only restyle. `signal-flow` is luminous and motion-forward. `blueprint` is a high-contrast engineering review: grid, squared corners, no glow. `classic` is the stable default. Color mode does not change the preset.

## Business

| Scenario | Type | The picture is for | Spec | Preset | Node | Motion | Signature layout |
|---|---|---|---|---|---|---|---|
| Business operating map | architecture | How the whole business runs and which system owns each part | `examples/business-operating-map.architecture.json` (same treatment in `business-operating-map-logo.architecture.json`) | classic | logo | trace | Three boundaries: procure-to-pay, order-to-cash, record-to-report. ~12 components. Goods one way, cash to the bank, books along the bottom |
| Business handoffs | workflow | Who owns each step, and where work changes hands | no own file; same treatment as the close runbook | classic | box | trace | One lane per team or system, phases left to right, a main path, an exception lane for stalls |
| Business pillars | pillars | What holds the business up, on one page | `examples/northline-operating-pillars.pillars.json` | classic | box | static | Roof = one sentence. 3–6 pillars, one system of record each, items are facts not tasks. Foundation = entity and truth rules. No edges |
| Order journey | sequence | One order in time across every system it touches | `examples/northline-order-path.sequence.json` | classic | box | trace | Participants across the stack. Segments: order and charge, later payout, books catch-up. Waits are their own messages |

## Finance

Numbers still come only from the user or `FINANCE-BRIEF.md`. The visual row does not permit invented amounts.

| Scenario | Type | The picture is for | Spec | Preset | Node | Motion | Signature layout |
|---|---|---|---|---|---|---|---|
| Money map | architecture | How money reaches the books, and the source of truth for orders, cash, and books | `examples/northline-money-map.architecture.json` | classic | logo | trace | Short order-to-cash rail. One boundary where payout becomes bank cash. Tax and gift cards off the revenue rail |
| Order path | sequence | One order or payout in time across commerce, payments, bank, and books | `examples/northline-order-path.sequence.json` | classic | box | trace | Same shape as order journey, told as capture, fee, payout batch, then the ledger split |
| Revenue walk | dataflow | Why commerce, the processor, and the books disagree | `examples/northline-revenue-walk.dataflow.json` | classic | box | trace | Stages: gross orders, deductions, net sales, books. Tax and gift cards read as not-income. Fees are an expense branch, not a contra |
| Month-end close | workflow | Close in order, and who owns each gate | `examples/northline-close.workflow.json` | classic | box | trace | Lanes for commerce, processor, bank, books, reviewer, and breaks. Phases gather → tie out → sign off. Main path ends at lock |
| Dispute lifecycle | lifecycle | Whether a commerce refund means the money is actually done | `examples/northline-dispute.lifecycle.json` | classic | box | trace | Lanes: refund path, waiting on the network, books catch-up, terminal. Open dispute is not terminal. Books still showing income is a recoverable failure with a way back |
| Payout reconciliation | dataflow | Where processor cash went versus the bank and the books | `examples/northline-payout-rec.dataflow.json` | classic | box | trace | Stages: processor, payout, bank (cash source of truth), books. Timing gaps are not breaks |
| Customer and AR | architecture | Who is allowed to say this customer owes us | `examples/northline-customer-ar.architecture.json` | classic | logo | static | Two boundaries: paid at checkout (no receivable) versus invoiced wholesale (the only A/R). One owner per fact |
| Cash to a date | dataflow | Whether named inflows cover a dated payroll | `examples/northline-cash-runway.dataflow.json` | classic | box | trace | Stages: opening cash, named inflows, available, dated outflows, the payroll date. Unnamed cash does not count |

## Systems and engineering

| Scenario | Type | The picture is for | Spec | Preset | Node | Motion | Signature layout |
|---|---|---|---|---|---|---|---|
| System overview | architecture | What exists, who owns it, and how it connects | `examples/web-app.architecture.json`, also `rag-pipeline.architecture.json`, `maka-architecture.architecture.json`, `brand-aware-delivery.architecture.json` | classic | box | static | Boundaries for region, runtime, or trust. Brand ids on real products are allowed; that is not logo mode. `maka` omits `quality_profile`; still deliver showcase |
| Deployment ownership | architecture | Where each workload runs, and what crosses a boundary | `examples/production-deployment.architecture.json` | blueprint | box | trace | Region and network boundaries, ownership tags, edge → private app → state → async. Set `meta.engineering_profile` to `deployment-ownership` only for this ask |
| Agent tool-call loop | workflow | How an agent plans, gets permission, acts, recovers, and reports | `examples/agent-tool-call.workflow.json` | signal-flow | box | trace | Lanes: UI, runtime, policy, exceptions, tools, observability. Phases intake → plan → execute. Approval sits on the main path |
| Delivery workflow | workflow | How a change moves from commit to production | `examples/release-delivery.workflow.json` | classic | box | trace | Lanes: author, CI, governance, production, comms, rollback. Main path commit → review → build → approve → deploy → verify |
| Incident runbook | workflow | Detect, triage, mitigate, verify, escalate | `examples/incident-response.workflow.json` | signal-flow | box | trace | Lanes: signal, command, mitigation, evidence, comms, escalation. Phases detect → mitigate → verify |
| API request chain | sequence | Who calls whom, in order, including cache miss and the return | `examples/cache-miss-request.sequence.json` | classic | box | trace | Segments: request, fallback, response and trace. Return traffic is drawn, not implied |
| Async roundtrip | sequence | What happens after the first response returns | `examples/async-job-roundtrip.sequence.json` | signal-flow | box | trace | Segments: accept, background work, notify and reconcile. The HTTP return is not the end of the picture |
| Data lineage | dataflow | Where data comes from, how it changes, and who consumes it | `examples/product-analytics.dataflow.json` | classic | box | trace | Stages: sources, ingest, process, store, consume |
| Event-stream topology | dataflow | Which events move through topics, processors, and failure paths | `examples/event-stream.dataflow.json` | signal-flow | box | trace | Stages: producers, transit, processors, state and recovery, consumers. Failure path is its own flow, not a footnote |
| Object lifecycle | lifecycle | Which states exist, what moves between them, and how it ends | `examples/agent-run.lifecycle.json` | classic | box | trace | Lanes: phases, interruptions, recovery, terminals. A retry is a real transition back, not a decoration |
| Deployment lifecycle | lifecycle | What state a release is in, and what can happen next | `examples/deployment-release.lifecycle.json` | signal-flow | box | trace | Lanes: release phases, approval and health wait, rollback, terminals |

## Comparison, not a picture to imitate

Checkout platform delta (`examples/checkout-platform.base.architecture.json` and `checkout-platform.head.architecture.json`, receipt `examples/checkout-platform-delta.receipt.json` in the upstream repo only) is an architecture before/after: fraud gate added, `visual_preset: "signal-flow"`, box, no trace, no `quality_profile`. Use it only when the user wants a base/head diff. A normal architecture deliver still follows system overview or deployment ownership and still sets showcase.

## Disagreements with `guide`

Follow the spec column, not `presentation`:

- Operating map and money map ship **logo + trace**. `presentation` says static. The logo file is not a separate scenario.
- Payout rec ships **classic** (preset omitted) **+ trace**. `presentation` says blueprint. Do not put a payout rec on blueprint unless the user asks for an engineering-review look.
- `guide` proof ids are stale for several engineering recipes (they point at a finance spec). Use the spec named in this table: deployment ownership is `production-deployment.architecture.json` (blueprint), not the customer-AR logo map; API request is the cache-miss sequence; lineage is product analytics; event stream is `event-stream.dataflow.json`; object lifecycle is the agent-run lifecycle.

## Defaults the examples do not override

Omit `meta.subtitle` unless the user asks. Omit `meta.legend` for auto. Omit `meta.locale` unless the fixed Viewer UI should stay the English default explicitly. Do not add `via`, `channelX`, `channelY`, or `labelAt` before a diagnostic asks. Rendered HTML is produced locally under the plugin `out/` directory and is not committed.
