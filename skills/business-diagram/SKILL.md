---
name: business-diagram
description: Map how a business or software system runs into a validated Mosofin diagram (architecture, workflow, sequence, data flow, lifecycle, or operating pillars). Use for business stack maps and finance diagrams such as ledger, close, revenue, payouts, or cash. The renderer lives in MosoFin/mosofin-diagram; this skill only points at it.
license: MIT
metadata:
  version: "1.0"
  author: MosoFin
  generator: https://github.com/MosoFin/mosofin-diagram
---

# Business Diagram

This skill is a lightweight entry point. It does not ship the renderer, schemas, examples, or vendor files. Do not copy those into this plugin.

The generator is [MosoFin/mosofin-diagram](https://github.com/MosoFin/mosofin-diagram):

- Skill contract: `mosofin/SKILL.md` in that repository
- Site: https://diagram.mosofin.com

Follow that contract for authoring, validation, and HTML delivery. Do not reimplement the renderer here.

## When this applies

Use this when the user wants a diagram of how a business or its software runs, or a finance picture that has to tie out (ledger, close, revenue walk, payout reconciliation, receivables, cash). Pure software maps (infrastructure, API chains, pipelines, state machines) use the same generator.

## How to produce the diagram

1. If `mosofin-diagram` is already on disk, open `mosofin/SKILL.md` there and use `node bin/mosofin.mjs`. That skill is authoritative.
2. If it is not on disk and a shell is available, clone `https://github.com/MosoFin/mosofin-diagram` and follow `mosofin/SKILL.md`. Leave generated HTML, `assets/template.html`, and minified vendor bundles out of this plugin repository.
3. If no shell is available, do not invent a rendered HTML file. Point the user at the repository and at https://diagram.mosofin.com, and say the validated artifact comes from that generator.

## Diagram types

The generator chooses one of these. When the request is ambiguous, use `node bin/mosofin.mjs guide "<scenario>" --json` inside `mosofin-diagram`.

- `architecture` for components, services, and trust boundaries
- `workflow` for processes, approvals, and handoffs
- `sequence` for call chains and request lifecycles
- `dataflow` for pipelines, lineage, and consumers
- `lifecycle` for states, retries, and terminal outcomes
- `pillars` for the operating model on one page

Business and finance mapping starts from that repo's `references/business-onboarding.md` or `references/finance-onboarding.md`, not from files in this plugin.
