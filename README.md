# Business Diagram plugin

This repository is the Claude directory submission. It contains a plugin that generates a validated business or software diagram on its own. The generator, schemas, renderer, and small examples live in `skills/business-diagram/`.

Published diagrams and docs: https://diagram.mosofin.com

[MosoFin/mosofin-diagram](https://github.com/MosoFin/mosofin-diagram) remains the full project. That repository keeps the large generated example HTML, the committed viewer template, and the other built files that make a directory scan time out. This plugin does not commit those files. It rebuilds the ones the CLI needs from smaller sources already in `skills/business-diagram/` (see that folder's `SKILL.md`).

## Directory submission

Submit this repository, not `mosofin-diagram`.

- Repository: `https://github.com/MosoFin/business-diagram-plugin`
- Plugin path: leave empty
- Branch: leave empty

## What is here

- `.claude-plugin/plugin.json` is the plugin manifest. `skills` points at `./skills/business-diagram`.
- `skills/business-diagram/` is the generator: `SKILL.md`, `bin/`, `schemas/`, `renderers/`, `references/`, and small JSON examples.
