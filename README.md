# Business Diagram plugin

This repository is the Claude directory submission. It contains a plugin that teaches Claude to produce a validated business or software diagram. The plugin is instructions and data only: the skill, its reference documents, the JSON schemas, and small examples live in `skills/business-diagram/`.

Published diagrams and docs: https://diagram.mosofin.com

The generator program lives in [MosoFin/mosofin-diagram](https://github.com/MosoFin/mosofin-diagram). The skill has Claude fetch it once at a pinned commit and run it locally; `skills/business-diagram/README.md` lists exactly what is downloaded, run, read, and sent.

## Directory submission

Submit this repository, not `mosofin-diagram`.

- Repository: `https://github.com/MosoFin/business-diagram-plugin`
- Plugin path: leave empty
- Branch: leave empty

## What is here

- `.claude-plugin/plugin.json` is the plugin manifest. `skills` points at `./skills/business-diagram`.
- `skills/business-diagram/` is the skill: `SKILL.md`, `schemas/`, `references/`, and small JSON examples. It holds no program; `SKILL.md` has Claude fetch the generator from `mosofin-diagram` at a pinned commit.
