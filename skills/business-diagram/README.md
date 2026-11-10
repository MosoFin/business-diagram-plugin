# Business Diagram skill

This folder teaches Claude to turn a description of a business or software system into a validated, interactive HTML diagram: architecture, workflow, sequence, data flow, lifecycle, or operating pillars. It contains instructions and data only: `SKILL.md`, the reference documents, the JSON schemas, and small JSON examples. There is no program, hook, or MCP server in this plugin.

Docs: https://diagram.mosofin.com

## What this skill downloads, runs, reads, and sends

- **Downloads:** on first use, Claude tells you and then clones the open-source generator from https://github.com/MosoFin/mosofin-diagram at the pinned commit `606ee532550d77ef937b760c154983fa9b4ea323` into a folder outside your project. It never pulls a newer commit.
- **Runs:** that checkout's `mosofin` CLI with Node.js, using only `guide`, `brands`, `icons`, `validate`, `deliver`, `visual-check`, and `doctor`. The CLI starts local programs only: `node` for the renderers, `git` when you ask for repository evidence with `--repo-root`, a headless Chrome or Chromium with a throwaway profile for `visual-check`, and the operating system opener for `--open`.
- **Reads:** the diagram JSON Claude writes and the files in this folder. It asks for no credentials, tokens, or accounts.
- **Sends:** nothing. Apart from the one-time clone from GitHub, the commands this skill uses make no network calls. The generator also has `brands capture` and `preview` commands, which this skill tells Claude never to run.
- **Generated HTML:** a diagram that uses a built-in brand shows that logo with an image link to `raw.githubusercontent.com/MosoFin/mosofin-diagram`, which the browser loads when someone opens the file.
