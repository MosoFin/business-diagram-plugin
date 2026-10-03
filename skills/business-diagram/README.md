# Business Diagram skill

This folder is the Mosofin diagram generator for the business-diagram plugin. From here, run `node bin/mosofin.mjs`. `SKILL.md` is the authoring contract and the one-time local build of the viewer template, validators, and brand marks.

Docs: https://diagram.mosofin.com

Upstream, including the large generated examples: https://github.com/MosoFin/mosofin-diagram

What runs locally is `node bin/mosofin.mjs` (`guide`, `brands`, `validate`, `deliver`). There is no package launcher, no MCP server, and no hooks. After the local build, validation and rendering make no network calls. The plugin does not read credentials, tokens, or secrets from the user machine (env files, keychain, or ambient process env). Optional Chrome for visual-check is a plugin user_config option (`chrome_path`, sensitive). Child processes receive an allowlisted env only.
