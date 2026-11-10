import { BRAND_MARKS } from './generated-brand-marks.mjs';
import { throwDiagnosticError } from './diagnostics.mjs';
import { esc, textUnits } from './utils.mjs';
import { domainIconFor } from './domain-icons.mjs';

const COLLECTIONS = Object.freeze({
  architecture: 'components',
  workflow: 'nodes',
  sequence: 'participants',
  dataflow: 'nodes',
  lifecycle: 'states',
  pillars: ['pillars', 'foundation'],
});
const MARK_BY_LOOKUP = new Map();
const MARK_BY_DOMAIN = new Map();
const RESOLVED_BY_NODE = new WeakMap();
const RESOLVED_MARK = Symbol('mosofin.brandMark');
// Preset plates are published by MosoFin/mosofin-diagram. Do not vendor them here.
const MOSOFIN_DIAGRAM_BRAND_SVG_BASE = 'https://raw.githubusercontent.com/MosoFin/mosofin-diagram/main/docs/design-handoff/assets/brand-marks/catalog/';

export function brandMarkAssetUrl(mark) {
  const id = mark?.id;
  if (!id) return '';
  return `${MOSOFIN_DIAGRAM_BRAND_SVG_BASE}${encodeURIComponent(id)}.svg`;
}

function lookupForms(value) {
  const raw = String(value ?? '').trim().toLocaleLowerCase('en-US');
  if (!raw) return [];
  const dashed = raw.replace(/[\s_]+/g, '-');
  const compact = raw.replace(/[\s_.-]+/g, '');
  return [...new Set([raw, dashed, compact])];
}

for (const mark of BRAND_MARKS) {
  for (const value of [mark.id, mark.title, ...mark.aliases]) {
    for (const form of lookupForms(value)) {
      if (!MARK_BY_LOOKUP.has(form)) MARK_BY_LOOKUP.set(form, mark);
    }
  }
  for (const domain of mark.domains) MARK_BY_DOMAIN.set(domain, mark);
}

function asUrl(value) {
  try {
    const url = new URL(String(value));
    return ['https:', 'http:'].includes(url.protocol) ? url : null;
  } catch {
    return null;
  }
}

function domainMark(hostname) {
  const host = hostname.toLocaleLowerCase('en-US').replace(/\.$/, '');
  const candidates = [...MARK_BY_DOMAIN.entries()]
    .filter(([domain]) => host === domain || host.endsWith(`.${domain}`))
    .sort(([left], [right]) => right.length - left.length);
  return candidates[0]?.[1] || null;
}

export function findBrandMark(value) {
  const url = asUrl(value);
  if (url) return domainMark(url.hostname);
  for (const form of lookupForms(value)) {
    const mark = MARK_BY_LOOKUP.get(form);
    if (mark) return mark;
  }
  return null;
}

export function listBrandMarks(query = '') {
  const needle = String(query).trim().toLocaleLowerCase('en-US');
  return BRAND_MARKS.filter((mark) => {
    if (!needle) return true;
    return [mark.id, mark.title, mark.category, ...mark.aliases, ...mark.domains]
      .some((value) => String(value).toLocaleLowerCase('en-US').includes(needle));
  }).map(({ path, ...mark }) => mark);
}

function suggestions(value) {
  const needle = lookupForms(value)[0] || '';
  return BRAND_MARKS.map((mark) => ({
    id: mark.id,
    score: lookupForms(mark.id).some((form) => form.includes(needle) || needle.includes(form)) ? 0 : 1,
  })).sort((left, right) => left.score - right.score || left.id.localeCompare(right.id))
    .slice(0, 5)
    .map((entry) => entry.id);
}

