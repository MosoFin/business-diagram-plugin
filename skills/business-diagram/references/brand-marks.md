# Brand marks

Use a brand mark only when a real product, provider, model family, channel, or
service identity helps the reader. Semantic `type` still explains what the node
does; `brand` explains whose product it is.

## When there is no brand mark

Many nodes have no logo to show: suppliers, a warehouse or 3PL, the controller,
a legal entity, truth rules. Some real products have no openly licensed mark in
the catalogue either (NetSuite, Ramp). Never draw, trace or borrow a logo for
them. Set `icon` to a **domain icon** instead, a business pictogram from the
curated Lucide set (ISC licence):

```bash
node <generator>/bin/mosofin.mjs icons --json
node <generator>/bin/mosofin.mjs icons warehouse
```

```json
{ "id": "erp", "type": "backend", "label": "Supply chain · ERP", "sublabel": "NetSuite · POs", "icon": "supply-chain" }
```

A domain icon draws in the node's semantic colour, so it reads as a role, never
as a brand. In logo mode it replaces the generic role sigil; in box mode it sits
in the corner slot a brand badge would use. A resolved `brand` always wins over
`icon`. Unknown values fail validation with the closest IDs. Architecture
components and pillars (pillars and foundation blocks) accept `icon`.

## Agent decision path

1. Search the built-in catalogue when the request names a recognizable brand:

   ```bash
   node <generator>/bin/mosofin.mjs brands "Claude" --json
   ```

2. Put the returned canonical ID in the node, participant, or state:

   ```json
   {
     "id": "planner",
     "type": "backend",
     "label": "Claude",
     "brand": "claude"
   }
   ```

3. If there is no catalogue match, omit `brand`. Do not invent a URL or
   silently assign a visually similar company. An architecture or pillars node
   may set a domain `icon` instead.

A website link is accepted only when its domain belongs to a built-in brand, and
it resolves to that bundled vector. The CLI never fetches a site icon and opens
no network connection.

Preset marks in the artifact point at MosoFin/mosofin-diagram, not a vendored
bundle. Each plate is
`https://raw.githubusercontent.com/MosoFin/mosofin-diagram/main/docs/design-handoff/assets/brand-marks/catalog/<id>.svg`
and the opened diagram loads that SVG.

Use `node <generator>/bin/mosofin.mjs brands --json` to inspect all canonical IDs, aliases,
categories, domains, and provenance. Current categories cover AI, cloud,
engineering, data, collaboration, business systems, channels, languages, and
frameworks.
