# Business Diagram plugin

Lightweight Claude plugin for mapping how a business or software system runs. This repository is only the plugin entry point. It does not contain the renderer, schemas, generated HTML, or vendor bundles.

The generator stays in [MosoFin/mosofin-diagram](https://github.com/MosoFin/mosofin-diagram). Diagrams are published at [diagram.mosofin.com](https://diagram.mosofin.com).

## What is here

- `.claude-plugin/plugin.json` is the plugin manifest.
- `skills/business-diagram/SKILL.md` tells Claude to use the generator in `mosofin-diagram` instead of copying it here.

## Directory submission

Submit this repository, not `mosofin-diagram`.

- Repository: `https://github.com/MosoFin/business-diagram-plugin`
- Plugin path: leave empty
- Branch: leave empty

`mosofin-diagram` is the implementation. Its generated HTML and minified bundles are what make that archive too large for a directory scan.