// Brand marks resolve only against the bundled catalogue. This module opens no
// network connection: a website link is matched to a preset by its domain, and
// anything else is rejected with a diagnostic.
export async function prepareDiagramBrandMarks(diagramType, diagram) {
  const collections = [COLLECTIONS[diagramType]].flat().filter(Boolean);
  const collection = collections.join('+');
  const nodes = collections.flatMap((name) => (Array.isArray(diagram[name])
    ? diagram[name].map((node, index) => ({ node, collection: name, index }))
    : []));
  const unknown = [];
  for (const { node, collection: name, index } of nodes) {
    if (!node.brand) continue;
    const reference = typeof node.brand === 'object' ? node.brand.url : node.brand;
    const preset = findBrandMark(reference);
    if (preset) {
      const resolved = { ...preset, kind: 'preset', status: 'preset', sourceUrl: preset.provenance.source };
      node[RESOLVED_MARK] = resolved;
      RESOLVED_BY_NODE.set(node, resolved);
      continue;
    }
    if (typeof node.brand === 'object' || asUrl(node.brand)) {
      unknown.push(`/${name}/${index}/brand ${JSON.stringify(node.brand)} is a website link with no built-in brand; this plugin does not fetch site icons`);
      continue;
    }
    unknown.push(`/${name}/${index}/brand ${JSON.stringify(node.brand)} is not a built-in brand; closest IDs: ${suggestions(node.brand).join(', ')}`);
  }
  if (unknown.length) {
    throwDiagnosticError(`Brand mark validation failed:\n- ${unknown.join('\n- ')}`, unknown.map((message) => ({
      code: message.includes('is a website link') ? 'brand/url-unsupported' : 'brand/unknown',
      severity: 'error',
      message,
      subject: { diagramType, collection },
      evidence: {},
      supportedFixes: ['choose an ID from `mosofin brands`', 'omit `brand` and set a domain `icon` instead'],
    })));
  }
}

export function brandMarkFor(node) {
  return node?.[RESOLVED_MARK] || RESOLVED_BY_NODE.get(node) || null;
}

export function brandMetadataFor(node) {
  const mark = brandMarkFor(node);
  return mark ? {
    brand: mark.title,
    brandId: mark.id,
    brandStatus: mark.status,
    brandSource: mark.sourceUrl,
  } : {};
}

function hasCornerMark(node) {
  return Boolean(brandMarkFor(node) || domainIconFor(node));
}

export function brandLabelFitWidth(node, width) {
  return hasCornerMark(node) ? Math.max(1, width - 48) : width;
}

export function brandTopRailProblem(node, width, minimumFontSize, subject = 'Node') {
  if (!hasCornerMark(node)) return null;
  const available = width - 48;
  const required = textUnits(node.label) * minimumFontSize * 0.6;
  if (available >= required) return null;
  return `${subject} "${node.id}" brand top rail leaves ${Math.max(0, available)}px for its label, but `
    + `"${node.label}" needs ~${Math.ceil(required)}px at the ${minimumFontSize}px legible minimum — widen the node or shorten the label.`;
}

function markAttrs(mark) {
  return [
    `data-brand-mark="${esc(mark.id)}"`,
    `data-brand-title="${esc(mark.title)}"`,
    `data-brand-status="${esc(mark.status)}"`,
    mark.sourceUrl ? `data-brand-source="${esc(mark.sourceUrl)}"` : '',
  ].filter(Boolean).join(' ');
}

export function renderBrandMark(node, { x, y, size = 16 } = {}) {
  const mark = brandMarkFor(node);
  if (!mark) return '';
  const inset = 3;
  let content;
  if (mark.kind === 'preset') {
    const href = brandMarkAssetUrl(mark);
    content = `<image href="${esc(href)}" x="${inset}" y="${inset}" width="${size - inset * 2}" height="${size - inset * 2}" preserveAspectRatio="xMidYMid meet"/>`;
  } else {
    const scale = size / 20;
    content = `<g transform="scale(${scale})" class="brand-mark-fallback"><circle cx="10" cy="10" r="5.2"/><path d="M4.8 10h10.4M10 4.8c1.6 1.6 2.4 3.3 2.4 5.2s-.8 3.6-2.4 5.2M10 4.8C8.4 6.4 7.6 8.1 7.6 10s.8 3.6 2.4 5.2"/></g>`;
  }
  return `<g aria-hidden="true" ${markAttrs(mark)} class="brand-mark" transform="translate(${x} ${y})">
            <rect width="${size}" height="${size}" rx="4" class="brand-mark-badge"/>
            ${content}
            <rect width="${size}" height="${size}" rx="4" class="brand-mark-frame"/>
          </g>`;
}
