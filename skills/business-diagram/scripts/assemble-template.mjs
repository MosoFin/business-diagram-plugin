#!/usr/bin/env node
// Rebuild assets/template.html from assets/template-parts/part-*.inc.
// The assembled viewer is larger than the plugin archive allows, so only the
// parts are tracked. The renderer still reads assets/template.html unchanged.
import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const partsDir = path.join(root, 'assets/template-parts');
const output = path.join(root, 'assets/template.html');
const parts = fs.readdirSync(partsDir)
  .filter((name) => /^part-\d+\.inc$/.test(name))
  .sort();
if (parts.length === 0) {
  console.error('no template parts in assets/template-parts');
  process.exit(1);
}
const data = Buffer.concat(parts.map((name) => fs.readFileSync(path.join(partsDir, name))));
const temporary = `${output}.${process.pid}.tmp`;
fs.writeFileSync(temporary, data);
fs.renameSync(temporary, output);
console.log(`assembled assets/template.html (${data.length} bytes, ${parts.length} parts)`);
