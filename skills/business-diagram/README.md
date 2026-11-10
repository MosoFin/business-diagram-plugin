# Business Diagram skill

This folder is the Mosofin diagram generator for the business-diagram plugin. From here, run `node bin/mosofin.mjs`. `SKILL.md` is the authoring contract and the one-time local build of the viewer template, validators, and brand marks.

Docs: https://diagram.mosofin.com

Upstream, including the large generated examples: https://github.com/MosoFin/mosofin-diagram

## What this plugin runs, reads, and sends

- **Runs:** `node bin/mosofin.mjs` (`guide`, `brands`, `icons`, `validate`, `deliver`, `visual-check`). There are no hooks, no MCP server, and no package launcher. The CLI starts only local programs: `node` for the renderers, `git` when you pass `--repo-root` (it checks that cited files exist at a pinned commit, with user and system Git configuration disabled), a headless Chrome or Chromium with a throwaway profile for `visual-check`, and the operating system opener for `--open`.
- **Sends:** nothing. The CLI opens no network connection and starts no server. `npm install`, run once during setup, downloads the build-time packages in `package-lock.json` from the npm registry.
- **Reads:** the diagram JSON you pass it and the files in this folder. The only environment variables it reads are listed in `renderers/shared/system-env.mjs`: `PATH`, `PATHEXT`, `SYSTEMROOT`, `WINDIR`, `PROGRAMFILES`, `PROGRAMFILES(X86)`, `TMP`, `TEMP`, `TMPDIR`, `LANG`, `LC_ALL`, `LC_CTYPE`, `TERM`, `COLORTERM`, and its own `MOSOFIN_QUALITY_PROFILE`, `MOSOFIN_REPO_ROOT`, and `MOSOFIN_DIAGNOSTIC_FORMAT`. Child processes receive only those.
- **Generated HTML:** a diagram that uses a built-in brand shows that logo with an image link to `raw.githubusercontent.com/MosoFin/mosofin-diagram`, which the browser loads when someone opens the file.
