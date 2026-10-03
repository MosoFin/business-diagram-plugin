<!DOCTYPE html>
<html lang="en" data-theme="dark" data-preset="[VISUAL PRESET]" data-node-style="[NODE STYLE]">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta name="generator" content="mosofin 1.0.0-dev.0">
  <title>[PROJECT NAME] Architecture Diagram</title>
  <script>
    // Resolve the theme before first paint so light-preference users don't
    // see a dark flash. The toolbar script below re-applies with UI state.
    (function () {
      try {
        var theme = null;
        try {
          var param = new URLSearchParams(window.location.search).get('theme');
          if (param === 'light' || param === 'dark') theme = param;
          if (new URLSearchParams(window.location.search).get('embed') === '1') {
            document.documentElement.setAttribute('data-embed', 'true');
          }
          if (new URLSearchParams(window.location.search).get('present') === '1') {
            document.documentElement.setAttribute('data-present', 'true');
          }
        } catch (_) {}
        if (!theme) {
          try { theme = localStorage.getItem('mosofin-theme'); } catch (_) {}
        }
        if (theme !== 'light' && theme !== 'dark') {
          theme = window.matchMedia('(prefers-color-scheme: light)').matches ? 'light' : 'dark';
        }
        document.documentElement.setAttribute('data-theme', theme);
      } catch (_) {}
    })();
  </script>
  <!-- Async font load: a blackholed network must not block first paint.
       The body font stack falls back to system monospace until it lands. -->
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;500;600;700&display=swap"
        rel="stylesheet" media="print" onload="this.media='all'">
  <noscript>
    <link href="https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;500;600;700&display=swap" rel="stylesheet">
  </noscript>
  <style>
    /* ==========================================================
       THEME VARIABLES — switch by toggling [data-theme] on <html>
       ========================================================== */
    :root,
    [data-theme="dark"] {
      --bg: #020617;
      --grid: #1e293b;
      --text: #ffffff;
      --text-muted: #94a3b8;
      --text-dim: #475569;
      /* UI chrome and menu hints need more contrast than SVG .t-dim accents */
      --text-faint: #7d8da1;

      --panel: rgba(15, 23, 42, 0.5);
      --panel-border: #1e293b;
      --lane-fill: rgba(15, 23, 42, 0.22);
      --lane-stroke: #334155;

      --arrow: #64748b;
      --arrow-emphasis: #34d399;

      /* Opaque mask color used behind semi-transparent component fills
         so the arrows drawn underneath are properly hidden. */
      --mask: #0f172a;

      --frontend-fill:    rgba(8, 51, 68, 0.4);
      --frontend-stroke:  #22d3ee;
      --backend-fill:     rgba(6, 78, 59, 0.4);
      --backend-stroke:   #34d399;
      --database-fill:    rgba(76, 29, 149, 0.4);
      --database-stroke:  #a78bfa;
      --cloud-fill:       rgba(120, 53, 15, 0.3);
      --cloud-stroke:     #fbbf24;
      --security-fill:    rgba(136, 19, 55, 0.4);
      --security-stroke:  #fb7185;
      --messagebus-fill:  rgba(251, 146, 60, 0.3);
      --messagebus-stroke:#fb923c;
      --external-fill:    rgba(30, 41, 59, 0.5);
      --external-stroke:  #94a3b8;

      --toolbar-bg:       rgba(15, 23, 42, 0.8);
      --toolbar-border:   #334155;
      --toolbar-text:     #e2e8f0;
      --toolbar-hover:    rgba(15, 23, 42, 0.95);
      --toolbar-menu-bg:  #0f172a;
    }

    [data-theme="light"] {
      --bg: #f8fafc;
      --grid: #e2e8f0;
      --text: #0f172a;
      --text-muted: #64748b;
      --text-dim: #94a3b8;
      --text-faint: #64748b;

      --panel: #ffffff;
      --panel-border: #e2e8f0;
      --lane-fill: rgba(248, 250, 252, 0.65);
      --lane-stroke: #cbd5e1;

      --arrow: #94a3b8;
      --arrow-emphasis: #059669;

      --mask: #ffffff;

      --frontend-fill:    rgba(34, 211, 238, 0.15);
      --frontend-stroke:  #0891b2;
      --backend-fill:     rgba(52, 211, 153, 0.18);
      --backend-stroke:   #059669;
      --database-fill:    rgba(167, 139, 250, 0.2);
      --database-stroke:  #7c3aed;
      --cloud-fill:       rgba(251, 191, 36, 0.18);
      --cloud-stroke:     #d97706;
      --security-fill:    rgba(251, 113, 133, 0.15);
      --security-stroke:  #e11d48;
      --messagebus-fill:  rgba(251, 146, 60, 0.15);
      --messagebus-stroke:#ea580c;
      --external-fill:    rgba(148, 163, 184, 0.18);
      --external-stroke:  #64748b;

      --toolbar-bg:       rgba(255, 255, 255, 0.92);
      --toolbar-border:   #cbd5e1;
      --toolbar-text:     #334155;
      --toolbar-hover:    #ffffff;
      --toolbar-menu-bg:  #ffffff;
    }

    /* ==========================================================
       SIGNAL FLOW — an opt-in, motion-forward visual preset.
       The variables also target a standalone exported SVG, whose
       root carries data-preset and data-theme directly.
       ========================================================== */
    [data-preset="signal-flow"][data-theme="dark"] {
      --bg: #030711;
      --grid: #15233a;
      --panel: rgba(6, 14, 28, 0.78);
      --panel-border: #1d3350;
      --lane-fill: rgba(9, 22, 40, 0.5);
      --lane-stroke: #2c4564;
      --text: #f5fbff;
      --text-muted: #9eb0c7;
      --text-dim: #52667f;
      --text-faint: #7890ad;
      --mask: #07101e;
      --frontend-fill: rgba(6, 182, 212, 0.14);
      --frontend-stroke: #67e8f9;
      --backend-fill: rgba(16, 185, 129, 0.14);
      --backend-stroke: #5eead4;
      --database-fill: rgba(139, 92, 246, 0.16);
      --database-stroke: #c4b5fd;
      --cloud-fill: rgba(245, 158, 11, 0.13);
      --cloud-stroke: #fcd34d;
      --security-fill: rgba(244, 63, 94, 0.13);
      --security-stroke: #fda4af;
      --messagebus-fill: rgba(249, 115, 22, 0.13);
      --messagebus-stroke: #fdba74;
      --external-fill: rgba(71, 85, 105, 0.24);
      --external-stroke: #a5b4c7;
      --arrow: #7890ad;
      --arrow-emphasis: #2dd4bf;
      --toolbar-bg: rgba(7, 16, 30, 0.86);
      --toolbar-border: #29415f;
      --toolbar-menu-bg: #07101e;
    }

    [data-preset="signal-flow"][data-theme="light"] {
      --bg: #f4f9fc;
      --grid: #d4e5ee;
      --panel: rgba(255, 255, 255, 0.82);
      --panel-border: #bfd5e2;
      --lane-fill: rgba(232, 243, 248, 0.62);
      --lane-stroke: #a9c5d5;
      --text: #102638;
      --text-muted: #587287;
      --text-dim: #8aa2b4;
      --text-faint: #668397;
      --mask: #ffffff;
      --frontend-fill: rgba(6, 182, 212, 0.09);
      --frontend-stroke: #0789a1;
      --backend-fill: rgba(5, 150, 105, 0.09);
      --backend-stroke: #087f69;
      --database-fill: rgba(124, 58, 237, 0.09);
      --database-stroke: #7254c7;
      --cloud-fill: rgba(217, 119, 6, 0.08);
      --cloud-stroke: #b9670b;
      --security-fill: rgba(225, 29, 72, 0.08);
      --security-stroke: #c53a59;
      --messagebus-fill: rgba(234, 88, 12, 0.08);
      --messagebus-stroke: #c65f27;
      --external-fill: rgba(100, 116, 139, 0.1);
      --external-stroke: #607a8c;
      --arrow: #7b97aa;
      --arrow-emphasis: #0d9488;
    }

    /* ==========================================================
       BLUEPRINT — a review-first drafting preset for deployment,
       infrastructure, and technical design documentation. It keeps
       the same semantic palette and geometry while trading glow for
       precise grid contrast, squared materials, and drafting marks.
       ========================================================== */
    [data-preset="blueprint"][data-theme="dark"] {
      --bg: #06131f;
      --grid: #17425a;
      --panel: rgba(7, 27, 43, 0.9);
      --panel-border: #27627f;
      --lane-fill: rgba(10, 43, 66, 0.36);
      --lane-stroke: #34799a;
      --text: #e3f6ff;
      --text-muted: #91b8ca;
      --text-dim: #557e92;
      --text-faint: #739caf;
      --mask: #0a2031;
      --frontend-fill: rgba(26, 157, 193, 0.14);
      --frontend-stroke: #66d9ef;
      --backend-fill: rgba(31, 155, 124, 0.13);
      --backend-stroke: #69dfbd;
      --database-fill: rgba(120, 105, 196, 0.14);
      --database-stroke: #b4a8ff;
      --cloud-fill: rgba(197, 145, 43, 0.13);
      --cloud-stroke: #ffd166;
      --security-fill: rgba(199, 76, 104, 0.13);
      --security-stroke: #ff8da1;
      --messagebus-fill: rgba(207, 112, 49, 0.13);
      --messagebus-stroke: #ffad66;
      --external-fill: rgba(84, 119, 139, 0.18);
      --external-stroke: #a7cad9;
      --arrow: #78a3b7;
      --arrow-emphasis: #64dfc1;
      --toolbar-bg: rgba(7, 28, 45, 0.92);
      --toolbar-border: #34799a;
      --toolbar-text: #d8f3ff;
      --toolbar-hover: rgba(18, 58, 83, 0.95);
      --toolbar-menu-bg: #0a2031;
    }

    [data-preset="blueprint"][data-theme="light"] {
      --bg: #edf7fa;
      --grid: #b5d5e1;
      --panel: rgba(249, 253, 255, 0.94);
      --panel-border: #78aabd;
      --lane-fill: rgba(210, 232, 240, 0.42);
      --lane-stroke: #83b2c4;
      --text: #123344;
      --text-muted: #4e7486;
      --text-dim: #86a6b4;
      --text-faint: #668c9d;
      --mask: #f9fdff;
      --frontend-fill: rgba(8, 145, 178, 0.08);
      --frontend-stroke: #087f9c;
      --backend-fill: rgba(5, 128, 101, 0.08);
      --backend-stroke: #08755f;
      --database-fill: rgba(100, 75, 180, 0.08);
      --database-stroke: #6757a8;
      --cloud-fill: rgba(181, 120, 15, 0.08);
      --cloud-stroke: #a86609;
      --security-fill: rgba(190, 48, 80, 0.07);
      --security-stroke: #b32f50;
      --messagebus-fill: rgba(196, 81, 22, 0.07);
      --messagebus-stroke: #b65120;
      --external-fill: rgba(79, 112, 128, 0.08);
      --external-stroke: #506f7e;
      --arrow: #6d93a5;
      --arrow-emphasis: #087f69;
      --toolbar-bg: rgba(247, 252, 254, 0.94);
      --toolbar-border: #8ab5c5;
      --toolbar-text: #294f61;
      --toolbar-hover: #ffffff;
      --toolbar-menu-bg: #f9fdff;
    }

    /* ==========================================================
       EDITORIAL — a warm, publication-minded preset for design
       reviews, launch notes, and documentation. The diagram keeps
       the same semantic palette and exact geometry, but trades
       software chrome for paper, ink, ruled structure, and one
       restrained vermilion accent.
       ========================================================== */
    [data-preset="editorial"][data-theme="dark"] {
      --bg: #181611;
      --grid: #39342a;
      --panel: rgba(35, 31, 24, 0.96);
      --panel-border: #625a4a;
      --lane-fill: rgba(52, 46, 35, 0.46);
      --lane-stroke: #726957;
      --text: #f4eddf;
      --text-muted: #b9ae9b;
      --text-dim: #776e60;
      --text-faint: #9d917e;
      --mask: #231f18;
      --frontend-fill: rgba(43, 133, 142, 0.16);
      --frontend-stroke: #7fc6c7;
      --backend-fill: rgba(63, 132, 92, 0.16);
      --backend-stroke: #8fc29e;
      --database-fill: rgba(117, 91, 141, 0.17);
      --database-stroke: #c0a4d0;
      --cloud-fill: rgba(170, 119, 49, 0.16);
      --cloud-stroke: #d8ad68;
      --security-fill: rgba(157, 69, 65, 0.17);
      --security-stroke: #df9085;
      --messagebus-fill: rgba(174, 79, 42, 0.16);
      --messagebus-stroke: #df946f;
      --external-fill: rgba(126, 115, 96, 0.18);
      --external-stroke: #b8ad99;
      --arrow: #948978;
      --arrow-emphasis: #dd6b3d;
      --toolbar-bg: rgba(35, 31, 24, 0.92);
      --toolbar-border: #625a4a;
      --toolbar-text: #eee5d5;
      --toolbar-hover: #302a20;
      --toolbar-menu-bg: #231f18;
    }

    [data-preset="editorial"][data-theme="light"] {
      --bg: #f2eee5;
      --grid: #d8d0c2;
      --panel: rgba(251, 248, 241, 0.97);
      --panel-border: #c4b9a6;
      --lane-fill: rgba(229, 221, 207, 0.42);
      --lane-stroke: #b8aa94;
      --text: #242018;
      --text-muted: #6f6658;
      --text-dim: #a09788;
      --text-faint: #817767;
      --mask: #fbf8f1;
      --frontend-fill: rgba(31, 117, 126, 0.09);
      --frontend-stroke: #287e84;
      --backend-fill: rgba(42, 119, 75, 0.09);
      --backend-stroke: #397b53;
      --database-fill: rgba(105, 75, 130, 0.09);
      --database-stroke: #765d86;
      --cloud-fill: rgba(157, 103, 31, 0.1);
      --cloud-stroke: #9a671f;
      --security-fill: rgba(153, 58, 52, 0.09);
      --security-stroke: #9e463f;
      --messagebus-fill: rgba(182, 70, 27, 0.09);
      --messagebus-stroke: #ad4b25;
      --external-fill: rgba(101, 91, 75, 0.09);
      --external-stroke: #746b5e;
      --arrow: #8a806f;
      --arrow-emphasis: #bb4c23;
      --toolbar-bg: rgba(251, 248, 241, 0.94);
      --toolbar-border: #c4b9a6;
      --toolbar-text: #40392f;
      --toolbar-hover: #fffdf8;
      --toolbar-menu-bg: #fbf8f1;
    }

    * { margin: 0; padding: 0; box-sizing: border-box; }

    body {
      font-family: 'JetBrains Mono', ui-monospace, SFMono-Regular, Menlo, Consolas, 'DejaVu Sans Mono', 'Liberation Mono', 'Noto Sans Mono CJK SC', 'PingFang SC', 'Hiragino Sans GB', 'Microsoft YaHei', monospace;
      background: var(--bg);
      min-height: 100vh;
      padding: 2rem;
      color: var(--text);
      transition: background 0.2s ease, color 0.2s ease;
    }

    html[data-preset="signal-flow"] body {
      background-image:
        radial-gradient(circle at 18% -8%, rgba(34, 211, 238, 0.14), transparent 34rem),
        radial-gradient(circle at 92% 18%, rgba(139, 92, 246, 0.12), transparent 30rem);
      background-attachment: fixed;
    }
    html[data-preset="signal-flow"][data-theme="light"] body {
      background-image:
        radial-gradient(circle at 18% -8%, rgba(6, 182, 212, 0.12), transparent 34rem),
        radial-gradient(circle at 92% 18%, rgba(139, 92, 246, 0.08), transparent 30rem);
    }
    html[data-preset="blueprint"] body {
      background-image:
        linear-gradient(var(--grid) 1px, transparent 1px),
        linear-gradient(90deg, var(--grid) 1px, transparent 1px);
      background-size: 32px 32px;
      background-position: -1px -1px;
      background-attachment: fixed;
    }
    html[data-preset="editorial"] body {
      background-image:
        linear-gradient(90deg, transparent 0, transparent 5.4rem, color-mix(in srgb, var(--security-stroke) 18%, transparent) 5.4rem, color-mix(in srgb, var(--security-stroke) 18%, transparent) calc(5.4rem + 1px), transparent calc(5.4rem + 1px)),
        repeating-linear-gradient(0deg, transparent 0, transparent 31px, color-mix(in srgb, var(--grid) 36%, transparent) 31px, color-mix(in srgb, var(--grid) 36%, transparent) 32px);
      background-attachment: fixed;
    }

    /* Gallery/embed mode keeps the generated artifact as the source of truth
       while presenting only its diagram surface inside a proof card. */
    html[data-embed="true"] body {
      min-height: 0;
      padding: 0;
      overflow: hidden;
      background: var(--bg);
      background-image: none;
    }
    html[data-embed="true"] .toolbar,
    html[data-embed="true"] .header,
    html[data-embed="true"] .cards,
    html[data-embed="true"] .diagram-nav,
    html[data-embed="true"] .overview-map,
    html[data-embed="true"] .focus-chip,
    html[data-embed="true"] .route-probe,
    html[data-embed="true"] .semantic-lens,
    html[data-embed="true"] .diagram-guide,
    html[data-embed="true"] .node-finder,
    html[data-embed="true"] .guided-views { display: none !important; }
    html[data-embed="true"] .container { max-width: none; }
    html[data-embed="true"] .diagram-container {
      padding: 0.5rem;
      border: 0;
      border-radius: 0;
      box-shadow: none;
    }
    html[data-embed="true"] .diagram-container svg { min-width: 0; }

    /* Share Chapter Cue lives in the HTML viewer layer, not inside the SVG.
       It gives one-shot embeds a title, authored note, truthful stop order,
       and lifecycle state while canonical diagram geometry stays untouched. */
    .share-chapter-cue { display: none; }
    html[data-embed="true"][data-share-playback="true"] .share-chapter-cue:not([hidden]),
    html[data-embed="true"][data-share-moment="true"] .share-chapter-cue:not([hidden]) {
      position: absolute;
      z-index: 18;
      top: 0.85rem;
      left: 50%;
      display: grid;
      grid-template-columns: auto minmax(9rem, 13.5rem) minmax(0, 1fr);
      align-items: center;
      gap: 0.45rem 0.85rem;
      width: min(52rem, calc(100% - 1.5rem));
      min-height: 3.35rem;
      padding: 0.55rem 0.75rem 0.65rem;
      overflow: hidden;
      border: 1px solid color-mix(in srgb, var(--frontend-stroke) 42%, var(--toolbar-border));
      border-radius: 0.75rem;
      background:
        linear-gradient(115deg, color-mix(in srgb, var(--frontend-fill) 52%, transparent), transparent 45%),
        color-mix(in srgb, var(--toolbar-menu-bg) 92%, transparent);
      color: var(--toolbar-text);
      box-shadow: 0 14px 42px rgba(0, 0, 0, 0.3), inset 0 1px 0 rgba(255, 255, 255, 0.045);
      backdrop-filter: blur(16px);
      pointer-events: none;
      animation: mosofin-share-cue-enter 280ms cubic-bezier(0.22, 1, 0.36, 1) both;
    }
    html[data-preset="blueprint"] .share-chapter-cue {
      border-radius: 0.2rem;
      background:
        linear-gradient(90deg, color-mix(in srgb, var(--frontend-fill) 46%, transparent), transparent 36%),
        var(--toolbar-menu-bg);
      box-shadow: 0 10px 28px rgba(0, 0, 0, 0.2);
      backdrop-filter: none;
    }
    html[data-preset="editorial"] .share-chapter-cue {
      border-color: var(--panel-border);
      background:
        linear-gradient(90deg, color-mix(in srgb, var(--security-fill) 58%, transparent), transparent 42%),
        var(--toolbar-menu-bg);
      box-shadow: 0 10px 30px color-mix(in srgb, #000 18%, transparent);
      backdrop-filter: none;
    }
    html[data-preset="editorial"] .share-chapter-state { color: var(--arrow-emphasis); }
    html[data-preset="editorial"] .share-chapter-state::before {
      border-radius: 0;
      box-shadow: none;
      transform: rotate(45deg) scale(0.8);
    }
    .share-chapter-index {
      display: grid;
      gap: 0.12rem;
      min-width: 5rem;
      padding-right: 0.75rem;
      border-right: 1px solid var(--toolbar-border);
    }
    .share-chapter-state {
      display: inline-flex;
      align-items: center;
      gap: 0.36rem;
      color: var(--frontend-stroke);
      font-size: 0.5rem;
      font-weight: 800;
      letter-spacing: 0.14em;
      text-transform: uppercase;
    }
    .share-chapter-state::before {
      width: 0.38rem;
      height: 0.38rem;
      border-radius: 50%;
      background: currentColor;
      box-shadow: 0 0 10px currentColor;
      content: '';
    }
    .share-chapter-cue[data-state="playing"] .share-chapter-state::before {
      animation: mosofin-share-cue-pulse 1.15s ease-in-out infinite;
    }
    .share-chapter-count {
      color: var(--text-faint);
      font-size: 0.5rem;
      font-weight: 700;
      letter-spacing: 0.08em;
      text-transform: uppercase;
    }
    .share-chapter-copy { min-width: 0; }
    .share-chapter-copy strong,
    .share-chapter-copy small,
    .share-chapter-route {
      display: block;
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .share-chapter-copy strong {
      color: var(--toolbar-text);
      font-size: 0.6875rem;
      line-height: 1.35;
    }
    .share-chapter-copy small {
      margin-top: 0.08rem;
      color: var(--text-muted);
      font-size: 0.5rem;
      line-height: 1.35;
    }
    .share-chapter-route {
      color: color-mix(in srgb, var(--frontend-stroke) 76%, var(--text-muted));
      font-size: 0.5625rem;
      font-weight: 650;
      letter-spacing: 0.015em;
      text-align: right;
    }
    .share-chapter-progress {
      position: absolute;
      right: 0;
      bottom: 0;
      left: 0;
      height: 2px;
      overflow: hidden;
      background: color-mix(in srgb, var(--toolbar-border) 72%, transparent);
    }
    .share-chapter-progress span {
      display: block;
      width: 100%;
      height: 100%;
      background: linear-gradient(90deg, var(--frontend-stroke), var(--database-stroke));
      box-shadow: 0 0 12px var(--frontend-stroke);
      transform: scaleX(0);
      transform-origin: left center;
    }

    /* Presentation Stage is a viewer-only layer: it gives the live diagram
       the viewport while keeping guided views, theme, focus, and exports real.
       Embed mode wins when both query parameters are present. */
    html[data-present="true"]:not([data-embed="true"]) body {
      height: 100dvh;
      min-height: 0;
      padding: 1rem;
      overflow: hidden;
    }
    html[data-present="true"]:not([data-embed="true"]) .container {
      display: flex;
      flex-direction: column;
      gap: 0.75rem;
      width: 100%;
      max-width: none;
      height: calc(100dvh - 2rem);
    }
    html[data-present="true"]:not([data-embed="true"]) .header {
      flex: none;
      min-width: 0;
      margin: 0;
      padding-right: 23rem;
    }
    html[data-present="true"]:not([data-embed="true"]) .header-row { margin-bottom: 0.25rem; }
    html[data-present="true"]:not([data-embed="true"]) .subtitle {
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    html[data-present="true"]:not([data-embed="true"]) .guided-views {
      flex: none;
      margin-bottom: 0;
    }
    html[data-present="true"]:not([data-embed="true"]) .diagram-container {
      display: flex;
      flex: 1 1 auto;
      align-items: center;
      justify-content: center;
      min-height: 0;
      padding: 0.75rem;
      padding-bottom: calc(0.75rem + var(--mosofin-nav-reserve));
    }
    html[data-present="true"]:not([data-embed="true"]) .diagram-container > svg {
      width: 100%;
      height: 100%;
      min-width: 0;
    }
    html[data-present="true"]:not([data-embed="true"]) .cards { display: none; }
    html[data-present="true"]:not([data-embed="true"]) #btn-present {
      border-color: var(--frontend-stroke);
      color: var(--frontend-stroke);
    }

    /* The reader owns only the outer scale. Wide diagrams keep one authored
       SVG/viewBox while the runtime chooses a desktop width from the actual
       vertical budget. This avoids device-specific files and breakpoint
       jumps between a laptop and a tall monitor. */
    .container {
      width: 100%;
      max-width: var(--mosofin-reader-width, 1440px);
      margin: 0 auto;
    }
    @media (min-width: 768px) and (max-height: 920px) {
      .header { margin-bottom: 1rem; }
      .diagram-container { padding: 1rem; }
      .cards { margin-top: 1rem; }
      .card { padding: 1rem; }
    }

    .header { margin-bottom: 1.5rem; padding-right: 29.5rem; }

    html[data-preset="signal-flow"] .header,
    html[data-preset="blueprint"] .header,
    html[data-preset="editorial"] .header { padding-right: 29.5rem; }

    .header-row {
      display: flex;
      align-items: center;
      gap: 1rem;
      margin-bottom: 0.5rem;
    }

    .pulse-dot {
      width: 12px;
      height: 12px;
      background: var(--frontend-stroke);
      border-radius: 50%;
      animation: none;
    }

    html[data-motion-capable="true"] .pulse-dot { animation: pulse 2s infinite; }

    @keyframes pulse {
      0%, 100% { opacity: 1; }
      50% { opacity: 0.5; }
    }

    h1 {
      font-size: 1.5rem;
      font-weight: 700;
      letter-spacing: -0.025em;
    }

    .subtitle {
      color: var(--text-muted);
      font-size: 0.875rem;
      margin-left: 1.75rem;
    }

    html[data-preset="signal-flow"] .header-row::after {
      content: attr(data-preset-badge-signal-flow);
      margin-left: auto;
      color: var(--frontend-stroke);
      border: 1px solid var(--frontend-stroke);
      border-radius: 999px;
      padding: 0.28rem 0.6rem;
      font-size: 0.625rem;
      letter-spacing: 0.12em;
      opacity: 0.82;
      box-shadow: 0 0 20px rgba(34, 211, 238, 0.12);
      white-space: nowrap;
    }

    html[data-preset="blueprint"] .pulse-dot {
      width: 10px;
      height: 10px;
      border: 1px solid var(--frontend-stroke);
      border-radius: 1px;
      background: transparent;
      box-shadow: inset 0 0 0 2px var(--bg);
      transform: rotate(45deg);
      animation: none;
    }
    html[data-preset="blueprint"] .header-row::after {
      content: attr(data-preset-badge-blueprint);
      margin-left: auto;
      padding: 0.28rem 0.55rem;
      border-top: 1px solid var(--frontend-stroke);
      border-bottom: 1px solid var(--frontend-stroke);
      color: var(--frontend-stroke);
      font-size: 0.5625rem;
      font-weight: 700;
      letter-spacing: 0.14em;
      white-space: nowrap;
    }

    html[data-preset="editorial"] h1,
    html[data-preset="editorial"] .card h3 {
      font-family: Georgia, 'Times New Roman', 'Songti SC', STSong, serif;
      letter-spacing: -0.02em;
    }
    html[data-preset="editorial"] h1 { font-size: 1.72rem; font-weight: 600; }
    html[data-preset="editorial"] .pulse-dot {
      width: 9px;
      height: 9px;
      border: 1px solid var(--arrow-emphasis);
      border-radius: 0;
      background: var(--arrow-emphasis);
      box-shadow: 0 0 0 3px color-mix(in srgb, var(--arrow-emphasis) 14%, transparent);
      transform: rotate(45deg);
      animation: none;
    }
    html[data-preset="editorial"] .header-row::after {
      content: attr(data-preset-badge-editorial);
      margin-left: auto;
      padding: 0.32rem 0 0.24rem;
      border-bottom: 2px solid var(--arrow-emphasis);
      color: var(--arrow-emphasis);
      font-family: Georgia, 'Times New Roman', serif;
      font-size: 0.625rem;
      font-style: italic;
      font-weight: 600;
      letter-spacing: 0.08em;
      white-space: nowrap;
    }

    .diagram-container {
      --mosofin-nav-reserve: 0px;
      background: var(--panel);
      border-radius: 1rem;
      border: 1px solid var(--panel-border);
      padding: 1.5rem;
      padding-bottom: calc(1.5rem + var(--mosofin-nav-reserve));
      overflow: hidden;
      position: relative;
    }

    html[data-preset="signal-flow"] .diagram-container {
      position: relative;
      overflow: hidden;
      background: linear-gradient(180deg, rgba(8, 20, 38, 0.82), rgba(3, 8, 18, 0.94));
      box-shadow: inset 0 1px 0 rgba(255, 255, 255, 0.04), 0 28px 80px rgba(0, 0, 0, 0.34);
    }
    html[data-preset="signal-flow"][data-theme="light"] .diagram-container {
      background: linear-gradient(180deg, rgba(255, 255, 255, 0.9), rgba(237, 246, 250, 0.94));
      box-shadow: inset 0 1px 0 rgba(255, 255, 255, 0.72), 0 24px 60px rgba(45, 78, 99, 0.12);
    }
    html[data-preset="blueprint"] .diagram-container {
      border-color: var(--panel-border);
      border-radius: 0.35rem;
      background: var(--panel);
      box-shadow:
        inset 0 0 0 1px color-mix(in srgb, var(--panel-border) 34%, transparent),
        0 18px 48px rgba(0, 0, 0, 0.16);
    }
    html[data-preset="blueprint"] .diagram-container::after {
      content: "";
      position: absolute;
      z-index: 10;
      inset: 0.7rem;
      pointer-events: none;
      background:
        linear-gradient(var(--frontend-stroke), var(--frontend-stroke)) left top / 1.35rem 1px no-repeat,
        linear-gradient(var(--frontend-stroke), var(--frontend-stroke)) left top / 1px 1.35rem no-repeat,
        linear-gradient(var(--frontend-stroke), var(--frontend-stroke)) right top / 1.35rem 1px no-repeat,
        linear-gradient(var(--frontend-stroke), var(--frontend-stroke)) right top / 1px 1.35rem no-repeat,
        linear-gradient(var(--frontend-stroke), var(--frontend-stroke)) left bottom / 1.35rem 1px no-repeat,
        linear-gradient(var(--frontend-stroke), var(--frontend-stroke)) left bottom / 1px 1.35rem no-repeat,
        linear-gradient(var(--frontend-stroke), var(--frontend-stroke)) right bottom / 1.35rem 1px no-repeat,
        linear-gradient(var(--frontend-stroke), var(--frontend-stroke)) right bottom / 1px 1.35rem no-repeat;
      opacity: 0.58;
    }
    html[data-preset="editorial"] .diagram-container {
      overflow: hidden;
      border-color: var(--panel-border);
      border-radius: 0.38rem;
      background:
        linear-gradient(90deg, color-mix(in srgb, var(--security-fill) 58%, transparent), transparent 12%),
        var(--panel);
      box-shadow:
        0 1px 0 color-mix(in srgb, var(--text) 6%, transparent),
        0 18px 48px color-mix(in srgb, #000 19%, transparent);
    }
    html[data-preset="editorial"] .diagram-container::after {
      position: absolute;
      z-index: 10;
      top: 0.72rem;
      right: 1.25rem;
      padding: 0.2rem 0.52rem;
      border: 1px solid var(--panel-border);
      background: var(--panel);
      color: var(--arrow-emphasis);
      content: attr(data-preset-badge-editorial-plate);
      font-family: Georgia, 'Times New Roman', serif;
      font-size: 0.52rem;
      font-style: italic;
      font-weight: 600;
      letter-spacing: 0.09em;
      pointer-events: none;
    }
    html[data-embed="true"][data-preset="editorial"] .diagram-container::after { display: none; }
    html[data-motion-capable="true"][data-preset="signal-flow"][data-ambient-motion="running"] .diagram-container::before {
      content: "";
      position: absolute;
      inset: 0;
      pointer-events: none;
      background: linear-gradient(115deg, transparent 28%, rgba(103, 232, 249, 0.035) 48%, transparent 68%);
      transform: translateX(-70%);
      animation: mosofin-signal-scan 4.8s ease-in-out 1 both;
    }

    /* Motion Governor gives the reader a real static state and gives the
       strongest semantic action the only motion budget. Atmosphere resets to
       its authored base instead of freezing mid-cycle. Static artifacts never
       start a decorative pulse or scan. */
    html:not([data-motion-capable="true"]) svg[data-animation="trace"] [data-animate] {
      animation: none !important;
    }
    html[data-motion="still"] .pulse-dot,
    html[data-motion="still"] .diagram-container::before,
    html[data-motion="still"] svg[data-animation="trace"] [data-animate],
    html[data-motion-owner] .pulse-dot,
    html[data-motion-owner] .diagram-container::before,
    html[data-motion-owner] svg[data-animation="trace"] [data-animate],
    html[data-embed="true"] .pulse-dot,
    html[data-embed="true"] .diagram-container::before,
    html[data-share-playback="true"] .pulse-dot,
    html[data-share-playback="true"] .diagram-container::before,
    html[data-document-hidden="true"] .pulse-dot,
    html[data-document-hidden="true"] .diagram-container::before,
    html[data-document-hidden="true"] svg[data-animation="trace"] [data-animate] {
      animation: none !important;
    }
    html[data-motion="still"] .diagram-container::before,
    html[data-motion-owner] .diagram-container::before,
    html[data-embed="true"] .diagram-container::before,
    html[data-share-playback="true"] .diagram-container::before {
      opacity: 0;
    }
    html[data-motion="still"] .guided-view-progress span,
    html[data-motion="still"] .guided-story-caption,
    html[data-motion="still"] .story-trail-flow,
    html[data-motion="still"] .intent-trace-flow,
    html[data-motion="still"] .route-probe-flow,
    html[data-motion="still"] .route-journey-flow,
    html[data-motion="still"] .semantic-lens-flow,
    html[data-motion="still"] .diagram-guide,
    html[data-motion="still"] [data-story-step],
    html[data-motion="still"] .overview-map-live,
    html[data-motion="still"] .guided-view-stop,
    html[data-motion="still"] .share-chapter-cue,
    html[data-motion="still"] .share-chapter-state::before,
    html[data-motion="still"] .share-chapter-progress span,
    html[data-document-hidden="true"] .guided-view-progress span,
    html[data-document-hidden="true"] .story-trail-flow,
    html[data-document-hidden="true"] .intent-trace-flow,
    html[data-document-hidden="true"] .route-probe-flow,
    html[data-document-hidden="true"] .route-journey-flow,
    html[data-document-hidden="true"] .semantic-lens-flow,
    html[data-document-hidden="true"] .diagram-guide,
    html[data-document-hidden="true"] [data-story-step],
    html[data-document-hidden="true"] .overview-map-live,
    html[data-document-hidden="true"] .guided-view-stop,
    html[data-document-hidden="true"] .share-chapter-cue,
    html[data-document-hidden="true"] .share-chapter-state::before,
    html[data-document-hidden="true"] .share-chapter-progress span {
      animation: none !important;
    }
    html[data-motion="still"] .guided-view-progress span {
      transform: scaleX(1) !important;
    }
    html[data-motion="still"] svg[data-story-playing="true"] .story-trail-flow {
      stroke-dasharray: none;
      stroke-dashoffset: 0;
      opacity: 0.55;
    }
    html[data-motion="still"] .intent-trace-flow,
    html[data-motion="still"] .route-probe-flow {
      stroke-dasharray: none;
      stroke-dashoffset: 0;
      opacity: 0.72;
    }
    html[data-motion="still"] .route-journey-overlay,
    html[data-document-hidden="true"] .route-journey-overlay { display: none !important; }
    html[data-motion="still"] .relationship-pulse-overlay,
    html[data-motion="still"] .story-carrier-overlay,
    html[data-document-hidden="true"] .story-carrier-overlay { display: none !important; }
    html[data-motion="still"] .chapter-handoff-overlay { display: none !important; }
    html[data-motion="still"] .chapter-handoff-anchor { animation: none !important; }
    html[data-motion="still"] .guided-view-chapter,
    html[data-motion="still"] svg [data-node-id],
    html[data-motion="still"] svg [data-edge-from],
    html[data-motion="still"] svg [data-detail],
    html[data-motion="still"] svg [data-detail-anchor],
    html[data-motion="still"] svg [data-legend-hit],
    html[data-motion="still"] svg {
      transition: none !important;
    }

    svg {
      width: 100%;
      min-width: min(900px, 100%);
      display: block;
      transform-origin: 0 0;
      transition: transform 0.18s ease;
    }

    .diagram-container.is-camera-moving svg {
      transition: transform 0.42s cubic-bezier(0.22, 1, 0.36, 1);
    }
    .diagram-container.is-camera-transaction svg { transition: none; }
    .diagram-container.is-pannable { cursor: grab; }
    .diagram-container.is-panning { cursor: grabbing; user-select: none; }
    .diagram-container.is-panning svg { transition: none; }

    .diagram-nav {
      position: absolute;
      right: 1rem;
      bottom: 1rem;
      z-index: 12;
      display: inline-flex;
      align-items: center;
      max-width: calc(100% - 2rem);
      padding: 0.15rem;
      gap: 0;
      overflow-x: auto;
      border: 1px solid color-mix(in srgb, var(--toolbar-border) 48%, transparent);
      border-radius: 0.58rem;
      background: color-mix(in srgb, var(--toolbar-bg) 82%, transparent);
      box-shadow: 0 5px 14px rgba(0, 0, 0, 0.07);
      backdrop-filter: blur(10px);
      scrollbar-width: none;
    }
    .diagram-nav::-webkit-scrollbar { display: none; }
    .diagram-nav button,
    .focus-chip button,
    .guided-views button {
      border: 0;
      background: transparent;
      color: var(--toolbar-text);
      font: inherit;
      cursor: pointer;
    }
    .diagram-nav button {
      min-width: 2rem;
      height: 2rem;
      padding: 0 0.35rem;
      border-radius: 0.35rem;
      color: color-mix(in srgb, var(--toolbar-text) 68%, transparent);
      font-size: 0.56rem;
      font-weight: 600;
      letter-spacing: 0.02em;
      transition: background 0.15s, color 0.15s;
    }
    .diagram-nav button:hover,
    .diagram-nav button:focus-visible,
    .focus-chip button:hover,
    .focus-chip button:focus-visible,
    .guided-views button:hover,
    .guided-views button:focus-visible { background: var(--toolbar-hover); }
    .diagram-nav button:focus-visible,
    .focus-chip button:focus-visible,
    .guided-views button:focus-visible {
      outline: 2px solid var(--frontend-stroke);
      outline-offset: 1px;
    }
    .diagram-nav button:disabled { opacity: 0.38; cursor: default; }
    .diagram-nav button:disabled:hover { background: transparent; }
    .diagram-nav #btn-route-probe,
    .diagram-nav #btn-overview-map,
    .diagram-nav #btn-semantic-lens {
      min-width: auto;
      padding-inline: 0.5rem;
      color: color-mix(in srgb, var(--toolbar-text) 86%, transparent);
      font-weight: 650;
    }
    .diagram-nav #btn-node-finder,
    .diagram-nav [data-view="out"] {
      position: relative;
      margin-left: 0.25rem;
    }
    .diagram-nav #btn-node-finder::before,
    .diagram-nav [data-view="out"]::before {
      position: absolute;
      top: 0.4rem;
      bottom: 0.4rem;
      left: -0.13rem;
      width: 1px;
      background: color-mix(in srgb, var(--toolbar-border) 62%, transparent);
      content: "";
      pointer-events: none;
    }
    .diagram-nav-icon {
      display: block;
      width: 0.72rem;
      height: 0.72rem;
      margin: auto;
      background: currentColor;
      -webkit-mask-position: center;
      -webkit-mask-repeat: no-repeat;
      -webkit-mask-size: contain;
      mask-position: center;
      mask-repeat: no-repeat;
      mask-size: contain;
    }
    .diagram-nav-icon.find {
      -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Ccircle cx='8.5' cy='8.5' r='4.5' fill='none' stroke='black' stroke-width='1.8'/%3E%3Cpath d='m12 12 4 4' fill='none' stroke='black' stroke-width='1.8' stroke-linecap='round'/%3E%3C/svg%3E");
      mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Ccircle cx='8.5' cy='8.5' r='4.5' fill='none' stroke='black' stroke-width='1.8'/%3E%3Cpath d='m12 12 4 4' fill='none' stroke='black' stroke-width='1.8' stroke-linecap='round'/%3E%3C/svg%3E");
    }
    .diagram-nav-icon.guide {
      -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Ccircle cx='10' cy='10' r='7' fill='none' stroke='black' stroke-width='1.7'/%3E%3Cpath d='M7.8 7.3a2.4 2.4 0 014.6.9c0 1.8-2.4 2-2.4 3.7M10 14.9h.01' fill='none' stroke='black' stroke-width='1.7' stroke-linecap='round'/%3E%3C/svg%3E");
      mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Ccircle cx='10' cy='10' r='7' fill='none' stroke='black' stroke-width='1.7'/%3E%3Cpath d='M7.8 7.3a2.4 2.4 0 014.6.9c0 1.8-2.4 2-2.4 3.7M10 14.9h.01' fill='none' stroke='black' stroke-width='1.7' stroke-linecap='round'/%3E%3C/svg%3E");
    }
    .diagram-nav-icon.minus {
      -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath d='M5 10h10' fill='none' stroke='black' stroke-width='1.8' stroke-linecap='round'/%3E%3C/svg%3E");
      mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath d='M5 10h10' fill='none' stroke='black' stroke-width='1.8' stroke-linecap='round'/%3E%3C/svg%3E");
    }
    .diagram-nav-icon.plus {
      -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath d='M10 5v10M5 10h10' fill='none' stroke='black' stroke-width='1.8' stroke-linecap='round'/%3E%3C/svg%3E");
      mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath d='M10 5v10M5 10h10' fill='none' stroke='black' stroke-width='1.8' stroke-linecap='round'/%3E%3C/svg%3E");
    }
    .diagram-nav [data-view="reset"] {
      display: inline-flex;
      align-items: center;
      justify-content: center;
      gap: 0.2rem;
      min-width: 3.1rem;
      padding-inline: 0.35rem;
    }
    .diagram-nav [data-view="reset"][data-detail-visible] {
      min-width: 3.8rem;
    }
    .diagram-nav-detail {
      color: color-mix(in srgb, var(--toolbar-text) 52%, transparent);
      font-size: 0.44rem;
      font-weight: 700;
      line-height: 1;
      letter-spacing: 0.07em;
    }
    .diagram-nav-percent {
      color: color-mix(in srgb, var(--toolbar-text) 84%, transparent);
      font-size: 0.55rem;
      font-weight: 650;
      line-height: 1;
    }
    .diagram-container[data-camera-indicator="true"] .diagram-nav-detail,
    .diagram-container[data-camera-indicator="true"] .diagram-nav-percent { color: var(--frontend-stroke); }
    .diagram-nav #btn-diagram-guide[aria-expanded="true"] {
      color: var(--cloud-stroke);
      background: var(--toolbar-hover);
    }

    /* Semantic Radar is a deliberately simplified, viewer-only overview. It
       uses authored node bounds and the live camera viewport rather than
       cloning the canonical SVG, so IDs, filters, animation, and export state
       stay singular and deterministic. */
    .overview-map {
      position: absolute;
      right: 1rem;
      bottom: calc(4.15rem + var(--mosofin-nav-reserve));
      z-index: 12;
      width: 11.25rem;
      overflow: hidden;
      border: 1px solid color-mix(in srgb, var(--frontend-stroke) 52%, var(--toolbar-border));
      border-radius: 0.78rem;
      background: var(--toolbar-menu-bg);
      color: var(--toolbar-text);
      box-shadow: 0 16px 42px rgba(0, 0, 0, 0.3);
      backdrop-filter: blur(16px);
      font-size: 0.625rem;
    }
    .overview-map[hidden] { display: none; }
    .overview-map[data-docked="true"] {
      position: fixed;
      right: var(--mosofin-radar-right, 1rem);
      top: var(--mosofin-radar-top, 1rem);
      bottom: auto;
    }
    .overview-map[data-docked="true"][data-dock-side="left"] {
      right: auto;
      left: var(--mosofin-radar-left, 1rem);
    }
    html[data-preset="blueprint"] .overview-map { border-radius: 0.28rem; }
    .overview-map-head {
      display: grid;
      grid-template-columns: auto minmax(0, 1fr) auto;
      align-items: center;
      gap: 0.45rem;
      min-height: 2.25rem;
      padding: 0.42rem 0.48rem 0.38rem 0.55rem;
      border-bottom: 1px solid var(--toolbar-border);
      background: linear-gradient(110deg, color-mix(in srgb, var(--frontend-fill) 45%, transparent), transparent 68%);
      cursor: grab;
      touch-action: none;
      user-select: none;
    }
    .overview-map[data-panel-dragging="true"] .overview-map-head { cursor: grabbing; }
    .overview-map[data-placement-invalid="true"] {
      border-color: var(--security-stroke);
      box-shadow: 0 0 0 2px color-mix(in srgb, var(--security-stroke) 34%, transparent), 0 16px 42px rgba(0, 0, 0, 0.3);
    }
    .overview-map[data-compact="true"] {
      width: 7.4rem;
    }
    .overview-map[data-compact="true"] .overview-map-head {
      grid-template-columns: minmax(0, 1fr) auto;
      gap: 0.24rem;
      min-height: 1.5rem;
      padding: 0.05rem 0.16rem 0.05rem 0.32rem;
      border-bottom: 0;
    }
    .overview-map[data-compact="true"] .overview-map-live,
    .overview-map[data-compact="true"] .overview-map-copy,
    .overview-map[data-compact="true"] .overview-map-surface,
    .overview-map[data-compact="true"] .overview-map-hint {
      display: none;
    }
    .overview-map[data-compact="true"] .overview-map-expand { display: inline-flex; }
    .overview-map[data-compact="true"] .overview-map-close {
      width: 1.15rem;
      height: 1.15rem;
      font-size: 0.72rem;
    }
    .overview-map-live {
      width: 0.42rem;
      height: 0.42rem;
      border-radius: 50%;
      background: var(--backend-stroke);
      box-shadow: 0 0 0 0 color-mix(in srgb, var(--backend-stroke) 52%, transparent);
      animation: mosofin-radar-live 2.4s ease-out infinite;
    }
    .overview-map-copy { min-width: 0; line-height: 1.1; }
    .overview-map-copy strong {
      display: block;
      overflow: hidden;
      color: var(--frontend-stroke);
      font-size: 0.59rem;
      letter-spacing: 0.12em;
      text-transform: uppercase;
      white-space: nowrap;
      text-overflow: ellipsis;
    }
    .overview-map-copy small {
      display: block;
      margin-top: 0.18rem;
      overflow: hidden;
      color: var(--text-faint);
      font-size: 0.52rem;
      letter-spacing: 0.04em;
      white-space: nowrap;
      text-overflow: ellipsis;
    }
    .overview-map-expand {
      display: none;
      align-items: center;
      justify-content: center;
      min-width: 0;
      height: 1.2rem;
      padding: 0 0.34rem;
      overflow: hidden;
      border: 0;
      border-radius: 0.3rem;
      background: transparent;
      color: var(--frontend-stroke);
      font: inherit;
      font-size: 0.49rem;
      font-weight: 700;
      letter-spacing: 0.06em;
      text-transform: uppercase;
      white-space: nowrap;
      cursor: pointer;
    }
    .overview-map-close {
      width: 1.55rem;
      height: 1.55rem;
      padding: 0;
      border: 0;
      border-radius: 0.34rem;
      background: transparent;
      color: var(--toolbar-text);
      font: inherit;
      font-size: 0.9rem;
      cursor: pointer;
    }
    .overview-map-expand:hover,
    .overview-map-expand:focus-visible,
    .overview-map-close:hover,
    .overview-map-close:focus-visible { background: var(--toolbar-hover); }
    .overview-map-expand:focus-visible,
    .overview-map-close:focus-visible,
    .overview-map-surface:focus-visible,
    .overview-map-node:focus-visible {
      outline: 2px solid var(--frontend-stroke);
      outline-offset: 1px;
    }
    .overview-map-surface {
      position: relative;
      height: 7rem;
      margin: 0.42rem;
      overflow: hidden;
      border: 1px solid color-mix(in srgb, var(--panel-border) 72%, transparent);
      border-radius: 0.42rem;
      background:
        linear-gradient(color-mix(in srgb, var(--panel-border) 26%, transparent) 1px, transparent 1px),
        linear-gradient(90deg, color-mix(in srgb, var(--panel-border) 26%, transparent) 1px, transparent 1px),
        color-mix(in srgb, var(--bg) 86%, transparent);
      background-size: 12px 12px;
      cursor: crosshair;
      touch-action: none;
    }
    html[data-preset="blueprint"] .overview-map-surface { border-radius: 0.1rem; }
    .overview-map-surface > svg {
      width: 100%;
      height: 100%;
      min-width: 0;
      transform: none !important;
      transition: none !important;
      display: block;
      overflow: visible;
    }
    .overview-map-node {
      fill: var(--backend-stroke);
      fill-opacity: 0.52;
      stroke: var(--backend-stroke);
      stroke-opacity: 0.86;
      vector-effect: non-scaling-stroke;
      cursor: pointer;
      transition: fill-opacity 140ms ease, stroke-width 140ms ease, filter 140ms ease;
    }
    .overview-map-node[data-kind="frontend"],
    .overview-map-node[data-kind="start"] { fill: var(--frontend-stroke); stroke: var(--frontend-stroke); }
    .overview-map-node[data-kind="database"],
    .overview-map-node[data-kind="success"] { fill: var(--database-stroke); stroke: var(--database-stroke); }
    .overview-map-node[data-kind="cloud"],
    .overview-map-node[data-kind="waiting"] { fill: var(--cloud-stroke); stroke: var(--cloud-stroke); }
    .overview-map-node[data-kind="messagebus"] { fill: var(--messagebus-stroke); stroke: var(--messagebus-stroke); }
    .overview-map-node[data-kind="security"],
    .overview-map-node[data-kind="decision"],
    .overview-map-node[data-kind="failure"] { fill: var(--security-stroke); stroke: var(--security-stroke); }
    .overview-map-node[data-kind="external"],
    .overview-map-node[data-kind="neutral"] { fill: var(--external-stroke); stroke: var(--external-stroke); }
    .overview-map-node[data-radar-active="true"],
    .overview-map-node:hover,
    .overview-map-node:focus-visible {
      fill-opacity: 0.92;
      stroke-width: 2.2;
      filter: drop-shadow(0 0 3px currentColor);
    }
    .overview-map-viewport {
      fill: color-mix(in srgb, var(--frontend-fill) 56%, transparent);
      stroke: var(--frontend-stroke);
      stroke-width: 2;
      vector-effect: non-scaling-stroke;
      pointer-events: none;
      filter: drop-shadow(0 0 3px color-mix(in srgb, var(--frontend-stroke) 54%, transparent));
    }
    .overview-map-hint {
      display: flex;
      justify-content: space-between;
      gap: 0.5rem;
      padding: 0 0.52rem 0.46rem;
      color: var(--text-faint);
      font-size: 0.49rem;
      letter-spacing: 0.06em;
      text-transform: uppercase;
    }
    .diagram-nav #btn-overview-map[aria-expanded="true"] {
      color: var(--frontend-stroke);
      background: var(--toolbar-hover);
    }
    .overview-map-feedback {
      position: absolute;
      right: 1rem;
      bottom: 4.15rem;
      z-index: 13;
      max-width: min(16rem, calc(100% - 2rem));
      padding: 0.38rem 0.55rem;
      border: 1px solid color-mix(in srgb, var(--frontend-stroke) 52%, var(--toolbar-border));
      border-radius: 0.5rem;
      background: var(--toolbar-menu-bg);
      color: var(--toolbar-text);
      box-shadow: 0 8px 24px rgba(0, 0, 0, 0.2);
      font-size: 0.52rem;
      line-height: 1.35;
      pointer-events: none;
    }
    .overview-map-feedback[hidden] { display: none; }
    .focus-chip[data-radar-yielded="true"] { display: none !important; }
    .diagram-nav #btn-route-probe[aria-pressed="true"] {
      color: var(--backend-stroke);
      background: var(--toolbar-hover);
    }
    .diagram-nav #btn-semantic-lens[aria-expanded="true"],
    .diagram-nav #btn-semantic-lens[aria-pressed="true"] {
      color: var(--database-stroke);
      background: var(--toolbar-hover);
    }

    /* Semantic Lens turns the compiled node-kind palette into an analytical
       legend. One kind reveals its real traffic; two kinds compare only their
       direct authored relationships. Geometry and the canonical SVG stay put. */
    .semantic-lens {
      position: absolute;
      right: 1rem;
      bottom: calc(4.15rem + var(--mosofin-nav-reserve));
      z-index: 24;
      width: min(20rem, calc(100% - 2rem));
      overflow: hidden;
      border: 1px solid color-mix(in srgb, var(--database-stroke) 58%, var(--toolbar-border));
      border-radius: 0.82rem;
      background:
        linear-gradient(135deg, color-mix(in srgb, var(--database-fill) 34%, transparent), transparent 48%),
        var(--toolbar-menu-bg);
      color: var(--toolbar-text);
      box-shadow: 0 18px 52px rgba(0, 0, 0, 0.36);
      backdrop-filter: blur(18px);
      animation: mosofin-lens-in 180ms cubic-bezier(0.22, 1, 0.36, 1);
    }
    .semantic-lens[hidden] { display: none; }
    .semantic-lens[data-dock-side="left"] { left: 1rem; right: auto; }
    .semantic-lens[data-dock-side="right"] { left: auto; right: 1rem; }
    html[data-preset="blueprint"] .semantic-lens { border-radius: 0.28rem; }
    .semantic-lens-head {
      display: grid;
      grid-template-columns: minmax(0, 1fr) auto;
      align-items: start;
      gap: 0.75rem;
      padding: 0.72rem 0.78rem 0.62rem;
      border-bottom: 1px solid var(--toolbar-border);
    }
    .semantic-lens-eyebrow {
      display: block;
      color: var(--database-stroke);
      font-size: 0.5rem;
      font-weight: 800;
      letter-spacing: 0.16em;
      text-transform: uppercase;
    }
    .semantic-lens-title {
      display: block;
      margin-top: 0.16rem;
      color: var(--text);
      font-size: 0.82rem;
      letter-spacing: -0.02em;
    }
    .semantic-lens-close {
      width: 1.75rem;
      height: 1.75rem;
      padding: 0;
      border: 0;
      border-radius: 0.35rem;
      background: transparent;
      color: var(--toolbar-text);
      font: inherit;
      font-size: 1rem;
      cursor: pointer;
    }
    .semantic-lens-close:hover,
    .semantic-lens-close:focus-visible { background: var(--toolbar-hover); }
    .semantic-lens-instruction {
      margin: 0;
      padding: 0.52rem 0.78rem 0.48rem;
      color: var(--text-faint);
      font-size: 0.55rem;
      line-height: 1.45;
    }
    .semantic-lens-kinds {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 0.3rem;
      max-height: min(15rem, 40vh);
      padding: 0 0.5rem 0.5rem;
      overflow-y: auto;
      scrollbar-width: thin;
    }
    .semantic-lens-kind {
      --lens-color: var(--external-stroke);
      display: grid;
      grid-template-columns: auto minmax(0, 1fr) auto;
      align-items: center;
      gap: 0.42rem;
      min-width: 0;
      min-height: 2.15rem;
      padding: 0.35rem 0.42rem;
      border: 1px solid color-mix(in srgb, var(--lens-color) 24%, var(--toolbar-border));
      border-radius: 0.45rem;
      background: color-mix(in srgb, var(--panel) 76%, transparent);
      color: var(--toolbar-text);
      font: inherit;
      text-align: left;
      cursor: pointer;
      transition: border-color 140ms ease, background 140ms ease, opacity 140ms ease;
    }
    html[data-preset="blueprint"] .semantic-lens-kind { border-radius: 0.12rem; }
    .semantic-lens-kind[data-kind="frontend"],
    .semantic-lens-kind[data-kind="start"] { --lens-color: var(--frontend-stroke); }
    .semantic-lens-kind[data-kind="backend"],
    .semantic-lens-kind[data-kind="active"] { --lens-color: var(--backend-stroke); }
    .semantic-lens-kind[data-kind="database"],
    .semantic-lens-kind[data-kind="success"] { --lens-color: var(--database-stroke); }
    .semantic-lens-kind[data-kind="cloud"],
    .semantic-lens-kind[data-kind="waiting"] { --lens-color: var(--cloud-stroke); }
    .semantic-lens-kind[data-kind="messagebus"] { --lens-color: var(--messagebus-stroke); }
    .semantic-lens-kind[data-kind="security"],
    .semantic-lens-kind[data-kind="decision"],
    .semantic-lens-kind[data-kind="failure"] { --lens-color: var(--security-stroke); }
    .semantic-lens-kind:hover,
    .semantic-lens-kind:focus-visible,
    .semantic-lens-kind[aria-pressed="true"] {
      border-color: color-mix(in srgb, var(--lens-color) 78%, var(--toolbar-border));
      background: color-mix(in srgb, var(--lens-color) 11%, var(--panel));
    }
    .semantic-lens-kind:focus-visible,
    .semantic-lens-close:focus-visible,
    .semantic-lens-actions button:focus-visible {
      outline: 2px solid var(--database-stroke);
      outline-offset: 1px;
    }
    .semantic-lens-kind:disabled { opacity: 0.34; cursor: default; }
    .semantic-lens-swatch {
      width: 0.48rem;
      height: 0.48rem;
      border-radius: 50%;
      background: var(--lens-color);
      box-shadow: 0 0 8px color-mix(in srgb, var(--lens-color) 58%, transparent);
    }
    .semantic-lens-kind strong {
      overflow: hidden;
      color: var(--text);
      font-size: 0.61rem;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .semantic-lens-kind em {
      color: var(--lens-color);
      font-size: 0.54rem;
      font-style: normal;
      font-weight: 800;
    }
    .semantic-lens-status {
      min-height: 2.5rem;
      margin: 0;
      padding: 0.55rem 0.78rem;
      border-top: 1px solid var(--toolbar-border);
      color: var(--text-faint);
      font-size: 0.55rem;
      line-height: 1.45;
    }
    .semantic-lens-status strong { color: var(--database-stroke); font-weight: 800; }
    .semantic-lens-actions {
      display: flex;
      justify-content: flex-end;
      gap: 0.28rem;
      padding: 0 0.62rem 0.62rem;
    }
    .semantic-lens-actions button {
      min-height: 1.8rem;
      padding: 0.28rem 0.48rem;
      border: 0;
      border-radius: 0.35rem;
      background: transparent;
      color: var(--toolbar-text);
      font: inherit;
      font-size: 0.56rem;
      cursor: pointer;
    }
    .semantic-lens-actions button:hover { background: var(--toolbar-hover); }
    .semantic-lens-actions #semantic-lens-copy { color: var(--database-stroke); }
    @keyframes mosofin-lens-in {
      from { opacity: 0; transform: translateY(5px) scale(0.985); }
      to { opacity: 1; transform: translateY(0) scale(1); }
    }

    /* Route Probe answers a reader question over the compiled semantic graph:
       choose two stable nodes and reveal the shortest authored directed route.
       The receipt and signal live entirely in the viewer layer. */
    .route-probe {
      position: absolute;
      left: 1rem;
      bottom: calc(1rem + var(--mosofin-nav-reserve));
      z-index: 12;
      width: min(46rem, calc(100% - 25rem));
      min-width: 18rem;
      overflow: hidden;
      border: 1px solid color-mix(in srgb, var(--backend-stroke) 62%, var(--toolbar-border));
      border-radius: 0.82rem;
      background:
        linear-gradient(115deg, color-mix(in srgb, var(--backend-fill) 50%, transparent), transparent 62%),
        var(--toolbar-menu-bg);
      color: var(--toolbar-text);
      box-shadow: 0 18px 48px rgba(0, 0, 0, 0.3);
      backdrop-filter: blur(16px);
      font-size: 0.625rem;
    }
    .route-probe[hidden] { display: none; }
    .route-probe[data-route-dock="top"] {
      top: 1rem;
      bottom: auto;
    }
    html[data-preset="blueprint"] .route-probe { border-radius: 0.25rem; }
    .route-probe-head {
      display: grid;
      grid-template-columns: minmax(0, 1fr) auto;
      align-items: start;
      gap: 0.65rem;
      padding: 0.52rem 0.58rem 0.44rem;
      border-bottom: 1px solid color-mix(in srgb, var(--toolbar-border) 80%, transparent);
    }
    .route-probe-copy { min-width: 0; }
    .route-probe-eyebrow {
      display: block;
      color: var(--backend-stroke);
      font-size: 0.48rem;
      font-weight: 800;
      letter-spacing: 0.15em;
      text-transform: uppercase;
    }
    .route-probe-title {
      display: block;
      margin-top: 0.14rem;
      overflow: hidden;
      color: var(--toolbar-text);
      font-size: 0.72rem;
      line-height: 1.25;
      white-space: nowrap;
      text-overflow: ellipsis;
    }
    .route-probe-actions { display: flex; gap: 0.2rem; }
    .route-probe button {
      min-height: 1.55rem;
      padding: 0.25rem 0.42rem;
      border: 0;
      border-radius: 0.36rem;
      background: transparent;
      color: var(--toolbar-text);
      font: inherit;
      cursor: pointer;
    }
    .route-probe button:hover,
    .route-probe button:focus-visible { background: var(--toolbar-hover); }
    .route-probe button:focus-visible {
      outline: 2px solid var(--backend-stroke);
      outline-offset: 1px;
    }
    .route-probe-copy-link { color: var(--frontend-stroke) !important; }
    .route-probe-find { color: var(--backend-stroke) !important; }
    .route-probe-copy-link[hidden] { display: none; }
    .route-probe-find[hidden] { display: none; }
    .route-probe-path {
      display: flex;
      align-items: center;
      min-height: 2.2rem;
      padding: 0.42rem 0.58rem 0.24rem;
      overflow-x: auto;
      scrollbar-width: thin;
      white-space: nowrap;
    }
    .route-probe-node,
    .route-probe button.route-probe-node {
      flex: none;
      max-width: 9.5rem;
      overflow: hidden;
      min-height: 1.75rem !important;
      padding: 0.22rem 0.38rem;
      border: 1px solid color-mix(in srgb, var(--backend-stroke) 46%, var(--toolbar-border));
      border-radius: 999px;
      background: color-mix(in srgb, var(--backend-fill) 52%, transparent);
      color: var(--text);
      font-size: 0.56rem;
      font-weight: 700;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .route-probe-node[data-endpoint="start"] {
      border-color: var(--frontend-stroke);
      color: var(--frontend-stroke);
    }
    .route-probe-node[data-endpoint="end"] {
      border-color: var(--security-stroke);
      color: var(--security-stroke);
    }
    .route-probe-node[data-route-journey-state="past"] { opacity: 0.66; }
    .route-probe-node[data-route-journey-state="future"] { opacity: 0.48; }
    .route-probe-node[aria-current="step"] {
      border-color: var(--backend-stroke);
      background: color-mix(in srgb, var(--backend-fill) 82%, var(--toolbar-menu-bg));
      color: var(--text);
      box-shadow: 0 0 0 1px color-mix(in srgb, var(--backend-stroke) 42%, transparent), 0 0 14px color-mix(in srgb, var(--backend-stroke) 34%, transparent);
      opacity: 1;
    }
    html[data-preset="blueprint"] .route-probe-node[aria-current="step"] {
      border-style: double;
      border-width: 3px;
      border-radius: 0.2rem;
      box-shadow: none;
    }
    .route-probe-arrow {
      flex: none;
      padding: 0 0.28rem;
      color: var(--text-faint);
      font-size: 0.65rem;
    }
    .route-probe-placeholder {
      color: var(--text-faint);
      font-size: 0.56rem;
      letter-spacing: 0.03em;
    }
    .route-probe-status {
      display: block;
      min-height: 1.45rem;
      padding: 0.08rem 0.58rem 0.44rem;
      color: var(--text-faint);
      font-size: 0.52rem;
      line-height: 1.35;
    }
    .route-journey-controls {
      display: grid;
      grid-template-columns: 2rem minmax(5.8rem, auto) 2rem minmax(4.8rem, auto);
      justify-content: start;
      gap: 0.22rem;
      padding: 0.08rem 0.58rem 0.28rem;
    }
    .route-journey-controls[hidden] { display: none; }
    .route-journey-controls button {
      min-height: 1.8rem;
      border: 1px solid color-mix(in srgb, var(--toolbar-border) 86%, transparent);
      background: color-mix(in srgb, var(--toolbar-bg) 72%, transparent);
      font-size: 0.54rem;
      font-weight: 700;
    }
    .route-journey-controls button:disabled {
      cursor: default;
      opacity: 0.34;
    }
    .route-journey-play { color: var(--backend-stroke) !important; }
    .route-journey-overview { color: var(--frontend-stroke) !important; }
    .route-journey-controls[data-playing="true"] .route-journey-play {
      border-color: color-mix(in srgb, var(--backend-stroke) 64%, var(--toolbar-border));
      background: color-mix(in srgb, var(--backend-fill) 62%, transparent);
    }
    .route-probe[data-state="error"] {
      border-color: color-mix(in srgb, var(--security-stroke) 72%, var(--toolbar-border));
    }
    .route-probe[data-state="error"] .route-probe-status { color: var(--security-stroke); }

    .focus-chip {
      position: absolute;
      left: 1rem;
      top: 1rem;
      z-index: 12;
      display: block;
      width: min(22rem, calc(100% - 13rem));
      max-width: calc(100% - 13rem);
      overflow: hidden;
      border: 1px solid var(--frontend-stroke);
      border-radius: 0.85rem;
      background: var(--toolbar-menu-bg);
      color: var(--toolbar-text);
      box-shadow: 0 18px 48px rgba(0, 0, 0, 0.3);
      backdrop-filter: blur(16px);
      font-size: 0.6875rem;
    }
    .focus-chip[hidden] { display: none; }
    html[data-preset="blueprint"] .focus-chip { border-radius: 0.3rem; }
    .focus-chip button {
      flex: none;
      padding: 0.3rem 0.45rem;
      border-radius: 0.35rem;
      font-size: 0.625rem;
    }
    .relationship-lens-head {
      display: grid;
      grid-template-columns: minmax(0, 1fr) auto;
      align-items: start;
      gap: 0.75rem;
      padding: 0.7rem 0.75rem 0.65rem;
      border-bottom: 1px solid var(--toolbar-border);
      background: linear-gradient(120deg, color-mix(in srgb, var(--frontend-fill) 34%, transparent), transparent 72%);
    }
    .relationship-lens-copy { min-width: 0; }
    .relationship-lens-actions {
      display: flex;
      flex-direction: column;
      align-items: flex-end;
      gap: 0.18rem;
    }
    .relationship-lens-actions button { white-space: nowrap; }
    .relationship-lens-actions #btn-focus-clear {
      display: grid;
      width: 1.75rem;
      height: 1.75rem;
      padding: 0;
      place-items: center;
      color: var(--text-muted);
      font-size: 1rem;
      line-height: 1;
    }
    .relationship-lens-actions #btn-focus-clear:hover,
    .relationship-lens-actions #btn-focus-clear:focus-visible {
      background: var(--toolbar-hover);
      color: var(--text);
    }
    .relationship-lens-actions #btn-focus-copy { color: var(--frontend-stroke); }
    .relationship-lens-actions #btn-focus-relations { display: none; }
    .relationship-lens-eyebrow {
      display: block;
      margin-bottom: 0.18rem;
      color: var(--frontend-stroke);
      font-size: 0.5rem;
      font-weight: 800;
      letter-spacing: 0.14em;
      text-transform: uppercase;
    }
    .relationship-lens-title {
      display: block;
      overflow: hidden;
      color: var(--text);
      font-size: 0.8rem;
      line-height: 1.35;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .semantic-passport-detail {
      display: block;
      margin-top: 0.12rem;
      overflow: hidden;
      color: var(--text-muted);
      font-size: 0.625rem;
      line-height: 1.4;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .semantic-passport-detail[hidden] { display: none; }
    .semantic-passport-meta {
      display: flex;
      flex-wrap: wrap;
      gap: 0.24rem;
      margin-top: 0.42rem;
    }
    .semantic-passport-meta span,
    .semantic-passport-meta code {
      max-width: 100%;
      overflow: hidden;
      padding: 0.14rem 0.34rem;
      border: 1px solid color-mix(in srgb, var(--toolbar-border) 82%, transparent);
      border-radius: 999px;
      background: color-mix(in srgb, var(--panel) 68%, transparent);
      color: var(--text-faint);
      font: inherit;
      font-size: 0.5rem;
      letter-spacing: 0.04em;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .semantic-passport-meta [data-passport="kind"] {
      border-color: color-mix(in srgb, var(--frontend-stroke) 50%, var(--toolbar-border));
      color: var(--frontend-stroke);
      font-weight: 800;
      text-transform: uppercase;
    }
    .semantic-passport-meta [data-passport="tag"] { color: var(--cloud-stroke); }
    .semantic-passport-meta [data-passport="brand"] {
      border-color: color-mix(in srgb, var(--messagebus-stroke) 52%, var(--toolbar-border));
      color: var(--messagebus-stroke);
      font-weight: 700;
    }
    .semantic-passport-meta [hidden] { display: none; }
    html[data-preset="blueprint"] .semantic-passport-meta span,
    html[data-preset="blueprint"] .semantic-passport-meta code { border-radius: 0.1rem; }
    .semantic-passport-evidence {
      margin-top: 0.52rem;
      padding: 0.5rem;
      border: 1px solid color-mix(in srgb, var(--backend-stroke) 32%, var(--toolbar-border));
      border-radius: 0.55rem;
      background: color-mix(in srgb, var(--backend-fill) 18%, var(--panel));
    }
    .semantic-passport-evidence[hidden] { display: none; }
    html[data-preset="blueprint"] .semantic-passport-evidence { border-radius: 0.18rem; }
    .semantic-passport-evidence-head {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 0.55rem;
      margin-bottom: 0.35rem;
    }
    .semantic-passport-evidence-status {
      display: inline-flex;
      align-items: center;
      gap: 0.28rem;
      color: var(--backend-stroke);
      font-size: 0.5rem;
      font-weight: 800;
      letter-spacing: 0.08em;
      text-transform: uppercase;
    }
    .semantic-passport-evidence-status::before {
      content: '';
      width: 0.38rem;
      height: 0.38rem;
      border-radius: 50%;
      background: var(--backend-stroke);
      box-shadow: 0 0 0 3px color-mix(in srgb, var(--backend-stroke) 14%, transparent);
    }
    .semantic-passport-repository {
      min-width: 0;
      overflow: hidden;
      color: var(--text-faint);
      font-size: 0.5rem;
      text-decoration: none;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .semantic-passport-repository:hover,
    .semantic-passport-repository:focus-visible { color: var(--backend-stroke); }
    .semantic-passport-evidence-links { display: grid; gap: 0.22rem; }
    .semantic-passport-source {
      display: grid;
      grid-template-columns: minmax(0, 1fr) auto;
      gap: 0.15rem 0.45rem;
      align-items: center;
      padding: 0.34rem 0.4rem;
      border: 1px solid transparent;
      border-radius: 0.38rem;
      color: var(--text-muted);
      text-decoration: none;
    }
    .semantic-passport-source:hover,
    .semantic-passport-source:focus-visible {
      border-color: color-mix(in srgb, var(--backend-stroke) 42%, transparent);
      background: color-mix(in srgb, var(--backend-fill) 32%, transparent);
      color: var(--text);
    }
    .semantic-passport-source strong {
      overflow: hidden;
      font-size: 0.5625rem;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .semantic-passport-source code {
      color: var(--backend-stroke);
      font: inherit;
      font-size: 0.5rem;
    }
    .semantic-passport-source small {
      grid-column: 1 / -1;
      overflow: hidden;
      color: var(--text-faint);
      font-size: 0.5rem;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .relationship-lens-summary {
      display: block;
      margin-top: 0.12rem;
      color: var(--text-faint);
      font-size: 0.5625rem;
    }
    .semantic-passport-reach {
      display: grid;
      gap: 0.28rem;
      margin-top: 0.52rem;
      padding-top: 0.48rem;
      border-top: 1px solid color-mix(in srgb, var(--toolbar-border) 76%, transparent);
    }
    .semantic-passport-reach[hidden] { display: none; }
    .semantic-passport-reach-label {
      color: var(--text-faint);
      font-size: 0.5rem;
      font-weight: 800;
      letter-spacing: 0.1em;
      text-transform: uppercase;
    }
    .semantic-passport-reach-actions {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 0.25rem;
    }
    .semantic-passport-reach-actions button {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 0.5rem;
      min-width: 0;
      min-height: 2rem;
      padding: 0.32rem 0.42rem;
      border: 1px solid color-mix(in srgb, var(--toolbar-border) 86%, transparent);
      background: color-mix(in srgb, var(--panel) 70%, transparent);
      color: var(--text-muted);
      text-align: left;
    }
    .semantic-passport-reach-actions button:hover:not(:disabled),
    .semantic-passport-reach-actions button:focus-visible {
      border-color: color-mix(in srgb, var(--frontend-stroke) 62%, var(--toolbar-border));
      color: var(--text);
    }
    .semantic-passport-reach-actions button:focus-visible {
      outline: 2px solid var(--frontend-stroke);
      outline-offset: 1px;
    }
    .semantic-passport-reach-actions button:disabled {
      cursor: default;
      opacity: 0.5;
    }
    .semantic-passport-reach-actions button[aria-pressed="true"] {
      border-color: color-mix(in srgb, var(--reach-color, var(--frontend-stroke)) 72%, var(--toolbar-border));
      background: color-mix(in srgb, var(--reach-fill, var(--frontend-fill)) 48%, transparent);
      color: var(--text);
    }
    #btn-reach-upstream {
      --reach-color: var(--database-stroke);
      --reach-fill: var(--database-fill);
    }
    #btn-reach-downstream {
      --reach-color: var(--backend-stroke);
      --reach-fill: var(--backend-fill);
    }
    .semantic-passport-reach-actions strong {
      color: var(--reach-color, var(--frontend-stroke));
      font-size: 0.625rem;
    }
    .semantic-passport-reach-status {
      color: var(--text-faint);
      font-size: 0.525rem;
      line-height: 1.4;
    }
    .semantic-passport-reach-status[hidden] { display: none; }
    .relationship-lens-list {
      max-height: min(18rem, 46vh);
      overflow: auto;
      padding: 0.4rem;
      scrollbar-width: thin;
      scrollbar-color: var(--frontend-stroke) transparent;
    }
    .relationship-lens-list::-webkit-scrollbar { width: 4px; }
    .relationship-lens-list::-webkit-scrollbar-thumb {
      border-radius: 999px;
      background: color-mix(in srgb, var(--frontend-stroke) 68%, transparent);
    }
    .relationship-lens-group + .relationship-lens-group { margin-top: 0.35rem; }
    .relationship-lens-group-title {
      display: block;
      padding: 0.24rem 0.38rem 0.16rem;
      color: var(--text-faint);
      font-size: 0.5rem;
      font-weight: 800;
      letter-spacing: 0.12em;
      text-transform: uppercase;
    }
    .relationship-lens-row {
      display: grid;
      grid-template-columns: 2.1rem minmax(0, 1fr);
      gap: 0.06rem 0.45rem;
      width: 100%;
      padding: 0.45rem 0.5rem !important;
      text-align: left;
    }
    .relationship-lens-row:hover,
    .relationship-lens-row:focus-visible,
    .relationship-lens-row[data-preview-active="true"] {
      background: var(--toolbar-hover);
      box-shadow: inset 2px 0 0 var(--frontend-stroke);
    }
    .relationship-lens-row:focus-visible {
      outline: 2px solid var(--frontend-stroke);
      outline-offset: -2px;
    }
    .relationship-lens-direction {
      grid-row: 1 / 3;
      align-self: center;
      color: var(--frontend-stroke);
      font-size: 0.5rem;
      font-weight: 800;
      letter-spacing: 0.08em;
    }
    .relationship-lens-row[data-direction="in"] .relationship-lens-direction { color: var(--database-stroke); }
    .relationship-lens-row[data-direction="loop"] .relationship-lens-direction { color: var(--security-stroke); }
    .relationship-lens-row strong {
      overflow: hidden;
      color: var(--text);
      font-size: 0.6875rem;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .relationship-lens-row small {
      overflow: hidden;
      color: var(--text-faint);
      font-size: 0.5625rem;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .relationship-lens-empty {
      margin: 0;
      padding: 0.75rem;
      color: var(--text-faint);
      font-size: 0.625rem;
      text-align: center;
    }

    /* Diagram Guide keeps the artifact's growing reader vocabulary
       discoverable without widening the permanent control bar. It derives
       honest counts at runtime and delegates every action to an existing
       production interaction instead of creating a second command system. */
    .diagram-guide {
      position: absolute;
      top: 1rem;
      right: 1rem;
      z-index: 26;
      width: min(32rem, calc(100% - 2rem));
      max-height: min(42rem, calc(100% - 2rem));
      overflow: auto;
      border: 1px solid color-mix(in srgb, var(--cloud-stroke) 58%, var(--toolbar-border));
      border-radius: 0.82rem;
      background:
        linear-gradient(135deg, color-mix(in srgb, var(--cloud-fill) 32%, transparent), transparent 42%),
        var(--toolbar-menu-bg);
      color: var(--toolbar-text);
      box-shadow: 0 22px 64px rgba(0, 0, 0, 0.4);
      backdrop-filter: blur(18px);
      animation: mosofin-guide-in 180ms cubic-bezier(0.22, 1, 0.36, 1);
    }
    .diagram-guide[hidden] { display: none; }
    html[data-preset="blueprint"] .diagram-guide { border-radius: 0.28rem; }
    .diagram-guide-head {
      display: grid;
      grid-template-columns: minmax(0, 1fr) auto;
      align-items: start;
      gap: 1rem;
      padding: 0.82rem 0.9rem 0.68rem;
      border-bottom: 1px solid var(--toolbar-border);
    }
    .diagram-guide-eyebrow {
      display: block;
      color: var(--cloud-stroke);
      font-size: 0.5rem;
      font-weight: 800;
      letter-spacing: 0.16em;
      text-transform: uppercase;
    }
    .diagram-guide-title {
      display: block;
      margin-top: 0.16rem;
      color: var(--text);
      font-size: 0.86rem;
      letter-spacing: -0.02em;
    }
    .diagram-guide-close {
      width: 1.8rem;
      height: 1.8rem;
      border: 0;
      border-radius: 0.35rem;
      background: transparent;
      color: var(--toolbar-text);
      font: inherit;
      font-size: 1rem;
      cursor: pointer;
    }
    .diagram-guide-close:hover,
    .diagram-guide-close:focus-visible { background: var(--toolbar-hover); }
    .diagram-guide-stats {
      display: flex;
      align-items: center;
      gap: 0.5rem;
      margin: 0;
      padding: 0.54rem 0.9rem;
      border-bottom: 1px solid color-mix(in srgb, var(--toolbar-border) 74%, transparent);
      color: var(--text-faint);
      font-size: 0.56rem;
      letter-spacing: 0.025em;
    }
    .diagram-guide-stats::before {
      content: '';
      width: 0.42rem;
      height: 0.42rem;
      flex: none;
      border-radius: 50%;
      background: var(--backend-stroke);
      box-shadow: 0 0 10px color-mix(in srgb, var(--backend-stroke) 72%, transparent);
    }
    .diagram-guide-actions {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 0.38rem;
      padding: 0.62rem;
    }
    .diagram-guide-action {
      --guide-accent: var(--frontend-stroke);
      display: grid;
      grid-template-columns: auto minmax(0, 1fr) auto;
      align-items: center;
      gap: 0.55rem;
      min-height: 4.15rem;
      padding: 0.55rem 0.58rem;
      border: 1px solid color-mix(in srgb, var(--guide-accent) 22%, var(--toolbar-border));
      border-radius: 0.55rem;
      background: color-mix(in srgb, var(--panel) 78%, transparent);
      color: var(--toolbar-text);
      font: inherit;
      text-align: left;
      cursor: pointer;
      transition: border-color 140ms ease, background 140ms ease, transform 140ms ease;
    }
    .diagram-guide-action[data-guide-action="route"] { --guide-accent: var(--backend-stroke); }
    .diagram-guide-action[data-guide-action="map"] { --guide-accent: var(--database-stroke); }
    .diagram-guide-action[data-guide-action="lens"] { --guide-accent: var(--messagebus-stroke); }
    .diagram-guide-action[data-guide-action="story"] { --guide-accent: var(--security-stroke); }
    .diagram-guide-action[data-guide-action="present"] {
      --guide-accent: var(--cloud-stroke);
    }
    .diagram-guide-action:hover,
    .diagram-guide-action:focus-visible {
      border-color: color-mix(in srgb, var(--guide-accent) 76%, var(--toolbar-border));
      background: color-mix(in srgb, var(--guide-accent) 9%, var(--panel));
      transform: translateY(-1px);
    }
    .diagram-guide-action:focus-visible,
    .diagram-guide-close:focus-visible {
      outline: 2px solid var(--cloud-stroke);
      outline-offset: 1px;
    }
    .diagram-guide-action:disabled {
      opacity: 0.42;
      cursor: default;
      transform: none;
    }
    .diagram-guide-index {
      display: grid;
      place-items: center;
      width: 1.65rem;
      height: 1.65rem;
      border: 1px solid color-mix(in srgb, var(--guide-accent) 64%, var(--toolbar-border));
      border-radius: 0.35rem;
      color: var(--guide-accent);
      font-size: 0.5rem;
      font-weight: 800;
      letter-spacing: 0.06em;
    }
    .diagram-guide-action-copy { min-width: 0; }
    .diagram-guide-action strong {
      display: block;
      overflow: hidden;
      color: var(--text);
      font-size: 0.66rem;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .diagram-guide-action small {
      display: -webkit-box;
      margin-top: 0.18rem;
      overflow: hidden;
      color: var(--text-faint);
      font-size: 0.5rem;
      line-height: 1.35;
      -webkit-box-orient: vertical;
      -webkit-line-clamp: 2;
    }
    .diagram-guide kbd {
      align-self: start;
      min-width: 1.45rem;
      padding: 0.12rem 0.3rem;
      border: 1px solid var(--toolbar-border);
      border-radius: 0.25rem;
      color: var(--text-faint);
      font: inherit;
      font-size: 0.5rem;
      text-align: center;
    }
    .diagram-guide-shortcuts {
      display: flex;
      flex-wrap: wrap;
      gap: 0.34rem 0.62rem;
      padding: 0.58rem 0.9rem 0.68rem;
      border-top: 1px solid var(--toolbar-border);
      color: var(--text-faint);
      font-size: 0.5rem;
    }
    .diagram-guide-shortcuts span {
      display: inline-flex;
      align-items: center;
      gap: 0.28rem;
      white-space: nowrap;
    }
    .diagram-guide-feedback {
      min-height: 1rem;
      margin: -0.18rem 0.9rem 0.62rem;
      color: var(--security-stroke);
      font-size: 0.5rem;
    }
    .diagram-guide-feedback:empty { display: none; }
    @keyframes mosofin-guide-in {
      from { opacity: 0; transform: translateY(-5px) scale(0.985); }
      to { opacity: 1; transform: translateY(0) scale(1); }
    }

    .node-finder {
      position: absolute;
      top: 1rem;
      left: 1rem;
      z-index: 24;
      display: flex;
      flex-direction: column;
      width: min(22rem, calc(100% - 2rem));
      max-height: calc(100% - 2rem);
      overflow: hidden;
      border: 1px solid var(--toolbar-border);
      border-radius: 0.85rem;
      background: var(--toolbar-menu-bg);
      color: var(--toolbar-text);
      box-shadow: 0 6px 8px rgba(0, 0, 0, 0.22);
      backdrop-filter: blur(16px);
    }
    .node-finder[hidden] { display: none; }
    .node-finder[data-context="route-source"] {
      border-color: color-mix(in srgb, var(--frontend-stroke) 68%, var(--toolbar-border));
    }
    .node-finder[data-context="route-target"] {
      border-color: color-mix(in srgb, var(--security-stroke) 68%, var(--toolbar-border));
    }
    html[data-preset="blueprint"] .node-finder { border-radius: 0.3rem; }
    .node-finder-head {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 1rem;
      padding: 0.7rem 0.8rem 0.45rem;
    }
    .node-finder-head strong {
      color: var(--text);
      font-size: 0.6875rem;
      letter-spacing: 0.08em;
      text-transform: uppercase;
    }
    .node-finder[data-context="route-source"] .node-finder-head strong { color: var(--frontend-stroke); }
    .node-finder[data-context="route-target"] .node-finder-head strong { color: var(--security-stroke); }
    .node-finder-close {
      width: 1.75rem;
      height: 1.75rem;
      border: 0;
      border-radius: 0.35rem;
      background: transparent;
      color: var(--toolbar-text);
      font: inherit;
      font-size: 1rem;
      cursor: pointer;
    }
    .node-finder-close:hover,
    .node-finder-close:focus-visible { background: var(--toolbar-hover); }
    .node-finder-close:focus-visible,
    .node-finder-result:focus-visible {
      outline: 2px solid var(--frontend-stroke);
      outline-offset: -2px;
    }
    .node-finder-search {
      display: grid;
      grid-template-columns: auto minmax(0, 1fr) auto;
      align-items: center;
      gap: 0.55rem;
      margin: 0 0.55rem 0.55rem;
      padding: 0 0.7rem;
      min-height: 2.6rem;
      border: 1px solid var(--toolbar-border);
      border-radius: 0.55rem;
      background: color-mix(in srgb, var(--panel) 78%, transparent);
      transition: border-color 160ms ease-out, box-shadow 160ms ease-out;
    }
    html[data-preset="blueprint"] .node-finder-search { border-radius: 0.18rem; }
    .node-finder-search > span { color: var(--frontend-stroke); font-size: 0.875rem; }
    .node-finder-search:focus-within {
      border-color: var(--frontend-stroke);
      box-shadow: 0 0 0 2px color-mix(in srgb, var(--frontend-stroke) 20%, transparent);
    }
    .node-finder-input {
      appearance: none;
      width: 100%;
      min-width: 0;
      border: 0;
      outline: 0;
      background: transparent;
      color: var(--text);
      font: inherit;
      font-size: 0.75rem;
    }
    .node-finder-input:focus-visible { outline: none; }
    .node-finder-input::placeholder { color: var(--text-faint); }
    .node-finder kbd {
      padding: 0.1rem 0.32rem;
      border: 1px solid var(--toolbar-border);
      border-radius: 0.25rem;
      color: var(--text-faint);
      font: inherit;
      font-size: 0.5625rem;
    }
    .node-finder-results {
      display: grid;
      flex: 1 1 auto;
      gap: 0;
      min-height: 0;
      max-height: min(18rem, 48vh);
      margin: 0 0.55rem 0.45rem;
      padding: 0;
      overflow-y: auto;
      overscroll-behavior: contain;
      border: 1px solid color-mix(in srgb, var(--toolbar-border) 78%, transparent);
      border-radius: 0.55rem;
      background: color-mix(in srgb, var(--panel) 54%, transparent);
      scroll-padding-block: 0.25rem;
      scrollbar-width: thin;
    }
    html[data-preset="blueprint"] .node-finder-results { border-radius: 0.18rem; }
    .node-finder-result {
      display: grid;
      grid-template-columns: minmax(0, 1fr) auto;
      gap: 0.2rem 0.75rem;
      width: 100%;
      min-height: 3.25rem;
      padding: 0.58rem 0.65rem;
      border: 0;
      border-radius: 0;
      background: transparent;
      color: var(--text);
      font: inherit;
      text-align: left;
      cursor: pointer;
      transition: background-color 160ms ease-out;
    }
    .node-finder-result:not(:last-child) {
      border-bottom: 1px solid color-mix(in srgb, var(--toolbar-border) 62%, transparent);
    }
    .node-finder-result:hover,
    .node-finder-result:focus-visible {
      position: relative;
      z-index: 1;
      background: color-mix(in srgb, var(--frontend-fill) 48%, var(--toolbar-hover));
    }
    .node-finder-result strong {
      overflow: hidden;
      font-size: 0.75rem;
      line-height: 1.25;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .node-finder-result small {
      grid-column: 1 / -1;
      overflow: hidden;
      color: var(--text-faint);
      font-size: 0.5625rem;
      letter-spacing: 0.015em;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .node-finder-result em {
      align-self: center;
      color: var(--frontend-stroke);
      font-size: 0.625rem;
      font-style: normal;
    }
    .node-finder[data-context^="route-"] .node-finder-result em {
      padding: 0.1rem 0.3rem;
      border: 1px solid color-mix(in srgb, currentColor 52%, transparent);
      border-radius: 999px;
      font-size: 0.5rem;
      font-weight: 800;
      letter-spacing: 0.05em;
      text-transform: uppercase;
    }
    .node-finder[data-context="route-target"] .node-finder-result em { color: var(--security-stroke); }
    .node-finder-empty {
      padding: 1.1rem 0.75rem 1.25rem;
      color: var(--text-faint);
      font-size: 0.6875rem;
      text-align: center;
    }
    .node-finder-status {
      margin: 0;
      padding: 0.1rem 0.8rem 0.65rem;
      color: var(--text-faint);
      font-size: 0.5625rem;
    }

    .guided-views {
      display: grid;
      grid-template-columns: 2.75rem minmax(0, 1fr) 2.75rem auto;
      align-items: stretch;
      gap: 0.35rem;
      min-width: 0;
      margin: 0 0 0.75rem;
      padding: 0.35rem;
      border: 1px solid var(--toolbar-border);
      border-radius: 0.85rem;
      background: var(--toolbar-bg);
      background: linear-gradient(120deg, var(--toolbar-bg), color-mix(in srgb, var(--toolbar-bg) 82%, var(--frontend-fill)));
      box-shadow: 0 12px 36px rgba(0, 0, 0, 0.16);
      backdrop-filter: blur(12px);
    }
    .guided-views[hidden] { display: none; }

    html[data-preset="blueprint"] .guided-views {
      border-color: var(--panel-border);
      border-radius: 0.35rem;
      background:
        linear-gradient(90deg, color-mix(in srgb, var(--frontend-fill) 36%, transparent), transparent 24%),
        var(--toolbar-bg);
      box-shadow: 0 12px 28px rgba(0, 0, 0, 0.12);
    }
    html[data-preset="blueprint"] .guided-views button { border-radius: 0.2rem; }
    html[data-preset="blueprint"] .guided-view-progress { border-radius: 0; }
    html[data-preset="blueprint"] .guided-view-progress span { border-radius: 0; }
    .guided-views button {
      min-width: 0;
      padding: 0.45rem 0.6rem;
      border-radius: 0.55rem;
      font-size: 0.75rem;
    }
    .guided-views > #guided-view-prev,
    .guided-views > #guided-view-next { min-width: 2.75rem; min-height: 2.75rem; }
    .guided-views button:disabled { opacity: 0.3; cursor: default; }
    .guided-view-copy {
      min-width: 0;
      padding: 0.2rem 0.5rem;
      border-left: 1px solid var(--toolbar-border);
      overflow: hidden;
    }
    .guided-view-meta {
      display: flex;
      align-items: center;
      gap: 0.5rem;
      min-height: 1rem;
      margin-bottom: 0.12rem;
      color: var(--frontend-stroke);
      font-size: 0.5625rem;
      font-weight: 700;
      letter-spacing: 0.12em;
      text-transform: uppercase;
    }
    .guided-view-handoff {
      box-sizing: border-box;
      height: 1rem;
      min-width: 0;
      max-width: 15rem;
      overflow: hidden;
      padding: 0 0.38rem;
      border: 1px solid color-mix(in srgb, var(--frontend-stroke) 46%, transparent);
      border-radius: 999px;
      color: var(--toolbar-text);
      background: color-mix(in srgb, var(--frontend-fill) 44%, transparent);
      font-size: 0.5rem;
      line-height: calc(1rem - 2px);
      letter-spacing: 0.06em;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .guided-view-handoff[hidden] { display: none; }
    html[data-preset="blueprint"] .guided-view-handoff { border-radius: 0.15rem; }
    .chapter-handoff-overlay { pointer-events: none; }
    .chapter-handoff-anchor {
      fill: color-mix(in srgb, var(--frontend-fill) 10%, transparent);
      stroke: var(--frontend-stroke);
      stroke-width: 2;
      stroke-dasharray: 4 4;
      vector-effect: non-scaling-stroke;
      opacity: 0.96;
      animation: mosofin-chapter-anchor 0.42s cubic-bezier(0.22, 1, 0.36, 1) both;
    }
    svg[data-chapter-handoff] [data-node-id][data-chapter-role="stay"] { opacity: 1; }
    svg[data-chapter-handoff] [data-node-id][data-chapter-role="enter"] { opacity: 0.78; }
    svg[data-chapter-handoff] [data-node-id][data-chapter-role="leave"] { opacity: 0.26; }
    @keyframes mosofin-chapter-anchor {
      0% { opacity: 0.35; stroke-dashoffset: 8; }
      45% { opacity: 1; }
      100% { opacity: 0.72; stroke-dashoffset: 0; }
    }
    .guided-view-progress {
      width: 3.5rem;
      height: 2px;
      overflow: hidden;
      border-radius: 999px;
      background: var(--toolbar-border);
    }
    .guided-view-progress span {
      display: block;
      width: 100%;
      height: 100%;
      border-radius: inherit;
      background: var(--frontend-stroke);
      transform: scaleX(0);
      transform-origin: left center;
    }
    .guided-view-copy strong,
    .guided-view-copy small { display: block; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }
    .guided-view-copy strong { color: var(--toolbar-text); font-size: 0.75rem; line-height: 1.35; }
    .guided-view-copy small { margin-top: 0; color: var(--text-muted); font-size: 0.625rem; line-height: 1.35; }
    .guided-story-caption {
      display: grid;
      grid-template-columns: auto minmax(0, 1fr) auto;
      gap: 0.12rem 0.55rem;
      align-items: center;
      min-width: 0;
      margin-top: 0.18rem;
      padding: 0.3rem 0.5rem;
      overflow: hidden;
      border: 1px solid color-mix(in srgb, var(--frontend-stroke) 32%, var(--toolbar-border));
      border-radius: 0.58rem;
      background: color-mix(in srgb, var(--frontend-fill) 26%, transparent);
      animation: mosofin-story-caption-in 140ms cubic-bezier(0.22, 1, 0.36, 1) both;
    }
    .guided-story-caption[hidden] { display: none; }
    .guided-story-caption-index {
      grid-row: 1 / span 2;
      align-self: stretch;
      display: inline-grid;
      min-width: 3.3rem;
      place-items: center;
      padding-right: 0.52rem;
      border-right: 1px solid color-mix(in srgb, var(--frontend-stroke) 28%, var(--toolbar-border));
      color: var(--frontend-stroke);
      font-size: 0.5625rem;
      font-weight: 800;
      letter-spacing: 0.08em;
      text-transform: uppercase;
    }
    .guided-story-caption strong,
    .guided-story-caption small {
      display: block;
      min-width: 0;
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .guided-story-caption strong { color: var(--toolbar-text); font-size: 0.6875rem; line-height: 1.3; }
    .guided-story-caption small { color: var(--text-muted); font-size: 0.5625rem; line-height: 1.35; }
    .guided-story-caption-next {
      grid-column: 3;
      grid-row: 1 / span 2;
      display: grid;
      min-width: 6.4rem;
      max-width: 9.5rem;
      align-self: stretch;
      align-content: center;
      padding-left: 0.58rem;
      border-left: 1px solid color-mix(in srgb, var(--frontend-stroke) 22%, var(--toolbar-border));
    }
    .guided-story-caption-next[hidden] { display: none; }
    .guided-story-caption-next small {
      color: color-mix(in srgb, var(--frontend-stroke) 72%, var(--text-muted));
      font-weight: 800;
      letter-spacing: 0.09em;
      text-transform: uppercase;
    }
    .guided-story-caption-next strong { color: var(--text-muted); font-weight: 700; }
    html[data-preset="blueprint"] .guided-story-caption { border-radius: 0.2rem; }
    .guided-views[data-story-beat] .guided-view-copy > #guided-view-label,
    .guided-views[data-story-beat] .guided-view-copy > #guided-view-note { display: none; }
    @keyframes mosofin-story-caption-in {
      from { opacity: 0; transform: translateY(3px); }
      to { opacity: 1; transform: translateY(0); }
    }
    .guided-view-trail {
      display: flex;
      align-items: center;
      gap: 0.28rem;
      min-width: 0;
      min-height: 1.7rem;
      margin-top: 0;
      padding: 0;
      overflow-x: auto;
      color: var(--text-faint);
      overscroll-behavior-x: contain;
      touch-action: pan-x;
      scrollbar-width: none;
      white-space: nowrap;
    }
    .guided-view-trail[hidden] { display: none; }
    .guided-view-trail::-webkit-scrollbar { display: none; }
    .guided-view-trail .guided-view-stop {
      appearance: none;
      box-sizing: border-box;
      display: inline-flex;
      flex: 0 0 auto;
      align-items: center;
      gap: 0.28rem;
      min-height: 1.5rem;
      padding: 0.1rem 0.3rem 0.1rem 0.18rem;
      border: 1px solid transparent;
      border-radius: 999px;
      background: transparent;
      color: var(--text-muted);
      cursor: pointer;
      font-family: inherit;
      font-size: 0.5625rem;
      font-weight: 650;
      letter-spacing: 0.02em;
      line-height: 1;
      transition: background 140ms ease, border-color 140ms ease, color 140ms ease, opacity 140ms ease;
    }
    .guided-view-stop:hover {
      border-color: color-mix(in srgb, var(--frontend-stroke) 36%, transparent);
      background: color-mix(in srgb, var(--frontend-fill) 34%, transparent);
    }
    .guided-view-stop:focus-visible {
      outline: 2px solid var(--frontend-stroke);
      outline-offset: 1px;
    }
    .guided-view-stop:disabled { cursor: wait; }
    .guided-view-stop::before {
      display: inline-grid;
      width: 1rem;
      height: 1rem;
      place-items: center;
      border: 1px solid color-mix(in srgb, var(--frontend-stroke) 54%, var(--toolbar-border));
      border-radius: 50%;
      background: color-mix(in srgb, var(--frontend-fill) 58%, var(--toolbar-bg));
      color: var(--frontend-stroke);
      content: attr(data-story-number);
      font-size: 0.5rem;
      line-height: 1;
    }
    .guided-view-stop + .guided-view-stop::after {
      order: -1;
      color: color-mix(in srgb, var(--frontend-stroke) 58%, var(--text-faint));
      content: '\00b7';
      font-size: 0.65rem;
    }
    .guided-view-stop[data-story-link="forward"]::after { content: '\2192'; }
    .guided-view-stop[data-story-link="reverse"]::after { content: '\2190'; }
    .guided-view-stop[data-story-link="multiple"]::after { content: '\21c4'; }
    .guided-view-stop[data-story-beat-state="pending"] { opacity: 0.38; }
    .guided-view-stop[data-story-beat-state="next"] {
      border-style: dashed;
      border-color: color-mix(in srgb, var(--frontend-stroke) 42%, var(--toolbar-border));
      color: color-mix(in srgb, var(--toolbar-text) 68%, var(--text-muted));
      opacity: 0.58;
    }
    .guided-view-stop[data-story-beat-state="past"] { color: var(--text-muted); opacity: 0.76; }
    .guided-view-stop[data-story-beat-state="active"] {
      border-color: color-mix(in srgb, var(--frontend-stroke) 72%, var(--toolbar-border));
      background: color-mix(in srgb, var(--frontend-fill) 42%, transparent);
      color: var(--toolbar-text);
      font-weight: 800;
      opacity: 1;
    }
    .guided-view-stop[data-story-beat-state="active"]::before {
      border-color: var(--frontend-stroke);
      box-shadow: 0 0 10px color-mix(in srgb, var(--frontend-stroke) 54%, transparent);
    }
    html[data-preset="blueprint"] .guided-view-stop,
    html[data-preset="blueprint"] .guided-view-stop::before { border-radius: 0.1rem; }
    html[data-preset="blueprint"] .guided-view-stop[data-story-beat-state="active"]::before { box-shadow: none; }
    .guided-view-actions { display: flex; align-items: stretch; gap: 0.2rem; }
    .guided-view-actions button { white-space: nowrap; }
    .guided-view-play {
      display: inline-flex;
      align-items: center;
      justify-content: center;
      gap: 0.38rem;
      min-width: 6.25rem !important;
      color: var(--frontend-stroke) !important;
      font-size: 0.625rem !important;
    }
    .guided-view-play[aria-pressed="true"] { background: var(--toolbar-hover); }
    .guided-view-all { min-width: 3.25rem !important; color: var(--text-muted) !important; font-size: 0.625rem !important; }
    .guided-view-beat-link {
      display: inline-flex;
      align-items: center;
      justify-content: center;
      gap: 0.3rem;
      min-width: 5.4rem !important;
      min-height: 1.5rem;
      color: var(--text-muted) !important;
      font-size: 0.5625rem !important;
      letter-spacing: 0.025em;
      transition: background 140ms ease, border-color 140ms ease, color 140ms ease;
    }
    .guided-view-beat-link:not(:disabled):hover {
      border-color: color-mix(in srgb, var(--frontend-stroke) 52%, var(--toolbar-border));
      color: var(--frontend-stroke) !important;
    }
    .guided-view-beat-link[data-copy-state="copied"] {
      border-color: color-mix(in srgb, var(--backend-stroke) 58%, var(--toolbar-border));
      background: color-mix(in srgb, var(--backend-fill) 46%, transparent);
      color: var(--backend-stroke) !important;
    }
    .guided-view-index {
      grid-column: 1 / -1;
      min-width: 0;
      padding: 0.28rem 0.12rem 0.08rem;
      overflow: hidden;
      border-top: 1px solid color-mix(in srgb, var(--toolbar-border) 72%, transparent);
    }
    @media (min-width: 721px) {
      html[data-present="true"]:not([data-embed="true"]) .guided-views[data-playing="true"] .guided-view-index,
      html[data-present="true"]:not([data-embed="true"]) .guided-views[data-playing="true"] .guided-view-beat-link,
      html[data-present="true"]:not([data-embed="true"]) .guided-views[data-playing="true"] .guided-view-all,
      html[data-present="true"]:not([data-embed="true"]) .guided-views[data-playing="true"] .guided-view-trail,
      html[data-present="true"]:not([data-embed="true"]) .guided-views[data-playing="true"] .guided-view-copy > #guided-view-label,
      html[data-present="true"]:not([data-embed="true"]) .guided-views[data-playing="true"] .guided-view-copy > #guided-view-note { display: none; }
      html[data-present="true"]:not([data-embed="true"]) .guided-views[data-playing="true"] .guided-story-caption { margin-top: 0.08rem; }
    }
    .guided-view-chapters {
      display: flex;
      gap: 0.28rem;
      min-width: 0;
      margin: 0;
      padding: 0.08rem 0.05rem 0.2rem;
      overflow-x: auto;
      list-style: none;
      scroll-padding-inline: 0.05rem;
      scroll-snap-type: x proximity;
      scrollbar-width: none;
    }
    .guided-view-chapters::-webkit-scrollbar { display: none; }
    .guided-view-chapters li {
      display: flex;
      flex: 1 1 0;
      min-width: 9.5rem;
      scroll-snap-align: center;
    }
    .guided-view-chapter {
      position: relative;
      display: grid !important;
      grid-template-columns: 1.55rem minmax(0, 1fr) auto;
      align-items: center;
      width: 100%;
      min-height: 2.75rem;
      padding: 0.36rem 0.48rem !important;
      overflow: hidden;
      border: 1px solid color-mix(in srgb, var(--toolbar-border) 86%, transparent) !important;
      border-radius: 0.48rem !important;
      background: color-mix(in srgb, var(--toolbar-bg) 88%, transparent) !important;
      color: var(--text-muted) !important;
      text-align: left;
      transition: background 0.16s ease, border-color 0.16s ease, opacity 0.16s ease;
    }
    .guided-view-chapter:hover {
      border-color: color-mix(in srgb, var(--frontend-stroke) 58%, var(--toolbar-border)) !important;
      background: color-mix(in srgb, var(--frontend-fill) 42%, var(--toolbar-bg)) !important;
    }
    .guided-view-chapter:focus-visible {
      outline: 2px solid var(--frontend-stroke);
      outline-offset: 1px;
    }
    .guided-view-chapter-index {
      display: inline-grid;
      width: 1.32rem;
      height: 1.32rem;
      place-items: center;
      border: 1px solid color-mix(in srgb, var(--frontend-stroke) 48%, var(--toolbar-border));
      border-radius: 50%;
      color: var(--frontend-stroke);
      font-size: 0.5rem;
      font-weight: 800;
      letter-spacing: 0.03em;
    }
    .guided-view-chapter-title {
      min-width: 0;
      overflow: hidden;
      color: var(--toolbar-text);
      font-size: 0.625rem;
      font-weight: 700;
      line-height: 1.3;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .guided-view-chapter-count {
      grid-column: 3;
      grid-row: 1;
      margin-left: 0.38rem;
      color: var(--text-faint);
      font-size: 0.5rem;
      font-style: normal;
      font-weight: 650;
      white-space: nowrap;
    }
    .guided-view-chapter-delta {
      display: inline-flex;
      grid-column: 3;
      grid-row: 1;
      align-items: center;
      gap: 0.22rem;
      margin-left: 0.38rem;
      color: var(--text-faint);
      font-size: 0.5rem;
      font-weight: 750;
      letter-spacing: 0.01em;
      white-space: nowrap;
    }
    .guided-view-chapter-delta[hidden] { display: none; }
    .guided-view-chapter-delta [data-delta-kind="stay"] { color: var(--text-muted); }
    .guided-view-chapter-delta [data-delta-kind="enter"] { color: var(--frontend-stroke); }
    .guided-view-chapter-delta [data-delta-kind="leave"] { color: var(--security-stroke); }
    .guided-view-chapter[data-preview-active="true"] {
      border-color: color-mix(in srgb, var(--frontend-stroke) 72%, var(--toolbar-border)) !important;
      background: color-mix(in srgb, var(--frontend-fill) 54%, var(--toolbar-bg)) !important;
    }
    .guided-view-chapter[data-chapter-position="current"] {
      border: 2px solid var(--frontend-stroke) !important;
      padding: calc(0.36rem - 1px) calc(0.48rem - 1px) !important;
      box-shadow: inset 3px 0 0 var(--frontend-stroke);
      color: var(--toolbar-text) !important;
    }
    .guided-view-chapter[data-chapter-position="current"] .guided-view-chapter-index {
      background: var(--frontend-stroke);
      color: var(--bg);
    }
    .guided-view-chapter[data-chapter-position="before"] {
      border-style: solid !important;
      opacity: 0.72;
    }
    .guided-view-chapter[data-chapter-position="after"] { border-style: dashed !important; }
    html[data-preset="signal-flow"] .guided-view-chapter[data-chapter-position="current"] {
      box-shadow: inset 3px 0 0 var(--frontend-stroke), 0 0 18px color-mix(in srgb, var(--frontend-stroke) 18%, transparent);
    }
    html[data-preset="blueprint"] .guided-view-chapter,
    html[data-preset="blueprint"] .guided-view-chapter-index { border-radius: 0.12rem !important; }

    /* Story Shelf — keep every authored chapter and the Play action visible,
       but do not spend a second row on unavailable director controls before
       the reader enters a chapter. Existing data-active-view state expands
       the full guided surface without a new toggle or stored preference. */
    .guided-views[data-active-view="all"] {
      grid-template-columns: minmax(11rem, .72fr) auto minmax(0, 2fr);
      align-items: center;
      padding: 0.28rem 0.35rem;
    }
    .guided-views[data-active-view="all"] > #guided-view-prev,
    .guided-views[data-active-view="all"] > #guided-view-next,
    .guided-views[data-active-view="all"] .guided-view-beat-link,
    .guided-views[data-active-view="all"] .guided-view-all { display: none; }
    .guided-views[data-active-view="all"] .guided-view-copy {
      grid-column: 1;
      grid-row: 1;
      padding: 0.1rem 0.55rem;
    }
    .guided-views[data-active-view="all"] .guided-view-copy > #guided-view-note { display: none; }
    .guided-views[data-active-view="all"] .guided-view-actions {
      display: block;
      grid-column: 2;
      grid-row: 1;
    }
    .guided-views[data-active-view="all"] .guided-view-play { min-height: 2.75rem; }
    .guided-views[data-active-view="all"] .guided-view-index {
      grid-column: 3;
      grid-row: 1;
      padding: 0 0.08rem 0 0.42rem;
      border-top: 0;
      border-left: 1px solid color-mix(in srgb, var(--toolbar-border) 72%, transparent);
    }
    .guided-views[data-active-view="all"] .guided-view-chapters {
      gap: 0.3rem;
      padding: 0;
    }

    .cards {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
      gap: 1rem;
      margin-top: 2rem;
    }

    .card {
      background: var(--panel);
      border-radius: 0.75rem;
      border: 1px solid var(--panel-border);
      padding: 1.25rem;
    }

    html[data-preset="signal-flow"] .card {
      background: linear-gradient(145deg, rgba(10, 24, 43, 0.86), rgba(5, 12, 24, 0.92));
      box-shadow: inset 0 1px 0 rgba(255, 255, 255, 0.035);
    }
    html[data-preset="signal-flow"][data-theme="light"] .card {
      background: linear-gradient(145deg, rgba(255, 255, 255, 0.92), rgba(237, 246, 250, 0.9));
      box-shadow: inset 0 1px 0 rgba(255, 255, 255, 0.82), 0 12px 32px rgba(45, 78, 99, 0.08);
    }

    /* Preserve the one-screen guarantee on ordinary laptop displays. The
       high-screen expansion above must not trade a fuller canvas for page
       scrolling at 900–1080px heights. */
    @media (min-width: 768px) and (max-height: 1100px) {
      .header { margin-bottom: 1rem; }
      .diagram-container {
        padding: 0.875rem;
        padding-bottom: calc(0.875rem + var(--mosofin-nav-reserve));
      }
      .cards { margin-top: 1rem; }
      .card { padding: 1rem; }
    }
    @media (min-width: 768px) and (max-height: 920px) {
      body { padding-block: 1.25rem; }
      .header { margin-bottom: 0.875rem; }
      .cards { margin-top: 0.75rem; }
    }

    html[data-preset="blueprint"] .card {
      position: relative;
      border-color: var(--panel-border);
      border-radius: 0.3rem;
      background: var(--panel);
      box-shadow: none;
    }
    html[data-preset="blueprint"] .card::before {
      content: "";
      position: absolute;
      top: -1px;
      left: 1rem;
      width: 2.5rem;
      border-top: 2px solid var(--frontend-stroke);
    }

    html[data-preset="editorial"] .card {
      position: relative;
      border-radius: 0.28rem;
      background: var(--panel);
      box-shadow: 0 10px 28px color-mix(in srgb, #000 10%, transparent);
    }
    html[data-preset="editorial"] .card::before {
      position: absolute;
      top: -1px;
      left: 1.25rem;
      width: 3.25rem;
      border-top: 3px solid var(--arrow-emphasis);
      content: "";
    }
    html[data-preset="editorial"] .card-dot { border-radius: 0; transform: rotate(45deg) scale(0.78); }

    .card-header {
      display: flex;
      align-items: center;
      gap: 0.5rem;
      margin-bottom: 0.75rem;
    }

    .card-dot {
      width: 8px;
      height: 8px;
      border-radius: 50%;
    }

    .card-dot.cyan     { background: var(--frontend-stroke); }
    .card-dot.emerald  { background: var(--backend-stroke); }
    .card-dot.violet   { background: var(--database-stroke); }
    .card-dot.amber    { background: var(--cloud-stroke); }
    .card-dot.rose     { background: var(--security-stroke); }
    .card-dot.orange   { background: var(--messagebus-stroke); }
    .card-dot.slate    { background: var(--external-stroke); }

    .card h3 { font-size: 0.875rem; font-weight: 600; color: var(--text); }

    .card ul {
      list-style: none;
      color: var(--text-muted);
      font-size: 0.75rem;
    }

    .card li { margin-bottom: 0.375rem; }

    /* ==========================================================
       Print stylesheet — hide interactive controls, force light,
       drop the grid so the diagram doesn't waste ink, use landscape
       so wide diagrams fit, pack summary cards into 2 columns to
       avoid an orphan card on a trailing page.
       ========================================================== */
    @page { size: landscape; margin: 1.5cm; }

    @media print {
      /* Force the FULL light palette — including component fills/strokes and
         lane/arrow colors — so printing from dark theme doesn't put neon
         strokes and translucent dark fills on white paper. */
      :root, [data-theme="dark"], [data-theme="light"] {
        --bg: #ffffff;
        --grid: transparent;
        --panel: #ffffff;
        --panel-border: #e2e8f0;
        --text: #0f172a;
        --text-muted: #475569;
        --text-dim: #94a3b8;
        --text-faint: #64748b;
        --mask: #ffffff;
        --lane-fill: rgba(248, 250, 252, 0.65);
        --lane-stroke: #cbd5e1;
        --arrow: #94a3b8;
        --arrow-emphasis: #059669;
        --frontend-fill:    rgba(34, 211, 238, 0.15);
        --frontend-stroke:  #0891b2;
        --backend-fill:     rgba(52, 211, 153, 0.18);
        --backend-stroke:   #059669;
        --database-fill:    rgba(167, 139, 250, 0.2);
        --database-stroke:  #7c3aed;
        --cloud-fill:       rgba(251, 191, 36, 0.18);
        --cloud-stroke:     #d97706;
        --security-fill:    rgba(251, 113, 133, 0.15);
        --security-stroke:  #e11d48;
        --messagebus-fill:  rgba(251, 146, 60, 0.15);
        --messagebus-stroke:#ea580c;
        --external-fill:    rgba(148, 163, 184, 0.18);
        --external-stroke:  #64748b;
      }
      body { background: #ffffff !important; padding: 0; }
      .container { max-width: none; }
      .toolbar, .diagram-nav, .focus-chip, .guided-views, .mosofin-toast, .no-print { display: none !important; }
      .chapter-handoff-overlay { display: none !important; }
      svg [data-node-id][data-chapter-role] { opacity: 1 !important; }
      svg[data-chapter-preview] [data-node-id],
      svg[data-chapter-preview] [data-edge-from] { opacity: 1 !important; filter: none !important; }
      .diagram-container, .card { box-shadow: none; border-color: #e2e8f0; }
      .diagram-container { --mosofin-nav-reserve: 0px !important; }
      html[data-preset="signal-flow"] .diagram-container,
      html[data-preset="signal-flow"] .card { background: #ffffff; }
      .cards { grid-template-columns: 1fr 1fr; }
      /* Ensure elements break sensibly across pages */
      .card { break-inside: avoid; page-break-inside: avoid; }
      .diagram-container { break-inside: avoid; page-break-inside: avoid; }
      .diagram-container svg [data-detail] {
        opacity: 1 !important;
        pointer-events: auto !important;
      }
      .diagram-container svg [data-detail-anchor] { transform: none !important; }
      .diagram-container svg[data-lens-active] [data-node-id],
      .diagram-container svg[data-lens-active] [data-edge-from],
      .diagram-container svg[data-legend-preview-active] [data-node-id],
      .diagram-container svg[data-legend-preview-active] [data-edge-from] {
        opacity: 1 !important;
        filter: none !important;
      }
      [data-legend-bridge-runtime] { display: none !important; }
      .semantic-lens-overlay { display: none !important; }
      .relationship-hit-overlay { display: none !important; }
      .relationship-pulse-overlay { display: none !important; }
      .route-probe-overlay, .route-journey-overlay { display: none !important; }
      .story-trail-overlay,
      .story-carrier-overlay { display: none !important; }
      .diagram-container svg[data-focus-active] [data-node-id],
      .diagram-container svg[data-focus-active] [data-edge-from],
      .diagram-container svg[data-reach-active] [data-node-id],
      .diagram-container svg[data-reach-active] [data-edge-from],
      .diagram-container svg[data-story-beat] [data-story-step],
      .diagram-container svg[data-story-beat] [data-edge-from][data-story-beat-step],
      .diagram-container svg[data-route-active] [data-node-id],
      .diagram-container svg[data-route-active] [data-edge-from] {
        opacity: 1 !important;
        filter: none !important;
        animation: none !important;
      }
      h1, .subtitle { color: #0f172a; }
    }

    /* ==========================================================
       TOOLBAR (theme toggle + export menu)
       ========================================================== */
    .toolbar {
      position: fixed;
      top: 1rem;
      right: 1rem;
      display: flex;
      align-items: center;
      gap: 0.5rem;
      padding: 0;
      border: 0;
      background: transparent;
      box-shadow: none;
      z-index: 100;
    }
    .toolbar button {
      background: var(--toolbar-bg);
      color: var(--toolbar-text);
      border: 1px solid var(--toolbar-border);
      padding: 0.5rem 0.875rem;
      border-radius: 0.625rem;
      font-family: inherit;
      font-size: 0.75rem;
      font-weight: 500;
      cursor: pointer;
      white-space: nowrap;
      backdrop-filter: blur(10px);
      box-shadow: 0 4px 14px rgba(0, 0, 0, 0.08);
      transition: background 0.15s, border-color 0.15s, color 0.15s;
      display: inline-flex;
      align-items: center;
      gap: 0.375rem;
      min-height: 2.75rem;
    }
    .toolbar button:hover {
      background: var(--toolbar-hover);
      border-color: color-mix(in srgb, var(--arrow) 62%, var(--toolbar-border));
    }
    .toolbar button[aria-expanded="true"],
    .toolbar button[aria-pressed="true"]:not(#btn-theme) {
      background: color-mix(in srgb, var(--frontend-stroke) 10%, var(--toolbar-hover));
      border-color: color-mix(in srgb, var(--frontend-stroke) 48%, var(--toolbar-border));
    }
    .toolbar #btn-export {
      padding-right: 0.82rem;
      padding-left: 0.82rem;
    }
    .toolbar #btn-export:hover,
    .toolbar #btn-export[aria-expanded="true"] {
      color: var(--toolbar-text);
      background: color-mix(in srgb, var(--frontend-stroke) 10%, var(--toolbar-hover));
      border-color: color-mix(in srgb, var(--frontend-stroke) 54%, var(--toolbar-border));
    }
    .toolbar button:focus-visible {
      outline: 2px solid var(--frontend-stroke);
      outline-offset: 2px;
    }
    .toolbar #btn-motion[hidden] { display: none !important; }
    .toolbar #btn-motion { min-width: 4.4rem; }
    .toolbar #btn-motion:disabled { cursor: default; opacity: 0.58; }
    .toolbar .preset-wrap,
    .toolbar .export-wrap {
      position: relative;
    }
    .toolbar-icon {
      display: inline-block;
      width: 0.95rem;
      height: 0.95rem;
      flex: 0 0 auto;
      background: currentColor;
      -webkit-mask-position: center;
      -webkit-mask-repeat: no-repeat;
      -webkit-mask-size: contain;
      mask-position: center;
      mask-repeat: no-repeat;
      mask-size: contain;
    }
    .toolbar-icon.nodestyle {
      -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Crect x='2.5' y='4' width='6.5' height='6.5' rx='1.4' fill='none' stroke='black' stroke-width='1.6'/%3E%3Crect x='11' y='9.5' width='6.5' height='6.5' rx='1.4' fill='none' stroke='black' stroke-width='1.6'/%3E%3C/svg%3E");
      mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Crect x='2.5' y='4' width='6.5' height='6.5' rx='1.4' fill='none' stroke='black' stroke-width='1.6'/%3E%3Crect x='11' y='9.5' width='6.5' height='6.5' rx='1.4' fill='none' stroke='black' stroke-width='1.6'/%3E%3C/svg%3E");
    }
    .toolbar-chevron {
      width: 0.65rem;
      height: 0.65rem;
      transition: transform 0.15s ease;
    }
    [aria-expanded="true"] > .toolbar-chevron { transform: rotate(180deg); }
    [data-theme="light"] #theme-icon {
      -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cg fill='none' stroke='black' stroke-width='1.7' stroke-linecap='round'%3E%3Ccircle cx='10' cy='10' r='3.1'/%3E%3Cpath d='M10 2.2v1.4M10 16.4v1.4M2.2 10h1.4M16.4 10h1.4M4.5 4.5l1 1M14.5 14.5l1 1M15.5 4.5l-1 1M5.5 14.5l-1 1'/%3E%3C/g%3E%3C/svg%3E");
      mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cg fill='none' stroke='black' stroke-width='1.7' stroke-linecap='round'%3E%3Ccircle cx='10' cy='10' r='3.1'/%3E%3Cpath d='M10 2.2v1.4M10 16.4v1.4M2.2 10h1.4M16.4 10h1.4M4.5 4.5l1 1M14.5 14.5l1 1M15.5 4.5l-1 1M5.5 14.5l-1 1'/%3E%3C/g%3E%3C/svg%3E");
    }
    [data-theme="dark"] #theme-icon {
      -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath d='M15.2 12.7A6.3 6.3 0 017.3 4.8a6.3 6.3 0 107.9 7.9Z' fill='none' stroke='black' stroke-width='1.7' stroke-linejoin='round'/%3E%3C/svg%3E");
      mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath d='M15.2 12.7A6.3 6.3 0 017.3 4.8a6.3 6.3 0 107.9 7.9Z' fill='none' stroke='black' stroke-width='1.7' stroke-linejoin='round'/%3E%3C/svg%3E");
    }
    #btn-present[aria-pressed="false"] #present-icon {
      -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath d='M7.2 3.2h-4v4M12.8 3.2h4v4M7.2 16.8h-4v-4M12.8 16.8h4v-4' fill='none' stroke='black' stroke-width='1.7' stroke-linecap='round'/%3E%3C/svg%3E");
      mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath d='M7.2 3.2h-4v4M12.8 3.2h4v4M7.2 16.8h-4v-4M12.8 16.8h4v-4' fill='none' stroke='black' stroke-width='1.7' stroke-linecap='round'/%3E%3C/svg%3E");
    }
    #btn-present[aria-pressed="true"] #present-icon {
      -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath d='M7.2 7.2h-4v-4M12.8 7.2h4v-4M7.2 12.8h-4v4M12.8 12.8h4v4' fill='none' stroke='black' stroke-width='1.7' stroke-linecap='round'/%3E%3C/svg%3E");
      mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath d='M7.2 7.2h-4v-4M12.8 7.2h4v-4M7.2 12.8h-4v4M12.8 12.8h4v4' fill='none' stroke='black' stroke-width='1.7' stroke-linecap='round'/%3E%3C/svg%3E");
    }
    .toolbar-chevron {
      -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 12 12'%3E%3Cpath d='m3 4.5 3 3 3-3' fill='none' stroke='black' stroke-width='1.7' stroke-linecap='round' stroke-linejoin='round'/%3E%3C/svg%3E");
      mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 12 12'%3E%3Cpath d='m3 4.5 3 3 3-3' fill='none' stroke='black' stroke-width='1.7' stroke-linecap='round' stroke-linejoin='round'/%3E%3C/svg%3E");
    }
    .toolbar #btn-preset { min-width: 5.4rem; }
    .preset-control-mark {
      width: 0.58rem;
      height: 0.58rem;
      flex: 0 0 auto;
      border: 1px solid var(--frontend-stroke);
      border-radius: 0.2rem;
      background: color-mix(in srgb, var(--frontend-stroke) 28%, transparent);
      transition: border-radius 0.15s, box-shadow 0.15s, transform 0.15s;
    }
    #btn-preset[data-preset-option="signal-flow"] .preset-control-mark {
      border-radius: 50%;
      box-shadow: 0 0 10px color-mix(in srgb, var(--frontend-stroke) 78%, transparent);
    }
    #btn-preset[data-preset-option="blueprint"] .preset-control-mark {
      border-radius: 0.06rem;
      background: transparent;
      box-shadow: inset 0 0 0 1px color-mix(in srgb, var(--frontend-stroke) 32%, transparent);
      transform: rotate(45deg) scale(0.84);
    }
    #btn-preset[data-preset-option="editorial"] .preset-control-mark {
      border-color: var(--arrow-emphasis);
      border-radius: 0;
      background: var(--arrow-emphasis);
      box-shadow: 0 0 0 2px color-mix(in srgb, var(--arrow-emphasis) 14%, transparent);
      transform: rotate(45deg) scale(0.72);
    }
    .toolbar .preset-menu {
      position: absolute;
      top: calc(100% + 0.375rem);
      right: 0;
      display: none;
      width: 18.5rem;
      max-width: calc(100vw - 2rem);
      padding: 0.45rem;
      border: 1px solid var(--toolbar-border);
      border-radius: 0.875rem;
      background: var(--toolbar-menu-bg);
      box-shadow: 0 18px 48px rgba(0, 0, 0, 0.24);
    }
    .toolbar .preset-menu.open { display: block; }
    .toolbar .preset-menu.open,
    .toolbar .export-menu.open {
      animation: toolbar-menu-in 0.16s cubic-bezier(0.2, 0.8, 0.2, 1);
      transform-origin: top right;
    }
    @keyframes toolbar-menu-in {
      from { opacity: 0; transform: translateY(-0.25rem) scale(0.985); }
      to { opacity: 1; transform: translateY(0) scale(1); }
    }
    .preset-menu-heading {
      display: flex;
      align-items: baseline;
      justify-content: space-between;
      gap: 1rem;
      padding: 0.42rem 0.6rem 0.55rem;
      color: var(--text-faint);
      font-size: 0.56rem;
      letter-spacing: 0.13em;
      text-transform: uppercase;
    }
    .preset-menu-heading span:last-child {
      font-size: 0.52rem;
      letter-spacing: 0.04em;
      text-transform: none;
    }
    .toolbar .preset-option {
      display: grid;
      grid-template-columns: 2rem minmax(0, 1fr) 1rem;
      align-items: center;
      gap: 0.68rem;
      width: 100%;
      min-height: 3.35rem;
      padding: 0.5rem 0.62rem;
      border: 0;
      border-radius: 0.5rem;
      background: transparent;
      box-shadow: none;
      backdrop-filter: none;
      text-align: left;
    }
    .toolbar .preset-option:hover,
    .toolbar .preset-option:focus-visible { background: var(--toolbar-hover); }
    .toolbar .preset-option[aria-checked="true"] {
      background: color-mix(in srgb, var(--frontend-stroke) 9%, transparent);
      border-color: color-mix(in srgb, var(--frontend-stroke) 36%, transparent);
    }
    .preset-option-swatch {
      position: relative;
      width: 2rem;
      height: 1.55rem;
      overflow: hidden;
      border: 1px solid #64748b;
      border-radius: 0.24rem;
      background: #0f172a;
      box-shadow: inset 0 0 0 1px rgba(255,255,255,0.04);
    }
    .preset-option-swatch::before,
    .preset-option-swatch::after { position: absolute; content: ""; }
    .preset-option-swatch.classic::before {
      top: 0.42rem;
      right: 0.35rem;
      bottom: 0.42rem;
      left: 0.35rem;
      border: 1px solid #22d3ee;
      border-radius: 0.16rem;
      background: rgba(34, 211, 238, 0.14);
    }
    .preset-option-swatch.signal-flow {
      border-color: #29415f;
      background: linear-gradient(135deg, #07101e 10%, #10243c 100%);
    }
    .preset-option-swatch.signal-flow::before {
      top: 50%;
      right: 0.2rem;
      left: 0.2rem;
      height: 1px;
      background: linear-gradient(90deg, #67e8f9, #c4b5fd);
      box-shadow: 0 0 8px #67e8f9;
    }
    .preset-option-swatch.signal-flow::after {
      top: calc(50% - 0.13rem);
      right: 0.28rem;
      width: 0.26rem;
      height: 0.26rem;
      border-radius: 50%;
      background: #67e8f9;
      box-shadow: 0 0 7px #67e8f9;
    }
    .preset-option-swatch.blueprint {
      border-color: #34799a;
      border-radius: 0.08rem;
      background-color: #0a2031;
      background-image:
        linear-gradient(rgba(102, 217, 239, 0.18) 1px, transparent 1px),
        linear-gradient(90deg, rgba(102, 217, 239, 0.18) 1px, transparent 1px);
      background-size: 0.42rem 0.42rem;
    }
    .preset-option-swatch.blueprint::before {
      top: 0.34rem;
      right: 0.38rem;
      bottom: 0.34rem;
      left: 0.38rem;
      border: 1px solid #66d9ef;
    }
    .preset-option-swatch.editorial {
      border-color: #b8aa94;
      border-radius: 0.08rem;
      background: repeating-linear-gradient(0deg, #f2eee5 0 0.34rem, #d8d0c2 0.36rem, #f2eee5 0.38rem);
    }
    .preset-option-swatch.editorial::before {
      top: 0;
      bottom: 0;
      left: 0.42rem;
      width: 1px;
      background: #c9502d;
      opacity: 0.72;
    }
    .preset-option-swatch.editorial::after {
      top: 0.52rem;
      right: 0.45rem;
      width: 0.38rem;
      height: 0.38rem;
      background: #c9502d;
      transform: rotate(45deg);
    }
    .preset-option-copy { min-width: 0; }
    .preset-option-copy strong,
    .preset-option-copy small { display: block; }
    .preset-option-copy strong {
      color: var(--toolbar-text);
      font-size: 0.72rem;
      font-weight: 600;
    }
    .preset-option-copy small {
      margin-top: 0.16rem;
      overflow: hidden;
      color: var(--text-faint);
      font-size: 0.57rem;
      font-weight: 400;
      white-space: nowrap;
      text-overflow: ellipsis;
    }
    .preset-option-check {
      display: inline-block;
      width: 0.9rem;
      height: 0.9rem;
      color: var(--frontend-stroke);
      background: currentColor;
      -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 16'%3E%3Cpath d='m3 8.5 3 3 7-7' fill='none' stroke='black' stroke-width='2' stroke-linecap='round' stroke-linejoin='round'/%3E%3C/svg%3E");
      -webkit-mask-position: center;
      -webkit-mask-repeat: no-repeat;
      -webkit-mask-size: contain;
      mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 16'%3E%3Cpath d='m3 8.5 3 3 7-7' fill='none' stroke='black' stroke-width='2' stroke-linecap='round' stroke-linejoin='round'/%3E%3C/svg%3E");
      mask-position: center;
      mask-repeat: no-repeat;
      mask-size: contain;
      opacity: 0;
    }
    .preset-option[aria-checked="true"] .preset-option-check { opacity: 1; }
    html[data-preset="editorial"] .toolbar button,
    html[data-preset="editorial"] .preset-menu,
    html[data-preset="editorial"] .export-menu,
    html[data-preset="editorial"] .overview-map,
    html[data-preset="editorial"] .semantic-lens,
    html[data-preset="editorial"] .route-probe,
    html[data-preset="editorial"] .focus-chip,
    html[data-preset="editorial"] .diagram-guide,
    html[data-preset="editorial"] .node-finder,
    html[data-preset="editorial"] .guided-views,
    html[data-preset="editorial"] .share-chapter-cue {
      border-radius: 0.3rem;
    }
    .motion-control-dot {
      width: 0.46rem;
      height: 0.46rem;
      flex: 0 0 auto;
      border: 1px solid var(--frontend-stroke);
      border-radius: 50%;
      background: var(--frontend-stroke);
      box-shadow: 0 0 9px color-mix(in srgb, var(--frontend-stroke) 72%, transparent);
    }
    #btn-motion[aria-pressed="false"] .motion-control-dot {
      background: transparent;
      box-shadow: none;
    }
    .toolbar .export-menu {
      position: absolute;
      top: calc(100% + 0.375rem);
      right: 0;
      background: var(--toolbar-menu-bg);
      border: 1px solid var(--toolbar-border);
      width: 19rem;
      max-width: calc(100vw - 2rem);
      max-height: calc(100vh - 5.5rem);
      overflow-y: auto;
      border-radius: 0.875rem;
      padding: 0.55rem;
      display: none;
      flex-direction: column;
      box-shadow: 0 18px 48px rgba(0,0,0,0.24);
    }
    .toolbar .export-menu.open { display: flex; }
    .export-menu-header {
      display: flex;
      align-items: flex-start;
      justify-content: space-between;
      gap: 1rem;
      padding: 0.5rem 0.55rem 0.72rem;
    }
    .export-menu-header strong,
    .export-menu-header span { display: block; }
    .export-menu-header strong {
      color: var(--toolbar-text);
      font-size: 0.78rem;
      font-weight: 650;
      letter-spacing: -0.015em;
    }
    .export-menu-header span {
      margin-top: 0.16rem;
      color: var(--text-faint);
      font-size: 0.56rem;
    }
    .export-menu-header kbd {
      padding: 0.16rem 0.36rem;
      border: 1px solid var(--toolbar-border);
      border-radius: 0.3rem;
      color: var(--text-faint);
      background: color-mix(in srgb, var(--toolbar-border) 16%, transparent);
      font-family: inherit;
      font-size: 0.54rem;
      font-weight: 600;
    }
    .export-menu-section {
      display: grid;
      grid-template-columns: minmax(0, 1fr);
      gap: 0.18rem;
    }
    .export-menu-section + .export-menu-section {
      margin-top: 0.42rem;
      padding-top: 0.42rem;
      border-top: 1px solid color-mix(in srgb, var(--toolbar-border) 68%, transparent);
    }
    .export-menu-heading {
      grid-column: 1 / -1;
      display: block;
      padding: 0.24rem 0.55rem 0.2rem;
      color: var(--text-faint);
      font-size: 0.54rem;
      font-weight: 600;
      letter-spacing: 0.12em;
      text-transform: uppercase;
    }
    .toolbar .export-menu button {
      display: grid;
      grid-template-columns: 1.15rem minmax(0, 1fr);
      align-items: center;
      column-gap: 0.52rem;
      background: transparent;
      border: 1px solid transparent;
      box-shadow: none;
      backdrop-filter: none;
      width: 100%;
      min-height: 3rem;
      text-align: left;
      padding: 0.48rem 0.55rem;
      border-radius: 0.5rem;
      white-space: nowrap;
    }
    .export-item-copy {
      display: block;
      min-width: 0;
    }
    .export-item-copy strong,
    .export-item-copy small { display: block; }
    .export-item-copy strong {
      overflow: hidden;
      color: inherit;
      font-size: 0.7rem;
      font-weight: 550;
      text-overflow: ellipsis;
    }
    .export-item-copy small {
      margin-top: 0.12rem;
      overflow: hidden;
      color: var(--text-faint);
      font-size: 0.55rem;
      font-weight: 450;
      text-overflow: ellipsis;
    }
    .toolbar .export-menu button::before {
      width: 1rem;
      height: 1rem;
      background: currentColor;
      content: "";
      opacity: 0.72;
      -webkit-mask-position: center;
      -webkit-mask-repeat: no-repeat;
      -webkit-mask-size: contain;
      mask-position: center;
      mask-repeat: no-repeat;
      mask-size: contain;
    }
    .toolbar .export-menu button[data-format="share-card"]::before,
    .toolbar .export-menu button[data-action="route-share-card"]::before,
    .toolbar .export-menu button[data-action="reach-share-card"]::before {
      -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath fill='none' stroke='black' stroke-width='1.7' d='M3 4.5h14v11H3zM6 12l2.5-2.5 2 2 1.5-1.5 2 2'/%3E%3Ccircle cx='13.8' cy='7.3' r='1.2' fill='black'/%3E%3C/svg%3E");
      mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath fill='none' stroke='black' stroke-width='1.7' d='M3 4.5h14v11H3zM6 12l2.5-2.5 2 2 1.5-1.5 2 2'/%3E%3Ccircle cx='13.8' cy='7.3' r='1.2' fill='black'/%3E%3C/svg%3E");
    }
    .toolbar .export-menu button[data-action^="copy"]::before {
      -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Crect x='6' y='6' width='10' height='10' rx='1.5' fill='none' stroke='black' stroke-width='1.7'/%3E%3Cpath d='M4 13H3.5A1.5 1.5 0 012 11.5v-8A1.5 1.5 0 013.5 2h8A1.5 1.5 0 0113 3.5V4' fill='none' stroke='black' stroke-width='1.7'/%3E%3C/svg%3E");
      mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Crect x='6' y='6' width='10' height='10' rx='1.5' fill='none' stroke='black' stroke-width='1.7'/%3E%3Cpath d='M4 13H3.5A1.5 1.5 0 012 11.5v-8A1.5 1.5 0 013.5 2h8A1.5 1.5 0 0113 3.5V4' fill='none' stroke='black' stroke-width='1.7'/%3E%3C/svg%3E");
    }
    .toolbar .export-menu button[data-format="png"]::before,
    .toolbar .export-menu button[data-format="jpeg"]::before,
    .toolbar .export-menu button[data-format="webp"]::before {
      -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath d='M5 2.5h7l3 3v12H5zM12 2.5v3h3' fill='none' stroke='black' stroke-width='1.7'/%3E%3Cpath d='M7.5 14l2-2 1.5 1.5 1.5-1.5' fill='none' stroke='black' stroke-width='1.7'/%3E%3C/svg%3E");
      mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath d='M5 2.5h7l3 3v12H5zM12 2.5v3h3' fill='none' stroke='black' stroke-width='1.7'/%3E%3Cpath d='M7.5 14l2-2 1.5 1.5 1.5-1.5' fill='none' stroke='black' stroke-width='1.7'/%3E%3C/svg%3E");
    }
    .toolbar .export-menu button[data-format="svg"]::before,
    .toolbar .export-menu button[data-format="webm"]::before {
      -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath d='M10 2.5l7 4v7l-7 4-7-4v-7zM3 6.5l7 4 7-4M10 10.5v7' fill='none' stroke='black' stroke-width='1.7'/%3E%3C/svg%3E");
      mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 20 20'%3E%3Cpath d='M10 2.5l7 4v7l-7 4-7-4v-7zM3 6.5l7 4 7-4M10 10.5v7' fill='none' stroke='black' stroke-width='1.7'/%3E%3C/svg%3E");
    }
    .toolbar .export-menu button[data-format="share-card"],
    .toolbar .export-menu button[data-action="route-share-card"],
    .toolbar .export-menu button[data-action="reach-share-card"] {
      min-height: 3.35rem;
      color: var(--toolbar-text);
      background: color-mix(in srgb, var(--frontend-stroke) 8%, transparent);
      border-color: color-mix(in srgb, var(--frontend-stroke) 24%, var(--toolbar-border));
    }
    .toolbar .export-menu button[hidden] { display: none; }
    .toolbar .export-menu button:hover {
      background: var(--toolbar-hover);
    }
    .toolbar .export-menu button:focus-visible {
      background: var(--toolbar-hover);
      outline-offset: -2px;
    }
    .toolbar .export-menu button:disabled {
      color: var(--text-faint);
      background: color-mix(in srgb, var(--toolbar-border) 18%, transparent);
      cursor: not-allowed;
    }
    .toolbar .export-menu button:disabled:hover {
      border-color: transparent;
    }
    .toolbar .export-menu .hint {
      color: var(--text-faint);
      font-size: 0.55rem;
    }

    /* Toast — brief feedback for actions like "copied" */
    .mosofin-toast {
      position: fixed;
      top: 1rem;
      left: 50%;
      transform: translateX(-50%) translateY(-8px);
      background: var(--toolbar-menu-bg);
      color: var(--text);
      border: 1px solid var(--toolbar-border);
      padding: 0.5rem 1rem;
      border-radius: 0.5rem;
      font-family: inherit;
      font-size: 0.75rem;
      opacity: 0;
      transition: opacity 0.2s, transform 0.2s;
      z-index: 200;
      pointer-events: none;
      box-shadow: 0 4px 20px rgba(0,0,0,0.25);
    }
    .mosofin-toast.show {
      opacity: 1;
      transform: translateX(-50%) translateY(0);
    }

    @media (max-width: 720px) {
      body { padding: 1rem; }
      .toolbar .preset-menu,
      .toolbar .export-menu {
        position: fixed;
        top: 4.5rem;
        right: 1rem;
        left: 1rem;
        width: auto;
        max-width: none;
      }
      .toolbar {
        position: relative;
        justify-content: flex-end;
        width: max-content;
        max-width: 100%;
        margin-left: auto;
        margin-bottom: 1rem;
      }
      .header { margin-bottom: 1.25rem; padding-right: 0; }
      html[data-preset="signal-flow"] .header,
      html[data-preset="blueprint"] .header,
      html[data-preset="editorial"] .header { padding-right: 0; }
      .header-row {
        gap: 0.75rem;
        align-items: flex-start;
      }
      html[data-preset="signal-flow"] .header-row,
      html[data-preset="blueprint"] .header-row,
      html[data-preset="editorial"] .header-row { flex-wrap: wrap; }
      html[data-preset="signal-flow"] .header-row::after {
        margin-left: 0;
        order: 3;
      }
      html[data-preset="blueprint"] .header-row::after {
        margin-left: 0;
        order: 3;
      }
      html[data-preset="editorial"] .header-row::after {
        margin-left: 0;
        order: 3;
      }
      h1 {
        font-size: 1.25rem;
        line-height: 1.25;
      }
      .subtitle {
        margin-left: 0;
        font-size: 0.8125rem;
        line-height: 1.55;
      }
      .diagram-container {
        padding: 0.75rem;
        border-radius: 0.75rem;
      }
      html:not([data-embed="true"]) .diagram-container {
        padding: 0.75rem 0.75rem 4.25rem;
      }
      html:not([data-embed="true"]) .diagram-container[data-wide-diagram="true"] {
        overflow-x: auto;
        overflow-y: hidden;
        overscroll-behavior-x: contain;
        scrollbar-width: thin;
        scroll-behavior: smooth;
      }
      html:not([data-embed="true"]) .diagram-container[data-wide-diagram="true"] > svg {
        min-width: 720px;
      }
      html:not([data-embed="true"]) .diagram-container[data-wide-diagram="true"] .diagram-nav,
      html:not([data-embed="true"]) .diagram-container[data-wide-diagram="true"] .overview-map,
      html:not([data-embed="true"]) .diagram-container[data-wide-diagram="true"] .focus-chip,
      html:not([data-embed="true"]) .diagram-container[data-wide-diagram="true"] .route-probe,
      html:not([data-embed="true"]) .diagram-container[data-wide-diagram="true"] .semantic-lens,
      html:not([data-embed="true"]) .diagram-container[data-wide-diagram="true"] .diagram-guide,
      html:not([data-embed="true"]) .diagram-container[data-wide-diagram="true"] .node-finder {
        transform: translateX(var(--mosofin-scroll-x, 0px));
      }
      .diagram-nav { right: 0.5rem; bottom: 0.5rem; }
      .diagram-nav button {
        min-width: 2.75rem;
        height: 2.75rem;
      }
      .diagram-nav [data-view="reset"] { min-width: 3.75rem; }
      .diagram-nav [data-view="reset"][data-detail-visible] { min-width: 4.35rem; }
      .overview-map {
        right: 0.5rem;
        bottom: 3.7rem;
        width: min(10.25rem, calc(100% - 1rem));
      }
      .overview-map[data-docked="true"] {
        right: var(--mosofin-radar-right, 0.5rem);
        top: var(--mosofin-radar-top, 0.5rem);
        bottom: auto;
        transform: none !important;
      }
      .overview-map[data-docked="true"][data-dock-side="left"] {
        right: auto;
        left: var(--mosofin-radar-left, 0.5rem);
      }
      .overview-map-surface { height: 4rem; }
      .overview-map-feedback { right: 0.5rem; bottom: 3.7rem; }
      .semantic-lens {
        right: 0.5rem;
        bottom: 3.7rem;
        width: calc(100% - 1rem);
        max-height: calc(100% - 4.2rem);
      }
      .semantic-lens[data-dock-side] { left: auto; right: 0.5rem; }
      .semantic-lens-kinds { max-height: min(12rem, 34vh); }
      .focus-chip {
        left: 0.5rem;
        top: 0.5rem;
        width: calc(100% - 1rem);
        max-width: none;
      }
      .relationship-lens-list { max-height: min(12rem, 38vh); }
      .relationship-lens-actions #btn-focus-relations { display: block; color: var(--database-stroke); }
      .focus-chip:not([data-relations-expanded="true"]) .relationship-lens-head { border-bottom-color: transparent; }
      .focus-chip:not([data-relations-expanded="true"]) .relationship-lens-list { display: none; }
      /* While a mobile reader previews one relationship, turn the full Lens
         into a compact peek card. This keeps the exact source and target on
         canvas instead of covering them with a long, scrollable list. */
      .focus-chip[data-relationship-previewing="true"] .relationship-lens-list {
        max-height: none;
        padding: 0.35rem;
        overflow: hidden;
      }
      .focus-chip[data-relationship-previewing="true"] .relationship-lens-group { margin: 0; }
      .focus-chip[data-relationship-previewing="true"] .relationship-lens-group-title,
      .focus-chip[data-relationship-previewing="true"] .relationship-lens-row:not([data-preview-active="true"]) {
        display: none;
      }
      .node-finder {
        top: 0.5rem;
        left: 0.5rem;
        width: min(22rem, calc(100% - 1rem));
        max-height: calc(100% - 1rem);
      }
      .diagram-guide {
        top: 0.5rem;
        right: 0.5rem;
        left: 0.5rem;
        width: auto;
        max-height: calc(100% - 1rem);
      }
      .diagram-guide-head { padding: 0.62rem 0.7rem 0.52rem; }
      .diagram-guide-stats { padding: 0.42rem 0.7rem; }
      .diagram-guide-actions {
        grid-template-columns: 1fr;
        gap: 0.28rem;
        padding: 0.42rem;
      }
      .diagram-guide-action {
        min-height: 3.1rem;
        padding: 0.42rem 0.48rem;
      }
      .diagram-guide-action small {
        display: block;
        overflow: hidden;
        text-overflow: ellipsis;
        white-space: nowrap;
      }
      .diagram-guide-index {
        width: 1.5rem;
        height: 1.5rem;
      }
      .diagram-guide-shortcuts { padding: 0.46rem 0.7rem 0.52rem; }
      .diagram-guide-action[data-guide-action="present"] { grid-column: auto; }
      .route-probe[data-guide-open="true"] {
        visibility: hidden;
        pointer-events: none;
      }
      .node-finder-results { max-height: min(16rem, 38vh); }
      .route-probe {
        left: 0.5rem;
        bottom: 3.7rem;
        width: calc(100% - 1rem);
        min-width: 0;
      }
      .route-probe[data-route-dock="top"] {
        top: 0.5rem;
        bottom: auto;
      }
      .route-probe[data-finder-open="true"] {
        visibility: hidden;
        pointer-events: none;
      }
      .route-probe-head { padding: 0.45rem 0.48rem 0.38rem; }
      .route-probe-path { padding: 0.35rem 0.48rem 0.2rem; }
      .route-probe-node { min-height: 2.75rem !important; }
      .route-journey-controls {
        grid-template-columns: 2.75rem minmax(0, 1fr) 2.75rem minmax(0, 1fr);
        padding: 0.08rem 0.48rem 0.32rem;
      }
      .route-journey-controls button { min-height: 2.75rem; }
      .route-probe-status { padding: 0.05rem 0.48rem 0.38rem; }
      .route-probe-node { max-width: 7.5rem; }
      .guided-views {
        grid-template-columns: 2.75rem minmax(0, 1fr) 2.75rem;
        gap: 0.2rem;
        padding: 0.25rem;
      }
      .guided-view-actions {
        grid-column: 1 / -1;
        display: grid;
        grid-template-columns: repeat(3, minmax(0, 1fr));
      }
      .guided-views > #guided-view-prev,
      .guided-views > #guided-view-next {
        align-self: center;
        height: 2.75rem;
      }
      .guided-view-play,
      .guided-view-all,
      .guided-view-beat-link { min-height: 2.75rem; }
      .guided-view-copy small { white-space: normal; }
      .guided-story-caption { padding: 0.32rem 0.45rem; }
      .guided-story-caption-index { min-width: 2.9rem; padding-right: 0.42rem; }
      .guided-story-caption strong,
      .guided-story-caption small { white-space: normal; }
      .guided-story-caption-next { display: none; }
      .guided-view-trail { min-height: 2rem; }
      .guided-view-trail .guided-view-stop {
        min-height: 2rem;
        padding: 0.22rem 0.42rem 0.22rem 0.28rem;
      }
      .guided-view-handoff { max-width: min(9rem, 32vw); }
      .guided-view-index { padding-top: 0.35rem; }
      .guided-view-chapters {
        gap: 0.35rem;
        scroll-padding-inline: 0.2rem;
      }
      .guided-view-chapters li {
        flex: 0 0 min(14rem, 78vw);
        min-width: 0;
      }
      .guided-view-chapter { min-height: 2.75rem; }
      .guided-views[data-active-view="all"] {
        grid-template-columns: minmax(0, 1fr) auto;
        gap: 0.2rem;
        padding: 0.25rem;
      }
      .guided-views[data-active-view="all"] .guided-view-copy {
        grid-column: 1;
        grid-row: 1;
      }
      .guided-views[data-active-view="all"] .guided-view-actions {
        grid-column: 2;
        grid-row: 1;
      }
      .guided-views[data-active-view="all"] .guided-view-play {
        min-width: 6rem !important;
        min-height: 2.75rem;
      }
      .guided-views[data-active-view="all"] .guided-view-index {
        grid-column: 1 / -1;
        grid-row: 2;
        padding: 0.3rem 0.08rem 0;
        border-top: 1px solid color-mix(in srgb, var(--toolbar-border) 72%, transparent);
        border-left: 0;
      }
      html[data-embed="true"][data-share-playback="true"] .share-chapter-cue:not([hidden]),
      html[data-embed="true"][data-share-moment="true"] .share-chapter-cue:not([hidden]) {
        top: 0.5rem;
        grid-template-columns: auto minmax(0, 1fr);
        gap: 0.28rem 0.55rem;
        width: calc(100% - 1rem);
        min-height: 0;
        padding: 0.45rem 0.55rem 0.55rem;
      }
      html[data-embed="true"][data-share-playback="true"] .diagram-container {
        padding-top: 4.25rem;
      }
      html[data-embed="true"][data-share-moment="true"] .diagram-container {
        padding-top: 4.25rem;
      }
      .share-chapter-index {
        min-width: 4.4rem;
        padding-right: 0.5rem;
      }
      .share-chapter-copy small { font-size: 0.46rem; }
      .share-chapter-route {
        grid-column: 1 / -1;
        font-size: 0.5rem;
        text-align: left;
      }
      .cards {
        grid-template-columns: 1fr;
        margin-top: 1rem;
      }
      html[data-present="true"]:not([data-embed="true"]) body { padding: 0.5rem; }
      html[data-present="true"]:not([data-embed="true"]) .container {
        gap: 0.5rem;
        height: calc(100dvh - 1rem);
      }
      html[data-present="true"]:not([data-embed="true"]) .toolbar {
        position: fixed;
        top: 0.5rem;
        right: 0.5rem;
        margin: 0;
        gap: 0.25rem;
      }
      html[data-present="true"]:not([data-embed="true"]) .toolbar button {
        min-height: 2.75rem;
        padding: 0.42rem 0.58rem;
      }
      html[data-present="true"]:not([data-embed="true"]) .header {
        min-height: 2.25rem;
        padding: 3.5rem 0 0;
      }
      html[data-present="true"]:not([data-embed="true"]) .header-row {
        min-height: 2.25rem;
        margin: 0;
        align-items: center;
        flex-wrap: nowrap;
      }
      html[data-present="true"]:not([data-embed="true"]) .header-row::after,
      html[data-present="true"]:not([data-embed="true"]) .pulse-dot,
      html[data-present="true"]:not([data-embed="true"]) .subtitle { display: none; }
      html[data-present="true"]:not([data-embed="true"]) h1 {
        flex: 1 1 auto;
        min-width: 0;
        max-width: 100%;
        overflow: hidden;
        font-size: 0.875rem;
        text-overflow: ellipsis;
        white-space: nowrap;
      }
      html[data-present="true"]:not([data-embed="true"]) .guided-views {
        padding: 0.2rem;
      }
      html[data-present="true"]:not([data-embed="true"]) .guided-view-copy small { display: none; }
      html[data-present="true"]:not([data-embed="true"]) .guided-story-caption small { display: block; }
      html[data-present="true"]:not([data-embed="true"]) .diagram-container { padding: 0.5rem 0.5rem 3.75rem; }
      html[data-present="true"]:not([data-embed="true"]) .diagram-container[data-wide-diagram="true"] > svg {
        width: 720px;
        height: auto;
        min-width: 720px;
      }
    }

    @media (max-width: 420px) {
      .export-menu-section { grid-template-columns: 1fr; }
      .export-menu-heading,
      .toolbar .export-menu button[data-format="share-card"],
      .toolbar .export-menu button[data-action="route-share-card"],
      .toolbar .export-menu button[data-action="reach-share-card"] { grid-column: 1; }
    }

    @media (prefers-reduced-motion: reduce) {
      .guided-story-caption { animation: none !important; }
      .toolbar .preset-menu.open,
      .toolbar .export-menu.open { animation: none; }
      .toolbar-chevron { transition: none; }
    }

    @media (max-width: 360px) {
      .toolbar { gap: 0.25rem; }
      .toolbar button { padding-right: 0.58rem; padding-left: 0.58rem; }
      .toolbar #btn-motion { min-width: 4.4rem; padding-right: 0.58rem; padding-left: 0.58rem; }
      #theme-label, #preset-label, #present-label { display: none; }
      .guided-view-handoff { max-width: 6.5rem; }
    }

    /* ==========================================================
       SVG SEMANTIC CLASSES — use these instead of inline fill/stroke
       ========================================================== */
    .c-grid       { stroke: var(--grid); fill: none; }
    .c-mask       { fill: var(--mask); stroke: none; }

    .c-frontend   { fill: var(--frontend-fill);   stroke: var(--frontend-stroke); }
    .c-backend    { fill: var(--backend-fill);    stroke: var(--backend-stroke); }
    .c-database   { fill: var(--database-fill);   stroke: var(--database-stroke); }
    .c-cloud      { fill: var(--cloud-fill);      stroke: var(--cloud-stroke); }
    .c-security   { fill: var(--security-fill);   stroke: var(--security-stroke); }
    .c-messagebus { fill: var(--messagebus-fill); stroke: var(--messagebus-stroke); }
    .c-external   { fill: var(--external-fill);   stroke: var(--external-stroke); }

    /* Text helpers */
    .t-primary { fill: var(--text); }
    .t-muted   { fill: var(--text-muted); }
    .t-dim     { fill: var(--text-dim); }
    .t-frontend   { fill: var(--frontend-stroke); }
    .t-backend    { fill: var(--backend-stroke); }
    .t-database   { fill: var(--database-stroke); }
    .t-cloud      { fill: var(--cloud-stroke); }
    .t-security   { fill: var(--security-stroke); }
    .t-messagebus { fill: var(--messagebus-stroke); }
    .t-external   { fill: var(--external-stroke); }

    /* ==========================================================
       Node style — box (default) or logo. Both layers are authored into the
       SVG; this rule chooses one, so a reader can switch a delivered artifact
       without re-rendering and every export keeps the active view. Geometry is
       identical in both, so routes, labels and focus targets never move. */
    svg .node-logo, svg .node-logoless, svg .node-logo-label { display: none; }
    svg .node-logo-plate { fill: #fff; stroke: none; }
    svg .node-logo-frame { fill: none; stroke: #cbd5e1; stroke-width: 0.8; vector-effect: non-scaling-stroke; }
    svg .node-logo-label { fill: var(--text); }
    /* The no-logo fallback: a neutral plate carrying the node role sigil, sized to
       match a real logo so the row reads evenly. The sigil keeps its own tone colour. */
    svg .node-logoless-plate { fill: var(--mask); stroke: var(--lane-stroke); stroke-width: 0.8; vector-effect: non-scaling-stroke; }
    svg .node-logoless .semantic-sigil { opacity: .92; stroke-width: 1.1; }

    [data-node-style="logo"] svg .node-logo,
    [data-node-style="logo"] svg .node-logoless,
    [data-node-style="logo"] svg .node-logo-label { display: block; }
    /* In logo mode the box art, its sigil and the small corner badge step aside. */
    [data-node-style="logo"] svg [data-node-id] > rect.c-mask,
    [data-node-style="logo"] svg [data-node-id] > rect:not(.node-logoless-box):not(.node-logo-plate):not(.node-logo-frame),
    [data-node-style="logo"] svg [data-node-id] > .semantic-sigil,
    [data-node-style="logo"] svg [data-node-id] > .brand-mark,
    [data-node-style="logo"] svg [data-node-id] > text:not(.node-logo-label) { display: none; }

    /* Semantic Sigils are small authored role stamps, not icons from a brand
       pack and not viewer controls. Prefix selectors with svg so the canonical
       export's SVG-only stylesheet collector keeps them automatically. */
    svg .semantic-sigil {
      fill: none;
      stroke: currentColor;
      stroke-width: 1.35;
      stroke-linecap: round;
      stroke-linejoin: round;
      opacity: 0.76;
      pointer-events: none;
    }
    svg .semantic-sigil > * { vector-effect: non-scaling-stroke; }
    svg .semantic-sigil .sigil-fill { fill: currentColor; stroke: none; }
    svg .s-frontend   { color: var(--frontend-stroke); }
    svg .s-backend    { color: var(--backend-stroke); }
    svg .s-database   { color: var(--database-stroke); }
    svg .s-cloud      { color: var(--cloud-stroke); }
    svg .s-security   { color: var(--security-stroke); }
    svg .s-messagebus { color: var(--messagebus-stroke); }
    svg .s-external   { color: var(--external-stroke); }

    /* Brand Marks are authored identity badges. Their color is contained
       inside one neutral plate so vendor color never replaces Mosofin's
       semantic node and relationship vocabulary. */
    svg .brand-mark { pointer-events: none; }
    svg .brand-mark-badge { fill: #fff; stroke: none; }
    svg .brand-mark-frame {
      fill: none;
      stroke: #cbd5e1;
      stroke-width: 0.8;
      vector-effect: non-scaling-stroke;
    }
    svg .brand-mark-fallback {
      fill: none;
      stroke: #475569;
      stroke-width: 1.15;
      stroke-linecap: round;
      stroke-linejoin: round;
    }
    svg[data-preset="blueprint"] .brand-mark-badge,
    svg[data-preset="blueprint"] .brand-mark-frame { rx: 1px; }

    /* Reading Depth is a viewer-only level-of-detail layer. MAP preserves the
       authored structure and primary labels, READ restores relationship labels
       and node context, and FULL restores tags and fine annotations. Semantic
       intent can reveal the relevant detail without moving any geometry. */
    svg [data-detail] { transition: opacity 160ms ease; }
    svg [data-detail-anchor] {
      transform-box: fill-box;
      transform-origin: center;
      transition: transform 160ms ease;
    }
    .diagram-container[data-detail-level="map"] svg [data-detail="context"],
    .diagram-container[data-detail-level="map"] svg [data-detail="fine"],
    .diagram-container[data-detail-level="read"] svg [data-detail="fine"] {
      opacity: 0;
      pointer-events: none;
    }
    .diagram-container[data-detail-level="map"] svg [data-detail-anchor] {
      transform: translateY(8px);
    }

    /* Focus, lens, preview, route, and story states are stronger reader intent than
       the global zoom level. Reveal only their exact semantic matches. */
    svg[data-focus-active] [data-focus-match][data-detail],
    svg[data-focus-active] [data-focus-match] [data-detail],
    svg[data-reach-active] [data-reach-match][data-detail],
    svg[data-reach-active] [data-reach-match] [data-detail],
    svg[data-lens-active] [data-lens-match][data-detail],
    svg[data-lens-active] [data-lens-match] [data-detail],
    svg[data-legend-preview-active] [data-legend-preview-match][data-detail],
    svg[data-legend-preview-active] [data-legend-preview-match] [data-detail],
    svg[data-intent-trace-active] [data-intent-trace-match][data-detail],
    svg[data-intent-trace-active] [data-intent-trace-match] [data-detail],
    svg[data-route-active] [data-route-match][data-detail],
    svg[data-route-active] [data-route-match] [data-detail],
    svg[data-story-active] [data-story-step][data-detail],
    svg[data-story-active] [data-story-step] [data-detail],
    svg[data-relationship-preview-active] [data-relationship-preview][data-detail],
    svg[data-relationship-preview-active] [data-relationship-preview] [data-detail],
    svg [data-node-id]:hover [data-detail],
    svg [data-node-id]:focus-visible [data-detail] {
      opacity: 1;
      pointer-events: auto;
    }
    svg[data-focus-active] [data-focus-match] [data-detail-anchor],
    svg[data-reach-active] [data-reach-match] [data-detail-anchor],
    svg[data-lens-active] [data-lens-match] [data-detail-anchor],
    svg[data-legend-preview-active] [data-legend-preview-match] [data-detail-anchor],
    svg[data-intent-trace-active] [data-intent-trace-match] [data-detail-anchor],
    svg[data-route-active] [data-route-match] [data-detail-anchor],
    svg[data-story-active] [data-story-step] [data-detail-anchor],
    svg [data-node-id]:hover [data-detail-anchor],
    svg [data-node-id]:focus-visible [data-detail-anchor] { transform: none; }

    /* Arrows */
    .a-default   { stroke: var(--arrow); fill: none; }
    .a-emphasis  { stroke: var(--arrow-emphasis); fill: none; }
    .a-security  { stroke: var(--security-stroke); fill: none; stroke-dasharray: 5,5; }
    .a-dashed    { stroke: var(--database-stroke); fill: none; stroke-dasharray: 4,4; }

    /* Arrowhead markers reference these */
    .m-default   { fill: var(--arrow); }
    .m-emphasis  { fill: var(--arrow-emphasis); }
    .m-security  { fill: var(--security-stroke); }
    .m-dashed    { fill: var(--database-stroke); }

    /* Stable semantic exploration hooks emitted by every renderer. */
    svg [data-node-id] {
      cursor: pointer;
      transition: opacity 0.18s ease, filter 0.18s ease;
    }
    svg [data-edge-from] { transition: opacity 0.18s ease, filter 0.18s ease; }
    svg [data-node-id]:hover,
    svg [data-node-id]:focus-visible {
      filter: drop-shadow(0 0 7px var(--frontend-stroke));
      outline: none;
    }
    /* Verified Source Beacons are installed at runtime only for nodes whose
       repository evidence passed the local revision/blob/line checks. They
       live inside the owning node so every focus, route, story, and guided
       view keeps one coherent opacity state. Export serialization removes
       them before producing the canonical SVG or any derived visual. */
    .source-evidence-beacon {
      pointer-events: none;
      opacity: 0.76;
      transition: opacity 160ms ease, filter 160ms ease;
    }
    .source-evidence-beacon rect {
      fill: color-mix(in srgb, var(--mask) 92%, var(--backend-stroke));
      stroke: color-mix(in srgb, var(--backend-stroke) 82%, var(--panel-border));
      stroke-width: 1;
      vector-effect: non-scaling-stroke;
    }
    .source-evidence-beacon text {
      fill: var(--backend-stroke);
      font-size: 5.5px;
      font-weight: 800;
      letter-spacing: 0.06em;
      text-anchor: middle;
      text-transform: uppercase;
    }
    svg [data-node-id]:hover .source-evidence-beacon,
    svg [data-node-id]:focus-visible .source-evidence-beacon,
    svg [data-node-id][data-focus-selected] .source-evidence-beacon {
      opacity: 1;
      filter: drop-shadow(0 0 4px color-mix(in srgb, var(--backend-stroke) 72%, transparent));
    }
    svg[data-preset="signal-flow"] .source-evidence-beacon { opacity: 0.9; }
    svg[data-preset="blueprint"] .source-evidence-beacon rect {
      rx: 0.8px;
      filter: none;
    }
    svg [data-legend-kind][role="button"] { cursor: pointer; }
    svg [data-legend-kind]:focus { outline: none; }
    svg [data-legend-hit] {
      fill: transparent;
      stroke: transparent;
      stroke-width: 1.4;
      pointer-events: all;
      vector-effect: non-scaling-stroke;
      transition: stroke 150ms ease, stroke-width 150ms ease, filter 150ms ease;
    }
    svg [data-legend-count-badge] { pointer-events: none; }
    svg [data-legend-count-badge] rect {
      fill: var(--mask);
      stroke: var(--panel-border);
      stroke-width: 1;
      vector-effect: non-scaling-stroke;
    }
    svg [data-legend-count-badge] text {
      fill: var(--text-muted);
      font-size: 8px;
      font-weight: 700;
      text-anchor: middle;
    }
    svg [data-legend-kind][role="button"]:hover [data-legend-hit] {
      stroke: color-mix(in srgb, var(--database-stroke) 58%, transparent);
    }
    svg [data-legend-kind][role="button"]:focus-visible [data-legend-hit] {
      stroke: var(--frontend-stroke);
      stroke-width: 1.8;
      filter: drop-shadow(0 0 3px var(--frontend-stroke));
    }
    svg [data-legend-kind][data-legend-selected] [data-legend-hit] {
      stroke: var(--database-stroke);
      stroke-width: 2.2;
      stroke-dasharray: 3 2;
    }
    svg [data-legend-kind][data-legend-selected] [data-legend-count-badge] rect {
      stroke: var(--database-stroke);
      stroke-width: 1.8;
    }
    svg [data-legend-kind][data-legend-zero] { opacity: 0.44; }
    svg[data-preset="signal-flow"] [data-legend-kind][data-legend-selected] [data-legend-hit] {
      filter: drop-shadow(0 0 4px var(--database-stroke));
    }
    svg[data-preset="blueprint"] [data-legend-hit],
    svg[data-preset="blueprint"] [data-legend-count-badge] rect { rx: 0; filter: none; }
    svg[data-legend-preview-active] [data-node-id],
    svg[data-legend-preview-active] [data-edge-from] { opacity: 0.11; }
    svg[data-legend-preview-active] [data-legend-preview-match] { opacity: 1; }
    svg[data-legend-preview-active] [data-legend-preview-peer] { opacity: 0.62; }
    svg[data-legend-preview-active] [data-legend-preview-selected] {
      filter: drop-shadow(0 0 8px var(--database-stroke));
    }
    svg[data-lens-active] [data-node-id],
    svg[data-lens-active] [data-edge-from] { opacity: 0.11; }
    svg[data-lens-active] [data-lens-match] { opacity: 1; }
    svg[data-lens-active] [data-lens-peer] { opacity: 0.62; }
    svg[data-lens-active] [data-lens-selected] {
      filter: drop-shadow(0 0 10px var(--database-stroke));
    }
    .semantic-lens-overlay { pointer-events: none; }
    .semantic-lens-flow {
      fill: none;
      stroke-width: 3.05;
      stroke-linecap: round;
      stroke-dasharray: 0.075 0.925;
      stroke-dashoffset: 0;
      vector-effect: non-scaling-stroke;
      opacity: 0.9;
      animation: mosofin-semantic-lens-flow 1.35s linear 1 both;
      animation-delay: var(--lens-flow-delay, 0s);
    }
    .semantic-lens-flow[data-direction="out"],
    .semantic-lens-flow[data-direction="forward"] {
      stroke: var(--frontend-stroke);
      filter: drop-shadow(0 0 3px var(--frontend-stroke));
    }
    .semantic-lens-flow[data-direction="in"],
    .semantic-lens-flow[data-direction="reverse"] {
      stroke: var(--database-stroke);
      filter: drop-shadow(0 0 3px var(--database-stroke));
    }
    .semantic-lens-flow[data-direction="within"] {
      stroke: var(--messagebus-stroke);
      filter: drop-shadow(0 0 3px var(--messagebus-stroke));
    }
    svg[data-preset="signal-flow"] .semantic-lens-flow {
      stroke-width: 3.7;
      opacity: 1;
    }
    svg[data-preset="signal-flow"] .semantic-lens-flow[data-direction="out"],
    svg[data-preset="signal-flow"] .semantic-lens-flow[data-direction="forward"] {
      filter: drop-shadow(0 0 6px var(--frontend-stroke));
    }
    svg[data-preset="signal-flow"] .semantic-lens-flow[data-direction="in"],
    svg[data-preset="signal-flow"] .semantic-lens-flow[data-direction="reverse"] {
      filter: drop-shadow(0 0 6px var(--database-stroke));
    }
    svg[data-preset="signal-flow"] .semantic-lens-flow[data-direction="within"] {
      filter: drop-shadow(0 0 6px var(--messagebus-stroke));
    }
    svg[data-preset="blueprint"] .semantic-lens-flow {
      stroke-width: 2.45;
      stroke-linecap: square;
      filter: none;
      opacity: 0.78;
    }
    html[data-embed="true"] .semantic-lens-overlay { display: none; }
    svg[data-focus-active] [data-node-id],
    svg[data-focus-active] [data-edge-from] { opacity: 0.13; }
    svg[data-focus-active] [data-focus-match] { opacity: 1; }
    svg[data-focus-active] [data-focus-selected] {
      filter: drop-shadow(0 0 10px var(--frontend-stroke));
    }

    /* Authored Reachability is a bounded graph query over the relationships
       already present in the artifact. Upstream walks incoming edges;
       downstream walks outgoing edges. It never claims runtime causality and
       changes only viewer emphasis, not canonical geometry or export bytes. */
    svg[data-reach-active] [data-node-id],
    svg[data-reach-active] [data-edge-from] { opacity: 0.09; }
    svg[data-reach-active] [data-reach-match] { opacity: 1; }
    svg[data-reach-active="upstream"] [data-reach-origin] {
      filter: drop-shadow(0 0 11px var(--database-stroke));
    }
    svg[data-reach-active="downstream"] [data-reach-origin] {
      filter: drop-shadow(0 0 11px var(--backend-stroke));
    }
    svg[data-reach-active="upstream"] [data-edge-from][data-reach-match] {
      filter: drop-shadow(0 0 4px color-mix(in srgb, var(--database-stroke) 72%, transparent));
    }
    svg[data-reach-active="downstream"] [data-edge-from][data-reach-match] {
      filter: drop-shadow(0 0 4px color-mix(in srgb, var(--backend-stroke) 72%, transparent));
    }
    svg[data-preset="blueprint"][data-reach-active] [data-reach-origin],
    svg[data-preset="blueprint"][data-reach-active] [data-edge-from][data-reach-match] {
      filter: none;
    }

    /* Direct Relationship Explorer makes authored paths a real input surface
       without changing their visible stroke or canonical SVG. A transparent
       runtime rail supplies forgiving pointer geometry; the exact authored
       path and endpoints continue to carry every visible state. */
    .relationship-hit-overlay { pointer-events: none; }
    .relationship-hit-target { outline: none; }
    .relationship-hit-rail {
      fill: none;
      stroke: transparent;
      stroke-width: 24;
      stroke-linecap: round;
      stroke-linejoin: round;
      vector-effect: non-scaling-stroke;
      pointer-events: stroke;
      cursor: pointer;
    }
    .relationship-focus-rail {
      fill: none;
      stroke: color-mix(in srgb, var(--frontend-stroke) 34%, transparent);
      stroke-width: 6;
      stroke-dasharray: 2 4;
      stroke-linecap: round;
      stroke-linejoin: round;
      vector-effect: non-scaling-stroke;
      pointer-events: none;
      opacity: 0;
    }
    .relationship-hit-target:focus-visible .relationship-focus-rail {
      opacity: 1;
    }
    svg[data-preset="blueprint"] .relationship-hit-target:focus-visible .relationship-focus-rail {
      stroke-linecap: square;
      filter: none;
    }
    svg[data-relationship-direct-active] [data-node-id],
    svg[data-relationship-direct-active] [data-edge-from] { opacity: 0.16; }
    svg[data-relationship-direct-active] [data-relationship-preview],
    svg[data-relationship-direct-active] [data-relationship-preview-node] { opacity: 1; }
    svg[data-focus-active]:not([data-relationship-pin-active]) .relationship-hit-rail,
    svg[data-story-active] .relationship-hit-rail,
    svg[data-route-picking] .relationship-hit-rail,
    svg[data-route-active] .relationship-hit-rail,
    svg[data-lens-active] .relationship-hit-rail,
    svg[data-chapter-preview] .relationship-hit-rail { pointer-events: none; }
    html[data-embed="true"] .relationship-hit-overlay { display: none; }
    @media (hover: none), (pointer: coarse) {
      .relationship-hit-rail { stroke-width: 24; }
    }

    /* Relationship Preview is temporary exploration state layered on top of
       neighborhood focus. It changes emphasis only; exported geometry and the
       relationship's original solid/dashed visual language stay untouched. */
    svg[data-relationship-preview-active] [data-focus-match] { opacity: 0.18; }
    svg[data-relationship-preview-active] [data-relationship-preview],
    svg[data-relationship-preview-active] [data-relationship-preview-node] { opacity: 1; }
    svg[data-relationship-preview-active] [data-relationship-preview] {
      filter: drop-shadow(0 0 5px var(--arrow-emphasis));
    }
    svg[data-relationship-preview-active] [data-relationship-preview] path,
    svg[data-relationship-preview-active] path[data-relationship-preview],
    svg[data-relationship-preview-active] [data-relationship-preview] line,
    svg[data-relationship-preview-active] line[data-relationship-preview],
    svg[data-relationship-preview-active] [data-relationship-preview] polyline,
    svg[data-relationship-preview-active] polyline[data-relationship-preview] {
      stroke-width: 2.75;
    }
    svg[data-relationship-preview-active] [data-relationship-preview-source] {
      filter: drop-shadow(0 0 7px var(--frontend-stroke));
    }
    svg[data-relationship-preview-active] [data-relationship-preview-target] {
      filter: drop-shadow(0 0 7px var(--database-stroke));
    }
    .relationship-pulse-overlay { pointer-events: none; }
    .relationship-flow-pulse {
      fill: none;
      stroke: var(--arrow-emphasis);
      stroke-width: 3.35;
      stroke-linecap: round;
      stroke-dasharray: 0.085 0.915;
      stroke-dashoffset: 0;
      vector-effect: non-scaling-stroke;
      opacity: 0;
      filter: drop-shadow(0 0 4px var(--arrow-emphasis));
      animation: mosofin-relationship-pulse 1.2s linear 1 both;
    }
    svg[data-preset="signal-flow"] .relationship-flow-pulse {
      stroke: var(--frontend-stroke);
      stroke-width: 3.85;
      filter: drop-shadow(0 0 7px var(--frontend-stroke));
    }
    svg[data-preset="blueprint"] .relationship-flow-pulse {
      stroke-width: 2.5;
      stroke-linecap: square;
      filter: none;
    }
    svg[data-preset="editorial"] .relationship-flow-pulse {
      stroke: var(--arrow-emphasis);
      stroke-width: 2.75;
      filter: none;
    }
    .semantic-flow-token {
      color: var(--frontend-stroke);
      opacity: 0;
      pointer-events: none;
    }
    .relationship-flow-token {
      animation: mosofin-relationship-token-life 1.2s linear 1 both;
    }
    .story-flow-token {
      animation: mosofin-relationship-token-life 0.78s linear 1 both;
    }
    .semantic-flow-token-halo {
      fill: var(--mask);
      stroke: currentColor;
      stroke-width: 1.1;
      vector-effect: non-scaling-stroke;
      opacity: 0.96;
    }
    .relationship-flow-token-shape {
      fill: var(--mask);
      stroke: currentColor;
      stroke-width: 1.35;
      stroke-linecap: round;
      stroke-linejoin: round;
      vector-effect: non-scaling-stroke;
    }
    .relationship-flow-token-ink {
      fill: none;
      stroke: currentColor;
      stroke-width: 1.05;
      stroke-linecap: round;
      vector-effect: non-scaling-stroke;
    }
    .relationship-flow-token-dot { fill: currentColor; stroke: none; }
    .semantic-flow-token[data-token-kind="data"] { color: var(--database-stroke); }
    .semantic-flow-token[data-token-kind="event"] { color: var(--messagebus-stroke); }
    .semantic-flow-token[data-token-kind="security"] { color: var(--security-stroke); }
    .semantic-flow-token[data-token-kind="state"] { color: var(--cloud-stroke); }
    svg[data-preset="signal-flow"] .semantic-flow-token {
      filter: drop-shadow(0 0 5px currentColor);
    }
    svg[data-preset="blueprint"] .semantic-flow-token { filter: none; }
    svg[data-preset="editorial"] .semantic-flow-token { filter: none; }
    html[data-embed="true"] .relationship-pulse-overlay { display: none; }

    /* Intent Trace is a temporary, pre-click exploration layer. Related
       topology stays readable while a short viewer-only signal runs over
       cloned edge geometry; the authored edge remains untouched underneath. */
    svg[data-intent-trace-active] [data-node-id],
    svg[data-intent-trace-active] [data-edge-from] { opacity: 0.2; }
    svg[data-intent-trace-active] [data-intent-trace-match] { opacity: 1; }
    svg[data-intent-trace-active] [data-intent-trace-selected] {
      filter: drop-shadow(0 0 10px var(--frontend-stroke));
    }
    .intent-trace-overlay { pointer-events: none; }
    .intent-trace-flow {
      fill: none;
      stroke-width: 3.1;
      stroke-linecap: round;
      stroke-dasharray: 0.1 0.9;
      stroke-dashoffset: 0;
      vector-effect: non-scaling-stroke;
      opacity: 0.94;
      animation: mosofin-intent-trace-flow 1.15s linear 1 both;
    }
    /* Direction names describe each edge relative to the focused node only.
       The cloned geometry already runs from the authored source to target, so
       incoming and outgoing signals must share the same playback direction. */
    .intent-trace-flow[data-direction="out"] {
      stroke: var(--frontend-stroke);
      filter: drop-shadow(0 0 4px var(--frontend-stroke));
    }
    .intent-trace-flow[data-direction="in"] {
      stroke: var(--database-stroke);
      filter: drop-shadow(0 0 4px var(--database-stroke));
      animation-direction: normal;
    }
    .intent-trace-flow[data-direction="loop"] {
      stroke: var(--security-stroke);
      filter: drop-shadow(0 0 4px var(--security-stroke));
    }
    svg[data-preset="blueprint"] [data-intent-trace-selected],
    svg[data-preset="blueprint"] .intent-trace-flow {
      filter: none;
      stroke-linecap: square;
    }
    svg[data-preset="editorial"] [data-intent-trace-selected],
    svg[data-preset="editorial"] .intent-trace-flow { filter: none; }
    .intent-trace-status {
      position: absolute;
      width: 1px;
      height: 1px;
      padding: 0;
      margin: -1px;
      overflow: hidden;
      clip: rect(0, 0, 0, 0);
      clip-path: inset(50%);
      white-space: nowrap;
      border: 0;
    }

    /* Route Probe separates a durable two-point query from selection and
       one-hop hover. Only the chosen shortest directed path stays prominent;
       the traveling signal is cloned viewer geometry and never authored SVG. */
    svg[data-route-picking="target"] [data-node-id] { opacity: 0.24; }
    svg[data-route-picking="target"] [data-route-candidate],
    svg[data-route-picking="target"] [data-route-start] { opacity: 1; }
    svg[data-route-picking] [data-node-id] { cursor: crosshair; }
    svg[data-route-picking] [data-route-start] {
      filter: drop-shadow(0 0 10px var(--frontend-stroke));
    }
    svg[data-route-active] [data-node-id],
    svg[data-route-active] [data-edge-from] { opacity: 0.11; }
    svg[data-route-active] [data-route-match] { opacity: 1; }
    svg[data-route-active] [data-route-start] {
      filter: drop-shadow(0 0 11px var(--frontend-stroke));
    }
    svg[data-route-active] [data-route-end] {
      filter: drop-shadow(0 0 11px var(--security-stroke));
    }
    svg[data-route-active] [data-route-step]:not([data-route-start]):not([data-route-end]) {
      filter: drop-shadow(0 0 7px var(--backend-stroke));
    }
    svg[data-route-journey] [data-route-match][data-route-journey-state="past"] { opacity: 0.62; }
    svg[data-route-journey] [data-route-match][data-route-journey-state="future"] { opacity: 0.34; }
    svg[data-route-journey] [data-route-match][data-route-journey-state="current"] { opacity: 1; }
    svg[data-route-journey] [data-node-id][data-route-journey-current] {
      filter: drop-shadow(0 0 10px var(--backend-stroke));
    }
    svg[data-route-journey] [data-edge-from][data-route-journey-current] {
      opacity: 1;
      filter: drop-shadow(0 0 6px var(--backend-stroke));
    }
    .route-probe-overlay { pointer-events: none; }
    .route-journey-overlay { pointer-events: none; }
    .route-probe-flow {
      fill: none;
      stroke: var(--backend-stroke);
      stroke-width: 3.25;
      stroke-linecap: round;
      stroke-dasharray: 0.085 0.915;
      stroke-dashoffset: 0;
      vector-effect: non-scaling-stroke;
      opacity: 0.96;
      filter: drop-shadow(0 0 5px var(--backend-stroke));
      animation: mosofin-route-probe-flow 1.1s cubic-bezier(0.22, 1, 0.36, 1) 1 both;
      animation-delay: calc(var(--route-step, 0) * 0.16s);
    }
    .route-journey-flow {
      fill: none;
      stroke: var(--backend-stroke);
      stroke-width: 3.5;
      stroke-linecap: round;
      stroke-dasharray: 0.1 0.9;
      stroke-dashoffset: 0;
      vector-effect: non-scaling-stroke;
      opacity: 0;
      filter: drop-shadow(0 0 6px var(--backend-stroke));
      animation: mosofin-route-journey-flow 780ms cubic-bezier(0.22, 1, 0.36, 1) 1 both;
    }
    svg[data-preset="blueprint"] [data-route-start],
    svg[data-preset="blueprint"] [data-route-end],
    svg[data-preset="blueprint"] [data-route-step],
    svg[data-preset="blueprint"] .route-probe-flow {
      filter: none;
      stroke-linecap: square;
    }
    svg[data-preset="blueprint"] .route-journey-flow {
      filter: none;
      stroke-linecap: square;
    }
    svg[data-preset="editorial"] [data-route-start],
    svg[data-preset="editorial"] [data-route-end],
    svg[data-preset="editorial"] [data-route-step],
    svg[data-preset="editorial"] .route-probe-flow,
    svg[data-preset="editorial"] .route-journey-flow { filter: none; }

    svg[data-preset="editorial"] .c-grid {
      stroke-dasharray: 1 4;
      opacity: 0.48;
    }
    svg[data-preset="editorial"] .c-region {
      fill: color-mix(in srgb, var(--cloud-fill) 48%, transparent);
      stroke-dasharray: 9 4;
    }
    svg[data-preset="editorial"] [data-node-id]:hover,
    svg[data-preset="editorial"] [data-node-id]:focus-visible,
    svg[data-preset="editorial"][data-focus-active] [data-focus-selected] {
      filter: drop-shadow(0 2px 2px color-mix(in srgb, var(--text) 18%, transparent));
    }

    svg[data-preset="blueprint"] .c-grid {
      stroke-dasharray: 1 3;
      opacity: 0.78;
    }
    svg[data-preset="blueprint"] .c-region {
      fill: color-mix(in srgb, var(--cloud-fill) 58%, transparent);
      stroke-dasharray: 12 4 2 4;
    }
    svg[data-preset="blueprint"] .c-security-group,
    svg[data-preset="blueprint"] .c-lane { stroke-dasharray: 7 3; }
    svg[data-preset="blueprint"] [data-node-id]:hover,
    svg[data-preset="blueprint"] [data-node-id]:focus-visible {
      filter: drop-shadow(0 0 3px var(--frontend-stroke));
    }
    svg[data-preset="blueprint"][data-focus-active] [data-focus-selected] {
      filter: drop-shadow(0 0 3px var(--frontend-stroke));
    }
    svg[data-preset="blueprint"][data-focus-active][data-reach-active] [data-focus-selected][data-reach-origin] {
      filter: none;
    }
    svg[data-preset="blueprint"][data-relationship-preview-active] [data-relationship-preview],
    svg[data-preset="blueprint"][data-relationship-preview-active] [data-relationship-preview-source],
    svg[data-preset="blueprint"][data-relationship-preview-active] [data-relationship-preview-target] {
      filter: drop-shadow(0 0 3px var(--frontend-stroke));
    }

    /* Story Trail is a viewer-only overlay derived from the ordered focus IDs
       of the active guided view. The authored edge stays underneath with its
       original color, dash pattern, marker, and geometry intact. */
    .story-trail-overlay { pointer-events: none; }
    .story-carrier-overlay { pointer-events: none; }
    .story-trail-flow {
      fill: none;
      stroke: var(--frontend-stroke);
      stroke-width: 2.8;
      stroke-linecap: round;
      vector-effect: non-scaling-stroke;
      opacity: 0.34;
      filter: drop-shadow(0 0 2px var(--frontend-stroke));
    }
    svg[data-story-beat] [data-story-step] {
      opacity: 0.22;
      filter: saturate(0.42);
      transition: opacity 180ms ease, filter 180ms ease;
    }
    svg[data-story-beat] [data-story-step][data-story-beat-state="past"] {
      opacity: 0.72;
      filter: saturate(0.82);
    }
    svg[data-story-beat] [data-story-step][data-story-beat-state="next"] {
      opacity: 0.5;
      filter: saturate(0.66);
    }
    svg[data-story-beat] [data-story-step][data-story-beat-state="active"] {
      opacity: 1;
      filter: drop-shadow(0 0 7px var(--frontend-stroke));
      animation: mosofin-story-beat-node 360ms cubic-bezier(0.22, 1, 0.36, 1) both;
    }
    svg[data-story-beat] [data-edge-from][data-story-beat-step] {
      opacity: 0.16;
      transition: opacity 160ms ease;
    }
    svg[data-story-beat] [data-edge-from][data-story-beat-step][data-story-beat-state="past"] { opacity: 0.58; }
    svg[data-story-beat] [data-edge-from][data-story-beat-step][data-story-beat-state="next"] { opacity: 0.34; }
    svg[data-story-beat] [data-edge-from][data-story-beat-step][data-story-beat-state="active"] { opacity: 1; }
    svg[data-story-beat] .story-trail-flow { opacity: 0; transition: opacity 160ms ease; }
    svg[data-story-beat] .story-trail-flow[data-story-beat-state="past"] { opacity: 0.34; }
    svg[data-story-beat] .story-trail-flow[data-story-beat-state="next"] { opacity: 0.2; }
    svg[data-story-beat] .story-trail-flow[data-story-beat-state="active"] { opacity: 0.92; }
    svg[data-story-beat] .story-trail-flow[data-story-pulse="true"] {
      stroke-dasharray: 2 13;
      stroke-dashoffset: 30;
      opacity: 0.92;
      animation: mosofin-story-flow 0.78s linear 1 both;
    }
    svg[data-preset="blueprint"] .story-trail-flow {
      stroke-linecap: square;
      filter: none;
      opacity: 0.5;
    }
    svg[data-preset="blueprint"][data-story-beat] [data-story-step][data-story-beat-state="active"] {
      filter: none;
      animation: none;
    }
    svg[data-preset="editorial"] .story-trail-flow { filter: none; }
    svg[data-preset="editorial"][data-story-beat] [data-story-step][data-story-beat-state="active"] {
      filter: drop-shadow(0 2px 2px color-mix(in srgb, var(--text) 16%, transparent));
    }

    /* Chapter Delta Preview is a static, pre-commit comparison between two
       chapter focus sets. Symbols in the rail carry the meaning without color;
       these SVG rules only make exact stable-ID membership easier to scan. */
    svg[data-chapter-preview] [data-node-id],
    svg[data-chapter-preview] [data-edge-from] {
      opacity: 0.1 !important;
      filter: none !important;
      transition: none !important;
    }
    svg[data-chapter-preview] .story-trail-overlay { opacity: 0.08 !important; }
    svg[data-chapter-preview] [data-node-id][data-chapter-preview-role="stay"] {
      opacity: 0.76 !important;
      filter: saturate(0.7) !important;
    }
    svg[data-chapter-preview] [data-node-id][data-chapter-preview-role="enter"] {
      opacity: 1 !important;
      filter: saturate(1.2) !important;
    }
    svg[data-chapter-preview] [data-node-id][data-chapter-preview-role="leave"] {
      opacity: 0.28 !important;
      filter: grayscale(0.72) !important;
    }
    svg[data-preset="blueprint"][data-chapter-preview] [data-node-id] { filter: none !important; }

    /* Dashed boundary variants */
    .c-security-group { fill: transparent; stroke: var(--security-stroke); stroke-dasharray: 4,4; }
    .c-region         { fill: rgba(251, 191, 36, 0.05); stroke: var(--cloud-stroke); stroke-dasharray: 8,4; }
    .c-lane           { fill: var(--lane-fill); stroke: var(--lane-stroke); stroke-dasharray: 6,6; }

    /* Optional trace animation, enabled only by meta.animation = "trace".
       The ordinary viewer performs one ambient pass and then restores the
       authored solid/security/async line language. WebM samples the same
       authored geometry into an explicit canvas timeline. */
    html[data-ambient-motion="running"] svg[data-animation="trace"] [data-animate="edge"] {
      animation: mosofin-edge-flow 2.4s linear 1;
      animation-delay: calc(var(--step, 0) * 160ms);
    }
    html[data-ambient-motion="running"] svg[data-animation="trace"] [data-animate="node"] {
      transform-box: fill-box;
      transform-origin: center;
      animation: mosofin-node-pulse 3.6s ease-in-out 1;
      animation-delay: calc(var(--step, 0) * 160ms);
    }
    svg[data-preset="signal-flow"][data-animation="trace"] [data-animate="edge"] {
      stroke-linecap: round;
      filter: drop-shadow(0 0 3px var(--arrow-emphasis));
      animation-duration: 1.75s;
    }
    svg[data-preset="signal-flow"][data-animation="trace"] [data-animate="node"] {
      animation-duration: 3.1s;
    }
    svg[data-preset="blueprint"][data-animation="trace"] [data-animate="edge"] {
      stroke-linecap: square;
      filter: none;
      animation-duration: 2.15s;
    }
    svg[data-preset="blueprint"][data-animation="trace"] [data-animate="node"] {
      animation-name: mosofin-blueprint-node-pulse;
      animation-duration: 3.8s;
    }
    svg[data-preset="editorial"][data-animation="trace"] [data-animate="edge"] {
      filter: none;
      animation-duration: 2.2s;
    }
    svg[data-preset="editorial"][data-animation="trace"] [data-animate="node"] {
      animation-name: mosofin-editorial-node-pulse;
      animation-duration: 3.5s;
    }
    /* Embedded proofs are static until a named Story Trail chapter is played.
       This keeps proof grids calm and ensures ?play=1 stops all viewer motion
       after one 3.2 second chapter instead of leaving ambient trace loops on. */
    html[data-embed="true"] svg[data-animation="trace"] [data-animate],
    html[data-share-playback="true"] svg[data-animation="trace"] [data-animate] {
      animation: none !important;
      stroke-dashoffset: 0;
    }
    @keyframes mosofin-edge-flow {
      0% { stroke-dasharray: 10 8; stroke-dashoffset: 54; opacity: 0.42; }
      88% { stroke-dasharray: 10 8; stroke-dashoffset: 0; opacity: 1; }
      99.9% { stroke-dasharray: 10 8; stroke-dashoffset: 0; opacity: 1; }
      100% { stroke-dashoffset: 0; opacity: 1; }
    }
    @keyframes mosofin-node-pulse {
      0%, 72%, 100% { filter: none; stroke-width: 1.5; }
      18%, 36% {
        filter: drop-shadow(0 0 8px var(--arrow-emphasis));
        stroke-width: 2.4;
      }
    }
    @keyframes mosofin-blueprint-node-pulse {
      0%, 72%, 100% { opacity: 1; filter: none; }
      18%, 36% { opacity: 0.72; filter: drop-shadow(0 0 2px var(--frontend-stroke)); }
    }
    @keyframes mosofin-editorial-node-pulse {
      0%, 72%, 100% { opacity: 1; filter: none; }
      18%, 36% { opacity: 0.78; filter: drop-shadow(0 2px 2px color-mix(in srgb, var(--text) 18%, transparent)); }
    }
    @keyframes mosofin-signal-scan {
      0%, 18% { opacity: 1; transform: translateX(-70%); }
      70% { opacity: 1; transform: translateX(70%); }
      100% { opacity: 0; transform: translateX(70%); }
    }
    @keyframes mosofin-radar-live {
      0% { box-shadow: 0 0 0 0 color-mix(in srgb, var(--backend-stroke) 52%, transparent); }
      55%, 100% { box-shadow: 0 0 0 0.32rem transparent; }
    }
    @keyframes mosofin-guided-progress {
      from { transform: scaleX(var(--guided-progress-start, 0)); }
      to { transform: scaleX(1); }
    }
    @keyframes mosofin-story-flow {
      to { stroke-dashoffset: 0; }
    }
    @keyframes mosofin-intent-trace-flow {
      0% { opacity: 0.28; stroke-dashoffset: 0; }
      12%, 78% { opacity: 0.94; }
      100% { opacity: 0; stroke-dashoffset: -1; }
    }
    @keyframes mosofin-route-probe-flow {
      0% { stroke-dashoffset: 0; opacity: 0.96; }
      78% { stroke-dashoffset: -1; opacity: 0.96; }
      100% { stroke-dasharray: none; stroke-dashoffset: 0; opacity: 0.58; }
    }
    @keyframes mosofin-route-journey-flow {
      0% { opacity: 0.16; stroke-dashoffset: 0; }
      14%, 76% { opacity: 1; }
      100% { opacity: 0; stroke-dashoffset: -1; }
    }
    @keyframes mosofin-semantic-lens-flow {
      0% { opacity: 0.2; stroke-dashoffset: 0; }
      12%, 78% { opacity: 0.9; }
      100% { opacity: 0; stroke-dashoffset: -1; }
    }
    @keyframes mosofin-relationship-pulse {
      0% { opacity: 0.18; stroke-dashoffset: 0; }
      10%, 76% { opacity: 0.98; }
      100% { opacity: 0; stroke-dashoffset: -1; }
    }
    @keyframes mosofin-relationship-token-life {
      0%, 7% { opacity: 0; }
      13%, 82% { opacity: 1; }
      100% { opacity: 0; }
    }
    @keyframes mosofin-story-beat-node {
      from { opacity: 0.48; filter: saturate(0.6); }
      58% { opacity: 1; filter: drop-shadow(0 0 10px var(--frontend-stroke)); }
      to { opacity: 1; filter: drop-shadow(0 0 7px var(--frontend-stroke)); }
    }
    @keyframes mosofin-share-cue-enter {
      from { opacity: 0; transform: translate(-50%, -0.45rem); }
      to { opacity: 1; transform: translate(-50%, 0); }
    }
    @keyframes mosofin-share-cue-pulse {
      0%, 100% { opacity: 1; transform: scale(1); }
      50% { opacity: 0.48; transform: scale(0.78); }
    }
    @media (prefers-reduced-motion: reduce) {
      html[data-preset="signal-flow"] .diagram-container::before {
        animation: none !important;
      }
      .pulse-dot { animation: none !important; }
      svg[data-animation="trace"] [data-animate] {
        animation: none !important;
        stroke-dashoffset: 0;
      }
      .guided-view-progress span {
        animation: none !important;
        transform: scaleX(1) !important;
      }
      .story-trail-flow,
      .intent-trace-flow,
      .route-probe-flow,
      .semantic-lens,
      .diagram-guide,
      [data-story-step],
      .overview-map-live,
      .guided-view-stop,
      .share-chapter-cue,
      .chapter-handoff-anchor,
      .share-chapter-state::before,
      .share-chapter-progress span { animation: none !important; }
      .guided-view-chapter,
      svg[data-chapter-preview] [data-node-id],
      svg[data-chapter-preview] [data-edge-from] { transition: none !important; }
      svg[data-story-playing="true"] .story-trail-flow {
        stroke-dasharray: none;
        stroke-dashoffset: 0;
        opacity: 0.55;
      }
      .intent-trace-flow {
        animation: none !important;
        stroke-dasharray: none;
        stroke-dashoffset: 0;
        opacity: 0.72;
      }
      .route-probe-flow {
        animation: none !important;
        stroke-dasharray: none;
        stroke-dashoffset: 0;
        opacity: 0.78;
      }
      .route-journey-overlay,
      .story-carrier-overlay { display: none !important; }
      .semantic-lens-flow {
        animation: none !important;
        stroke-dasharray: none;
        stroke-width: 2.2;
        opacity: 0.34;
        filter: none;
      }
      .relationship-pulse-overlay { display: none !important; }
      .guided-view-chapter { transition: none !important; }
      .source-evidence-beacon,
      svg [data-node-id], svg [data-edge-from], svg [data-detail], svg [data-detail-anchor], svg [data-legend-hit], svg { transition: none !important; }
    }
  </style>
</head>
<body>
  <!-- Toolbar: theme, live visual style, trace motion, presentation stage, and export.
       Motion appears only for trace-enabled artifacts and has no shortcut;
       Keyboard: T toggles theme, S cycles style, F toggles the stage, E opens export. -->
  <div class="toolbar" role="toolbar" aria-label="{{i18n:viewer.toolbar.actions}}">
    <button id="btn-theme" type="button"
            title="{{i18n:viewer.theme.toggle.title}}"
            aria-label="{{i18n:viewer.theme.toggle}}"
            aria-pressed="false">
      <span id="theme-icon" class="toolbar-icon" aria-hidden="true"></span>
      <span id="theme-label">{{i18n:viewer.theme.dark}}</span>
    </button>
    <div class="preset-wrap">
      <button id="btn-preset" type="button"
              title="{{i18n:viewer.preset.choose.title}}"
              aria-label="{{i18n:viewer.preset.choose}}"
              aria-haspopup="menu"
              aria-expanded="false"
              aria-controls="preset-menu"
              aria-keyshortcuts="S">
        <span class="preset-control-mark" aria-hidden="true"></span>
        <span id="preset-label">{{i18n:viewer.preset.style}}</span>
        <span class="toolbar-icon toolbar-chevron" aria-hidden="true"></span>
      </button>
      <div class="preset-menu" id="preset-menu" role="menu" aria-label="{{i18n:viewer.preset.menu}}">
        <div class="preset-menu-heading" role="presentation"><span>{{i18n:viewer.preset.identity}}</span><span>{{i18n:viewer.preset.cycles}}</span></div>
        <button class="preset-option" data-preset-value="classic" type="button" role="menuitemradio" aria-checked="false" tabindex="-1">
          <span class="preset-option-swatch classic" aria-hidden="true"></span>
          <span class="preset-option-copy"><strong>{{i18n:viewer.preset.classic}}</strong><small>{{i18n:viewer.preset.classic.hint}}</small></span>
          <span class="preset-option-check" aria-hidden="true"></span>
        </button>
        <button class="preset-option" data-preset-value="signal-flow" type="button" role="menuitemradio" aria-checked="false" tabindex="-1">
          <span class="preset-option-swatch signal-flow" aria-hidden="true"></span>
          <span class="preset-option-copy"><strong>{{i18n:viewer.preset.flow}}</strong><small>{{i18n:viewer.preset.flow.hint}}</small></span>
          <span class="preset-option-check" aria-hidden="true"></span>
        </button>
        <button class="preset-option" data-preset-value="blueprint" type="button" role="menuitemradio" aria-checked="false" tabindex="-1">
          <span class="preset-option-swatch blueprint" aria-hidden="true"></span>
          <span class="preset-option-copy"><strong>{{i18n:viewer.preset.blueprint}}</strong><small>{{i18n:viewer.preset.blueprint.hint}}</small></span>
          <span class="preset-option-check" aria-hidden="true"></span>
        </button>
        <button class="preset-option" data-preset-value="editorial" type="button" role="menuitemradio" aria-checked="false" tabindex="-1">
          <span class="preset-option-swatch editorial" aria-hidden="true"></span>
          <span class="preset-option-copy"><strong>{{i18n:viewer.preset.editorial}}</strong><small>{{i18n:viewer.preset.editorial.hint}}</small></span>
          <span class="preset-option-check" aria-hidden="true"></span>
        </button>
      </div>
    </div>
    <button id="btn-nodestyle" type="button" title="{{i18n:viewer.nodestyle.title}}" aria-pressed="false">
      <span class="toolbar-icon nodestyle" aria-hidden="true"></span>
      <span id="nodestyle-label">{{i18n:viewer.nodestyle.box}}</span>
    </button>
    <button id="btn-motion" type="button" hidden
            title="{{i18n:viewer.motion.pause}}"
            aria-label="{{i18n:viewer.motion.pause}}"
            aria-pressed="true">
      <span class="motion-control-dot" aria-hidden="true"></span>
      <span id="motion-label">{{i18n:viewer.motion.live}}</span>
    </button>
    <button id="btn-present" type="button"
            title="{{i18n:viewer.present.enter.title}}"
            aria-label="{{i18n:viewer.present.enter}}"
            aria-pressed="false">
      <span id="present-icon" class="toolbar-icon" aria-hidden="true"></span>
      <span id="present-label">{{i18n:viewer.present.present}}</span>
    </button>
    <div class="export-wrap">
      <button id="btn-export" type="button"
              title="{{i18n:viewer.export.button.title}}"
              aria-label="{{i18n:viewer.export.diagram}}"
              aria-haspopup="menu"
              aria-expanded="false"
              aria-controls="export-menu">
        {{i18n:viewer.export.button}} <span class="toolbar-icon toolbar-chevron" aria-hidden="true"></span>
      </button>
      <div class="export-menu" id="export-menu" role="menu" aria-label="{{i18n:viewer.export.menu}}">
        <div class="export-menu-header" role="presentation">
          <div><strong>{{i18n:viewer.export.diagram}}</strong><span>{{i18n:viewer.export.subtitle}}</span></div>
          <kbd>E</kbd>
        </div>
        <div class="export-menu-section" role="group" aria-label="{{i18n:viewer.export.share}}">
          <span class="export-menu-heading" aria-hidden="true">{{i18n:viewer.export.share}}</span>
          <button data-format="share-card" type="button" role="menuitem" tabindex="-1"><span class="export-item-copy"><strong>{{i18n:viewer.export.shareCard}}</strong><small class="hint">1200&times;630 PNG</small></span></button>
          <button data-action="route-share-card" type="button" role="menuitem" tabindex="-1" hidden disabled><span class="export-item-copy"><strong>{{i18n:viewer.export.routeShareCard}}</strong><small class="hint">1200&times;630 PNG</small></span></button>
          <button data-action="reach-share-card" type="button" role="menuitem" tabindex="-1" hidden disabled><span class="export-item-copy"><strong>{{i18n:viewer.export.reachShareCard}}</strong><small class="hint">1200&times;630 PNG</small></span></button>
          <button data-action="copy-share-card" type="button" role="menuitem" tabindex="-1"><span class="export-item-copy"><strong>{{i18n:viewer.export.copyShareCard}}</strong><small class="hint">{{i18n:viewer.export.clipboardPng}}</small></span></button>
          <button data-action="copy" type="button" role="menuitem" tabindex="-1"><span class="export-item-copy"><strong>{{i18n:viewer.export.copyDiagram}}</strong><small class="hint">{{i18n:viewer.export.clipboardPng}}</small></span></button>
        </div>
        <div class="export-menu-section" role="group" aria-label="{{i18n:viewer.export.raster}}">
          <span class="export-menu-heading" aria-hidden="true">{{i18n:viewer.export.image}}</span>
          <button data-format="png" type="button" role="menuitem" tabindex="-1"><span class="export-item-copy"><strong>PNG</strong><small class="hint">{{i18n:viewer.export.lossless}}</small></span></button>
          <button data-format="jpeg" type="button" role="menuitem" tabindex="-1"><span class="export-item-copy"><strong>JPEG</strong><small class="hint">{{i18n:viewer.export.compact}}</small></span></button>
          <button data-format="webp" type="button" role="menuitem" tabindex="-1"><span class="export-item-copy"><strong>WebP</strong><small class="hint">{{i18n:viewer.export.modern}}</small></span></button>
        </div>
        <div class="export-menu-section" role="group" aria-label="{{i18n:viewer.export.vectorMotion}}">
          <span class="export-menu-heading" aria-hidden="true">{{i18n:viewer.export.vectorMotion.heading}}</span>
          <button data-format="svg" type="button" role="menuitem" tabindex="-1"><span class="export-item-copy"><strong>SVG</strong><small class="hint">{{i18n:viewer.export.editable}}</small></span></button>
          <button data-format="webm" type="button" role="menuitem" tabindex="-1"><span class="export-item-copy"><strong>WebM</strong><small class="hint">{{i18n:viewer.export.motion6s}}</small></span></button>
        </div>
      </div>
    </div>
  </div>

  <div class="container">
    <!-- Header -->
    <div class="header">
      <div class="header-row"
           data-preset-badge-signal-flow="{{i18n:viewer.preset.badge.signalFlow}}"
           data-preset-badge-blueprint="{{i18n:viewer.preset.badge.blueprint}}"
           data-preset-badge-editorial="{{i18n:viewer.preset.badge.editorial}}">
        <div class="pulse-dot"></div>
        <h1>[PROJECT NAME] Architecture</h1>
      </div>
      <p class="subtitle">[Subtitle description]</p>
    </div>

    <!-- MOSOFIN:GUIDED_VIEWS_DATA -->
    <!-- MOSOFIN:SOURCE_EVIDENCE_DATA -->
    <!-- MOSOFIN:I18N_DATA -->
    <div class="guided-views no-print" id="guided-views" hidden aria-label="{{i18n:viewer.guided.region}}">
      <button id="guided-view-prev" type="button" aria-label="{{i18n:viewer.guided.previous}}" title="{{i18n:viewer.guided.previous.title}}">&#8592;</button>
      <div class="guided-view-copy">
        <div class="guided-view-meta">
          <span>{{i18n:viewer.guided.views}}</span>
          <span id="guided-view-count"></span>
          <span class="guided-view-progress" aria-hidden="true"><span id="guided-view-progress-bar"></span></span>
          <span class="guided-view-handoff" id="guided-view-handoff" hidden aria-hidden="true"></span>
        </div>
        <strong id="guided-view-label">{{i18n:viewer.guided.explore}}</strong>
        <small id="guided-view-note" aria-live="polite" aria-atomic="true">{{i18n:viewer.guided.intro}}</small>
        <div class="guided-view-trail" id="guided-view-trail" hidden role="group" aria-label="{{i18n:viewer.guided.trail}}"></div>
        <div class="guided-story-caption" id="guided-story-caption" hidden aria-live="polite" aria-atomic="true">
          <span class="guided-story-caption-index" id="guided-story-caption-index">{{i18n:viewer.guided.beat}}</span>
          <strong id="guided-story-caption-route"></strong>
          <small id="guided-story-caption-detail"></small>
          <span class="guided-story-caption-next" id="guided-story-caption-next" hidden aria-hidden="true">
            <small>{{i18n:viewer.guided.nextBeat}}</small>
            <strong id="guided-story-caption-next-label"></strong>
          </span>
        </div>
      </div>
      <button id="guided-view-next" type="button" aria-label="{{i18n:viewer.guided.next}}" title="{{i18n:viewer.guided.next.title}}">&#8594;</button>
      <div class="guided-view-actions">
        <button class="guided-view-play" id="guided-view-play" type="button" aria-label="{{i18n:viewer.guided.play}}" aria-pressed="false" title="{{i18n:viewer.guided.play.title}}">
          <span id="guided-view-play-icon" aria-hidden="true">&#9654;</span>
          <span id="guided-view-play-label">{{i18n:viewer.guided.playStory}}</span>
        </button>
        <button class="guided-view-beat-link" id="guided-view-beat-link" type="button" aria-label="{{i18n:viewer.guided.selectBeatLink}}" title="{{i18n:viewer.guided.selectBeatLink}}" disabled>
          <span aria-hidden="true">&#35;</span>
          <span id="guided-view-beat-link-label">{{i18n:viewer.guided.copyMoment}}</span>
        </button>
        <button class="guided-view-all" id="guided-view-all" type="button" aria-label="{{i18n:viewer.guided.showAll.aria}}">{{i18n:viewer.guided.showAll}}</button>
      </div>
      <nav class="guided-view-index" id="guided-view-index" aria-label="{{i18n:viewer.guided.chapters}}">
        <ol class="guided-view-chapters" id="guided-view-chapters"></ol>
      </nav>
    </div>

    <!-- Main Diagram -->
    <div class="diagram-container" data-detail-level="read"
         data-preset-badge-editorial-plate="{{i18n:viewer.preset.badge.editorialPlate}}">
      <!-- MOSOFIN:SVG_SLOT_START -->
      <svg viewBox="0 0 1000 680" role="img" aria-label="[PROJECT NAME] architecture diagram">
        <!-- Definitions -->
        <defs>
          <marker id="arrowhead" markerWidth="10" markerHeight="7" refX="9" refY="3.5" orient="auto">
            <polygon points="0 0, 10 3.5, 0 7" class="m-default" />
          </marker>
          <marker id="arrowhead-emphasis" markerWidth="10" markerHeight="7" refX="9" refY="3.5" orient="auto">
            <polygon points="0 0, 10 3.5, 0 7" class="m-emphasis" />
          </marker>
          <marker id="arrowhead-security" markerWidth="10" markerHeight="7" refX="9" refY="3.5" orient="auto">
            <polygon points="0 0, 10 3.5, 0 7" class="m-security" />
          </marker>
          <marker id="arrowhead-dashed" markerWidth="10" markerHeight="7" refX="9" refY="3.5" orient="auto">
            <polygon points="0 0, 10 3.5, 0 7" class="m-dashed" />
          </marker>
          <pattern id="grid" width="40" height="40" patternUnits="userSpaceOnUse">
            <path d="M 40 0 L 0 0 0 40" class="c-grid" stroke-width="0.5"/>
          </pattern>
        </defs>

        <!-- Background Grid -->
        <rect width="100%" height="100%" fill="url(#grid)" />

        <!-- =================================================================
             COMPONENT EXAMPLES — use CSS classes, not inline fill/stroke
             (so theme toggle works). Each box pattern:
               1. opaque mask rect (.c-mask) to hide arrows underneath
               2. styled rect (.c-<type>) with 1.5px stroke
               3. text (.t-primary / .t-muted)
             ================================================================= -->

        <!-- External/Generic Component -->
        <rect x="30" y="280" width="100" height="50" rx="6" class="c-mask"/>
        <rect x="30" y="280" width="100" height="50" rx="6" class="c-external" stroke-width="1.5"/>
        <text x="80" y="300" class="t-primary" font-size="11" font-weight="600" text-anchor="middle">Users</text>
        <text x="80" y="316" class="t-muted" font-size="9" text-anchor="middle">Browser/Mobile</text>

        <!-- Security Component -->
        <rect x="30" y="80" width="100" height="60" rx="6" class="c-mask"/>
        <rect x="30" y="80" width="100" height="60" rx="6" class="c-security" stroke-width="1.5"/>
        <text x="80" y="105" class="t-primary" font-size="11" font-weight="600" text-anchor="middle">Auth Provider</text>
        <text x="80" y="121" class="t-muted" font-size="9" text-anchor="middle">OAuth 2.0</text>

        <!-- Region/Cloud Boundary -->
        <rect x="160" y="40" width="820" height="620" rx="12" class="c-region" stroke-width="1"/>
        <text x="172" y="58" class="t-cloud" font-size="10" font-weight="600">AWS Region: us-west-2</text>

        <!-- AWS/Cloud Service -->
        <rect x="200" y="280" width="110" height="50" rx="6" class="c-mask"/>
        <rect x="200" y="280" width="110" height="50" rx="6" class="c-cloud" stroke-width="1.5"/>
        <text x="255" y="300" class="t-primary" font-size="11" font-weight="600" text-anchor="middle">CloudFront</text>
        <text x="255" y="316" class="t-muted" font-size="9" text-anchor="middle">CDN</text>

        <!-- Multi-line AWS Component (S3 Buckets example) -->
        <rect x="200" y="380" width="110" height="100" rx="6" class="c-mask"/>
        <rect x="200" y="380" width="110" height="100" rx="6" class="c-cloud" stroke-width="1.5"/>
        <text x="255" y="400" class="t-primary" font-size="11" font-weight="600" text-anchor="middle">S3 Buckets</text>
        <text x="255" y="420" class="t-muted" font-size="8" text-anchor="middle">&bull; bucket-one</text>
        <text x="255" y="434" class="t-muted" font-size="8" text-anchor="middle">&bull; bucket-two</text>
        <text x="255" y="448" class="t-muted" font-size="8" text-anchor="middle">&bull; bucket-three</text>
        <text x="255" y="466" class="t-cloud" font-size="7" text-anchor="middle">OAI Protected</text>

        <!-- Security Group (dashed boundary). NOTE: extra vertical headroom
             so the sg-name label has ~12px breathing room above the inner
             component. Pattern: boundary y = inner.y - 30, h = inner.h + 50. -->
        <rect x="350" y="250" width="120" height="100" rx="8" class="c-security-group" stroke-width="1"/>
        <text x="358" y="268" class="t-security" font-size="8">sg-name :port</text>

        <!-- Component inside security group -->
        <rect x="360" y="280" width="100" height="50" rx="6" class="c-mask"/>
        <rect x="360" y="280" width="100" height="50" rx="6" class="c-cloud" stroke-width="1.5"/>
        <text x="410" y="300" class="t-primary" font-size="11" font-weight="600" text-anchor="middle">Load Balancer</text>
        <text x="410" y="316" class="t-muted" font-size="9" text-anchor="middle">HTTPS :443</text>

        <!-- Backend Component -->
        <rect x="510" y="280" width="110" height="50" rx="6" class="c-mask"/>
        <rect x="510" y="280" width="110" height="50" rx="6" class="c-backend" stroke-width="1.5"/>
        <text x="565" y="300" class="t-primary" font-size="11" font-weight="600" text-anchor="middle">API Server</text>
        <text x="565" y="316" class="t-muted" font-size="9" text-anchor="middle">FastAPI :8000</text>

        <!-- Database Component -->
        <rect x="700" y="280" width="120" height="50" rx="6" class="c-mask"/>
        <rect x="700" y="280" width="120" height="50" rx="6" class="c-database" stroke-width="1.5"/>
        <text x="760" y="300" class="t-primary" font-size="11" font-weight="600" text-anchor="middle">Database</text>
        <text x="760" y="316" class="t-muted" font-size="9" text-anchor="middle">PostgreSQL</text>

        <!-- Frontend Component -->
        <rect x="200" y="520" width="200" height="110" rx="8" class="c-mask"/>
        <rect x="200" y="520" width="200" height="110" rx="8" class="c-frontend" stroke-width="1.5"/>
        <text x="300" y="545" class="t-primary" font-size="12" font-weight="600" text-anchor="middle">Frontend</text>
        <text x="300" y="565" class="t-muted" font-size="9" text-anchor="middle">React + TypeScript</text>
        <text x="300" y="580" class="t-muted" font-size="9" text-anchor="middle">Additional detail</text>
        <text x="300" y="595" class="t-muted" font-size="9" text-anchor="middle">More info</text>
        <text x="300" y="615" class="t-frontend" font-size="8" text-anchor="middle">domain.example.com</text>

        <!-- =================================================================
             ARROW EXAMPLES (use .a-* classes and stroke-width on the line)
             ================================================================= -->

        <!-- Standard arrow with label -->
        <line x1="130" y1="305" x2="198" y2="305" class="a-default" stroke-width="1.5" marker-end="url(#arrowhead)"/>
        <text x="164" y="299" class="t-muted" font-size="9" text-anchor="middle">HTTPS</text>

        <!-- Simple arrow (no label) -->
        <line x1="310" y1="305" x2="358" y2="305" class="a-default" stroke-width="1.5" marker-end="url(#arrowhead)"/>

        <!-- Vertical arrow -->
        <line x1="255" y1="330" x2="255" y2="378" class="a-default" stroke-width="1.5" marker-end="url(#arrowhead)"/>
        <text x="270" y="358" class="t-muted" font-size="9">OAI</text>

        <!-- Straight arrows between services -->
        <line x1="460" y1="305" x2="508" y2="305" class="a-default" stroke-width="1.5" marker-end="url(#arrowhead)"/>
        <line x1="620" y1="305" x2="698" y2="305" class="a-default" stroke-width="1.5" marker-end="url(#arrowhead)"/>
        <text x="655" y="299" class="t-muted" font-size="9">TLS</text>

        <!-- Curved dashed path for auth/security flow -->
        <path d="M 80 140 L 80 200 Q 80 220 100 220 L 200 220 Q 220 220 220 240 L 220 278"
              class="a-security" stroke-width="1.5"/>
        <text x="150" y="210" class="t-security" font-size="8">JWT + PKCE</text>

        <!-- =================================================================
             LEGEND
             ================================================================= -->
        <text x="720" y="70" class="t-primary" font-size="10" font-weight="600">Legend</text>

        <rect x="720" y="82" width="16" height="10" rx="2" class="c-frontend" stroke-width="1"/>
        <text x="742" y="90" class="t-muted" font-size="8">Frontend</text>

        <rect x="720" y="98" width="16" height="10" rx="2" class="c-backend" stroke-width="1"/>
        <text x="742" y="106" class="t-muted" font-size="8">Backend</text>

        <rect x="720" y="114" width="16" height="10" rx="2" class="c-cloud" stroke-width="1"/>
        <text x="742" y="122" class="t-muted" font-size="8">Cloud Service</text>

        <rect x="720" y="130" width="16" height="10" rx="2" class="c-database" stroke-width="1"/>
        <text x="742" y="138" class="t-muted" font-size="8">Database</text>

        <rect x="720" y="146" width="16" height="10" rx="2" class="c-security" stroke-width="1"/>
        <text x="742" y="154" class="t-muted" font-size="8">Security</text>

        <line x1="720" y1="168" x2="736" y2="168" class="a-security" stroke-width="1"/>
        <text x="742" y="171" class="t-muted" font-size="8">Auth Flow</text>

        <rect x="720" y="178" width="16" height="10" rx="2" class="c-security-group" stroke-width="1"/>
        <text x="742" y="186" class="t-muted" font-size="8">Security Group</text>
      </svg>
      <!-- MOSOFIN:SVG_SLOT_END -->
      <p class="intent-trace-status no-print" id="intent-trace-status" role="status" aria-live="polite" aria-atomic="true"></p>
      <div class="share-chapter-cue no-print" id="share-chapter-cue" hidden role="status" aria-live="polite">
        <div class="share-chapter-index">
          <span class="share-chapter-state" id="share-chapter-state">{{i18n:viewer.guided.state.ready}}</span>
          <span class="share-chapter-count" id="share-chapter-count">{{i18n:viewer.guided.share.initial}}</span>
        </div>
        <div class="share-chapter-copy">
          <strong id="share-chapter-label">{{i18n:viewer.guided.share.default}}</strong>
          <small id="share-chapter-note"></small>
        </div>
        <span class="share-chapter-route" id="share-chapter-route"></span>
        <span class="share-chapter-progress" aria-hidden="true"><span id="share-chapter-progress-bar"></span></span>
      </div>
      <div class="diagram-guide no-print" id="diagram-guide" hidden role="dialog" aria-modal="false" aria-labelledby="diagram-guide-title">
        <div class="diagram-guide-head">
          <span>
            <span class="diagram-guide-eyebrow">{{i18n:viewer.guide.eyebrow}}</span>
            <strong class="diagram-guide-title" id="diagram-guide-title">{{i18n:viewer.guided.explore}}</strong>
          </span>
          <button class="diagram-guide-close" id="diagram-guide-close" type="button" aria-label="{{i18n:viewer.guide.close}}">&#215;</button>
        </div>
        <p class="diagram-guide-stats" id="diagram-guide-stats">{{i18n:viewer.guide.inspecting}}</p>
        <div class="diagram-guide-actions" id="diagram-guide-actions" role="group" aria-label="{{i18n:viewer.guide.actions}}">
          <button class="diagram-guide-action" type="button" data-guide-action="find" aria-label="{{i18n:viewer.guide.find}}">
            <span class="diagram-guide-index" aria-hidden="true">01</span>
            <span class="diagram-guide-action-copy"><strong>{{i18n:viewer.guide.find}}</strong><small>{{i18n:viewer.guide.find.hint}}</small></span>
            <kbd aria-hidden="true">/</kbd>
          </button>
          <button class="diagram-guide-action" type="button" data-guide-action="route" aria-label="{{i18n:viewer.guide.route.aria}}">
            <span class="diagram-guide-index" aria-hidden="true">02</span>
            <span class="diagram-guide-action-copy"><strong>{{i18n:viewer.guide.route}}</strong><small>{{i18n:viewer.guide.route.hint}}</small></span>
            <kbd aria-hidden="true">R</kbd>
          </button>
          <button class="diagram-guide-action" type="button" data-guide-action="map" aria-label="{{i18n:viewer.guide.map}}">
            <span class="diagram-guide-index" aria-hidden="true">03</span>
            <span class="diagram-guide-action-copy"><strong>{{i18n:viewer.guide.map}}</strong><small>{{i18n:viewer.guide.map.hint}}</small></span>
            <kbd aria-hidden="true">M</kbd>
          </button>
          <button class="diagram-guide-action" type="button" data-guide-action="lens" aria-label="{{i18n:viewer.guide.lens}}">
            <span class="diagram-guide-index" aria-hidden="true">04</span>
            <span class="diagram-guide-action-copy"><strong>{{i18n:viewer.guide.lens}}</strong><small>{{i18n:viewer.guide.lens.hint}}</small></span>
            <kbd aria-hidden="true">L</kbd>
          </button>
          <button class="diagram-guide-action" type="button" data-guide-action="story" aria-label="{{i18n:viewer.guide.story}}">
            <span class="diagram-guide-index" aria-hidden="true">05</span>
            <span class="diagram-guide-action-copy"><strong>{{i18n:viewer.guide.story}}</strong><small id="diagram-guide-story-copy">{{i18n:viewer.guide.story.hint}}</small></span>
            <kbd aria-hidden="true">P</kbd>
          </button>
          <button class="diagram-guide-action" type="button" data-guide-action="present" aria-label="{{i18n:viewer.present.enter}}">
            <span class="diagram-guide-index" aria-hidden="true">06</span>
            <span class="diagram-guide-action-copy"><strong>{{i18n:viewer.guide.present}}</strong><small>{{i18n:viewer.guide.present.hint}}</small></span>
            <kbd aria-hidden="true">F</kbd>
          </button>
        </div>
        <div class="diagram-guide-shortcuts" aria-label="{{i18n:viewer.guide.shortcuts}}">
          <span><kbd>E</kbd> {{i18n:viewer.guide.shortcut.export}}</span><span><kbd>T</kbd> {{i18n:viewer.guide.shortcut.theme}}</span><span><kbd>S</kbd> {{i18n:viewer.guide.shortcut.style}}</span><span><kbd>0</kbd> {{i18n:viewer.guide.shortcut.reset}}</span><span><kbd>+</kbd> {{i18n:viewer.guide.shortcut.zoomIn}}</span><span><kbd>-</kbd> {{i18n:viewer.guide.shortcut.zoomOut}}</span><span><kbd>Esc</kbd> {{i18n:viewer.guide.shortcut.close}}</span>
        </div>
        <p class="diagram-guide-feedback" id="diagram-guide-feedback" aria-live="polite"></p>
      </div>
      <div class="node-finder no-print" id="node-finder" hidden role="dialog" aria-modal="false" aria-labelledby="node-finder-title">
        <div class="node-finder-head">
          <strong id="node-finder-title">{{i18n:viewer.finder.title}}</strong>
          <button class="node-finder-close" id="node-finder-close" type="button" aria-label="{{i18n:viewer.finder.close}}">&#215;</button>
        </div>
        <div class="node-finder-search">
          <span aria-hidden="true">&#8981;</span>
          <input class="node-finder-input" id="node-finder-input" type="search" autocomplete="off" spellcheck="false" placeholder="{{i18n:viewer.finder.placeholder}}" aria-label="{{i18n:viewer.finder.search}}" aria-describedby="node-finder-status">
          <kbd aria-hidden="true">/</kbd>
        </div>
        <div class="node-finder-results" id="node-finder-results" role="group" aria-label="{{i18n:viewer.finder.results}}"></div>
        <p class="node-finder-empty" id="node-finder-empty" hidden>{{i18n:viewer.finder.empty}}</p>
        <p class="node-finder-status" id="node-finder-status" aria-live="polite"></p>
      </div>
      <div class="focus-chip relationship-lens no-print" id="focus-chip" hidden role="region" aria-labelledby="relationship-lens-title">
        <div class="relationship-lens-head">
          <div class="relationship-lens-copy">
            <span class="relationship-lens-eyebrow">{{i18n:viewer.passport.eyebrow}}</span>
            <strong class="relationship-lens-title" id="relationship-lens-title"><span id="focus-label"></span></strong>
            <span class="semantic-passport-detail" id="focus-detail" hidden></span>
            <div class="semantic-passport-meta" id="focus-passport-meta" aria-label="{{i18n:viewer.passport.metadata}}">
              <span id="focus-kind" data-passport="kind"></span>
              <span id="focus-context" data-passport="context" hidden></span>
              <span id="focus-tag" data-passport="tag" hidden></span>
              <span id="focus-brand" data-passport="brand" hidden></span>
              <code id="focus-id" data-passport="id"></code>
            </div>
            <div class="semantic-passport-evidence" id="focus-evidence" hidden aria-label="{{i18n:viewer.passport.evidence}}">
              <div class="semantic-passport-evidence-head">
                <span class="semantic-passport-evidence-status">{{i18n:viewer.passport.verified}}</span>
                <a class="semantic-passport-repository" id="focus-repository" target="_blank" rel="noopener noreferrer" referrerpolicy="no-referrer"></a>
              </div>
              <div class="semantic-passport-evidence-links" id="focus-evidence-links"></div>
            </div>
            <span class="relationship-lens-summary" id="focus-summary" aria-live="polite"></span>
            <div class="semantic-passport-reach" id="focus-reach" hidden>
              <span class="semantic-passport-reach-label">{{i18n:viewer.passport.reach}}</span>
              <div class="semantic-passport-reach-actions" role="group" aria-label="{{i18n:viewer.passport.reach.trace}}">
                <button id="btn-reach-upstream" type="button" aria-label="{{i18n:viewer.passport.upstream.trace}}" aria-pressed="false">
                  <span>{{i18n:viewer.passport.upstream}}</span><strong id="focus-reach-upstream-count">0</strong>
                </button>
                <button id="btn-reach-downstream" type="button" aria-label="{{i18n:viewer.passport.downstream.trace}}" aria-pressed="false">
                  <span>{{i18n:viewer.passport.downstream}}</span><strong id="focus-reach-downstream-count">0</strong>
                </button>
              </div>
              <small class="semantic-passport-reach-status" id="focus-reach-status" hidden aria-live="polite"></small>
            </div>
          </div>
          <div class="relationship-lens-actions">
            <button id="btn-focus-clear" type="button" aria-label="{{i18n:viewer.passport.close}}" title="{{i18n:viewer.common.close}}">&#215;</button>
            <button id="btn-focus-copy" type="button" aria-label="{{i18n:viewer.passport.copy.focus}}">{{i18n:viewer.passport.copy}}</button>
            <button id="btn-focus-relations" type="button" aria-label="{{i18n:viewer.passport.relations.show}}" aria-expanded="false" aria-controls="relationship-lens-list">{{i18n:viewer.passport.relations}}</button>
          </div>
        </div>
        <div class="relationship-lens-list" id="relationship-lens-list" aria-label="{{i18n:viewer.passport.relations.list}}"></div>
      </div>
      <div class="route-probe no-print" id="route-probe" hidden role="region" aria-labelledby="route-probe-title" data-state="idle">
        <div class="route-probe-head">
          <span class="route-probe-copy">
            <span class="route-probe-eyebrow">{{i18n:viewer.route.eyebrow}}</span>
            <strong class="route-probe-title" id="route-probe-title">{{i18n:viewer.route.start}}</strong>
          </span>
          <span class="route-probe-actions">
            <button class="route-probe-find" id="route-probe-find" type="button" aria-label="{{i18n:viewer.route.start.find.aria}}" data-node-finder-trigger>{{i18n:viewer.route.start.find}}</button>
            <button class="route-probe-copy-link" id="route-probe-copy" type="button" aria-label="{{i18n:viewer.route.copy.aria}}" hidden>{{i18n:viewer.route.copy}}</button>
            <button id="route-probe-clear" type="button" aria-label="{{i18n:viewer.route.clear.aria}}">{{i18n:viewer.route.clear}}</button>
          </span>
        </div>
        <div class="route-probe-path" id="route-probe-path" aria-label="{{i18n:viewer.route.traced}}">
          <span class="route-probe-placeholder">{{i18n:viewer.route.pickTwo}}</span>
        </div>
        <div class="route-journey-controls" id="route-journey-controls" hidden role="group" aria-label="{{i18n:viewer.route.controls}}" data-playing="false">
          <button id="route-journey-prev" type="button" aria-label="{{i18n:viewer.route.previous}}" title="{{i18n:viewer.route.previous}}">&#8592;</button>
          <button class="route-journey-play" id="route-journey-play" type="button" aria-label="{{i18n:viewer.route.play}}" aria-pressed="false" title="{{i18n:viewer.route.play}}">
            <span id="route-journey-play-icon" aria-hidden="true">&#9654;</span>
            <span id="route-journey-play-label">{{i18n:viewer.route.journey}}</span>
          </button>
          <button id="route-journey-next" type="button" aria-label="{{i18n:viewer.route.next}}" title="{{i18n:viewer.route.next}}">&#8594;</button>
          <button class="route-journey-overview" id="route-journey-overview" type="button" aria-label="{{i18n:viewer.route.overview.aria}}" title="{{i18n:viewer.route.overview.aria}}">{{i18n:viewer.route.overview}}</button>
        </div>
        <small class="route-probe-status" id="route-probe-status" aria-live="polite">{{i18n:viewer.route.instructions}}</small>
      </div>
      <div class="semantic-lens no-print" id="semantic-lens" hidden role="dialog" aria-modal="false" aria-labelledby="semantic-lens-title">
        <div class="semantic-lens-head">
          <span>
            <span class="semantic-lens-eyebrow">{{i18n:viewer.lens.eyebrow}}</span>
            <strong class="semantic-lens-title" id="semantic-lens-title">{{i18n:viewer.lens.title}}</strong>
          </span>
          <button class="semantic-lens-close" id="semantic-lens-close" type="button" aria-label="{{i18n:viewer.lens.close}}">&#215;</button>
        </div>
        <p class="semantic-lens-instruction">{{i18n:viewer.lens.instruction}}</p>
        <div class="semantic-lens-kinds" id="semantic-lens-kinds" role="group" aria-label="{{i18n:viewer.lens.kinds}}"></div>
        <p class="semantic-lens-status" id="semantic-lens-status" aria-live="polite">{{i18n:viewer.lens.choose}}</p>
        <div class="semantic-lens-actions">
          <button id="semantic-lens-copy" type="button" aria-label="{{i18n:viewer.lens.copy}}" hidden>{{i18n:viewer.common.copyLink}}</button>
          <button id="semantic-lens-clear" type="button" aria-label="{{i18n:viewer.lens.clear}}">{{i18n:viewer.common.clear}}</button>
        </div>
      </div>
      <div class="overview-map no-print" id="overview-map" hidden role="region" aria-labelledby="overview-map-title">
        <div class="overview-map-head">
          <span class="overview-map-live" aria-hidden="true"></span>
          <span class="overview-map-copy">
            <strong id="overview-map-title">{{i18n:viewer.radar.title}}</strong>
            <small id="overview-map-status" aria-live="polite">{{i18n:viewer.radar.building}}</small>
          </span>
          <button class="overview-map-expand" id="overview-map-expand" type="button" aria-label="{{i18n:viewer.radar.openFull}}">{{i18n:viewer.radar.open}}</button>
          <button class="overview-map-close" id="overview-map-close" type="button" aria-label="{{i18n:viewer.radar.close}}">&#215;</button>
        </div>
        <div class="overview-map-surface" id="overview-map-surface" tabindex="0" role="group" aria-label="{{i18n:viewer.radar.surface}}"></div>
        <div class="overview-map-hint" aria-hidden="true"><span>{{i18n:viewer.radar.click}}</span><span>{{i18n:viewer.radar.drag}}</span></div>
      </div>
      <p class="overview-map-feedback no-print" id="overview-map-feedback" role="status" aria-live="polite" hidden>{{i18n:viewer.radar.space}}</p>
      <div class="diagram-nav no-print" role="toolbar" aria-label="{{i18n:viewer.nav.controls}}">
        <button id="btn-route-probe" type="button" aria-label="{{i18n:viewer.nav.route}}" aria-pressed="false" aria-controls="route-probe" title="{{i18n:viewer.nav.route.title}}">{{i18n:viewer.nav.route.short}}</button>
        <button id="btn-overview-map" type="button" aria-label="{{i18n:viewer.nav.radar}}" aria-expanded="false" aria-controls="overview-map" title="{{i18n:viewer.nav.radar.title}}">{{i18n:viewer.nav.radar.short}}</button>
        <button id="btn-semantic-lens" type="button" aria-label="{{i18n:viewer.nav.lens}}" aria-haspopup="dialog" aria-expanded="false" aria-pressed="false" aria-controls="semantic-lens" title="{{i18n:viewer.nav.lens.title}}">{{i18n:viewer.nav.lens.short}}</button>
        <button id="btn-node-finder" type="button" aria-label="{{i18n:viewer.nav.find}}" aria-haspopup="dialog" aria-expanded="false" aria-controls="node-finder" title="{{i18n:viewer.nav.find.title}}" data-node-finder-trigger><span class="diagram-nav-icon find" aria-hidden="true"></span></button>
        <button id="btn-diagram-guide" type="button" aria-label="{{i18n:viewer.nav.guide}}" aria-haspopup="dialog" aria-expanded="false" aria-controls="diagram-guide" title="{{i18n:viewer.nav.guide.title}}"><span class="diagram-nav-icon guide" aria-hidden="true"></span></button>
        <button data-view="out" type="button" aria-label="{{i18n:viewer.nav.zoomOut}}" title="{{i18n:viewer.nav.zoomOut.title}}"><span class="diagram-nav-icon minus" aria-hidden="true"></span></button>
        <button data-view="reset" type="button" aria-label="{{i18n:viewer.nav.reset}}" title="{{i18n:viewer.nav.reset.title}}"><span class="diagram-nav-detail" data-view-detail hidden>{{i18n:viewer.nav.read}}</span><span class="diagram-nav-percent" data-view-percent>100%</span></button>
        <button data-view="in" type="button" aria-label="{{i18n:viewer.nav.zoomIn}}" title="{{i18n:viewer.nav.zoomIn.title}}"><span class="diagram-nav-icon plus" aria-hidden="true"></span></button>
      </div>
    </div>

    <!-- MOSOFIN:CARDS_SLOT_START -->
    <div class="cards">
      <div class="card">
        <div class="card-header">
          <div class="card-dot rose"></div>
          <h3>Card Title 1</h3>
        </div>
        <ul>
          <li>&bull; Item one</li>
          <li>&bull; Item two</li>
          <li>&bull; Item three</li>
          <li>&bull; Item four</li>
        </ul>
      </div>

      <div class="card">
        <div class="card-header">
          <div class="card-dot amber"></div>
          <h3>Card Title 2</h3>
        </div>
        <ul>
          <li>&bull; Item one</li>
          <li>&bull; Item two</li>
          <li>&bull; Item three</li>
          <li>&bull; Item four</li>
        </ul>
      </div>

      <div class="card">
        <div class="card-header">
          <div class="card-dot violet"></div>
          <h3>Card Title 3</h3>
        </div>
        <ul>
          <li>&bull; Item one</li>
          <li>&bull; Item two</li>
          <li>&bull; Item three</li>
          <li>&bull; Item four</li>
        </ul>
      </div>
    </div>
    <!-- MOSOFIN:CARDS_SLOT_END -->

  </div>

  <script>
    var Mosofin = {};
    var mosofinI18nData = (function () {
      var node = document.getElementById('mosofin-i18n-data');
      try { return JSON.parse(node ? node.textContent : '{}'); }
      catch (_) { return { locale: 'en', messages: {} }; }
    })();
    Mosofin.locale = mosofinI18nData.locale || 'en';

    function viewerText(key, values) {
      var messages = mosofinI18nData.messages || {};
      var template = Object.prototype.hasOwnProperty.call(messages, key) ? messages[key] : key;
      return String(template).replace(/\{([a-zA-Z0-9_]+)\}/g, function (match, name) {
        return values && Object.prototype.hasOwnProperty.call(values, name) ? String(values[name]) : match;
      });
    }

    function viewerCount(key, count, values) {
      var suffix = Number(count) === 1 ? '.one' : '.other';
      var payload = Object.assign({}, values || {}, { count: count });
      return viewerText(key + suffix, payload);
    }

    function viewerKindLabel(value) {
      var normalized = String(value || 'node').toLowerCase();
      var knownKey = 'viewer.kind.' + normalized;
      var known = viewerText(knownKey);
      if (known !== knownKey) return known;
      return normalized
        .replace(/messagebus/gi, 'message bus')
        .replace(/[-_]+/g, ' ')
        .replace(/\b\w/g, function (letter) { return letter.toUpperCase(); });
    }

    function hasDrawableGeometry(element) {
      if (!element) return false;
      var geometries = /^(path|line|polyline)$/i.test(element.tagName)
        ? [element]
        : Array.prototype.slice.call(element.querySelectorAll('path, line, polyline'));
      return geometries.some(function (geometry) {
        var source = geometry.tagName.toLowerCase() === 'path'
          ? geometry.getAttribute('d')
          : geometry.tagName.toLowerCase() === 'polyline'
            ? geometry.getAttribute('points')
            : [geometry.getAttribute('x1'), geometry.getAttribute('y1'), geometry.getAttribute('x2'), geometry.getAttribute('y2')].join(' ');
        if (!source || /(?:^|[^a-z])(?:nan|infinity)(?:[^a-z]|$)/i.test(source) || typeof geometry.getTotalLength !== 'function') return false;
        try {
          var length = Number(geometry.getTotalLength());
          return Number.isFinite(length) && length > 0;
        } catch (_) {
          return false;
        }
      });
    }

    /* ============================================================
       Visual style try-on — one topology, four bundled presets.
       The reader's choice is intentionally session-only: it updates the
       live page and canonical SVG together, but never rewrites the source,
       URL, or a later diagram's authored default.
       ============================================================ */
    Mosofin.preset = (function () {
      var PRESETS = ['classic', 'signal-flow', 'blueprint', 'editorial'];
      var LABELS = {
        classic: viewerText('viewer.preset.classic.short'),
        'signal-flow': viewerText('viewer.preset.flow.short'),
        blueprint: viewerText('viewer.preset.blueprint'),
        editorial: viewerText('viewer.preset.editorial')
      };
      var html = document.documentElement;
      var svg = document.querySelector('.diagram-container svg');
      var btn = document.getElementById('btn-preset');
      var label = document.getElementById('preset-label');
      var menu = document.getElementById('preset-menu');
      var options = function () {
        return Array.prototype.slice.call(menu.querySelectorAll('[data-preset-value]'));
      };
      var authored = PRESETS.indexOf(html.getAttribute('data-preset')) >= 0
        ? html.getAttribute('data-preset')
        : 'classic';

      function current() {
        var value = html.getAttribute('data-preset');
        return PRESETS.indexOf(value) >= 0 ? value : authored;
      }
      function nextAfter(preset) {
        return PRESETS[(PRESETS.indexOf(preset) + 1) % PRESETS.length];
      }
      function apply(preset) {
        if (html.getAttribute('data-embed') === 'true') return false;
        if (PRESETS.indexOf(preset) < 0) return false;
        html.setAttribute('data-preset', preset);
        svg.setAttribute('data-preset', preset);
        btn.setAttribute('data-preset-option', preset);
        label.textContent = LABELS[preset];
        btn.setAttribute('aria-label', viewerText('viewer.preset.current', { style: LABELS[preset] }));
        btn.title = viewerText('viewer.preset.choose.title');
        options().forEach(function (option) {
          var selected = option.getAttribute('data-preset-value') === preset;
          option.setAttribute('aria-checked', String(selected));
        });
        return true;
      }
      function cycle() { return apply(nextAfter(current())); }

      function isOpen() { return menu.classList.contains('open'); }
      function open(focusLast) {
        if (html.getAttribute('data-embed') === 'true') return false;
        if (Mosofin.exportMenu && Mosofin.exportMenu.isOpen()) Mosofin.exportMenu.close(false);
        menu.classList.add('open');
        btn.setAttribute('aria-expanded', 'true');
        var available = options();
        var selected = available.find(function (option) { return option.getAttribute('aria-checked') === 'true'; });
        var target = focusLast ? available[available.length - 1] : (selected || available[0]);
        if (target) target.focus();
        return true;
      }
      function close(focusTrigger) {
        menu.classList.remove('open');
        btn.setAttribute('aria-expanded', 'false');
        if (focusTrigger) btn.focus();
        return false;
      }

      apply(authored);
      btn.addEventListener('click', function (e) {
        e.stopPropagation();
        if (isOpen()) close(false);
        else open(false);
      });
      btn.addEventListener('keydown', function (e) {
        if (e.key === 'ArrowDown' || e.key === 'ArrowUp') {
          e.preventDefault();
          if (!isOpen()) open(e.key === 'ArrowUp');
        }
      });
      document.addEventListener('click', function (e) {
        if (!menu.contains(e.target) && e.target !== btn) close(false);
      });
      menu.addEventListener('click', function (e) {
        var option = e.target.closest('[data-preset-value]');
        if (!option || !menu.contains(option)) return;
        if (apply(option.getAttribute('data-preset-value'))) close(true);
      });
      menu.addEventListener('keydown', function (e) {
        if (e.key === 'Escape') { e.preventDefault(); close(true); return; }
        if (e.key === 'Tab') { close(false); return; }
        var available = options();
        var active = available.indexOf(document.activeElement);
        switch (e.key) {
          case 'ArrowDown':
            e.preventDefault();
            if (available.length) available[(active + 1 + available.length) % available.length].focus();
            break;
          case 'ArrowUp':
            e.preventDefault();
            if (available.length) available[(active - 1 + available.length) % available.length].focus();
            break;
          case 'Home':
            e.preventDefault();
            if (available[0]) available[0].focus();
            break;
          case 'End':
            e.preventDefault();
            if (available.length) available[available.length - 1].focus();
            break;
        }
      });
      return { cycle: cycle, apply: apply, current: current, authored: authored, open: open, close: close, isOpen: isOpen };
    })();

    /* ============================================================
       Theme toggle — persists to localStorage, respects system pref
       ============================================================ */

    Mosofin.nodeStyle = (function () {
      var storedName = 'mosofin-node-style';
      var html = document.documentElement;
      var btn = document.getElementById('btn-nodestyle');
      var label = document.getElementById('nodestyle-label');
      var authored = html.getAttribute('data-node-style') === 'logo' ? 'logo' : 'box';

      function readStored() {
        try { return localStorage.getItem(storedName); } catch (_) { return null; }
      }
      function writeStored(value) {
        try { localStorage.setItem(storedName, value); } catch (_) {}
      }
      function urlOverride() {
        try {
          var param = new URLSearchParams(window.location.search).get('nodeStyle');
          if (param === 'box' || param === 'logo') return param;
        } catch (_) {}
        return null;
      }

      function apply(style) {
        html.setAttribute('data-node-style', style);
        label.textContent = viewerText(style === 'logo' ? 'viewer.nodestyle.logo' : 'viewer.nodestyle.box');
        btn.setAttribute('aria-pressed', style === 'logo' ? 'true' : 'false');
      }

      function toggle() {
        var next = html.getAttribute('data-node-style') === 'logo' ? 'box' : 'logo';
        apply(next);
        writeStored(next);
      }

      // The authored default wins unless the reader or a share link says otherwise.
      var saved = readStored();
      apply(urlOverride() || (saved === 'box' || saved === 'logo' ? saved : authored));
      btn.addEventListener('click', toggle);
      return { apply: apply, toggle: toggle, current: function () { return html.getAttribute('data-node-style'); } };
    })();

    Mosofin.theme = (function () {
      var storedName = 'mosofin-theme';
      var html = document.documentElement;
      var btn = document.getElementById('btn-theme');
      var label = document.getElementById('theme-label');

      // localStorage can throw (blocked cookies, sandboxed iframes); the whole
      // script element dies on an uncaught error, so guard every access.
      function readStored() {
        try { return localStorage.getItem(storedName); } catch (_) { return null; }
      }
      function writeStored(value) {
        try { localStorage.setItem(storedName, value); } catch (_) {}
      }
      function urlOverride() {
        // URL param wins (useful for deterministic screenshots / share links)
        try {
          var param = new URLSearchParams(window.location.search).get('theme');
          if (param === 'light' || param === 'dark') return param;
        } catch (_) {}
        return null;
      }

      function resolveInitial() {
        var fromUrl = urlOverride();
        if (fromUrl) return fromUrl;
        var saved = readStored();
        if (saved === 'light' || saved === 'dark') return saved;
        return window.matchMedia('(prefers-color-scheme: light)').matches ? 'light' : 'dark';
      }

      function apply(theme) {
        html.setAttribute('data-theme', theme);
        label.textContent = viewerText(theme === 'dark' ? 'viewer.theme.dark' : 'viewer.theme.light');
        btn.setAttribute('aria-pressed', theme === 'light' ? 'true' : 'false');
      }

      function toggle() {
