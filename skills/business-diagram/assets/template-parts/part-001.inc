        var next = html.getAttribute('data-theme') === 'dark' ? 'light' : 'dark';
        apply(next);
        writeStored(next);
      }

      apply(resolveInitial());
      btn.addEventListener('click', toggle);

      // Follow live OS theme changes while the user has no explicit preference.
      try {
        var media = window.matchMedia('(prefers-color-scheme: light)');
        var onChange = function (e) {
          var saved = readStored();
          if (urlOverride() || saved === 'light' || saved === 'dark') return;
          apply(e.matches ? 'light' : 'dark');
        };
        if (media.addEventListener) media.addEventListener('change', onChange);
        else if (media.addListener) media.addListener(onChange);
      } catch (_) {}

      return { toggle: toggle };
    })();

    /* ============================================================
       Export — Share Card / PNG / JPEG / WebP / SVG / WebM and clipboard

       Raster exports are always rendered at 4x source resolution for
       maximum sharpness. The trick: we set the serialized SVG's
       `width`/`height` to viewBox * 4 so the browser rasterizes the
       vectors at that resolution natively. drawImage then draws at
       the image's natural size (no upscaling = no blur).

       JPEG/WebP paint the current theme's background explicitly since
       those formats have no alpha channel.
       ============================================================ */
    (function () {
      var RASTER_SCALE = 4;
      var MOTION_DURATION = 6000;
      var MOTION_FPS = 30;
      var SHARE_CARD_WIDTH = 1200;
      var SHARE_CARD_HEIGHT = 630;
      var SHARE_CARD_PADDING = 36;
      var SHARE_CARD_HEADER = 112;

      function exportError(key, values) {
        var error = new Error(viewerText(key, values));
        error.mosofinViewerMessage = true;
        return error;
      }

      function exportMessage(error) {
        return error && error.mosofinViewerMessage
          ? error.message
          : viewerText('viewer.export.unknown');
      }

      function diagramFilename() {
        var title = (document.title || 'diagram')
          .replace(/\s+Architecture(\s+Diagram)?$/i, '')
          .replace(/\s+Diagram$/i, '')
          .trim();
        return title.replace(/[^a-z0-9_\-]+/gi, '-')
                    .toLowerCase()
                    .replace(/^-+|-+$/g, '') || 'diagram';
      }

      function currentBg() {
        return getComputedStyle(document.body).backgroundColor || '#ffffff';
      }

      /**
       * Serialize the main SVG into a standalone document.
       *
       * Two modes:
       * - Default (opts.autoTheme=false) — locks the serialized SVG to the
       *   viewer's current theme. Used by the raster pipeline
       *   (PNG/JPEG/WebP/clipboard) because canvas rasterization needs
       *   deterministic colors; a raster cannot react to
       *   prefers-color-scheme after encoding.
       * - autoTheme=true — emits BOTH dark and light variable sets plus a
       *   `@media (prefers-color-scheme)` rule so the resulting SVG
       *   self-themes when embedded in GitHub READMEs or other hosts that
       *   expose a color scheme. Used for "Download SVG".
       */
      function applyRouteSnapshot(clone, snapshot) {
        if (!snapshot || !Array.isArray(snapshot.nodeIds) || !Array.isArray(snapshot.edges) ||
            snapshot.nodeIds.length < 2 || snapshot.edges.length !== snapshot.nodeIds.length - 1 ||
            snapshot.hops !== snapshot.edges.length ||
            !snapshot.source || !snapshot.target ||
            snapshot.source.id !== snapshot.nodeIds[0] ||
            snapshot.target.id !== snapshot.nodeIds[snapshot.nodeIds.length - 1]) return false;

        clone.removeAttribute('data-animation');
        Array.prototype.forEach.call(clone.querySelectorAll('[data-animate]'), function (element) {
          element.removeAttribute('data-animate');
          element.style.removeProperty('--step');
        });

        var cloneNodes = Array.prototype.slice.call(clone.querySelectorAll('[data-node-id]'));
        var cloneEdges = Array.prototype.slice.call(clone.querySelectorAll('[data-edge-key]'));
        var nodeIds = Object.create(null);
        var edgeKeys = Object.create(null);
        var nodeMatches = [];
        var edgeMatches = [];

        for (var nodeIndex = 0; nodeIndex < snapshot.nodeIds.length; nodeIndex++) {
          var nodeId = snapshot.nodeIds[nodeIndex];
          if (typeof nodeId !== 'string' || !nodeId || nodeIds[nodeId]) return false;
          var matchedNodes = cloneNodes.filter(function (candidate) {
            return candidate.getAttribute('data-node-id') === nodeId;
          });
          if (matchedNodes.length !== 1) return false;
          nodeIds[nodeId] = true;
          nodeMatches.push(matchedNodes);
        }

        for (var edgeIndex = 0; edgeIndex < snapshot.edges.length; edgeIndex++) {
          var edge = snapshot.edges[edgeIndex];
          var fromId = snapshot.nodeIds[edgeIndex];
          var toId = snapshot.nodeIds[edgeIndex + 1];
          if (!edge || typeof edge.key !== 'string' || !edge.key || edgeKeys[edge.key] ||
              edge.from !== fromId || edge.to !== toId) return false;
          var matchedEdges = cloneEdges.filter(function (candidate) {
            return candidate.getAttribute('data-edge-key') === edge.key;
          });
          var drawableMatches = matchedEdges.filter(hasDrawableGeometry);
          if (!matchedEdges.length || drawableMatches.length !== 1 || !matchedEdges.every(function (candidate) {
            return candidate.getAttribute('data-edge-from') === edge.from &&
              candidate.getAttribute('data-edge-to') === edge.to &&
              (candidate.getAttribute('data-edge-id') || '') === (edge.id || '');
          })) return false;
          edgeKeys[edge.key] = true;
          edgeMatches.push(matchedEdges);
        }

        clone.setAttribute('data-share-route', '');
        nodeMatches.forEach(function (matches, step) {
          matches.forEach(function (element) {
            element.setAttribute('data-share-route-match', '');
            element.setAttribute('data-share-route-step', String(step));
            if (step === 0) element.setAttribute('data-share-route-start', '');
            else if (step === snapshot.nodeIds.length - 1) element.setAttribute('data-share-route-end', '');
            else element.setAttribute('data-share-route-middle', '');
          });
        });
        edgeMatches.forEach(function (matches, step) {
          matches.forEach(function (element) {
            element.setAttribute('data-share-route-match', '');
            element.setAttribute('data-share-route-step', String(step));
          });
        });

        return clone.hasAttribute('data-share-route') &&
          !clone.hasAttribute('data-animation') &&
          !clone.hasAttribute('data-route-active') &&
          !clone.hasAttribute('data-route-journey') &&
          clone.querySelectorAll('[data-animate], [data-route-match], [data-route-step], [data-route-start], [data-route-end], [data-route-journey-state], [data-route-journey-current], [data-route-journey-overlay]').length === 0 &&
          clone.querySelectorAll('[data-share-route-match]').length ===
            nodeMatches.reduce(function (count, matches) { return count + matches.length; }, 0) +
            edgeMatches.reduce(function (count, matches) { return count + matches.length; }, 0);
      }

      function applyReachSnapshot(clone, snapshot) {
        if (!snapshot || (snapshot.direction !== 'upstream' && snapshot.direction !== 'downstream') ||
            !snapshot.origin || typeof snapshot.origin.id !== 'string' || !snapshot.origin.id ||
            typeof snapshot.origin.label !== 'string' || !snapshot.origin.label.trim() ||
            !Array.isArray(snapshot.nodeIds) || snapshot.nodeIds.length < 2 ||
            !Array.isArray(snapshot.edges) || !snapshot.edges.length ||
            !snapshot.depths || typeof snapshot.depths !== 'object' ||
            !Number.isInteger(snapshot.maxDepth) || snapshot.maxDepth < 1 ||
            snapshot.nodeIds[0] !== snapshot.origin.id) return false;

        clone.removeAttribute('data-animation');
        Array.prototype.forEach.call(clone.querySelectorAll('[data-animate]'), function (element) {
          element.removeAttribute('data-animate');
          element.style.removeProperty('--step');
        });

        var cloneNodes = Array.prototype.slice.call(clone.querySelectorAll('[data-node-id]'));
        var cloneEdges = Array.prototype.slice.call(clone.querySelectorAll('[data-edge-key]'));
        var nodeIds = Object.create(null);
        var edgeKeys = Object.create(null);
        var nodeMatches = [];
        var edgeMatches = [];
        var measuredMaxDepth = 0;

        for (var nodeIndex = 0; nodeIndex < snapshot.nodeIds.length; nodeIndex++) {
          var nodeId = snapshot.nodeIds[nodeIndex];
          var depth = snapshot.depths[nodeId];
          if (typeof nodeId !== 'string' || !nodeId || nodeIds[nodeId] ||
              !Number.isInteger(depth) || depth < 0 || depth > snapshot.maxDepth ||
              (nodeId === snapshot.origin.id ? depth !== 0 : depth < 1)) return false;
          var matchedNodes = cloneNodes.filter(function (candidate) {
            return candidate.getAttribute('data-node-id') === nodeId;
          });
          if (matchedNodes.length !== 1) return false;
          nodeIds[nodeId] = true;
          measuredMaxDepth = Math.max(measuredMaxDepth, depth);
          nodeMatches.push({ elements: matchedNodes, id: nodeId, depth: depth });
        }
        if (measuredMaxDepth !== snapshot.maxDepth) return false;

        for (var edgeIndex = 0; edgeIndex < snapshot.edges.length; edgeIndex++) {
          var edge = snapshot.edges[edgeIndex];
          if (!edge || typeof edge.key !== 'string' || !edge.key || edgeKeys[edge.key] ||
              !nodeIds[edge.from] || !nodeIds[edge.to] ||
              !Number.isInteger(edge.depth) || edge.depth < 1 || edge.depth > snapshot.maxDepth ||
              edge.depth !== Math.max(snapshot.depths[edge.from], snapshot.depths[edge.to])) return false;
          var matchedEdges = cloneEdges.filter(function (candidate) {
            return candidate.getAttribute('data-edge-key') === edge.key;
          });
          var drawableMatches = matchedEdges.filter(hasDrawableGeometry);
          if (!matchedEdges.length || drawableMatches.length !== 1 || !matchedEdges.every(function (candidate) {
            return candidate.getAttribute('data-edge-from') === edge.from &&
              candidate.getAttribute('data-edge-to') === edge.to &&
              (candidate.getAttribute('data-edge-id') || '') === (edge.id || '');
          })) return false;
          edgeKeys[edge.key] = true;
          edgeMatches.push({ elements: matchedEdges, depth: edge.depth });
        }

        clone.setAttribute('data-share-reach', snapshot.direction);
        nodeMatches.forEach(function (match) {
          match.elements.forEach(function (element) {
            element.setAttribute('data-share-reach-match', '');
            element.setAttribute('data-share-reach-depth', String(match.depth));
            if (match.id === snapshot.origin.id) element.setAttribute('data-share-reach-origin', '');
          });
        });
        edgeMatches.forEach(function (match) {
          match.elements.forEach(function (element) {
            element.setAttribute('data-share-reach-match', '');
            element.setAttribute('data-share-reach-depth', String(match.depth));
          });
        });

        return clone.getAttribute('data-share-reach') === snapshot.direction &&
          !clone.hasAttribute('data-animation') &&
          !clone.hasAttribute('data-reach-active') &&
          clone.querySelectorAll('[data-animate], [data-reach-match], [data-reach-origin], [data-reach-depth]').length === 0 &&
          clone.querySelectorAll('[data-share-reach-origin]').length === 1 &&
          clone.querySelectorAll('[data-share-reach-match]').length ===
            nodeMatches.reduce(function (count, match) { return count + match.elements.length; }, 0) +
            edgeMatches.reduce(function (count, match) { return count + match.elements.length; }, 0);
      }

      function serializeSvg(scale, opts) {
        // scale: integer multiplier for intrinsic SVG pixel dimensions used by
        // the raster path. Defaults to 1 (natural size) for SVG download.
        scale = scale || 1;
        opts = opts || {};
        var autoTheme = opts.autoTheme === true;
        var svg = document.querySelector('.diagram-container svg');
        var clone = svg.cloneNode(true);

        // View transforms and neighborhood focus are HTML exploration state,
        // never part of a downloaded full-diagram artifact.
        clone.style.removeProperty('transform');
        clone.removeAttribute('data-view-scale');
        clone.removeAttribute('data-focus-active');
        clone.removeAttribute('data-reach-active');
        clone.removeAttribute('data-lens-active');
        clone.removeAttribute('data-lens-flow-count');
        clone.removeAttribute('data-lens-flow-density');
        clone.removeAttribute('data-legend-preview-active');
        clone.removeAttribute('data-relationship-preview-active');
        clone.removeAttribute('data-relationship-direct-active');
        clone.removeAttribute('data-relationship-pin-active');
        clone.removeAttribute('data-intent-trace-active');
        clone.removeAttribute('data-route-picking');
        clone.removeAttribute('data-route-active');
        clone.removeAttribute('data-route-journey');
        clone.removeAttribute('data-share-route');
        clone.removeAttribute('data-share-reach');
        clone.removeAttribute('data-story-active');
        clone.removeAttribute('data-story-playing');
        clone.removeAttribute('data-story-beat');
        clone.removeAttribute('data-story-next');
        clone.removeAttribute('data-story-follow');
        clone.removeAttribute('data-chapter-handoff');
        clone.removeAttribute('data-chapter-anchor');
        clone.removeAttribute('data-chapter-preview');
        Array.prototype.forEach.call(clone.querySelectorAll('[data-chapter-handoff-overlay]'), function (el) {
          el.remove();
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-chapter-role]'), function (el) {
          el.removeAttribute('data-chapter-role');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-chapter-preview-role]'), function (el) {
          el.removeAttribute('data-chapter-preview-role');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-story-overlay], [data-story-carrier-overlay]'), function (el) {
          el.remove();
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-intent-trace-overlay]'), function (el) {
          el.remove();
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-route-probe-overlay]'), function (el) {
          el.remove();
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-route-journey-overlay]'), function (el) {
          el.remove();
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-semantic-lens-overlay]'), function (el) {
          el.remove();
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-legend-bridge-runtime]'), function (el) {
          el.remove();
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-legend-kind]'), function (el) {
          el.removeAttribute('data-legend-kind');
          el.removeAttribute('data-legend-label');
          el.removeAttribute('data-legend-count');
          el.removeAttribute('data-legend-zero');
          el.removeAttribute('data-legend-selected');
          el.removeAttribute('role');
          el.removeAttribute('tabindex');
          el.removeAttribute('aria-label');
          el.removeAttribute('aria-pressed');
          el.removeAttribute('aria-haspopup');
          el.removeAttribute('aria-controls');
          el.removeAttribute('aria-expanded');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-legend-bridge]'), function (el) {
          el.removeAttribute('data-legend-bridge');
          el.removeAttribute('role');
          el.removeAttribute('aria-label');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-relationship-pulse-overlay]'), function (el) {
          el.remove();
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-relationship-hit-overlay]'), function (el) {
          el.remove();
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-source-evidence-beacon]'), function (el) {
          el.remove();
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-source-evidence-count]'), function (el) {
          var originalLabel = el.getAttribute('data-source-evidence-original-label');
          if (originalLabel == null || originalLabel === '') el.removeAttribute('aria-label');
          else el.setAttribute('aria-label', originalLabel);
          el.removeAttribute('data-source-evidence-count');
          el.removeAttribute('data-source-evidence-original-label');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-story-step], [data-story-beat-state], [data-story-beat-step]'), function (el) {
          el.removeAttribute('data-story-step');
          el.removeAttribute('data-story-beat-state');
          el.removeAttribute('data-story-beat-step');
          el.style.removeProperty('--story-step');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-focus-match], [data-focus-selected]'), function (el) {
          el.removeAttribute('data-focus-match');
          el.removeAttribute('data-focus-selected');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-reach-match], [data-reach-origin], [data-reach-depth]'), function (el) {
          el.removeAttribute('data-reach-match');
          el.removeAttribute('data-reach-origin');
          el.removeAttribute('data-reach-depth');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-lens-match], [data-lens-selected], [data-lens-peer]'), function (el) {
          el.removeAttribute('data-lens-match');
          el.removeAttribute('data-lens-selected');
          el.removeAttribute('data-lens-peer');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-legend-preview-match], [data-legend-preview-selected], [data-legend-preview-peer]'), function (el) {
          el.removeAttribute('data-legend-preview-match');
          el.removeAttribute('data-legend-preview-selected');
          el.removeAttribute('data-legend-preview-peer');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-relationship-preview], [data-relationship-preview-node], [data-relationship-preview-source], [data-relationship-preview-target]'), function (el) {
          el.removeAttribute('data-relationship-preview');
          el.removeAttribute('data-relationship-preview-node');
          el.removeAttribute('data-relationship-preview-source');
          el.removeAttribute('data-relationship-preview-target');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-intent-trace-match], [data-intent-trace-selected]'), function (el) {
          el.removeAttribute('data-intent-trace-match');
          el.removeAttribute('data-intent-trace-selected');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-route-match], [data-route-start], [data-route-end], [data-route-step], [data-route-candidate], [data-route-journey-state], [data-route-journey-current]'), function (el) {
          el.removeAttribute('data-route-match');
          el.removeAttribute('data-route-start');
          el.removeAttribute('data-route-end');
          el.removeAttribute('data-route-step');
          el.removeAttribute('data-route-candidate');
          el.removeAttribute('data-route-journey-state');
          el.removeAttribute('data-route-journey-current');
          el.style.removeProperty('--route-step');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-share-route-match], [data-share-route-step], [data-share-route-start], [data-share-route-end], [data-share-route-middle]'), function (el) {
          el.removeAttribute('data-share-route-match');
          el.removeAttribute('data-share-route-step');
          el.removeAttribute('data-share-route-start');
          el.removeAttribute('data-share-route-end');
          el.removeAttribute('data-share-route-middle');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-share-reach-match], [data-share-reach-origin], [data-share-reach-depth]'), function (el) {
          el.removeAttribute('data-share-reach-match');
          el.removeAttribute('data-share-reach-origin');
          el.removeAttribute('data-share-reach-depth');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-detail], [data-detail-anchor]'), function (el) {
          el.removeAttribute('data-detail');
          el.removeAttribute('data-detail-anchor');
        });
        Array.prototype.forEach.call(clone.querySelectorAll('[data-node-id][aria-pressed]'), function (el) {
          el.setAttribute('aria-pressed', 'false');
        });
        var canonicalStateClean = !clone.hasAttribute('data-view-scale') &&
          !clone.hasAttribute('data-focus-active') &&
          !clone.hasAttribute('data-reach-active') &&
          !clone.hasAttribute('data-lens-active') &&
          !clone.hasAttribute('data-lens-flow-count') &&
          !clone.hasAttribute('data-lens-flow-density') &&
          !clone.hasAttribute('data-legend-preview-active') &&
          !clone.hasAttribute('data-relationship-preview-active') &&
          !clone.hasAttribute('data-relationship-direct-active') &&
          !clone.hasAttribute('data-intent-trace-active') &&
          !clone.hasAttribute('data-route-picking') &&
          !clone.hasAttribute('data-route-active') &&
          !clone.hasAttribute('data-route-journey') &&
          !clone.hasAttribute('data-share-route') &&
          !clone.hasAttribute('data-share-reach') &&
          !clone.hasAttribute('data-story-active') &&
          !clone.hasAttribute('data-story-playing') &&
          !clone.hasAttribute('data-story-beat') &&
          !clone.hasAttribute('data-story-next') &&
          !clone.hasAttribute('data-story-follow') &&
          !clone.hasAttribute('data-chapter-handoff') &&
          !clone.hasAttribute('data-chapter-anchor') &&
          !clone.hasAttribute('data-chapter-preview') &&
          !clone.style.getPropertyValue('transform') &&
          clone.querySelectorAll('[data-story-overlay], [data-story-carrier-overlay], [data-story-carrier-token], [data-story-step], [data-story-beat-state], [data-story-beat-step], [data-chapter-handoff-overlay], [data-chapter-role], [data-chapter-preview-role], [data-focus-match], [data-focus-selected], [data-reach-match], [data-reach-origin], [data-reach-depth], [data-semantic-lens-overlay], [data-lens-match], [data-lens-selected], [data-lens-peer], [data-legend-bridge], [data-legend-kind], [data-legend-bridge-runtime], [data-legend-count], [data-legend-zero], [data-legend-selected], [data-legend-preview-match], [data-legend-preview-selected], [data-legend-preview-peer], [data-relationship-hit-overlay], [data-relationship-pulse-overlay], [data-relationship-preview], [data-relationship-preview-node], [data-relationship-preview-source], [data-relationship-preview-target], [data-intent-trace-overlay], [data-intent-trace-match], [data-intent-trace-selected], [data-route-probe-overlay], [data-route-journey-overlay], [data-route-match], [data-route-start], [data-route-end], [data-route-step], [data-route-candidate], [data-route-journey-state], [data-route-journey-current], [data-share-route-match], [data-share-route-step], [data-share-route-start], [data-share-route-end], [data-share-route-middle], [data-share-reach-match], [data-share-reach-origin], [data-share-reach-depth], [data-source-evidence-beacon], [data-source-evidence-count], [data-source-evidence-original-label], [data-detail], [data-detail-anchor]').length === 0;

        var vb = svg.viewBox.baseVal;
        var finiteSvgDimensions = Number.isFinite(vb.x) && Number.isFinite(vb.y) &&
          Number.isFinite(vb.width) && Number.isFinite(vb.height) && vb.width > 0 && vb.height > 0;
        var routeStateClean = opts.routeSnapshot
          ? canonicalStateClean && finiteSvgDimensions && applyRouteSnapshot(clone, opts.routeSnapshot)
          : null;
        var reachStateClean = opts.reachSnapshot
          ? canonicalStateClean && finiteSvgDimensions && !opts.routeSnapshot && applyReachSnapshot(clone, opts.reachSnapshot)
          : null;
        // Scale width/height so the browser rasterizes the vectors at target
        // resolution directly. viewBox stays unchanged so coordinates don't
        // shift. This is the key to sharp rasters — do NOT scale later via
        // canvas upscaling.
        clone.setAttribute('width', vb.width * scale);
        clone.setAttribute('height', vb.height * scale);
        clone.setAttribute('xmlns', 'http://www.w3.org/2000/svg');

        // Only the SVG-relevant rules: semantic classes, markers, and the
        // theme variable blocks. Toolbar/cards/print CSS can never apply
        // inside a standalone SVG and would only bloat the export.
        var hostStyle = (function () {
          var out = [];
          Array.prototype.forEach.call(document.styleSheets, function (sheet) {
            var rules;
            try { rules = sheet.cssRules; } catch (_) { return; } // cross-origin
            if (!rules) return;
            Array.prototype.forEach.call(rules, function (rule) {
              if (rule.type === 7 && /^mosofin-/.test(rule.name || '')) {
                out.push(rule.cssText);
                return;
              }
              if (rule.type !== 1) return; // plain style rules only
              var sel = rule.selectorText || '';
              if (/(^|,)\s*(svg|:root|\[data-theme|\[data-preset|\.c-|\.t-|\.a-|\.m-)/.test(sel)) {
                out.push(rule.cssText);
              }
            });
          });
          return out.join('\n');
        })();

        // Derive the variable list from the stylesheet so newly added theme
        // variables can never be missed by the export pipeline again
        // (--lane-fill/--lane-stroke once were).
        var varNames = (function () {
          var seen = {};
          var names = [];
          (hostStyle.match(/--[a-zA-Z0-9-]+(?=\s*:)/g) || []).forEach(function (n) {
            if (!seen[n]) { seen[n] = true; names.push(n); }
          });
          return names;
        })();

        // Resolve the full variable set for a given data-theme via an
        // off-DOM probe, independent of what the viewer is currently set to.
        function resolveVars(themeAttr) {
          var probe = document.createElement('div');
          probe.setAttribute('data-theme', themeAttr);
          probe.setAttribute('data-preset', document.documentElement.getAttribute('data-preset') || 'classic');
          probe.style.cssText = 'position:absolute;width:0;height:0;visibility:hidden;';
          document.body.appendChild(probe);
          try {
            var c = getComputedStyle(probe);
            return varNames.map(function (n) {
              return n + ': ' + c.getPropertyValue(n).trim() + ';';
            }).join(' ');
          } finally {
            document.body.removeChild(probe);
          }
        }

        // Prepend a local()-only @font-face block for each weight so that the
        // sandboxed image-rendering context used during PNG/JPEG/WebP export
        // can find JetBrains Mono if the user has it installed locally, and
        // falls through cleanly to the monospace stack otherwise. The usual
        // Google-Fonts <link> in the host document is unreachable from the
        // serialized SVG, so url()-sourced faces would just hang.
        var fontFallback = [400, 500, 600, 700].map(function (w) {
          return "@font-face { font-family: 'JetBrains Mono'; font-weight: " + w +
                 "; src: local('JetBrains Mono'), local('JetBrainsMono-Regular'); }";
        }).join('\n');

        var style = document.createElementNS('http://www.w3.org/2000/svg', 'style');
        var bgRect = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
        bgRect.setAttribute('width', '100%');
        bgRect.setAttribute('height', '100%');

        if (autoTheme) {
          // Dual-theme SVG. Dark is the default (so hosts without
          // prefers-color-scheme still render), light swaps in via media
          // query, and svg[data-theme="..."] still lets downstream
          // consumers force a specific theme.
          var darkVars = resolveVars('dark');
          var lightVars = resolveVars('light');

          style.textContent =
            fontFallback + "\n" +
            "svg { font-family: 'JetBrains Mono', ui-monospace, SFMono-Regular, Menlo, Consolas, 'DejaVu Sans Mono', 'Liberation Mono', 'Noto Sans Mono CJK SC', 'PingFang SC', 'Hiragino Sans GB', 'Microsoft YaHei', monospace; }\n" +
            hostStyle + "\n" +
            ":root, svg { " + darkVars + " }\n" +
            "@media (prefers-color-scheme: light) { :root, svg { " + lightVars + " } }\n" +
            "svg[data-theme=\"light\"] { " + lightVars + " }\n" +
            "svg[data-theme=\"dark\"] { " + darkVars + " }\n" +
            "rect.c-bg-rect { fill: var(--bg); }\n";

          // Don't lock the serialized SVG to the viewer's current theme.
          clone.removeAttribute('data-theme');
          // Background follows the CSS variable, not a fixed color, so it
          // swaps with the media query.
          bgRect.setAttribute('class', 'c-bg-rect');
        } else {
          // Raster path: lock to the viewer's current theme.
          var theme = document.documentElement.getAttribute('data-theme') || 'dark';
          var themeHost = document.querySelector('[data-theme="' + theme + '"]') || document.documentElement;
          var computed = getComputedStyle(themeHost);
          var vars = varNames.map(function (n) {
            return n + ': ' + computed.getPropertyValue(n).trim() + ';';
          }).join(' ');

          // IMPORTANT: inject the resolved variables AFTER hostStyle,
          // otherwise hostStyle's ":root, [data-theme=\"dark\"] { ... }" rule
          // overrides our chosen theme via later-in-cascade equal-specificity.
          // Keep this order.
          style.textContent =
            fontFallback + "\n" +
            "svg { font-family: 'JetBrains Mono', ui-monospace, SFMono-Regular, Menlo, Consolas, 'DejaVu Sans Mono', 'Liberation Mono', 'Noto Sans Mono CJK SC', 'PingFang SC', 'Hiragino Sans GB', 'Microsoft YaHei', monospace; }\n" +
            hostStyle + "\n" +
            ":root, svg { " + vars + " }\n";

          bgRect.setAttribute('fill', computed.getPropertyValue('--bg').trim() || '#ffffff');
        }

        if (opts.routeSnapshot) {
          style.textContent += "\nsvg[data-share-route] [data-node-id], svg[data-share-route] [data-edge-from] { opacity: 0.18; }\n" +
            "svg[data-share-route] [data-share-route-match] { opacity: 1; }\n" +
            "svg[data-share-route] [data-share-route-start] > :is(rect, circle, polygon):not(.c-mask) { stroke-width: 3; stroke-dasharray: 5 3; }\n" +
            "svg[data-share-route] [data-share-route-middle] > :is(rect, circle, polygon):not(.c-mask) { stroke-width: 2.2; }\n" +
            "svg[data-share-route] [data-share-route-end] > :is(rect, circle, polygon):not(.c-mask) { stroke-width: 3.4; stroke-dasharray: 1 0; }\n";
        }
        if (opts.reachSnapshot) {
          style.textContent += "\nsvg[data-share-reach] [data-node-id], svg[data-share-reach] [data-edge-from] { opacity: 0.14; }\n" +
            "svg[data-share-reach] [data-share-reach-match] { opacity: 1; }\n" +
            "svg[data-share-reach] [data-edge-from][data-share-reach-match] { stroke-width: 1.55; }\n" +
            "svg[data-share-reach=\"upstream\"] [data-share-reach-origin] > :is(rect, circle, polygon):not(.c-mask) { stroke: var(--database-stroke); stroke-width: 3.4; stroke-dasharray: 5 3; }\n" +
            "svg[data-share-reach=\"downstream\"] [data-share-reach-origin] > :is(rect, circle, polygon):not(.c-mask) { stroke: var(--backend-stroke); stroke-width: 3.4; stroke-dasharray: 1 0; }\n" +
            "svg[data-preset=\"blueprint\"][data-share-reach] [data-share-reach-origin], svg[data-preset=\"blueprint\"][data-share-reach] [data-edge-from][data-share-reach-match] { filter: none; }\n";
        }

        clone.insertBefore(style, clone.firstChild);
        clone.insertBefore(bgRect, style.nextSibling);

        return {
          svgString: new XMLSerializer().serializeToString(clone),
          width: vb.width * scale,
          height: vb.height * scale,
          canonicalStateClean: canonicalStateClean,
          routeStateClean: routeStateClean,
          reachStateClean: reachStateClean
        };
      }

      function download(blob, filename) {
        var url = URL.createObjectURL(blob);
        var a = document.createElement('a');
        a.href = url;
        a.download = filename;
        document.body.appendChild(a);
        a.click();
        a.remove();
        setTimeout(function () { URL.revokeObjectURL(url); }, 1000);
      }

      // Conservative upper bound on total canvas pixels. Older Safari on iOS
      // silently produces a blank canvas above ~16 Mpx; Chrome / Firefox /
      // desktop Safari are far higher but start failing on memory-constrained
      // devices. We pick the largest integer scale in {4,3,2,1} whose target
      // pixel count fits under this cap.
      var MAX_CANVAS_PIXELS = 16 * 1024 * 1024;

      function pickSafeScale(vbW, vbH) {
        for (var s = RASTER_SCALE; s >= 1; s--) {
          if (vbW * s * vbH * s <= MAX_CANVAS_PIXELS) return s;
        }
        return 1;
      }

      function rasterize(format) {
        // Serialize at a safe scale (RASTER_SCALE=4 by default, reduced if the
        // resulting canvas would exceed MAX_CANVAS_PIXELS). The SVG itself is
        // rasterized at target resolution natively; drawImage draws at natural
        // size — no upsampling blur.
        var svg = document.querySelector('.diagram-container svg');
        var vb = svg.viewBox.baseVal;
        var scale = pickSafeScale(vb.width, vb.height);
        var data = serializeSvg(scale);
        var svgBlob = new Blob([data.svgString], { type: 'image/svg+xml;charset=utf-8' });
        var svgUrl = URL.createObjectURL(svgBlob);

        return new Promise(function (resolve, reject) {
          var img = new Image();
          img.onload = function () {
            try {
              var canvas = document.createElement('canvas');
              canvas.width = data.width;
              canvas.height = data.height;
              var ctx = canvas2dOrThrow(canvas, format);
              if (format === 'jpeg') {
                ctx.fillStyle = currentBg();
                ctx.fillRect(0, 0, canvas.width, canvas.height);
              }
              // Draw at natural size — SVG was already rasterized at target res.
              ctx.drawImage(img, 0, 0);
              URL.revokeObjectURL(svgUrl);
              var mime = format === 'jpeg' ? 'image/jpeg' :
                         format === 'webp' ? 'image/webp' : 'image/png';
              var quality = format === 'png' ? undefined : 0.95;
              canvas.toBlob(function (blob) {
                if (!blob) reject(exportError('viewer.export.error.toBlobNull', { label: format }));
                else resolve(blob);
              }, mime, quality);
            } catch (error) {
              URL.revokeObjectURL(svgUrl);
              reject(error);
            }
          };
          img.onerror = function (e) {
            URL.revokeObjectURL(svgUrl);
            reject(e);
          };
          img.src = svgUrl;
        });
      }

      function fitCanvasText(ctx, text, maxWidth, startSize, minSize, weight) {
        var value = String(text || '').trim();
        var size = startSize;
        var family = "'JetBrains Mono', ui-monospace, SFMono-Regular, Menlo, Consolas, monospace";
        while (size > minSize) {
          ctx.font = (weight || '600') + ' ' + size + 'px ' + family;
          if (ctx.measureText(value).width <= maxWidth) return value;
          size -= 1;
        }
        ctx.font = (weight || '600') + ' ' + minSize + 'px ' + family;
        if (ctx.measureText(value).width <= maxWidth) return value;
        var suffix = '\u2026';
        while (value.length > 1 && ctx.measureText(value + suffix).width > maxWidth) {
          value = value.slice(0, -1);
        }
        return value + suffix;
      }

      function canvas2dOrThrow(canvas, label) {
        if (!canvas || typeof canvas.getContext !== 'function') {
          throw exportError('viewer.export.error.canvasUnavailable', { label: label });
        }
        var ctx = canvas.getContext('2d');
        if (!ctx) throw exportError('viewer.export.error.contextUnavailable', { label: label });
        if (typeof canvas.toBlob !== 'function') throw exportError('viewer.export.error.toBlobUnavailable', { label: label });
        return ctx;
      }

      function renderShareCard(options) {
        options = options || {};
        var routeSnapshot = options.routeSnapshot || null;
        var reachSnapshot = options.reachSnapshot || null;
        if (routeSnapshot && reachSnapshot) return Promise.reject(exportError('viewer.export.error.variantsCombined'));
        var svg = document.querySelector('.diagram-container svg');
        var vb = svg.viewBox.baseVal;
        var sourceScale = Math.min(2, pickSafeScale(vb.width, vb.height));
        var data = serializeSvg(sourceScale, { routeSnapshot: routeSnapshot, reachSnapshot: reachSnapshot });
        if (!data.canonicalStateClean) return Promise.reject(exportError('viewer.export.error.viewerState'));
        if (routeSnapshot && !data.routeStateClean) return Promise.reject(exportError('viewer.export.error.routeState'));
        if (reachSnapshot && !data.reachStateClean) return Promise.reject(exportError('viewer.export.error.reachState'));
        var svgBlob = new Blob([data.svgString], { type: 'image/svg+xml;charset=utf-8' });
        var svgUrl = URL.createObjectURL(svgBlob);

        return new Promise(function (resolve, reject) {
          var img = new Image();
          img.onload = function () {
            try {
              var canvas = document.createElement('canvas');
              canvas.width = SHARE_CARD_WIDTH;
              canvas.height = SHARE_CARD_HEIGHT;
              var ctx = canvas2dOrThrow(canvas, viewerText('viewer.export.shareCard'));
              var computed = getComputedStyle(document.documentElement);
              var bg = computed.getPropertyValue('--bg').trim() || currentBg();
              var text = computed.getPropertyValue('--text').trim() || '#ffffff';
              var muted = computed.getPropertyValue('--text-muted').trim() || '#94a3b8';
              var border = computed.getPropertyValue('--panel-border').trim() || '#334155';
              var accentProperty = reachSnapshot
                ? (reachSnapshot.direction === 'upstream' ? '--database-stroke' : '--backend-stroke')
                : '--frontend-stroke';
              var accent = computed.getPropertyValue(accentProperty).trim() || '#22d3ee';
              var titleNode = document.querySelector('.header h1');
              var subtitleNode = document.querySelector('.header .subtitle');
              var title = titleNode ? titleNode.textContent : document.title;
              var directionLabel = reachSnapshot
                ? viewerText('viewer.export.direction.' + reachSnapshot.direction)
                : '';
              var subtitle = routeSnapshot
                ? viewerCount('viewer.export.card.routeSummary', routeSnapshot.hops, {
                    source: routeSnapshot.source.label,
                    target: routeSnapshot.target.label
                  })
                : reachSnapshot
                  ? viewerText('viewer.export.card.reachSummary', {
                      direction: directionLabel,
                      origin: reachSnapshot.origin.label,
                      nodes: viewerCount('viewer.export.card.node', reachSnapshot.nodeIds.length - 1),
                      links: viewerCount('viewer.export.card.link', reachSnapshot.edges.length),
                      hops: viewerCount('viewer.export.card.hop', reachSnapshot.maxDepth)
                    })
                  : subtitleNode ? subtitleNode.textContent : '';
              var preset = document.documentElement.getAttribute('data-preset') || 'classic';
              var theme = document.documentElement.getAttribute('data-theme') || 'dark';
              var presetKey = preset === 'signal-flow'
                ? 'viewer.preset.flow.short'
                : 'viewer.preset.' + preset;
              var presetLabel = viewerText(presetKey).toUpperCase();
              var themeLabel = viewerText('viewer.theme.' + theme).toUpperCase();
              var cardLabel = routeSnapshot
                ? viewerText('viewer.export.card.routeBadge', {
                    hops: viewerCount('viewer.export.card.hop', routeSnapshot.hops).toUpperCase()
                  })
                : reachSnapshot
                  ? viewerText('viewer.export.card.reachBadge', { direction: directionLabel.toUpperCase() })
                  : viewerText('viewer.export.card.defaultBadge', { preset: presetLabel, theme: themeLabel });

              ctx.fillStyle = bg;
              ctx.fillRect(0, 0, SHARE_CARD_WIDTH, SHARE_CARD_HEIGHT);

              ctx.fillStyle = accent;
              ctx.fillRect(SHARE_CARD_PADDING, 27, 42, 3);

              ctx.textBaseline = 'alphabetic';
              ctx.fillStyle = text;
              var fittedTitle = fitCanvasText(ctx, title, SHARE_CARD_WIDTH - SHARE_CARD_PADDING * 2 - 330, 29, 18, '700');
              ctx.fillText(fittedTitle, SHARE_CARD_PADDING, 62);

              ctx.fillStyle = muted;
              var fittedSubtitle = fitCanvasText(ctx, subtitle, SHARE_CARD_WIDTH - SHARE_CARD_PADDING * 2 - 280, 13, 11, '500');
              ctx.fillText(fittedSubtitle, SHARE_CARD_PADDING, 87);

              ctx.font = "600 12px 'JetBrains Mono', ui-monospace, SFMono-Regular, Menlo, Consolas, monospace";
              ctx.textAlign = 'right';
              ctx.fillStyle = accent;
              ctx.fillText(cardLabel, SHARE_CARD_WIDTH - SHARE_CARD_PADDING, 50);
              ctx.textAlign = 'left';

              var availableWidth = SHARE_CARD_WIDTH - SHARE_CARD_PADDING * 2;
              var availableHeight = SHARE_CARD_HEIGHT - SHARE_CARD_HEADER - SHARE_CARD_PADDING;
              var fit = Math.min(availableWidth / data.width, availableHeight / data.height);
              var drawWidth = data.width * fit;
              var drawHeight = data.height * fit;
              var drawX = SHARE_CARD_PADDING + (availableWidth - drawWidth) / 2;
              var drawY = SHARE_CARD_HEADER + (availableHeight - drawHeight) / 2;

              ctx.strokeStyle = border;
              ctx.lineWidth = 1;
              ctx.strokeRect(drawX - 0.5, drawY - 0.5, drawWidth + 1, drawHeight + 1);
              ctx.drawImage(img, drawX, drawY, drawWidth, drawHeight);
              URL.revokeObjectURL(svgUrl);

              canvas.toBlob(function (blob) {
                if (!blob) reject(exportError('viewer.export.error.toBlobNull', { label: viewerText('viewer.export.shareCard') }));
                else resolve(blob);
              }, 'image/png');
            } catch (error) {
              URL.revokeObjectURL(svgUrl);
              reject(error);
            }
          };
          img.onerror = function (e) {
            URL.revokeObjectURL(svgUrl);
            reject(e);
          };
          img.src = svgUrl;
        });
      }

      function rasterizeShareCard(options) {
        options = options || {};
        if (!options.variant) return renderShareCard();
        if (options.variant !== 'route' && options.variant !== 'reach') {
          return Promise.reject(exportError('viewer.export.unknownVariant', { variant: options.variant }));
        }
        if (options.variant === 'route') {
          var routeSnapshot = Mosofin.routeProbe && Mosofin.routeProbe.exportSnapshot();
          if (!routeSnapshot) return Promise.reject(exportError('viewer.export.routeRequired'));
          return renderShareCard({ routeSnapshot: routeSnapshot });
        }
        var snapshot = Mosofin.focus && typeof Mosofin.focus.reachabilitySnapshot === 'function'
          ? Mosofin.focus.reachabilitySnapshot()
          : null;
        if (!snapshot) return Promise.reject(exportError('viewer.export.reachRequired'));
        return renderShareCard({ reachSnapshot: snapshot });
      }

      function motionMimeType() {
        if (typeof MediaRecorder === 'undefined') return '';
        var candidates = [
          'video/webm;codecs=vp9',
          'video/webm;codecs=vp8',
          'video/webm'
        ];
        for (var i = 0; i < candidates.length; i++) {
          if (!MediaRecorder.isTypeSupported || MediaRecorder.isTypeSupported(candidates[i])) return candidates[i];
        }
        return '';
      }

      function canRecordMotion() {
        var svg = document.querySelector('.diagram-container svg');
        return !!(svg && svg.getAttribute('data-animation') === 'trace' &&
          typeof MediaRecorder !== 'undefined' && motionMimeType() &&
          typeof HTMLCanvasElement !== 'undefined' &&
          typeof HTMLCanvasElement.prototype.captureStream === 'function');
      }

      // Record the live CSS animation without Puppeteer, ffmpeg, or a network
      // dependency. SVG files loaded through Image are commonly rasterized as
      // one cached bitmap, so repeatedly drawing that Image does not reliably
      // advance its CSS animation. Keep one crisp static SVG background, then
      // render an explicit time-varying signal scene over the real authored
      // relationship geometry on every captured canvas frame.
      function recordWebm(options) {
        options = options || {};
        if (!canRecordMotion()) {
          return Promise.reject(exportError('viewer.export.error.webmRequirements'));
        }
        var duration = Math.max(250, Number(options.duration) || MOTION_DURATION);
        var fps = Math.max(1, Number(options.fps) || MOTION_FPS);
        var svg = document.querySelector('.diagram-container svg');
        var vb = svg.viewBox.baseVal;
        var scale = Math.min(1, 1280 / vb.width);
        var data = serializeSvg(scale);
        var sourceUrl = URL.createObjectURL(new Blob([data.svgString], { type: 'image/svg+xml;charset=utf-8' }));

        function createMotionScene(root) {
          var rootMatrix = root.getCTM ? root.getCTM() : null;

          function pointInRoot(element, point) {
            if (!rootMatrix || !element.getCTM || !point.matrixTransform) return { x: point.x, y: point.y };
            try {
              var elementMatrix = element.getCTM();
              if (!elementMatrix) return { x: point.x, y: point.y };
              var mapped = point.matrixTransform(rootMatrix.inverse().multiply(elementMatrix));
              return { x: mapped.x, y: mapped.y };
            } catch (_) {
              return { x: point.x, y: point.y };
            }
          }

          function samplesFor(element) {
            if (typeof element.getTotalLength !== 'function' || typeof element.getPointAtLength !== 'function') return [];
            var length;
            try { length = element.getTotalLength(); } catch (_) { return []; }
            if (!Number.isFinite(length) || length <= 0) return [];
            var count = Math.max(12, Math.min(72, Math.ceil(length / 12)));
            var points = [];
            for (var i = 0; i <= count; i++) {
              points.push(pointInRoot(element, element.getPointAtLength(length * i / count)));
            }
            return points;
          }

          function authoredStep(element, fallback) {
            var raw = element.style.getPropertyValue('--step') || getComputedStyle(element).getPropertyValue('--step');
            var value = Number(raw);
            return Number.isFinite(value) ? value : fallback;
          }

          var edges = Array.prototype.slice.call(root.querySelectorAll('[data-animate="edge"]')).map(function (element, index) {
            var computed = getComputedStyle(element);
            return {
              points: samplesFor(element),
              color: computed.stroke && computed.stroke !== 'none' ? computed.stroke : '#22d3ee',
              width: Math.max(1.5, parseFloat(computed.strokeWidth) || 1.5),
              delay: Math.min(12, authoredStep(element, index)) * 0.16
            };
          }).filter(function (edge) { return edge.points.length > 1; });

          var nodes = Array.prototype.slice.call(root.querySelectorAll('[data-node-id][data-animate="node"]')).map(function (element, index) {
            var box;
            try { box = element.getBBox(); } catch (_) { return null; }
            var painted = element.querySelector('[class*="c-"]') || element;
            var computed = getComputedStyle(painted);
            return {
              x: box.x,
              y: box.y,
              width: box.width,
              height: box.height,
              color: computed.stroke && computed.stroke !== 'none' ? computed.stroke : '#22d3ee',
              delay: Math.min(12, authoredStep(element, index)) * 0.16
            };
          }).filter(Boolean);

          return { edges: edges, nodes: nodes, x: vb.x, y: vb.y, width: vb.width, height: vb.height };
        }

        function pointAlong(points, progress) {
          var scaled = Math.max(0, Math.min(1, progress)) * (points.length - 1);
          var index = Math.min(points.length - 2, Math.floor(scaled));
          var mix = scaled - index;
          return {
            x: points[index].x + (points[index + 1].x - points[index].x) * mix,
            y: points[index].y + (points[index + 1].y - points[index].y) * mix
          };
        }

        function roundedRectPath(ctx, x, y, width, height, radius) {
          var r = Math.max(0, Math.min(radius, width / 2, height / 2));
          ctx.beginPath();
          ctx.moveTo(x + r, y);
          ctx.lineTo(x + width - r, y);
          ctx.quadraticCurveTo(x + width, y, x + width, y + r);
          ctx.lineTo(x + width, y + height - r);
          ctx.quadraticCurveTo(x + width, y + height, x + width - r, y + height);
          ctx.lineTo(x + r, y + height);
          ctx.quadraticCurveTo(x, y + height, x, y + height - r);
          ctx.lineTo(x, y + r);
          ctx.quadraticCurveTo(x, y, x + r, y);
          ctx.closePath();
        }

        function drawMotionFrame(ctx, backgroundImage, motionScene, elapsed) {
          ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);
          ctx.drawImage(backgroundImage, 0, 0, ctx.canvas.width, ctx.canvas.height);
          ctx.save();
          ctx.scale(ctx.canvas.width / motionScene.width, ctx.canvas.height / motionScene.height);
          ctx.translate(-motionScene.x, -motionScene.y);

          motionScene.edges.forEach(function (edge) {
            var duration = 1.75;
            var progress = (elapsed - edge.delay) / duration;
            if (progress < 0 || progress > 1) return;
            var head = Math.max(0, Math.min(1, progress));
            var tail = Math.max(0, head - 0.24);
            var opacity = Math.sin(Math.PI * Math.min(1, progress));

            ctx.save();
            ctx.strokeStyle = edge.color;
            ctx.fillStyle = edge.color;
            ctx.lineCap = 'round';
            ctx.lineJoin = 'round';
            ctx.lineWidth = Math.max(3.2, edge.width * 2.2);
            ctx.globalAlpha = 0.42 + opacity * 0.5;
            ctx.shadowColor = edge.color;
            ctx.shadowBlur = 10;
            ctx.beginPath();
            for (var i = 0; i <= 14; i++) {
              var trailPoint = pointAlong(edge.points, tail + (head - tail) * i / 14);
              if (i === 0) ctx.moveTo(trailPoint.x, trailPoint.y);
              else ctx.lineTo(trailPoint.x, trailPoint.y);
            }
            ctx.stroke();

            var point = pointAlong(edge.points, head);
            ctx.globalAlpha = 0.95;
            ctx.beginPath();
            ctx.arc(point.x, point.y, 4.8, 0, Math.PI * 2);
            ctx.fill();
            ctx.restore();
          });

          motionScene.nodes.forEach(function (node) {
            var progress = (elapsed - node.delay) / 2.6;
            if (progress < 0 || progress > 1) return;
            var pulse = Math.sin(Math.PI * progress);
            if (pulse <= 0.02) return;
            ctx.save();
            ctx.strokeStyle = node.color;
            ctx.lineWidth = 1.4 + pulse * 1.8;
            ctx.globalAlpha = pulse * 0.5;
            ctx.shadowColor = node.color;
            ctx.shadowBlur = 12 * pulse;
            var inset = 2 + pulse * 3;
            roundedRectPath(ctx, node.x - inset, node.y - inset, node.width + inset * 2, node.height + inset * 2, 8);
            ctx.stroke();
            ctx.restore();
          });

          ctx.restore();
        }

        var motionScene = createMotionScene(svg);

        return new Promise(function (resolve, reject) {
          var backgroundImage = new Image();
          backgroundImage.onload = function () {
            var canvas = document.createElement('canvas');
            canvas.width = Math.max(2, Math.round(data.width / 2) * 2);
            canvas.height = Math.max(2, Math.round(data.height / 2) * 2);
            var ctx = canvas.getContext('2d');
            var stream = canvas.captureStream(fps);
            var mime = motionMimeType();
            var recorder;
            try {
              recorder = new MediaRecorder(stream, { mimeType: mime, videoBitsPerSecond: 6000000 });
            } catch (err) {
              URL.revokeObjectURL(sourceUrl);
              stream.getTracks().forEach(function (track) { track.stop(); });
              reject(err);
              return;
            }
            var chunks = [];
            var raf = 0;
            var stopped = false;
            var startedAt = performance.now();
            function cleanup() {
              if (stopped) return;
              stopped = true;
              cancelAnimationFrame(raf);
              URL.revokeObjectURL(sourceUrl);
              stream.getTracks().forEach(function (track) { track.stop(); });
            }
            function draw(now) {
              var elapsed = Math.max(0, ((Number(now) || performance.now()) - startedAt) / 1000);
              drawMotionFrame(ctx, backgroundImage, motionScene, elapsed);
              raf = requestAnimationFrame(draw);
            }
            recorder.ondataavailable = function (event) {
              if (event.data && event.data.size) chunks.push(event.data);
            };
            recorder.onerror = function (event) {
              cleanup();
              reject(event.error || exportError('viewer.export.error.mediaRecorder'));
            };
            recorder.onstop = function () {
              cleanup();
              var blob = new Blob(chunks, { type: recorder.mimeType || mime });
              if (!blob.size) reject(exportError('viewer.export.error.emptyWebm'));
              else resolve(blob);
            };
            drawMotionFrame(ctx, backgroundImage, motionScene, 0);
            recorder.start(250);
            startedAt = performance.now();
            raf = requestAnimationFrame(draw);
            setTimeout(function () {
              if (recorder.state === 'inactive') return;
              // Chromium normally emits the final chunk during stop(), but
              // some embedded browser shells need an explicit encoder flush
              // first. Keep the short grace window bounded and harmless for
              // browsers that already delivered timeslice chunks.
              try { recorder.requestData(); } catch (_) {}
              setTimeout(function () {
                if (recorder.state !== 'inactive') recorder.stop();
              }, 120);
            }, duration);
          };
          backgroundImage.onerror = function () {
            URL.revokeObjectURL(sourceUrl);
            reject(exportError('viewer.export.error.webmBackground'));
          };
          backgroundImage.src = sourceUrl;
        });
      }

      var menu = document.getElementById('export-menu');
      var btn = document.getElementById('btn-export');
      var routeShareItem = menu.querySelector('button[data-action="route-share-card"]');
      var reachShareItem = menu.querySelector('button[data-action="reach-share-card"]');
      var items = function () {
        return Array.prototype.slice.call(menu.querySelectorAll('button[role="menuitem"]'));
      };

      function syncRouteShareItem() {
        var snapshot = Mosofin.routeProbe && typeof Mosofin.routeProbe.exportSnapshot === 'function'
          ? Mosofin.routeProbe.exportSnapshot()
          : null;
        routeShareItem.hidden = !snapshot;
        routeShareItem.disabled = !snapshot;
        return snapshot;
      }

      function syncReachShareItem() {
        var snapshot = Mosofin.focus && typeof Mosofin.focus.reachabilitySnapshot === 'function'
          ? Mosofin.focus.reachabilitySnapshot()
          : null;
        reachShareItem.hidden = !snapshot;
        reachShareItem.disabled = !snapshot;
        return snapshot;
      }

      // ---- Clipboard support ----------------------------------------------
      function canCopyImage() {
        return typeof ClipboardItem !== 'undefined' &&
               navigator.clipboard &&
               typeof navigator.clipboard.write === 'function';
      }

      // ---- Raster format detection ----------------------------------------
      // canvas.toBlob('image/webp') silently returns a PNG on browsers without
      // WebP encoding (older Safari), so detect explicitly.
      function supports(format) {
        if (format === 'share-card') return true;
        if (format === 'svg' || format === 'png') return true;
        if (format === 'webm') return canRecordMotion();
        var mime = format === 'jpeg' ? 'image/jpeg' : 'image/webp';
        try {
          var c = document.createElement('canvas');
          c.width = c.height = 2;
          return c.toDataURL(mime).indexOf('data:' + mime) === 0;
        } catch (_) { return false; }
      }

      // Gray out unsupported items.
      items().forEach(function (it) {
        if (it.dataset.format && !supports(it.dataset.format)) {
          it.disabled = true;
          it.title = viewerText('viewer.export.unsupported');
        }
        if ((it.dataset.action === 'copy' || it.dataset.action === 'copy-share-card') && !canCopyImage()) {
          it.disabled = true;
          it.title = viewerText('viewer.export.clipboardUnsupported');
        }
      });

      // ---- Toast ----------------------------------------------------------
      // A live region only announces CHANGES to an existing node, so the toast
      // element is created once up front and updated in place.
      var toastEl = document.createElement('div');
      toastEl.className = 'mosofin-toast';
      toastEl.setAttribute('role', 'status');
      document.body.appendChild(toastEl);
      var toastTimer = null;

      function toast(msg) {
        toastEl.textContent = '';
        requestAnimationFrame(function () {
          toastEl.textContent = msg;
          toastEl.classList.add('show');
        });
        clearTimeout(toastTimer);
        toastTimer = setTimeout(function () { toastEl.classList.remove('show'); }, 1500);
      }

      function open(focusLast) {
        if (Mosofin.preset && Mosofin.preset.isOpen()) Mosofin.preset.close(false);
        if (Mosofin.semanticLens && typeof Mosofin.semanticLens.clearPreview === 'function') Mosofin.semanticLens.clearPreview();
        if (Mosofin.semanticLens && Mosofin.semanticLens.isOpen()) Mosofin.semanticLens.close({ restoreFocus: false });
        syncRouteShareItem();
        syncReachShareItem();
        menu.classList.add('open');
        btn.setAttribute('aria-expanded', 'true');
        var available = items().filter(function (i) { return !i.hidden && !i.disabled; });
        var target = focusLast ? available[available.length - 1] : available[0];
        if (target) target.focus();
      }
      function close(focusTrigger) {
        menu.classList.remove('open');
        btn.setAttribute('aria-expanded', 'false');
        if (focusTrigger) btn.focus();
      }
      function isOpen() { return menu.classList.contains('open'); }

      btn.addEventListener('click', function (e) {
        e.stopPropagation();
        if (isOpen()) close(false);
        else open();
      });
      // APG menu-button pattern: ArrowDown/ArrowUp on the trigger opens the menu.
      btn.addEventListener('keydown', function (e) {
        if (e.key === 'ArrowDown' || e.key === 'ArrowUp') {
          e.preventDefault();
          if (!isOpen()) open(e.key === 'ArrowUp');
        }
      });
      document.addEventListener('click', function (e) {
        if (!menu.contains(e.target) && e.target !== btn) close(false);
      });

      // Keyboard navigation inside the menu.
      //   Menu items: Up/Down, Home/End.
      //   Esc closes + returns focus to trigger; Tab closes.
      menu.addEventListener('keydown', function (e) {
        if (e.key === 'Escape') { e.preventDefault(); close(true); return; }
        if (e.key === 'Tab')    { close(false); return; }

        var available = items().filter(function (i) { return !i.hidden && !i.disabled; });
        var current = available.indexOf(document.activeElement);
        switch (e.key) {
          case 'ArrowDown':
            e.preventDefault();
            if (available.length) available[(current + 1 + available.length) % available.length].focus();
            break;
          case 'ArrowUp':
            e.preventDefault();
            if (available.length) available[(current - 1 + available.length) % available.length].focus();
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

      function runExport(format) {
        var base = diagramFilename();
        close(true);
        clearExportReceipt();
        if (format === 'webm') toast(viewerText('viewer.export.recording'));
        return (format === 'share-card'
          ? rasterizeShareCard().then(function (blob) {
              recordExportReceipt('share-card', blob, true, { width: SHARE_CARD_WIDTH, height: SHARE_CARD_HEIGHT });
              download(blob, base + '-share-card.png');
              toast(viewerText('viewer.export.downloadedShare'));
            })
          : format === 'svg'
          ? Promise.resolve(serializeSvg(1, { autoTheme: true })).then(function (d) {
              var blob = new Blob([d.svgString], { type: 'image/svg+xml;charset=utf-8' });
              recordExportReceipt('svg', blob, d.canonicalStateClean);
              download(blob, base + '.svg');
            })
          : format === 'webm'
            ? recordWebm().then(function (blob) {
                document.documentElement.setAttribute('data-last-motion-bytes', String(blob.size));
                recordExportReceipt('webm', blob, true);
                download(blob, base + '.webm');
                toast(viewerText('viewer.export.downloadedWebm'));
              })
          : rasterize(format).then(function (blob) {
              recordExportReceipt(format, blob, true);
              download(blob, base + '.' + format);
            })
        ).catch(function (err) {
          console.error(err);
          var technicalMessage = err && err.message ? err.message : format;
          var message = exportMessage(err);
          document.documentElement.setAttribute('data-last-export-error-format', format);
          document.documentElement.setAttribute('data-last-export-error', technicalMessage);
          if (format === 'webm') {
            var motionItem = menu.querySelector('button[data-format="webm"]');
            if (motionItem) {
              motionItem.disabled = true;
              motionItem.title = viewerText('viewer.export.motionUnavailable');
              motionItem.style.opacity = '0.5';
            }
            toast(viewerText('viewer.export.webmUnavailable'));
            return;
          }
          alert(viewerText('viewer.export.failed', { message: message }));
        });
      }

      function runRouteShareCard() {
        close(false);
        clearExportReceipt();
        var snapshot = Mosofin.routeProbe && Mosofin.routeProbe.exportSnapshot();
        var blobPromise = snapshot
          ? renderShareCard({ routeSnapshot: snapshot })
          : Promise.reject(exportError('viewer.export.routeRequired'));
        return blobPromise.then(function (blob) {
          recordExportReceipt('share-card', blob, false, { width: SHARE_CARD_WIDTH, height: SHARE_CARD_HEIGHT }, 'route', true);
          download(blob, diagramFilename() + '-route-share-card.png');
          toast(viewerText('viewer.export.downloadedRoute'));
          return blob;
        }).catch(function (err) {
          console.error(err);
          var technicalMessage = err && err.message ? err.message : 'share-card';
          var message = exportMessage(err);
          document.documentElement.setAttribute('data-last-export-error-format', 'share-card');
          document.documentElement.setAttribute('data-last-export-error', technicalMessage);
          alert(viewerText('viewer.export.routeFailed', { message: message }));
        });
      }

      function runReachShareCard() {
        close(false);
        clearExportReceipt();
        var snapshot = Mosofin.focus && typeof Mosofin.focus.reachabilitySnapshot === 'function'
          ? Mosofin.focus.reachabilitySnapshot()
          : null;
        var blobPromise = snapshot
          ? renderShareCard({ reachSnapshot: snapshot })
          : Promise.reject(exportError('viewer.export.reachRequired'));
        return blobPromise.then(function (blob) {
          recordExportReceipt('share-card', blob, false, { width: SHARE_CARD_WIDTH, height: SHARE_CARD_HEIGHT }, 'reach', false, true);
          download(blob, diagramFilename() + '-' + snapshot.direction + '-reach-share-card.png');
          toast(viewerText('viewer.export.downloadedReach'));
          return blob;
        }).catch(function (err) {
          console.error(err);
          var technicalMessage = err && err.message ? err.message : 'share-card';
          var message = exportMessage(err);
          document.documentElement.setAttribute('data-last-export-error-format', 'share-card');
          document.documentElement.setAttribute('data-last-export-error', technicalMessage);
          alert(viewerText('viewer.export.reachFailed', { message: message }));
        });
      }

      // A compact, machine-readable receipt for local QA and generated proof
      // galleries. It exposes no diagram contents, only the completed format,
      // byte count, and whether temporary viewer state was excluded.
      function recordExportReceipt(format, blob, canonical, dimensions, variant, routeStateClean, reachStateClean) {
        document.documentElement.setAttribute('data-last-export-format', format);
        document.documentElement.setAttribute('data-last-export-bytes', String(blob.size));
        document.documentElement.setAttribute('data-last-export-canonical', canonical ? 'true' : 'false');
        if (variant) {
          document.documentElement.setAttribute('data-last-export-variant', variant);
        } else {
          document.documentElement.removeAttribute('data-last-export-variant');
        }
        if (routeStateClean === true) {
          document.documentElement.setAttribute('data-last-export-route-state-clean', 'true');
        } else {
          document.documentElement.removeAttribute('data-last-export-route-state-clean');
        }
        if (reachStateClean === true) {
          document.documentElement.setAttribute('data-last-export-reach-state-clean', 'true');
        } else {
          document.documentElement.removeAttribute('data-last-export-reach-state-clean');
        }
        if (dimensions) {
          document.documentElement.setAttribute('data-last-export-width', String(dimensions.width));
          document.documentElement.setAttribute('data-last-export-height', String(dimensions.height));
        } else {
          document.documentElement.removeAttribute('data-last-export-width');
          document.documentElement.removeAttribute('data-last-export-height');
        }
      }

      function clearExportReceipt() {
        document.documentElement.removeAttribute('data-last-export-format');
        document.documentElement.removeAttribute('data-last-export-bytes');
        document.documentElement.removeAttribute('data-last-export-canonical');
        document.documentElement.removeAttribute('data-last-export-width');
        document.documentElement.removeAttribute('data-last-export-height');
        document.documentElement.removeAttribute('data-last-export-variant');
        document.documentElement.removeAttribute('data-last-export-route-state-clean');
        document.documentElement.removeAttribute('data-last-export-reach-state-clean');
        document.documentElement.removeAttribute('data-last-export-error-format');
        document.documentElement.removeAttribute('data-last-export-error');
      }

      function writePngToClipboard(blobPromise) {
        // WebKit requires ClipboardItem to be constructed synchronously inside
        // the user gesture. Chromium accepts the same pending Promise<Blob>;
        // engines that reject promise values fall back to an awaited Blob.
        try {
          return navigator.clipboard.write([new ClipboardItem({ 'image/png': blobPromise })]);
        } catch (_) {
          return blobPromise.then(function (blob) {
            return navigator.clipboard.write([new ClipboardItem({ 'image/png': blob })]);
          });
        }
      }

      function runCopyShareCard() {
        close(true);
        clearExportReceipt();
        if (!canCopyImage()) {
          alert(viewerText('viewer.export.clipboardUnsupported.period'));
          return;
        }
        var blobPromise = rasterizeShareCard();
        return writePngToClipboard(blobPromise).then(function () {
          return blobPromise.then(function (blob) {
            recordExportReceipt('share-card', blob, true, { width: SHARE_CARD_WIDTH, height: SHARE_CARD_HEIGHT });
            toast(viewerText('viewer.export.copiedShare'));
          });
        }).catch(function (err) {
          console.error(err);
          var technicalMessage = err && err.message ? err.message : 'share-card';
          var message = exportMessage(err);
          document.documentElement.setAttribute('data-last-export-error-format', 'share-card');
          document.documentElement.setAttribute('data-last-export-error', technicalMessage);
          alert(viewerText('viewer.export.copyFailed', { message: message }));
        });
      }

      function runCopy() {
        close(true);
        clearExportReceipt();
        if (!canCopyImage()) {
          alert(viewerText('viewer.export.clipboardUnsupported.short'));
          return;
        }
        var blobPromise = rasterize('png');
        return writePngToClipboard(blobPromise).then(function () {
          toast(viewerText('viewer.export.copiedPng'));
        }).catch(function (err) {
          console.error(err);
          alert(viewerText('viewer.export.copyFailed', {
            message: exportMessage(err)
          }));
        });
      }

      menu.addEventListener('click', function (e) {
        var routeShareCardBtn = e.target.closest('button[data-action="route-share-card"]');
        if (routeShareCardBtn && !routeShareCardBtn.disabled && !routeShareCardBtn.hidden) { runRouteShareCard(); return; }

        var reachShareCardBtn = e.target.closest('button[data-action="reach-share-card"]');
        if (reachShareCardBtn && !reachShareCardBtn.disabled && !reachShareCardBtn.hidden) { runReachShareCard(); return; }

        var copyShareCardBtn = e.target.closest('button[data-action="copy-share-card"]');
        if (copyShareCardBtn && !copyShareCardBtn.disabled) { runCopyShareCard(); return; }

        var copyBtn = e.target.closest('button[data-action="copy"]');
        if (copyBtn && !copyBtn.disabled) { runCopy(); return; }

        var formatBtn = e.target.closest('button[data-format]');
        if (formatBtn && !formatBtn.disabled) { runExport(formatBtn.dataset.format); }
      });

      Mosofin.motion = { canRecord: canRecordMotion, recordWebm: recordWebm };
      Mosofin.exportMenu = {
        open: open,
        close: close,
        isOpen: isOpen,
        run: runExport,
        shareCard: rasterizeShareCard,
        downloadRouteShareCard: runRouteShareCard,
        downloadReachShareCard: runReachShareCard,
        syncRouteShare: syncRouteShareItem,
        syncReachShare: syncReachShareItem,
        copyShareCard: runCopyShareCard
      };

      // Auto-open on page load for demo/screenshot purposes: ?openExport=1
      // Wait for fonts (so the menu doesn't flash before typography lands)
      // and paint before opening. Fallback timeout for browsers without the
      // Font Loading API.
      try {
        if (new URLSearchParams(window.location.search).get('openExport') === '1') {
          var openWhenReady = function () {
            requestAnimationFrame(function () { requestAnimationFrame(open); });
          };
          if (document.fonts && document.fonts.ready) {
            document.fonts.ready.then(openWhenReady);
          } else {
            setTimeout(openWhenReady, 200);
          }
        }
      } catch (_) {}
    })();

    /* ============================================================
       Motion Governor — one reader switch and one motion budget. Ambient
       trace yields to the strongest semantic owner; Still also parks bounded
       viewer signals without discarding their static meaning.
       ============================================================ */
    Mosofin.motionGovernor = (function () {
      var storedName = 'mosofin-motion';
      var html = document.documentElement;
      var svg = document.querySelector('.diagram-container svg');
      var btn = document.getElementById('btn-motion');
      var label = document.getElementById('motion-label');
      var motionQuery = window.matchMedia ? window.matchMedia('(prefers-reduced-motion: reduce)') : null;
      var capable = !!(svg && svg.getAttribute('data-animation') === 'trace');
      var readerPaused = false;
      var suspensions = Object.create(null);
      var explicitOwner = '';
      var owner = '';
      var ownerToken = 0;
      var ownerCleanup = null;
      var lastEffectivePaused = null;
      var ambientStarted = false;
      var ambientPending = new Set();

      function detachAmbientBoundary() {
        if (!svg) return;
        svg.removeEventListener('animationend', onAmbientBoundary, true);
        svg.removeEventListener('animationcancel', onAmbientBoundary, true);
      }
      function settleAmbient(reason) {
        if (!capable) return false;
        ambientStarted = true;
        ambientPending.clear();
        detachAmbientBoundary();
        html.setAttribute('data-ambient-motion', 'settled');
        html.setAttribute('data-ambient-settle-reason', reason || 'complete');
        return true;
      }
      function onAmbientBoundary(event) {
        if (!ambientPending.has(event.target)) return;
        ambientPending.delete(event.target);
        if (!ambientPending.size) settleAmbient('complete');
      }
      function startAmbient() {
        if (ambientStarted || !capable) return false;
        ambientStarted = true;
        ambientPending = new Set(Array.prototype.slice.call(svg.querySelectorAll('[data-animate="edge"], [data-animate="node"]')));
        if (!ambientPending.size) return settleAmbient('empty');
        svg.addEventListener('animationend', onAmbientBoundary, true);
        svg.addEventListener('animationcancel', onAmbientBoundary, true);
        html.setAttribute('data-ambient-motion', 'running');
        html.removeAttribute('data-ambient-settle-reason');
        return true;
      }

      function readStored() {
        try { return localStorage.getItem(storedName); } catch (_) { return null; }
      }
      function writeStored() {
        try {
          if (readerPaused) localStorage.setItem(storedName, 'still');
          else localStorage.removeItem(storedName);
        } catch (_) {}
      }
      function reducedMotion() {
        return !!(motionQuery && motionQuery.matches);
      }
      function hasSuspension() {
        return Object.keys(suspensions).length > 0;
      }
      function effectivePaused() {
        return readerPaused || reducedMotion() || hasSuspension();
      }
      function ownerLabel(value) {
        if (value === 'story') return viewerText('viewer.owner.story');
        if (value === 'chapter') return viewerText('viewer.owner.chapter');
        if (value === 'chapter-preview') return viewerText('viewer.owner.chapterPreview');
        if (value === 'handoff') return viewerText('viewer.owner.handoff');
        if (value === 'route') return viewerText('viewer.owner.route');
        if (value === 'lens') return viewerText('viewer.owner.lens');
        if (value === 'relationship') return viewerText('viewer.owner.relationship');
        if (value === 'intent') return viewerText('viewer.owner.intent');
        if (value === 'focus') return viewerText('viewer.owner.focus');
        if (value === 'legend') return viewerText('viewer.owner.legend');
        return viewerText('viewer.owner.reader');
      }
      function render() {
        if (!capable) return;
        var systemPaused = reducedMotion();
        var paused = effectivePaused();
        html.setAttribute('data-motion', paused ? 'still' : 'live');
        if (paused || owner || html.hasAttribute('data-embed') || html.hasAttribute('data-share-playback') || html.hasAttribute('data-document-hidden')) {
          settleAmbient('suppressed');
        } else if (!ambientStarted) {
          startAmbient();
        }
        btn.setAttribute('aria-pressed', paused ? 'false' : 'true');
        btn.disabled = systemPaused;
        label.textContent = viewerText(paused ? 'viewer.motion.still' : 'viewer.motion.live');
        if (paused && lastEffectivePaused !== true && Mosofin.guidedViews && Mosofin.guidedViews.isPlaying()) {
          Mosofin.guidedViews.pause();
        }
        if (paused && lastEffectivePaused !== true && Mosofin.guidedViews && Mosofin.guidedViews.settleHandoff) {
          Mosofin.guidedViews.settleHandoff(systemPaused ? 'reduced-motion' : (hasSuspension() ? 'hidden' : 'still'));
        }
        if (paused && lastEffectivePaused !== true && Mosofin.routeProbe && Mosofin.routeProbe.isJourneyPlaying && Mosofin.routeProbe.isJourneyPlaying()) {
          Mosofin.routeProbe.pauseJourney({ preserveElapsed: true, reason: systemPaused ? 'reduced-motion' : (hasSuspension() ? 'hidden' : 'still') });
        }
        lastEffectivePaused = paused;
        if (Mosofin.routeProbe && typeof Mosofin.routeProbe.syncMotion === 'function') Mosofin.routeProbe.syncMotion();
        if (systemPaused) {
          btn.setAttribute('aria-label', viewerText('viewer.motion.reduced'));
          btn.title = viewerText('viewer.motion.reduced');
        } else if (hasSuspension()) {
          btn.setAttribute('aria-label', viewerText('viewer.motion.hidden'));
          btn.title = viewerText('viewer.motion.hidden');
        } else if (paused) {
          btn.setAttribute('aria-label', viewerText('viewer.motion.resume'));
          btn.title = viewerText('viewer.motion.resume');
        } else if (owner) {
          btn.setAttribute('aria-label', viewerText('viewer.motion.yielding', { owner: ownerLabel(owner) }));
          btn.title = viewerText('viewer.motion.yielding.title', { owner: ownerLabel(owner) });
        } else {
          btn.setAttribute('aria-label', viewerText('viewer.motion.pause'));
          btn.title = viewerText('viewer.motion.pause');
        }
      }
      function setPaused(next, options) {
        options = options || {};
        readerPaused = !!next;
        if (options.persist !== false) writeStored();
        render();
        return readerPaused;
      }
      function publishOwner() {
        owner = explicitOwner || deriveOwner();
        if (owner) html.setAttribute('data-motion-owner', owner);
        else html.removeAttribute('data-motion-owner');
        render();
        return owner;
      }
      function deriveOwner() {
        if (!svg) return '';
        if (svg.hasAttribute('data-story-playing') || svg.hasAttribute('data-story-follow')) return 'story';
        if (svg.hasAttribute('data-story-active')) return 'chapter';
        if (svg.hasAttribute('data-route-picking') || svg.hasAttribute('data-route-active')) return 'route';
        if (svg.hasAttribute('data-lens-active')) return 'lens';
        if (svg.hasAttribute('data-relationship-preview-active')) return 'relationship';
        if (svg.hasAttribute('data-intent-trace-active')) return 'intent';
        if (svg.hasAttribute('data-focus-active')) return 'focus';
        if (svg.hasAttribute('data-legend-preview-active')) return 'legend';
        return '';
      }
      function clearClaim(preempted) {
        var cleanup = ownerCleanup;
        ownerCleanup = null;
        explicitOwner = '';
        if (preempted && cleanup) {
          try { cleanup(); } catch (_) {}
        }
      }
      function claim(next, cleanup) {
        if (!capable || !next) return 0;
        clearClaim(true);
        ownerToken += 1;
        explicitOwner = next;
        ownerCleanup = typeof cleanup === 'function' ? cleanup : null;
        publishOwner();
        return ownerToken;
      }
      function release(token) {
        if (!capable || token !== ownerToken || !explicitOwner) return false;
        clearClaim(false);
        ownerToken += 1;
        publishOwner();
        return true;
      }
      function suspend(reason) {
        var key = String(reason || 'runtime');
        var active = true;
        suspensions[key] = (suspensions[key] || 0) + 1;
        render();
        return function () {
          if (!active) return false;
          active = false;
          if (suspensions[key] > 1) suspensions[key] -= 1;
          else delete suspensions[key];
          render();
          return true;
        };
      }
      function syncVisibility() {
        if (document.hidden) {
          suspensions.visibility = true;
          html.setAttribute('data-document-hidden', 'true');
        } else {
          delete suspensions.visibility;
          html.removeAttribute('data-document-hidden');
        }
        render();
      }

      if (!capable) {
        btn.hidden = true;
        html.removeAttribute('data-motion-capable');
        html.removeAttribute('data-motion');
        html.removeAttribute('data-motion-owner');
        html.removeAttribute('data-ambient-motion');
        html.removeAttribute('data-ambient-settle-reason');
        return {
          capable: false,
          pause: function () { return false; },
          resume: function () { return false; },
          toggle: function () { return false; },
          setMode: function () { return 'still'; },
          mode: function () { return 'still'; },
          claim: function () { return 0; },
          release: function () { return false; },
          suspend: function () { return function () { return false; }; },
          isPaused: function () { return true; },
          owner: function () { return ''; }
        };
      }

      html.setAttribute('data-motion-capable', 'true');
      btn.hidden = false;
      readerPaused = readStored() === 'still';
      btn.addEventListener('click', function () { setPaused(!readerPaused); });
      if (motionQuery) {
        if (typeof motionQuery.addEventListener === 'function') motionQuery.addEventListener('change', render);
        else if (typeof motionQuery.addListener === 'function') motionQuery.addListener(render);
      }
      document.addEventListener('visibilitychange', syncVisibility);
      if (document.documentElement.getAttribute('data-embed') !== 'true' && typeof MutationObserver !== 'undefined' && typeof Node !== 'undefined' && svg instanceof Node) {
        var ownerObserver = new MutationObserver(function () { publishOwner(); });
        ownerObserver.observe(svg, {
          attributes: true,
          attributeFilter: [
            'data-story-playing', 'data-story-follow', 'data-story-active', 'data-route-picking', 'data-route-active',
            'data-lens-active', 'data-relationship-preview-active', 'data-intent-trace-active',
            'data-focus-active', 'data-legend-preview-active'
          ]
        });
      }
      syncVisibility();
      publishOwner();
      render();

      return {
        capable: true,
        pause: function () { return setPaused(true); },
        resume: function () { return setPaused(false); },
        toggle: function () { return setPaused(!readerPaused); },
        setMode: function (next, options) {
          setPaused(next === 'still', options);
          return effectivePaused() ? 'still' : 'live';
        },
        mode: function () { return effectivePaused() ? 'still' : 'live'; },
        claim: claim,
        release: release,
        suspend: suspend,
        isPaused: effectivePaused,
        owner: function () { return owner; }
      };
    })();

    /* Verified repository evidence is emitted only after the renderer checks
       the authored GitHub revision, source blobs, and optional line ranges
       against the explicit --repo-root. It remains outside the SVG so every
       canonical visual export stays free of repository paths. */
    Mosofin.sourceEvidence = (function () {
      var element = document.getElementById('mosofin-source-evidence-data');
      var payload = null;
      var svgNamespace = 'http://www.w3.org/2000/svg';
      if (element) {
        try {
          var parsed = JSON.parse(element.textContent || 'null');
          if (parsed && parsed.verified === true && parsed.repository && parsed.nodes) payload = parsed;
        } catch (_) {}
      }
      function installBeacons() {
        if (!payload) return 0;
        var svg = document.querySelector('.diagram-container svg');
        if (!svg) return 0;
        var installed = 0;
        Array.prototype.forEach.call(svg.querySelectorAll('[data-node-id]'), function (node) {
          var id = node.getAttribute('data-node-id');
          var sources = payload.nodes[id];
          if (!Array.isArray(sources) || !sources.length || node.querySelector('[data-source-evidence-beacon]')) return;
          var shape = node.querySelector('[data-animate="node"]') || node.querySelector('[class*="c-"]');
          var box;
          try { box = (shape || node).getBBox(); } catch (_) { return; }
          if (!box || !Number.isFinite(box.x) || !Number.isFinite(box.y) || !Number.isFinite(box.width) || box.width < 36) return;

          var count = sources.length;
          var label = viewerCount('viewer.passport.beacon', count);
          var beacon = document.createElementNS(svgNamespace, 'g');
          beacon.classList.add('source-evidence-beacon');
          beacon.setAttribute('data-source-evidence-beacon', '');
          beacon.setAttribute('aria-hidden', 'true');
          var brandOffset = node.hasAttribute('data-node-brand') ? 24 : 0;
          beacon.setAttribute('transform', 'translate(' + (box.x + box.width - 35 - brandOffset) + ' ' + (box.y + 5) + ')');
          var title = document.createElementNS(svgNamespace, 'title');
          title.textContent = label;
          var plate = document.createElementNS(svgNamespace, 'rect');
          plate.setAttribute('width', '30');
          plate.setAttribute('height', '12');
          plate.setAttribute('rx', '6');
          var text = document.createElementNS(svgNamespace, 'text');
          text.setAttribute('x', '15');
          text.setAttribute('y', '8.25');
          text.textContent = viewerText('viewer.passport.sourceMarker') + ' ' + count;
          beacon.appendChild(title);
          beacon.appendChild(plate);
          beacon.appendChild(text);
          node.appendChild(beacon);

          var originalLabel = node.getAttribute('aria-label') || '';
          node.setAttribute('data-source-evidence-count', String(count));
          node.setAttribute('data-source-evidence-original-label', originalLabel);
          node.setAttribute('aria-label', (originalLabel ? originalLabel + ', ' : '') + viewerCount('viewer.passport.sourceCount', count));
          installed += 1;
        });
        return installed;
      }
      return {
        available: function () { return Boolean(payload); },
        repository: function () { return payload ? payload.repository : null; },
        node: function (id) {
          var sources = payload && payload.nodes ? payload.nodes[id] : null;
          return Array.isArray(sources) ? sources.slice() : [];
        },
        installBeacons: installBeacons
      };
    })();
    Mosofin.sourceEvidence.installBeacons();

    /* ============================================================
       Explore — stable-ID neighborhood focus + dependency-free pan/zoom.
       Renderer IDs become deep-linkable semantic hooks without turning the
       standalone artifact into a canvas editor.
       ============================================================ */
    Mosofin.focus = (function () {
      var html = document.documentElement;
      var container = document.querySelector('.diagram-container');
      var svg = container.querySelector('svg');
      var chip = document.getElementById('focus-chip');
      var label = document.getElementById('focus-label');
      var detail = document.getElementById('focus-detail');
      var kind = document.getElementById('focus-kind');
      var context = document.getElementById('focus-context');
      var tag = document.getElementById('focus-tag');
      var semanticId = document.getElementById('focus-id');
      var evidence = document.getElementById('focus-evidence');
      var repositoryLink = document.getElementById('focus-repository');
      var evidenceLinks = document.getElementById('focus-evidence-links');
      var summary = document.getElementById('focus-summary');
      var reachSection = document.getElementById('focus-reach');
      var reachStatus = document.getElementById('focus-reach-status');
      var upstreamBtn = document.getElementById('btn-reach-upstream');
      var downstreamBtn = document.getElementById('btn-reach-downstream');
      var upstreamCount = document.getElementById('focus-reach-upstream-count');
      var downstreamCount = document.getElementById('focus-reach-downstream-count');
      var relationshipList = document.getElementById('relationship-lens-list');
      var copyBtn = document.getElementById('btn-focus-copy');
      var relationsBtn = document.getElementById('btn-focus-relations');
      var clearBtn = document.getElementById('btn-focus-clear');
      var activeIds = [];
      var hoveredRelationship = null;
      var focusedRelationship = null;
      var pinnedRelationship = null;
      var pinnedRelationshipKey = null;
      var activeRelationshipPreview = null;
      var relationshipHitOverlay = null;
      var relationshipHitTargets = [];
      var directPreviewTimer = null;
      var reachabilityMode = null;
      var activeReachability = null;
      var svgNamespace = 'http://www.w3.org/2000/svg';
      var reducedMotionQuery = window.matchMedia ? window.matchMedia('(prefers-reduced-motion: reduce)') : null;
      var finePointerQuery = window.matchMedia ? window.matchMedia('(hover: hover) and (pointer: fine)') : null;

      function nodes() {
        return Array.prototype.slice.call(svg.querySelectorAll('[data-node-id]'));
      }
      function edges() {
        return Array.prototype.slice.call(svg.querySelectorAll('[data-edge-from][data-edge-to]'));
      }
      function nodeLabel(node, fallback) {
        return node.getAttribute('data-node-label') || (node.getAttribute('aria-label') || fallback).replace(/^Focus\s+/, '');
      }
      function reachabilityRelationships() {
        var seen = Object.create(null);
        var relationships = [];
        edges().forEach(function (edge) {
          var from = edge.getAttribute('data-edge-from');
          var to = edge.getAttribute('data-edge-to');
          var key = edge.getAttribute('data-edge-key') || (from + '\u0000' + to + '\u0000' + (edge.getAttribute('data-edge-label') || ''));
          if (!from || !to || seen[key]) return;
          seen[key] = true;
          relationships.push({ key: key, from: from, to: to });
        });
        return relationships;
      }
      function computeReachability(originId, direction, relationships) {
        if (typeof originId !== 'string' || !originId ||
            (direction !== 'upstream' && direction !== 'downstream') ||
            !Array.isArray(relationships)) return null;
        var records = [];
        var seenKeys = Object.create(null);
        relationships.forEach(function (relationship, index) {
          if (!relationship || typeof relationship.from !== 'string' || !relationship.from ||
              typeof relationship.to !== 'string' || !relationship.to) return;
          var key = typeof relationship.key === 'string' && relationship.key
            ? relationship.key : String(index);
          if (seenKeys[key]) return;
          seenKeys[key] = true;
          records.push({ key: key, from: relationship.from, to: relationship.to });
        });

        var depths = Object.create(null);
        var order = [];
        var queue = [originId];
        depths[originId] = 0;
        for (var cursor = 0; cursor < queue.length; cursor += 1) {
          var current = queue[cursor];
          records.forEach(function (relationship) {
            var next = null;
            if (direction === 'downstream' && relationship.from === current) next = relationship.to;
            else if (direction === 'upstream' && relationship.to === current) next = relationship.from;
            if (next == null || Object.prototype.hasOwnProperty.call(depths, next)) return;
            depths[next] = depths[current] + 1;
            queue.push(next);
          });
        }
        queue.forEach(function (id) { order.push(id); });

        var edgeKeys = [];
        records.forEach(function (relationship) {
          if (Object.prototype.hasOwnProperty.call(depths, relationship.from) &&
              Object.prototype.hasOwnProperty.call(depths, relationship.to)) edgeKeys.push(relationship.key);
        });
        var maxDepth = order.reduce(function (maximum, id) {
          return Math.max(maximum, depths[id]);
        }, 0);
        return {
          direction: direction,
          originId: originId,
          nodeIds: order,
          edgeKeys: edgeKeys,
          depths: depths,
          maxDepth: maxDepth
        };
      }
      function reachabilityFor(id, direction) {
        return computeReachability(id, direction, reachabilityRelationships());
      }
      function resetReachabilityButtons() {
        upstreamBtn.setAttribute('aria-pressed', 'false');
        downstreamBtn.setAttribute('aria-pressed', 'false');
      }
      function clearReachability(options) {
        options = options || {};
        reachabilityMode = null;
        activeReachability = null;
        svg.removeAttribute('data-reach-active');
        chip.removeAttribute('data-reach-mode');
        nodes().forEach(function (node) {
          node.removeAttribute('data-reach-match');
          node.removeAttribute('data-reach-origin');
          node.removeAttribute('data-reach-depth');
        });
        edges().forEach(function (edge) {
          edge.removeAttribute('data-reach-match');
          edge.removeAttribute('data-reach-depth');
        });
        resetReachabilityButtons();
        reachStatus.textContent = '';
        reachStatus.hidden = true;
        if (Mosofin.exportMenu && typeof Mosofin.exportMenu.syncReachShare === 'function') {
          Mosofin.exportMenu.syncReachShare();
        }
        if (options.updateUrl === true && activeIds.length === 1) {
          try {
            history.replaceState(null, '', location.pathname + location.search + '#focus=' + encodeURIComponent(activeIds[0]));
          } catch (_) {}
        }
      }
      function renderReachabilityControls(id) {
        var upstream = reachabilityFor(id, 'upstream');
        var downstream = reachabilityFor(id, 'downstream');
        var upstreamReach = upstream ? Math.max(0, upstream.nodeIds.length - 1) : 0;
        var downstreamReach = downstream ? Math.max(0, downstream.nodeIds.length - 1) : 0;
        upstreamCount.textContent = String(upstreamReach);
        downstreamCount.textContent = String(downstreamReach);
        upstreamBtn.disabled = upstreamReach === 0;
        downstreamBtn.disabled = downstreamReach === 0;
        upstreamBtn.setAttribute('aria-label', upstreamReach
          ? viewerCount('viewer.passport.reach.upstream', upstreamReach)
          : viewerText('viewer.passport.reach.noUpstream'));
        downstreamBtn.setAttribute('aria-label', downstreamReach
          ? viewerCount('viewer.passport.reach.downstream', downstreamReach)
          : viewerText('viewer.passport.reach.noDownstream'));
        reachSection.hidden = false;
      }
      function applyReachability(direction, options) {
        options = options || {};
        if (activeIds.length !== 1 || (direction !== 'upstream' && direction !== 'downstream')) return false;
        if (reachabilityMode === direction && options.toggle !== false) {
          clearReachability({ updateUrl: options.updateUrl !== false });
          return true;
        }
        var result = reachabilityFor(activeIds[0], direction);
        if (!result || result.nodeIds.length <= 1) return false;
        if (Mosofin.guidedViews && typeof Mosofin.guidedViews.showAll === 'function') {
          Mosofin.guidedViews.showAll({ clearFocus: false, updateUrl: false, resetView: false });
        }
        clearRelationshipPreview({ clearPin: true });
        clearReachability({ updateUrl: false });
        reachabilityMode = direction;
        activeReachability = result;
        var edgeKeySet = Object.create(null);
        result.edgeKeys.forEach(function (key) { edgeKeySet[key] = true; });
        svg.setAttribute('data-reach-active', direction);
        chip.setAttribute('data-reach-mode', direction);
        nodes().forEach(function (node) {
          var id = node.getAttribute('data-node-id');
          if (!Object.prototype.hasOwnProperty.call(result.depths, id)) return;
          node.setAttribute('data-reach-match', '');
          node.setAttribute('data-reach-depth', String(result.depths[id]));
          if (id === result.originId) node.setAttribute('data-reach-origin', '');
        });
        edges().forEach(function (edge) {
          var key = edge.getAttribute('data-edge-key') || (
            edge.getAttribute('data-edge-from') + '\u0000' +
            edge.getAttribute('data-edge-to') + '\u0000' +
            (edge.getAttribute('data-edge-label') || '')
          );
          if (!edgeKeySet[key]) return;
          edge.setAttribute('data-reach-match', '');
          var fromDepth = result.depths[edge.getAttribute('data-edge-from')];
          var toDepth = result.depths[edge.getAttribute('data-edge-to')];
          edge.setAttribute('data-reach-depth', String(Math.max(fromDepth || 0, toDepth || 0)));
        });
        resetReachabilityButtons();
        var activeButton = direction === 'upstream' ? upstreamBtn : downstreamBtn;
        activeButton.setAttribute('aria-pressed', 'true');
        var reachableCount = result.nodeIds.length - 1;
        var directionLabel = viewerText(direction === 'upstream'
          ? 'viewer.passport.upstream'
          : 'viewer.passport.downstream');
        reachStatus.textContent = viewerText('viewer.passport.reach.status', {
          direction: directionLabel,
          nodes: reachableCount,
          links: result.edgeKeys.length,
          hops: result.maxDepth
        });
        reachStatus.hidden = false;
        if (Mosofin.exportMenu && typeof Mosofin.exportMenu.syncReachShare === 'function') {
          Mosofin.exportMenu.syncReachShare();
        }
        if (options.updateUrl !== false) {
          try {
            history.replaceState(null, '', location.pathname + location.search + '#focus=' +
              encodeURIComponent(activeIds[0]) + '&reach=' + direction);
          } catch (_) {}
        }
        if (options.reveal !== false && Mosofin.view && typeof Mosofin.view.reveal === 'function') {
          Mosofin.view.reveal(result.nodeIds, { includeNeighbors: false, reason: 'reachability' });
        }
        requestLensPlacement();
        return true;
      }
      function reachabilitySnapshot() {
        if (!activeReachability || activeIds.length !== 1 ||
            (reachabilityMode !== 'upstream' && reachabilityMode !== 'downstream') ||
            activeReachability.direction !== reachabilityMode ||
            activeReachability.originId !== activeIds[0] ||
            svg.getAttribute('data-reach-active') !== reachabilityMode) return null;

        var originId = activeReachability.originId;
        var nodeIds = activeReachability.nodeIds.slice();
        var edgeKeys = activeReachability.edgeKeys.slice();
        if (nodeIds.length < 2 || nodeIds[0] !== originId || !edgeKeys.length ||
            !activeReachability.depths || !Number.isInteger(activeReachability.maxDepth) ||
            activeReachability.maxDepth < 1) return null;

        var allNodes = nodes();
        var allEdges = edges();
        var seenNodeIds = Object.create(null);
        var seenEdgeKeys = Object.create(null);
        var nodeIdSet = Object.create(null);
        var depths = Object.create(null);
        var originNode = null;
        var measuredMaxDepth = 0;

        if (nodeIds.some(function (id) {
          var depth = activeReachability.depths[id];
          var matches = allNodes.filter(function (node) {
            return node.getAttribute('data-node-id') === id;
          });
          if (typeof id !== 'string' || !id || seenNodeIds[id] || matches.length !== 1 ||
              !matches[0].hasAttribute('data-reach-match') ||
              !Number.isInteger(depth) || depth < 0 || depth > activeReachability.maxDepth ||
              (id === originId
                ? (!matches[0].hasAttribute('data-reach-origin') || depth !== 0)
                : depth < 1)) return true;
          seenNodeIds[id] = true;
          nodeIdSet[id] = true;
          depths[id] = depth;
          measuredMaxDepth = Math.max(measuredMaxDepth, depth);
          if (id === originId) originNode = matches[0];
          return false;
        }) || !originNode || measuredMaxDepth !== activeReachability.maxDepth ||
            allNodes.filter(function (node) { return node.hasAttribute('data-reach-match'); }).length !== nodeIds.length) return null;

        var edgeRecords = [];
        if (edgeKeys.some(function (key) {
          if (typeof key !== 'string' || !key || seenEdgeKeys[key]) return true;
          var fragments = allEdges.filter(function (edge) {
            return edge.getAttribute('data-edge-key') === key;
          });
          var drawableFragments = fragments.filter(hasDrawableGeometry);
          if (!fragments.length || drawableFragments.length !== 1 ||
              !fragments.every(function (fragment) { return fragment.hasAttribute('data-reach-match'); })) return true;
          var first = fragments[0];
          var from = first.getAttribute('data-edge-from');
          var to = first.getAttribute('data-edge-to');
          var id = first.getAttribute('data-edge-id') || '';
          var labelValue = first.getAttribute('data-edge-label') || '';
          if (!nodeIdSet[from] || !nodeIdSet[to] || !fragments.every(function (fragment) {
            return fragment.getAttribute('data-edge-from') === from &&
              fragment.getAttribute('data-edge-to') === to &&
              (fragment.getAttribute('data-edge-id') || '') === id;
          })) return true;
          seenEdgeKeys[key] = true;
          edgeRecords.push({
            key: key,
            id: id,
            from: from,
            to: to,
            label: labelValue,
            depth: Math.max(depths[from], depths[to])
          });
          return false;
        })) return null;

        var liveEdgeKeys = Object.create(null);
        if (allEdges.some(function (edge) {
          if (!edge.hasAttribute('data-reach-match')) return false;
          var key = edge.getAttribute('data-edge-key');
          if (!key || !seenEdgeKeys[key]) return true;
          liveEdgeKeys[key] = true;
          return false;
        }) || Object.keys(liveEdgeKeys).length !== edgeKeys.length) return null;

        return {
          direction: reachabilityMode,
          origin: { id: originId, label: nodeLabel(originNode, originId) },
          nodeIds: nodeIds,
          depths: depths,
          maxDepth: activeReachability.maxDepth,
          edges: edgeRecords
        };
      }
      function setPassportValue(element, value) {
        var normalized = value == null ? '' : String(value).trim();
        element.textContent = normalized;
        element.hidden = !normalized;
      }
      function renderSourceEvidence(id) {
        evidenceLinks.textContent = '';
        repositoryLink.removeAttribute('href');
        repositoryLink.textContent = '';
        var sources = Mosofin.sourceEvidence.node(id);
        var repository = Mosofin.sourceEvidence.repository();
        if (!repository || !sources.length) {
          evidence.hidden = true;
          return;
        }
        var slug = repository.url.replace(/^https:\/\/github\.com\//, '').replace(/\/$/, '');
        repositoryLink.href = repository.url + '/tree/' + repository.revision;
        repositoryLink.textContent = slug + ' @ ' + repository.shortRevision;
        repositoryLink.setAttribute('aria-label', viewerText('viewer.passport.repository.open', { revision: repository.revision }));
        sources.forEach(function (source) {
          var link = document.createElement('a');
          link.className = 'semantic-passport-source';
          link.href = source.href;
          link.target = '_blank';
          link.rel = 'noopener noreferrer';
          link.referrerPolicy = 'no-referrer';
          link.setAttribute('aria-label', viewerText('viewer.passport.source.open', { path: source.path, revision: repository.shortRevision }));
          var name = document.createElement('strong');
          name.textContent = source.label || source.path.split('/').pop() || source.path;
          var location = document.createElement('code');
          location.textContent = source.line
            ? 'L' + source.line + (source.endLine && source.endLine !== source.line ? '–' + source.endLine : '') + ' ↗'
            : viewerText('viewer.passport.source.openLink');
          var sourcePath = document.createElement('small');
          sourcePath.textContent = source.path;
          link.appendChild(name);
          link.appendChild(location);
          link.appendChild(sourcePath);
          evidenceLinks.appendChild(link);
        });
        evidence.hidden = false;
      }
      function renderPassport(id, node) {
        setPassportValue(detail, node.getAttribute('data-node-sublabel'));
        setPassportValue(kind, viewerKindLabel(node.getAttribute('data-node-kind') || 'node'));
        setPassportValue(context, node.getAttribute('data-node-context'));
        setPassportValue(tag, node.getAttribute('data-node-tag'));
        setPassportValue(document.getElementById('focus-brand'), node.getAttribute('data-node-brand'));
        semanticId.textContent = id;
        semanticId.hidden = false;
        renderSourceEvidence(id);
      }
      function relationshipsFor(id, byId) {
        var seen = {};
        var relationships = [];
        edges().forEach(function (edge) {
          var from = edge.getAttribute('data-edge-from');
          var to = edge.getAttribute('data-edge-to');
          if (from !== id && to !== id) return;
          var edgeLabel = edge.getAttribute('data-edge-label') || '';
          var edgeKey = edge.getAttribute('data-edge-key') || (from + '\u0000' + to + '\u0000' + edgeLabel);
          var edgeId = edge.getAttribute('data-edge-id') || '';
          if (seen[edgeKey]) return;
          seen[edgeKey] = true;
          var direction = from === id && to === id ? 'loop' : (from === id ? 'out' : 'in');
          var neighborId = direction === 'in' ? from : to;
          var neighbor = byId[neighborId];
          relationships.push({
            key: edgeKey,
            id: edgeId,
            from: from,
            to: to,
            direction: direction,
            neighborId: neighborId,
            neighborLabel: neighbor ? nodeLabel(neighbor, neighborId) : neighborId,
            label: edgeLabel || viewerText(direction === 'loop'
              ? 'viewer.passport.relationship.loopsBack'
              : direction === 'out'
                ? 'viewer.passport.relationship.connectsTo'
                : 'viewer.passport.relationship.connectsFrom')
          });
        });
        return relationships;
      }
      function relationshipEdgeShapes(edge) {
        if (!edge) return [];
        if (/^(path|line|polyline)$/i.test(edge.tagName || '')) return [edge];
        return Array.prototype.slice.call(edge.querySelectorAll('path, line, polyline'));
      }
      function relationshipHitRecords() {
        var recordsByKey = {};
        var recordsById = {};
        var byId = {};
        nodes().forEach(function (node) { byId[node.getAttribute('data-node-id')] = node; });
        var records = [];
        edges().forEach(function (edge) {
          var shapes = relationshipEdgeShapes(edge);
          if (!shapes.length) return;
          var from = edge.getAttribute('data-edge-from');
          var to = edge.getAttribute('data-edge-to');
          var labelValue = edge.getAttribute('data-edge-label') || '';
          var edgeId = edge.getAttribute('data-edge-id') || '';
          var key = edge.getAttribute('data-edge-key') || (from + '\u0000' + to + '\u0000' + labelValue);
          var existing = recordsByKey[key];
          if (existing) {
            if (existing.from !== from || existing.to !== to || existing.labelValue !== labelValue || existing.id !== edgeId) {
              existing.invalid = true;
              return;
            }
            shapes.forEach(function (shape) {
              if (existing.shapes.indexOf(shape) === -1) existing.shapes.push(shape);
            });
            return;
          }
          var record = {
            key: key,
            id: edgeId,
            from: from,
            to: to,
            fromLabel: byId[from] ? nodeLabel(byId[from], from) : from,
            toLabel: byId[to] ? nodeLabel(byId[to], to) : to,
            labelValue: labelValue,
            label: labelValue || viewerText(from === to
              ? 'viewer.passport.relationship.loopsBack'
              : 'viewer.passport.relationship.connectsTo'),
            edge: edge,
            shapes: shapes,
            invalid: false
          };
          recordsByKey[key] = record;
          if (edgeId) {
            if (recordsById[edgeId]) {
              recordsById[edgeId].invalid = true;
              record.invalid = true;
            } else {
              recordsById[edgeId] = record;
            }
          }
          records.push(record);
        });
        return records.filter(function (record) { return !record.invalid && record.from && record.to; });
      }
      function relationshipRecordForKey(key) {
        return relationshipHitRecords().find(function (record) { return record.key === key; }) || null;
      }
      function pinnedRelationshipRecord() {
        return pinnedRelationshipKey ? relationshipRecordForKey(pinnedRelationshipKey) : null;
      }
      function renderRelationshipCopyAction() {
        var record = pinnedRelationshipRecord();
        if (record && record.id) {
          copyBtn.textContent = viewerText('viewer.passport.copyRelation');
          copyBtn.setAttribute('aria-label', viewerText('viewer.passport.copyPinned'));
        } else if (pinnedRelationshipKey) {
          copyBtn.textContent = viewerText('viewer.passport.copyNode');
          copyBtn.setAttribute('aria-label', viewerText('viewer.passport.copySource'));
        } else {
          copyBtn.textContent = viewerText('viewer.passport.copy');
          copyBtn.setAttribute('aria-label', viewerText('viewer.passport.copy.focus'));
        }
      }
      function relationshipHitGeometry(shape, className) {
        var clone = shape.cloneNode(false);
        clone.removeAttribute('id');
        clone.removeAttribute('class');
        clone.removeAttribute('style');
        clone.removeAttribute('filter');
        clone.removeAttribute('marker-start');
        clone.removeAttribute('marker-mid');
        clone.removeAttribute('marker-end');
        clone.removeAttribute('role');
        clone.removeAttribute('tabindex');
        clone.removeAttribute('aria-label');
        clone.removeAttribute('aria-labelledby');
        clone.removeAttribute('aria-hidden');
        clone.removeAttribute('data-animate');
        clone.removeAttribute('data-edge-from');
        clone.removeAttribute('data-edge-to');
        clone.removeAttribute('data-edge-key');
        clone.removeAttribute('data-edge-id');
        clone.removeAttribute('data-edge-label');
        clone.setAttribute('class', className || 'relationship-hit-rail');
        return clone;
      }
      function removeRelationshipPulse() {
        Array.prototype.forEach.call(svg.querySelectorAll('[data-relationship-pulse-overlay]'), function (element) {
          element.remove();
        });
      }
      function relationshipPulseGeometry(shape) {
        var clone = shape.cloneNode(false);
        clone.removeAttribute('id');
        clone.removeAttribute('class');
        clone.removeAttribute('style');
        clone.removeAttribute('transform');
        clone.removeAttribute('filter');
        clone.removeAttribute('marker-start');
        clone.removeAttribute('marker-mid');
        clone.removeAttribute('marker-end');
        clone.removeAttribute('role');
        clone.removeAttribute('tabindex');
        clone.removeAttribute('aria-label');
        clone.removeAttribute('aria-hidden');
        clone.removeAttribute('data-animate');
        clone.removeAttribute('data-edge-from');
        clone.removeAttribute('data-edge-to');
        clone.removeAttribute('data-edge-key');
        clone.removeAttribute('data-edge-id');
        clone.removeAttribute('data-edge-label');
        clone.removeAttribute('data-focus-match');
        clone.removeAttribute('data-relationship-preview');
        clone.setAttribute('class', 'relationship-flow-pulse');
        clone.setAttribute('pathLength', '1');
        return clone;
      }
      function relationshipTokenKind(edge) {
        var shapes = relationshipEdgeShapes(edge);
        var classEvidence = [edge.getAttribute('class') || ''].concat(shapes.map(function (shape) {
          return shape.getAttribute('class') || '';
        })).join(' ');
        var from = edge.getAttribute('data-edge-from') || '';
        var to = edge.getAttribute('data-edge-to') || '';
        var source = nodes().find(function (node) { return node.getAttribute('data-node-id') === from; });
        var target = nodes().find(function (node) { return node.getAttribute('data-node-id') === to; });
        var sourceKind = source ? source.getAttribute('data-node-kind') || 'neutral' : 'neutral';
        var targetKind = target ? target.getAttribute('data-node-kind') || 'neutral' : 'neutral';
        if (/\ba-security\b/.test(classEvidence) || sourceKind === 'security' || targetKind === 'security' || targetKind === 'failure') return 'security';
        if (/\ba-dashed\b/.test(classEvidence) || sourceKind === 'messagebus' || targetKind === 'messagebus') return 'event';
        if (sourceKind === 'database' || targetKind === 'database') return 'data';
        if (targetKind === 'waiting' || targetKind === 'success') return 'state';
        return 'call';
      }
      function relationshipTokenPath(shape) {
        if (!shape) return '';
        var tagName = String(shape.tagName || '').toLowerCase();
        if (tagName === 'path') return shape.getAttribute('d') || '';
        if (tagName === 'line') {
          return 'M ' + shape.getAttribute('x1') + ' ' + shape.getAttribute('y1') +
            ' L ' + shape.getAttribute('x2') + ' ' + shape.getAttribute('y2');
        }
        if (tagName === 'polyline' && shape.points && shape.points.numberOfItems > 1) {
          var commands = [];
          for (var i = 0; i < shape.points.numberOfItems; i += 1) {
            var point = shape.points.getItem(i);
            commands.push((i === 0 ? 'M ' : 'L ') + point.x + ' ' + point.y);
          }
          return commands.join(' ');
        }
        return '';
      }
      function relationshipTokenPart(tagName, className, attrs) {
        var part = document.createElementNS(svgNamespace, tagName);
        part.setAttribute('class', className);
        Object.keys(attrs || {}).forEach(function (name) { part.setAttribute(name, attrs[name]); });
        return part;
      }
      function relationshipTokenGeometry(shape, kind, key, options) {
        options = options || {};
        var pathData = relationshipTokenPath(shape);
        if (!pathData) return null;
        var token = document.createElementNS(svgNamespace, 'g');
        token.setAttribute('class', 'semantic-flow-token ' + (options.className || 'relationship-flow-token'));
        token.setAttribute('data-token-kind', kind);
        token.setAttribute('data-token-edge-key', key);
        token.setAttribute('aria-hidden', 'true');
        token.appendChild(relationshipTokenPart('circle', 'semantic-flow-token-halo', { cx: '0', cy: '0', r: '7' }));
        if (kind === 'data') {
          token.appendChild(relationshipTokenPart('rect', 'relationship-flow-token-shape', { x: '-5', y: '-4', width: '10', height: '8', rx: '2' }));
          token.appendChild(relationshipTokenPart('path', 'relationship-flow-token-ink', { d: 'M -2.8 -1.2 h 5.6 M -2.8 1.4 h 3.8' }));
        } else if (kind === 'event') {
          token.appendChild(relationshipTokenPart('rect', 'relationship-flow-token-shape', { x: '-7', y: '-3', width: '4', height: '6', rx: '1' }));
          token.appendChild(relationshipTokenPart('rect', 'relationship-flow-token-shape', { x: '-2', y: '-3', width: '4', height: '6', rx: '1' }));
          token.appendChild(relationshipTokenPart('rect', 'relationship-flow-token-shape', { x: '3', y: '-3', width: '4', height: '6', rx: '1' }));
        } else if (kind === 'security') {
          token.appendChild(relationshipTokenPart('path', 'relationship-flow-token-shape', { d: 'M 0 -5 L 4 -3.4 V 0 c 0 3 -1.6 4.5 -4 5.5 C -2.4 4.5 -4 3 -4 0 v -3.4 Z' }));
          token.appendChild(relationshipTokenPart('path', 'relationship-flow-token-ink', { d: 'm -2 .2 1.4 1.4 L 2 -1.4' }));
        } else if (kind === 'state') {
          token.appendChild(relationshipTokenPart('circle', 'relationship-flow-token-shape', { cx: '0', cy: '0', r: '5' }));
          token.appendChild(relationshipTokenPart('circle', 'relationship-flow-token-dot', { cx: '0', cy: '0', r: '1.35' }));
        } else {
          token.appendChild(relationshipTokenPart('path', 'relationship-flow-token-ink', { d: 'M -5 -3 L -1 0 L -5 3 M 0 -3 L 4 0 L 0 3' }));
        }
        var motion = document.createElementNS(svgNamespace, 'animateMotion');
        motion.setAttribute('path', pathData);
        motion.setAttribute('dur', options.duration || '1.2s');
        motion.setAttribute('begin', '0s');
        motion.setAttribute('fill', 'freeze');
        motion.setAttribute('rotate', 'auto');
        motion.setAttribute('calcMode', 'spline');
        motion.setAttribute('keyTimes', '0;1');
        motion.setAttribute('keySplines', '.2 0 .2 1');
        token.appendChild(motion);
        return token;
      }
      function createSemanticFlowToken(edge, shape, options) {
        if (!edge || !shape) return null;
        var key = edge.getAttribute('data-edge-key') || (
          edge.getAttribute('data-edge-from') + '\u0000' +
          edge.getAttribute('data-edge-to') + '\u0000' +
          (edge.getAttribute('data-edge-label') || '')
        );
        return relationshipTokenGeometry(shape, relationshipTokenKind(edge), key, options);
      }
      Mosofin.flowTokens = {
        create: createSemanticFlowToken,
        kind: function (edge) { return relationshipTokenKind(edge); },
        path: relationshipTokenPath
      };
      function renderRelationshipPulse(key) {
        removeRelationshipPulse();
        if (!key || document.documentElement.getAttribute('data-embed') === 'true') return false;
        if (document.hidden || (Mosofin.motionGovernor && Mosofin.motionGovernor.isPaused())) return false;
        if (reducedMotionQuery && reducedMotionQuery.matches) return false;
        var matchingEdges = edges().filter(function (edge) {
          return edge.getAttribute('data-edge-key') === key;
        });
        if (!matchingEdges.length) return false;
        var overlay = document.createElementNS(svgNamespace, 'g');
        overlay.setAttribute('class', 'relationship-pulse-overlay');
        overlay.setAttribute('data-relationship-pulse-overlay', '');
        overlay.setAttribute('data-relationship-pulse-key', key);
        overlay.setAttribute('aria-hidden', 'true');
        var tokenAdded = false;
        matchingEdges.forEach(function (edge) {
          var wrapper = document.createElementNS(svgNamespace, 'g');
          if (edge.hasAttribute('transform')) wrapper.setAttribute('transform', edge.getAttribute('transform'));
          var shapes = relationshipEdgeShapes(edge);
          shapes.forEach(function (shape) {
            wrapper.appendChild(relationshipPulseGeometry(shape));
          });
          if (!tokenAdded && shapes.length) {
            var tokenKind = relationshipTokenKind(edge);
            var token = relationshipTokenGeometry(shapes[0], tokenKind, key);
            if (token) {
              wrapper.appendChild(token);
              overlay.setAttribute('data-relationship-token-kind', tokenKind);
              tokenAdded = true;
            }
          }
          if (wrapper.childNodes.length) overlay.appendChild(wrapper);
        });
        if (!overlay.childNodes.length) return false;
        var finishPulse = function () {
          if (overlay.parentNode) overlay.remove();
        };
        overlay.addEventListener('animationend', finishPulse, { once: true });
        overlay.addEventListener('animationcancel', finishPulse, { once: true });
        var firstNode = svg.querySelector('[data-node-id]');
        if (firstNode) svg.insertBefore(overlay, firstNode);
        else svg.appendChild(overlay);
        return true;
      }
      function clearRelationshipPreview(options) {
        options = options || {};
        if (directPreviewTimer) window.clearTimeout(directPreviewTimer);
        directPreviewTimer = null;
        removeRelationshipPulse();
        activeRelationshipPreview = null;
        svg.removeAttribute('data-relationship-preview-active');
        svg.removeAttribute('data-relationship-direct-active');
        if (options.clearPin === true) {
          pinnedRelationship = null;
          pinnedRelationshipKey = null;
          svg.removeAttribute('data-relationship-pin-active');
        }
        chip.removeAttribute('data-relationship-previewing');
        edges().forEach(function (edge) { edge.removeAttribute('data-relationship-preview'); });
        nodes().forEach(function (node) {
          node.removeAttribute('data-relationship-preview-node');
          node.removeAttribute('data-relationship-preview-source');
          node.removeAttribute('data-relationship-preview-target');
        });
        Array.prototype.forEach.call(relationshipList.querySelectorAll('[data-preview-active]'), function (button) {
          button.removeAttribute('data-preview-active');
        });
        relationshipHitTargets.forEach(function (target) {
          target.setAttribute('aria-pressed', pinnedRelationshipKey && target.getAttribute('data-relationship-key') === pinnedRelationshipKey ? 'true' : 'false');
          target.removeAttribute('data-preview-active');
        });
        renderRelationshipCopyAction();
        requestLensPlacement();
      }
      function previewRelationship(button, options) {
        options = options || {};
        if (pinnedRelationshipKey && pinnedRelationship && button !== pinnedRelationship) return;
        clearRelationshipPreview();
        if (!button) return;
        var key = button.getAttribute('data-relationship-key');
        var from = button.getAttribute('data-relationship-from');
        var to = button.getAttribute('data-relationship-to');
        if (!key || !from || !to) return;
        svg.setAttribute('data-relationship-preview-active', key);
        if (options.direct === true) svg.setAttribute('data-relationship-direct-active', key);
        edges().forEach(function (edge) {
          if (edge.getAttribute('data-edge-key') === key) edge.setAttribute('data-relationship-preview', '');
        });
        nodes().forEach(function (node) {
          var id = node.getAttribute('data-node-id');
          if (id !== from && id !== to) return;
          node.setAttribute('data-relationship-preview-node', '');
          if (id === from) node.setAttribute('data-relationship-preview-source', '');
          if (id === to) node.setAttribute('data-relationship-preview-target', '');
        });
        button.setAttribute('data-preview-active', 'true');
        if (!chip.hidden && options.direct !== true) chip.setAttribute('data-relationship-previewing', 'true');
        activeRelationshipPreview = button;
        renderRelationshipPulse(key);
        requestLensPlacement();
      }
      function syncRelationshipPreview() {
        var next = pinnedRelationship || focusedRelationship || hoveredRelationship;
        if (next === activeRelationshipPreview) return;
        previewRelationship(next, { direct: !!(next && next.hasAttribute('data-relationship-hit-key')) });
      }
      function directRelationshipBlocked() {
        return html.getAttribute('data-embed') === 'true' ||
          html.getAttribute('data-guide-open') === 'true' ||
          container.classList.contains('is-panning') ||
          (activeIds.length > 0 && !pinnedRelationshipKey) ||
          svg.hasAttribute('data-story-active') ||
          svg.hasAttribute('data-route-picking') ||
          svg.hasAttribute('data-route-active') ||
          svg.hasAttribute('data-lens-active') ||
          svg.hasAttribute('data-chapter-preview');
      }
      function scheduleDirectRelationshipPreview(target) {
        if (directPreviewTimer) window.clearTimeout(directPreviewTimer);
        if (pinnedRelationshipKey) return;
        directPreviewTimer = window.setTimeout(function () {
          directPreviewTimer = null;
          if (pinnedRelationshipKey || hoveredRelationship !== target || directRelationshipBlocked()) return;
          previewRelationship(target, { direct: true });
        }, reducedMotionQuery && reducedMotionQuery.matches ? 0 : 90);
      }
      function relationshipHitTarget(key) {
        return relationshipHitTargets.find(function (target) {
          return target.getAttribute('data-relationship-key') === key;
        }) || null;
      }
      function revealPinnedRelationship(record) {
        function reveal() {
          if (!record || pinnedRelationshipKey !== record.key) return true;
          if (!Mosofin.view || typeof Mosofin.view.reveal !== 'function') return false;
          Mosofin.view.reveal([record.from, record.to], { reason: 'relationship-direct' });
          return true;
        }
        if (!reveal()) requestAnimationFrame(reveal);
      }
      function inspectRelationship(key, options) {
        options = options || {};
        if (html.getAttribute('data-embed') === 'true') return false;
        if (directPreviewTimer) window.clearTimeout(directPreviewTimer);
        directPreviewTimer = null;
        hoveredRelationship = null;
        focusedRelationship = null;
        if (pinnedRelationshipKey === key) {
          if (options.toggle === false) return true;
          clear({ updateUrl: options.updateUrl !== false });
          return true;
        }
        var record = relationshipRecordForKey(key);
        if (!record) return false;
        if (Mosofin.guidedViews && typeof Mosofin.guidedViews.showAll === 'function') {
          Mosofin.guidedViews.showAll({ clearFocus: false, updateUrl: false });
        }
        set(record.from, { toggle: false, updateUrl: false });
        var row = Array.prototype.slice.call(relationshipList.querySelectorAll('[data-relationship-key]')).find(function (candidate) {
          return candidate.getAttribute('data-relationship-key') === key;
        });
        if (!row) return false;
        previewRelationship(row);
        pinnedRelationship = row;
        pinnedRelationshipKey = key;
        svg.setAttribute('data-relationship-pin-active', key);
        var target = relationshipHitTarget(key);
        if (target) target.setAttribute('aria-pressed', 'true');
        renderRelationshipCopyAction();
        summary.textContent = viewerText('viewer.passport.relationship.pinned', {
          from: record.fromLabel,
          to: record.toLabel,
          label: record.label
        });
        revealPinnedRelationship(record);
        if (options.updateUrl !== false && record.id) {
          try { history.replaceState(null, '', location.pathname + location.search + '#relation=' + encodeURIComponent(record.id)); } catch (_) {}
        }
        return true;
      }
      function inspectRelationshipById(id, options) {
        var record = relationshipHitRecords().find(function (item) { return item.id === id; });
        return record ? inspectRelationship(record.key, options) : false;
      }
      function installRelationshipHitTargets() {
        if (html.getAttribute('data-embed') === 'true') return 0;
        var records = relationshipHitRecords();
        if (!records.length) return 0;
        relationshipHitOverlay = document.createElementNS(svgNamespace, 'g');
        relationshipHitOverlay.setAttribute('class', 'relationship-hit-overlay');
        relationshipHitOverlay.setAttribute('data-relationship-hit-overlay', '');
        relationshipHitOverlay.setAttribute('role', 'group');
        relationshipHitOverlay.setAttribute('aria-label', viewerText('viewer.passport.relationship.explorer'));
        var relationshipHelp = document.createElementNS(svgNamespace, 'desc');
        relationshipHelp.id = 'mosofin-relationship-help';
        relationshipHelp.textContent = viewerText('viewer.passport.relationship.help');
        relationshipHitOverlay.appendChild(relationshipHelp);
        records.forEach(function (record, index) {
          var target = document.createElementNS(svgNamespace, 'g');
          target.setAttribute('class', 'relationship-hit-target');
          target.setAttribute('data-relationship-hit-key', record.key);
          target.setAttribute('data-relationship-key', record.key);
          target.setAttribute('data-relationship-from', record.from);
          target.setAttribute('data-relationship-to', record.to);
          if (record.id) target.setAttribute('data-relationship-id', record.id);
          target.setAttribute('role', 'button');
          target.setAttribute('tabindex', index === 0 ? '0' : '-1');
          target.setAttribute('aria-pressed', 'false');
          target.setAttribute('aria-describedby', relationshipHelp.id);
          var description = viewerText('viewer.passport.relationship.inspect', {
            index: index + 1,
            total: records.length,
            from: record.fromLabel,
            to: record.toLabel,
            label: record.label
          });
          target.setAttribute('aria-label', description);
          var title = document.createElementNS(svgNamespace, 'title');
          title.textContent = record.fromLabel + ' \u2192 ' + record.toLabel + ' \u00b7 ' + record.label;
          target.appendChild(title);
          record.shapes.forEach(function (shape) {
            target.appendChild(relationshipHitGeometry(shape));
            target.appendChild(relationshipHitGeometry(shape, 'relationship-focus-rail'));
          });
          if (target.childNodes.length > 1) {
            relationshipHitTargets.push(target);
            relationshipHitOverlay.appendChild(target);
          }
        });
        if (!relationshipHitTargets.length) return 0;
        var firstNode = svg.querySelector('[data-node-id]');
        var nodeLayer = firstNode;
        while (nodeLayer && nodeLayer.parentNode && nodeLayer.parentNode !== svg) nodeLayer = nodeLayer.parentNode;
        if (nodeLayer && nodeLayer.parentNode === svg) svg.insertBefore(relationshipHitOverlay, nodeLayer);
        else svg.appendChild(relationshipHitOverlay);

        relationshipHitOverlay.addEventListener('pointerdown', function (event) {
          if (event.target.closest('[data-relationship-hit-key]')) event.stopPropagation();
        });
        relationshipHitOverlay.addEventListener('pointerover', function (event) {
          if (event.pointerType === 'touch') return;
          if (finePointerQuery && !finePointerQuery.matches) return;
          var target = event.target.closest('[data-relationship-hit-key]');
          if (!target || directRelationshipBlocked() || pinnedRelationshipKey) return;
          if (event.relatedTarget && target.contains(event.relatedTarget)) return;
          hoveredRelationship = target;
          scheduleDirectRelationshipPreview(target);
        });
        relationshipHitOverlay.addEventListener('pointerout', function (event) {
          var target = event.target.closest('[data-relationship-hit-key]');
          if (!target || (event.relatedTarget && target.contains(event.relatedTarget))) return;
          if (hoveredRelationship === target) hoveredRelationship = null;
          syncRelationshipPreview();
        });
        relationshipHitOverlay.addEventListener('focusin', function (event) {
          var target = event.target.closest('[data-relationship-hit-key]');
          if (!target || directRelationshipBlocked()) return;
          focusedRelationship = target;
          if (!pinnedRelationshipKey) previewRelationship(target, { direct: true });
        });
        relationshipHitOverlay.addEventListener('focusout', function (event) {
          var target = event.target.closest('[data-relationship-hit-key]');
          if (!target || (event.relatedTarget && target.contains(event.relatedTarget))) return;
          if (focusedRelationship === target) focusedRelationship = null;
          syncRelationshipPreview();
        });
        relationshipHitOverlay.addEventListener('click', function (event) {
          var target = event.target.closest('[data-relationship-hit-key]');
          if (!target || directRelationshipBlocked()) return;
          event.preventDefault();
          event.stopPropagation();
          focusedRelationship = null;
          hoveredRelationship = null;
          inspectRelationship(target.getAttribute('data-relationship-key'));
        });
        relationshipHitOverlay.addEventListener('keydown', function (event) {
          var target = event.target.closest('[data-relationship-hit-key]');
          if (!target) return;
          if (event.key === 'Escape' && pinnedRelationshipKey) {
            event.preventDefault();
            clear({ updateUrl: false });
            return;
          }
          if (event.key === 'Enter' || event.key === ' ') {
            event.preventDefault();
            focusedRelationship = null;
            hoveredRelationship = null;
            inspectRelationship(target.getAttribute('data-relationship-key'));
            return;
          }
          if (event.key !== 'ArrowRight' && event.key !== 'ArrowLeft' && event.key !== 'ArrowDown' && event.key !== 'ArrowUp' && event.key !== 'Home' && event.key !== 'End') return;
          var index = relationshipHitTargets.indexOf(target);
          if (event.key === 'Home') index = 0;
          else if (event.key === 'End') index = relationshipHitTargets.length - 1;
          else if (event.key === 'ArrowRight' || event.key === 'ArrowDown') index = (index + 1) % relationshipHitTargets.length;
          else index = (index - 1 + relationshipHitTargets.length) % relationshipHitTargets.length;
          event.preventDefault();
          relationshipHitTargets.forEach(function (item, itemIndex) { item.setAttribute('tabindex', itemIndex === index ? '0' : '-1'); });
          try { relationshipHitTargets[index].focus({ preventScroll: true }); }
          catch (_) { try { relationshipHitTargets[index].focus(); } catch (_) {} }
        });
        return relationshipHitTargets.length;
      }
      function renderRelationshipLens(id, byId) {
        hoveredRelationship = null;
        focusedRelationship = null;
        clearRelationshipPreview({ clearPin: true });
        renderPassport(id, byId[id]);
        var relationships = relationshipsFor(id, byId);
        chip.removeAttribute('data-relations-expanded');
        relationsBtn.setAttribute('aria-expanded', 'false');
        relationsBtn.textContent = viewerCount('viewer.passport.relationship.count', relationships.length);
        relationsBtn.setAttribute('aria-label', viewerCount('viewer.passport.relationship.show', relationships.length));
        var counts = { out: 0, in: 0, loop: 0 };
        relationships.forEach(function (relationship) { counts[relationship.direction] += 1; });
        summary.textContent = viewerText('viewer.passport.relationship.summary', {
          out: counts.out,
          in: counts.in,
          loops: counts.loop ? viewerText('viewer.passport.relationship.loops', { count: counts.loop }) : ''
        });
        renderReachabilityControls(id);
        relationshipList.textContent = '';
        if (!relationships.length) {
          var empty = document.createElement('p');
          empty.className = 'relationship-lens-empty';
          empty.textContent = viewerText('viewer.passport.relationship.none');
          relationshipList.appendChild(empty);
          return;
        }

        [
          { id: 'out', label: viewerText('viewer.passport.relationship.group.out') },
          { id: 'in', label: viewerText('viewer.passport.relationship.group.in') },
          { id: 'loop', label: viewerText('viewer.passport.relationship.group.loop') }
        ].forEach(function (group) {
          var items = relationships.filter(function (relationship) { return relationship.direction === group.id; });
          if (!items.length) return;
          var section = document.createElement('div');
          section.className = 'relationship-lens-group';
          var heading = document.createElement('span');
          heading.className = 'relationship-lens-group-title';
          heading.textContent = group.label + ' · ' + items.length;
          section.appendChild(heading);
          items.forEach(function (relationship) {
            var button = document.createElement('button');
            button.type = 'button';
            button.className = 'relationship-lens-row';
            button.setAttribute('data-direction', relationship.direction);
            button.setAttribute('data-relationship-target', relationship.neighborId);
            button.setAttribute('data-relationship-key', relationship.key);
            button.setAttribute('data-relationship-from', relationship.from);
            button.setAttribute('data-relationship-to', relationship.to);
            if (relationship.id) button.setAttribute('data-relationship-id', relationship.id);
            button.setAttribute('aria-label', viewerText('viewer.passport.relationship.row', {
              group: group.label,
              relationship: relationship.label,
              neighbor: relationship.neighborLabel
            }));
            var direction = document.createElement('span');
            direction.className = 'relationship-lens-direction';
            direction.setAttribute('aria-hidden', 'true');
            direction.textContent = viewerText(relationship.direction === 'out'
              ? 'viewer.passport.relationship.direction.out'
              : relationship.direction === 'in'
                ? 'viewer.passport.relationship.direction.in'
                : 'viewer.passport.relationship.direction.loop');
            var target = document.createElement('strong');
            target.textContent = relationship.neighborLabel;
            var relation = document.createElement('small');
            relation.textContent = relationship.label;
            button.appendChild(direction);
            button.appendChild(target);
            button.appendChild(relation);
            section.appendChild(button);
          });
          relationshipList.appendChild(section);
        });
      }
      var lensFrame = 0;
      function placeRelationshipLens() {
        lensFrame = 0;
        if (chip.hidden || activeIds.length !== 1) return;
        var node = svg.querySelector('[data-node-id="' + activeIds[0] + '"]');
        if (!node) return;
        var containerRect = container.getBoundingClientRect();
        var nodeRect = node.getBoundingClientRect();
        if (containerRect.bottom <= 0 || containerRect.top >= window.innerHeight) return;
        var padding = window.innerWidth <= 720 ? 8 : 16;
        var visibleTop = Math.max(padding, -containerRect.top + padding);
        var visibleBottom = Math.min(containerRect.height - padding, window.innerHeight - containerRect.top - padding);
        var maxTop = Math.max(padding, visibleBottom - chip.offsetHeight);
        var minTop = Math.min(visibleTop, maxTop);
        var nodeCenter = nodeRect.top - containerRect.top + nodeRect.height / 2;
        var mobile = window.innerWidth <= 720;
        var previewingOnMobile = mobile && chip.getAttribute('data-relationship-previewing') === 'true';
        var compactOnMobile = mobile && chip.getAttribute('data-relations-expanded') !== 'true';
        var preferred;
        if (compactOnMobile) {
          var nodeTop = nodeRect.top - containerRect.top;
          var nodeBottom = nodeRect.bottom - containerRect.top;
          var gap = 10;
          var above = nodeTop - chip.offsetHeight - gap;
          var below = nodeBottom + gap;
          if (above >= visibleTop) preferred = above;
          else if (below + chip.offsetHeight <= visibleBottom - 56) preferred = below;
          else preferred = nodeCenter < (visibleTop + visibleBottom) / 2
            ? Math.max(minTop, visibleBottom - chip.offsetHeight - 56)
            : visibleTop;
        } else if (previewingOnMobile) {
          var pinnedTop = visibleTop;
          var pinnedBottom = Math.max(minTop, visibleBottom - chip.offsetHeight - 56);
          preferred = nodeCenter < (visibleTop + visibleBottom) / 2 ? pinnedBottom : pinnedTop;
        } else {
          preferred = nodeCenter - chip.offsetHeight / 2;
        }
        var top = Math.max(minTop, Math.min(maxTop, preferred));
        var chipRect = chip.getBoundingClientRect();
        var safeGap = 10;
        var protectViewerChrome = !mobile || compactOnMobile || previewingOnMobile;
        var protectedRects = (protectViewerChrome
          ? [svg.querySelector('[data-legend]'), container.querySelector('.diagram-nav')]
          : [])
          .filter(function (element) {
            if (!element || element.hidden) return false;
            var style = window.getComputedStyle(element);
            return style.display !== 'none' && style.visibility !== 'hidden';
          })
          .map(function (element) { return element.getBoundingClientRect(); })
          .filter(function (rect) {
            return rect.width > 0 && rect.height > 0 &&
              chipRect.left < rect.right + safeGap && chipRect.right > rect.left - safeGap;
          });
        if (protectedRects.length) {
          var candidates = [top, minTop, maxTop];
          protectedRects.forEach(function (rect) {
            candidates.push(
              rect.top - containerRect.top - chip.offsetHeight - safeGap,
              rect.bottom - containerRect.top + safeGap
            );
          });
          var valid = candidates.map(function (candidate) {
            return Math.max(minTop, Math.min(maxTop, candidate));
          }).filter(function (candidate, index, all) {
            if (all.indexOf(candidate) !== index) return false;
            var candidateTop = containerRect.top + candidate;
            var candidateBottom = candidateTop + chip.offsetHeight;
            return protectedRects.every(function (rect) {
              return candidateBottom <= rect.top - safeGap || candidateTop >= rect.bottom + safeGap;
            });
          });
          valid.sort(function (first, second) {
            return Math.abs(first - preferred) - Math.abs(second - preferred);
          });
          if (valid.length) top = valid[0];
        }
        chip.style.top = Math.round(top) + 'px';
        if (Mosofin.radar && typeof Mosofin.radar.sync === 'function') Mosofin.radar.sync();
      }
      function requestLensPlacement() {
        if (lensFrame) return;
        lensFrame = requestAnimationFrame(placeRelationshipLens);
      }
      function clear(options) {
        options = options || {};
        var restoreNode = options.restoreFocus === true && activeIds.length === 1
          ? svg.querySelector('[data-node-id="' + activeIds[0] + '"]')
          : null;
        clearReachability({ updateUrl: false });
        if (Mosofin.intentTrace && typeof Mosofin.intentTrace.clear === 'function') {
          Mosofin.intentTrace.clear({ announce: false });
        }
        hoveredRelationship = null;
        focusedRelationship = null;
        clearRelationshipPreview({ clearPin: true });
        activeIds = [];
        svg.removeAttribute('data-focus-active');
        nodes().forEach(function (node) {
          node.removeAttribute('data-focus-match');
          node.removeAttribute('data-focus-selected');
          node.setAttribute('aria-pressed', 'false');
        });
        edges().forEach(function (edge) { edge.removeAttribute('data-focus-match'); });
        chip.hidden = true;
        label.textContent = '';
        detail.textContent = '';
        detail.hidden = true;
        kind.textContent = '';
        kind.hidden = true;
        context.textContent = '';
        context.hidden = true;
        tag.textContent = '';
        tag.hidden = true;
        semanticId.textContent = '';
        semanticId.hidden = true;
        evidence.hidden = true;
        evidenceLinks.textContent = '';
        repositoryLink.removeAttribute('href');
        repositoryLink.textContent = '';
        summary.textContent = '';
        reachSection.hidden = true;
        upstreamCount.textContent = '0';
        downstreamCount.textContent = '0';
        upstreamBtn.disabled = true;
        downstreamBtn.disabled = true;
        relationshipList.textContent = '';
        copyBtn.textContent = viewerText('viewer.passport.copy');
        copyBtn.setAttribute('aria-label', viewerText('viewer.passport.copy.focus'));
        relationsBtn.textContent = viewerText('viewer.passport.relations');
        relationsBtn.setAttribute('aria-label', viewerText('viewer.passport.relations.show'));
        relationsBtn.setAttribute('aria-expanded', 'false');
        chip.removeAttribute('data-relations-expanded');
        chip.style.removeProperty('top');
        if (options.preserveView !== true && Mosofin.view && typeof Mosofin.view.reset === 'function') {
          Mosofin.view.reset({ automatic: true });
        }
        if (options.updateUrl !== false) {
          try { history.replaceState(null, '', location.pathname + location.search); } catch (_) {}
        }
        if (restoreNode) {
          try { restoreNode.focus({ preventScroll: true }); }
          catch (_) { try { restoreNode.focus(); } catch (_) {} }
        }
      }

      function setMany(ids, options) {
        options = options || {};
        if (Mosofin.semanticLens && typeof Mosofin.semanticLens.clearPreview === 'function') Mosofin.semanticLens.clearPreview();
        if (Mosofin.semanticLens && Mosofin.semanticLens.active()) {
          Mosofin.semanticLens.clear({ updateUrl: false, preserveView: true, closePanel: true });
        }
        if (options.preserveRoute !== true && Mosofin.routeProbe && typeof Mosofin.routeProbe.clear === 'function') {
          Mosofin.routeProbe.clear({ updateUrl: false, restoreFocus: false });
        }
        var nodeList = nodes();
        var byId = {};
        nodeList.forEach(function (node) { byId[node.getAttribute('data-node-id')] = node; });
        var normalized = [];
        (ids || []).forEach(function (id) {
          if (byId[id] && normalized.indexOf(id) === -1) normalized.push(id);
        });
        if (!normalized.length) return false;
        if (normalized.length === activeIds.length && normalized.every(function (id, index) { return activeIds[index] === id; }) && options.toggle !== false) {
          clear();
          return true;
        }

        clear({ updateUrl: false, preserveView: true });
        activeIds = normalized;
        var selected = {};
        var related = {};
        var seenEdges = {};
        var matchedEdges = 0;
        normalized.forEach(function (id) { selected[id] = true; related[id] = true; });
        var selectionMode = options.mode === 'selection' || normalized.length > 1;

        edges().forEach(function (edge) {
          var from = edge.getAttribute('data-edge-from');
          var to = edge.getAttribute('data-edge-to');
          var match = selectionMode ? selected[from] && selected[to] : selected[from] || selected[to];
          if (!match) return;
          edge.setAttribute('data-focus-match', '');
          if (!selectionMode) { related[from] = true; related[to] = true; }
          var edgeKey = edge.getAttribute('data-edge-key') || (from + '\u0000' + to + '\u0000' + (edge.getAttribute('data-edge-label') || ''));
          if (!seenEdges[edgeKey]) { seenEdges[edgeKey] = true; matchedEdges += 1; }
        });
        nodeList.forEach(function (node) {
          var nodeId = node.getAttribute('data-node-id');
          if (related[nodeId]) node.setAttribute('data-focus-match', '');
          if (selected[nodeId]) {
            node.setAttribute('data-focus-selected', '');
            node.setAttribute('aria-pressed', 'true');
          }
        });
        svg.setAttribute('data-focus-active', normalized.join(' '));
        var defaultLabel = normalized.length === 1
          ? nodeLabel(byId[normalized[0]], normalized[0])
          : viewerText('viewer.guided.chapter.selectedNodes', { count: normalized.length });
        label.textContent = options.label || defaultLabel;
        chip.hidden = options.hideChip === true || normalized.length !== 1 || selectionMode;
        if (!chip.hidden) {
          renderRelationshipLens(normalized[0], byId);
          requestLensPlacement();
        }
        if (options.updateUrl !== false) {
          var key = options.urlKey || 'focus';
          var value = options.urlValue || normalized[0];
          try { history.replaceState(null, '', location.pathname + location.search + '#' + key + '=' + encodeURIComponent(value)); } catch (_) {}
        }
        return true;
      }

      function set(id, options) {
        options = options || {};
        options.mode = 'neighborhood';
        return setMany([id], options);
      }

      function fallbackCopy(value) {
        var field = document.createElement('textarea');
        field.value = value;
        field.setAttribute('readonly', '');
        field.style.position = 'fixed';
        field.style.opacity = '0';
        document.body.appendChild(field);
        field.select();
        var copied = false;
        try { copied = document.execCommand('copy'); } catch (_) {}
        field.remove();
        return copied;
      }

      function copyFocusLink() {
        if (activeIds.length !== 1) return Promise.resolve(false);
        var record = pinnedRelationshipRecord();
        var relationId = record && record.id;
        var value = location.href.replace(/#.*$/, '') + (relationId
          ? '#relation=' + encodeURIComponent(relationId)
          : '#focus=' + encodeURIComponent(activeIds[0]) + (reachabilityMode ? '&reach=' + reachabilityMode : ''));
        var copy = navigator.clipboard && typeof navigator.clipboard.writeText === 'function'
          ? navigator.clipboard.writeText(value).then(function () { return true; }).catch(function () { return fallbackCopy(value); })
          : Promise.resolve(fallbackCopy(value));
        return copy.then(function (copied) {
          copyBtn.textContent = viewerText(copied ? 'viewer.common.copied' : 'viewer.common.copyFailed');
          copyBtn.setAttribute('aria-label', copied
            ? viewerText(relationId ? 'viewer.passport.copy.pinned.success' : 'viewer.passport.copy.focused.success')
            : viewerText(relationId ? 'viewer.passport.copy.pinned.failed' : 'viewer.passport.copy.focused.failed'));
          window.setTimeout(function () {
            renderRelationshipCopyAction();
          }, 1600);
          return copied;
        });
      }

      svg.addEventListener('click', function (event) {
        if (container.getAttribute('data-just-panned') === 'true') return;
        var node = event.target.closest('[data-node-id]');
        if (node) {
          var id = node.getAttribute('data-node-id');
          set(id);
          if (activeIds.indexOf(id) !== -1 && Mosofin.view && typeof Mosofin.view.reveal === 'function') {
            Mosofin.view.reveal([id], { includeNeighbors: true, reason: 'focus' });
          }
        }
        else if (activeIds.length) clear();
      });
      svg.addEventListener('keydown', function (event) {
        var node = event.target.closest('[data-node-id]');
        if (!node || (event.key !== 'Enter' && event.key !== ' ')) return;
        event.preventDefault();
        var id = node.getAttribute('data-node-id');
        set(id);
        if (activeIds.indexOf(id) !== -1 && Mosofin.view && typeof Mosofin.view.reveal === 'function') {
          Mosofin.view.reveal([id], { includeNeighbors: true, reason: 'focus' });
        }
      });
      clearBtn.addEventListener('click', function () { clear({ restoreFocus: true }); });
      copyBtn.addEventListener('click', copyFocusLink);
      upstreamBtn.addEventListener('click', function () { applyReachability('upstream'); });
      downstreamBtn.addEventListener('click', function () { applyReachability('downstream'); });
      relationsBtn.addEventListener('click', function () {
        var expanded = chip.getAttribute('data-relations-expanded') === 'true';
        if (expanded) chip.removeAttribute('data-relations-expanded');
        else chip.setAttribute('data-relations-expanded', 'true');
        relationsBtn.setAttribute('aria-expanded', expanded ? 'false' : 'true');
        relationsBtn.setAttribute('aria-label', viewerText(expanded
          ? 'viewer.passport.relations.show'
          : 'viewer.passport.relations.hide'));
        requestLensPlacement();
      });
      relationshipList.addEventListener('click', function (event) {
        var button = event.target.closest('[data-relationship-target]');
        if (!button) return;
        var id = button.getAttribute('data-relationship-target');
        if (Mosofin.guidedViews && typeof Mosofin.guidedViews.showAll === 'function') {
          Mosofin.guidedViews.showAll({ clearFocus: false, updateUrl: false });
        }
        set(id, { toggle: false });
        if (Mosofin.view && typeof Mosofin.view.reveal === 'function') {
          Mosofin.view.reveal([id], { includeNeighbors: true, reason: 'relationship' });
        }
        var node = svg.querySelector('[data-node-id="' + id + '"]');
        if (node) {
          try { node.focus({ preventScroll: true }); } catch (_) { try { node.focus(); } catch (_) {} }
        }
      });
      relationshipList.addEventListener('pointerover', function (event) {
        if (event.pointerType === 'touch') return;
        if (finePointerQuery && !finePointerQuery.matches) return;
        var button = event.target.closest('[data-relationship-key]');
        if (!button || !relationshipList.contains(button)) return;
        if (event.relatedTarget && button.contains(event.relatedTarget)) return;
        hoveredRelationship = button;
        syncRelationshipPreview();
      });
      relationshipList.addEventListener('pointerout', function (event) {
        var button = event.target.closest('[data-relationship-key]');
        if (!button || (event.relatedTarget && button.contains(event.relatedTarget))) return;
        if (hoveredRelationship === button) hoveredRelationship = null;
        syncRelationshipPreview();
      });
      relationshipList.addEventListener('focusin', function (event) {
        var button = event.target.closest('[data-relationship-key]');
        if (!button) return;
        focusedRelationship = button;
        syncRelationshipPreview();
      });
      relationshipList.addEventListener('focusout', function (event) {
        var button = event.target.closest('[data-relationship-key]');
        if (!button || (event.relatedTarget && button.contains(event.relatedTarget))) return;
        if (focusedRelationship === button) focusedRelationship = null;
        syncRelationshipPreview();
      });
      relationshipList.addEventListener('keydown', function (event) {
        if (event.key !== 'ArrowDown' && event.key !== 'ArrowUp' && event.key !== 'Home' && event.key !== 'End') return;
        var buttons = Array.prototype.slice.call(relationshipList.querySelectorAll('[data-relationship-target]'));
        if (!buttons.length) return;
        var index = buttons.indexOf(document.activeElement);
        if (event.key === 'Home') index = 0;
        else if (event.key === 'End') index = buttons.length - 1;
        else if (event.key === 'ArrowDown') index = Math.min(buttons.length - 1, Math.max(0, index + 1));
        else index = Math.max(0, index < 0 ? 0 : index - 1);
        event.preventDefault();
        buttons[index].focus();
      });
      document.addEventListener('click', function (event) {
        var target = event.target;
        if (chip.hidden || !target || typeof target.closest !== 'function' || chip.contains(target)) return;
        if (container.getAttribute('data-just-panned') === 'true') return;
        if (target.closest('[data-node-id], [data-relationship-hit-key], .overview-map')) return;
        clear();
      }, true);
      window.addEventListener('scroll', requestLensPlacement, { passive: true });
      window.addEventListener('resize', requestLensPlacement);
      container.addEventListener('scroll', requestLensPlacement, { passive: true });
      document.addEventListener('visibilitychange', function () {
        if (document.hidden) removeRelationshipPulse();
      });
      function syncRelationshipMotionPreference(event) {
        if (event.matches) removeRelationshipPulse();
      }
      if (reducedMotionQuery) {
        if (typeof reducedMotionQuery.addEventListener === 'function') {
          reducedMotionQuery.addEventListener('change', syncRelationshipMotionPreference);
        } else if (typeof reducedMotionQuery.addListener === 'function') {
          reducedMotionQuery.addListener(syncRelationshipMotionPreference);
        }
      }

      installRelationshipHitTargets();

      function syncFocusFromHash() {
        try {
          var params = new URLSearchParams(location.hash.replace(/^#/, ''));
          var relation = params.get('relation');
          var initial = params.get('focus');
          var reach = params.get('reach');
          if (relation) {
            if (html.getAttribute('data-embed') === 'true' ||
                !inspectRelationshipById(relation, { updateUrl: false, toggle: false })) clear({ updateUrl: false });
          }
          else if (initial) {
            if (set(initial, { updateUrl: false, toggle: false }) &&
                (reach === 'upstream' || reach === 'downstream')) {
              applyReachability(reach, { updateUrl: false, toggle: false, reveal: false });
            }
          }
          else if (!params.get('view')) clear({ updateUrl: false });
        } catch (_) {}
      }

      window.addEventListener('hashchange', syncFocusFromHash);
      syncFocusFromHash();

      return {
        set: set,
        setMany: setMany,
        clear: clear,
        copyLink: copyFocusLink,
        reach: applyReachability,
        clearReach: clearReachability,
        reachabilitySnapshot: reachabilitySnapshot,
        inspectRelationship: inspectRelationship,
        inspectRelationshipById: inspectRelationshipById,
        reposition: requestLensPlacement,
        relationship: function () {
          var record = pinnedRelationshipRecord();
          return record ? { id: record.id || null, key: record.key, from: record.from, to: record.to, label: record.label } : null;
        },
        reachability: function () {
          return activeReachability ? {
            direction: activeReachability.direction,
            originId: activeReachability.originId,
            nodeIds: activeReachability.nodeIds.slice(),
            edgeKeys: activeReachability.edgeKeys.slice(),
            maxDepth: activeReachability.maxDepth
          } : null;
        },
        active: function () { return activeIds.length === 0 ? null : (activeIds.length === 1 ? activeIds[0] : activeIds.slice()); }
      };
    })();

    /* ============================================================
       Intent Trace — temporary one-hop topology before durable focus.
       Fine-pointer hover and keyboard focus share the same stable-ID preview;
       touch continues directly to the existing click-to-focus contract.
       ============================================================ */
    Mosofin.intentTrace = (function () {
      var html = document.documentElement;
      var container = document.querySelector('.diagram-container');
      var svg = container.querySelector(':scope > svg');
      var status = document.getElementById('intent-trace-status');
      var namespace = 'http://www.w3.org/2000/svg';
      var activeId = null;
      var hoveredNode = null;
      var focusedNode = null;
      var enterTimer = null;

      function nodes() {
        return Array.prototype.slice.call(svg.querySelectorAll('[data-node-id]'));
      }
      function edges() {
        return Array.prototype.slice.call(svg.querySelectorAll('[data-edge-from][data-edge-to]'));
      }
      function nodeLabel(node, fallback) {
        return node.getAttribute('data-node-label') ||
          (node.getAttribute('aria-label') || fallback).replace(/^Focus\s+/, '').split(',')[0];
      }
      function finePointer() {
        return !window.matchMedia || window.matchMedia('(hover: hover) and (pointer: fine)').matches;
      }
      function reducedMotion() {
        return !!(window.matchMedia && window.matchMedia('(prefers-reduced-motion: reduce)').matches);
      }
      function blocked() {
        return html.getAttribute('data-embed') === 'true' ||
          html.getAttribute('data-guide-open') === 'true' ||
          container.classList.contains('is-panning') ||
          svg.hasAttribute('data-lens-active') ||
          svg.hasAttribute('data-story-active') ||
          svg.hasAttribute('data-relationship-preview-active') ||
          !!(Mosofin.routeProbe && typeof Mosofin.routeProbe.active === 'function' && Mosofin.routeProbe.active()) ||
          !!(Mosofin.focus && typeof Mosofin.focus.active === 'function' && Mosofin.focus.active());
      }
      function edgeShapes(edge) {
        if (/^(path|line|polyline)$/i.test(edge.tagName)) return [edge];
        return Array.prototype.slice.call(edge.querySelectorAll('path, line, polyline'));
      }
      function removeOverlay() {
        Array.prototype.forEach.call(svg.querySelectorAll('[data-intent-trace-overlay]'), function (overlay) {
          overlay.remove();
        });
      }
      function clear(options) {
        options = options || {};
        if (enterTimer) window.clearTimeout(enterTimer);
        enterTimer = null;
        activeId = null;
        svg.removeAttribute('data-intent-trace-active');
        removeOverlay();
        nodes().forEach(function (node) {
          node.removeAttribute('data-intent-trace-match');
          node.removeAttribute('data-intent-trace-selected');
        });
        edges().forEach(function (edge) { edge.removeAttribute('data-intent-trace-match'); });
        if (options.announce !== false) status.textContent = '';
      }
      function traceGeometry(shape, direction) {
        var clone = shape.cloneNode(false);
        clone.removeAttribute('id');
        clone.removeAttribute('class');
        clone.removeAttribute('style');
        clone.removeAttribute('marker-start');
        clone.removeAttribute('marker-mid');
        clone.removeAttribute('marker-end');
        clone.removeAttribute('role');
        clone.removeAttribute('aria-label');
        clone.removeAttribute('aria-labelledby');
        clone.removeAttribute('data-animate');
        clone.removeAttribute('data-edge-from');
        clone.removeAttribute('data-edge-to');
        clone.removeAttribute('data-edge-key');
        clone.removeAttribute('data-edge-id');
        clone.removeAttribute('data-edge-label');
        clone.removeAttribute('data-intent-trace-match');
        clone.removeAttribute('data-intent-trace-selected');
        clone.setAttribute('class', 'intent-trace-flow');
        clone.setAttribute('data-direction', direction);
        clone.setAttribute('pathLength', '1');
        return clone;
      }
      function show(id, options) {
        options = options || {};
        if (!id || blocked()) {
          clear({ announce: false });
          return false;
        }
        if (activeId === id) return true;
        clear({ announce: false });
        var nodeList = nodes();
        var edgeList = edges();
        var byId = {};
        nodeList.forEach(function (node) { byId[node.getAttribute('data-node-id')] = node; });
        var selected = byId[id];
        if (!selected) return false;

        var overlay = document.createElementNS(namespace, 'g');
        overlay.setAttribute('class', 'intent-trace-overlay');
        overlay.setAttribute('data-intent-trace-overlay', '');
        overlay.setAttribute('aria-hidden', 'true');
        var related = {};
        var seen = {};
        var counts = { out: 0, in: 0, loop: 0 };
        related[id] = true;

        edgeList.forEach(function (edge) {
          var from = edge.getAttribute('data-edge-from');
          var to = edge.getAttribute('data-edge-to');
          if (from !== id && to !== id) return;
          var direction = from === id && to === id ? 'loop' : (from === id ? 'out' : 'in');
          var key = edge.getAttribute('data-edge-key') ||
            (from + '\u0000' + to + '\u0000' + (edge.getAttribute('data-edge-label') || ''));
          if (!seen[key]) {
            seen[key] = true;
            counts[direction] += 1;
          }
          related[from] = true;
          related[to] = true;
          edge.setAttribute('data-intent-trace-match', '');
          var wrapper = document.createElementNS(namespace, 'g');
          if (edge.hasAttribute('transform')) wrapper.setAttribute('transform', edge.getAttribute('transform'));
          edgeShapes(edge).forEach(function (shape) {
            wrapper.appendChild(traceGeometry(shape, direction));
          });
          if (wrapper.childNodes.length) overlay.appendChild(wrapper);
        });

        nodeList.forEach(function (node) {
          var nodeId = node.getAttribute('data-node-id');
          if (related[nodeId]) node.setAttribute('data-intent-trace-match', '');
          if (nodeId === id) node.setAttribute('data-intent-trace-selected', '');
        });
        if (overlay.childNodes.length) {
          var firstEdge = edgeList[0];
          var firstNode = svg.querySelector('[data-node-id]');
          if (firstEdge && firstEdge.parentNode) firstEdge.parentNode.insertBefore(overlay, firstEdge);
          else if (firstNode) svg.insertBefore(overlay, firstNode);
          else svg.appendChild(overlay);
        }
        activeId = id;
        svg.setAttribute('data-intent-trace-active', id);
        if (options.announce === true) {
          var total = counts.out + counts.in + counts.loop;
          status.textContent = viewerText('viewer.intent.summary', {
            label: nodeLabel(selected, id),
            out: counts.out,
            in: counts.in,
            loops: counts.loop ? viewerText('viewer.intent.loops', { count: counts.loop }) : '',
            total: total
          });
        }
        return true;
      }
      function schedule(node) {
        if (enterTimer) window.clearTimeout(enterTimer);
        enterTimer = window.setTimeout(function () {
          enterTimer = null;
          if (hoveredNode === node) show(node.getAttribute('data-node-id'), { announce: false });
        }, reducedMotion() ? 0 : 90);
      }
      function sync() {
        var candidate = focusedNode || hoveredNode;
        if (!candidate) {
          clear();
          return;
        }
        show(candidate.getAttribute('data-node-id'), { announce: candidate === focusedNode });
      }

      svg.addEventListener('pointerover', function (event) {
        var node = event.target.closest('[data-node-id]');
        if (!node || !finePointer() || event.pointerType === 'touch') return;
        if (event.relatedTarget && node.contains(event.relatedTarget)) return;
        hoveredNode = node;
        schedule(node);
      });
      svg.addEventListener('pointerout', function (event) {
        var node = event.target.closest('[data-node-id]');
        if (!node || (event.relatedTarget && node.contains(event.relatedTarget))) return;
        if (hoveredNode === node) hoveredNode = null;
        sync();
      });
      svg.addEventListener('focusin', function (event) {
        var node = event.target.closest('[data-node-id]');
        if (!node) return;
        focusedNode = node;
        show(node.getAttribute('data-node-id'), { announce: true });
      });
      svg.addEventListener('focusout', function (event) {
        var node = event.target.closest('[data-node-id]');
        if (!node || (event.relatedTarget && node.contains(event.relatedTarget))) return;
        if (focusedNode === node) focusedNode = null;
        sync();
      });
      container.addEventListener('pointerdown', function (event) {
        if (!event.target.closest('[data-node-id]')) clear({ announce: false });
      });
      window.addEventListener('blur', function () { clear({ announce: false }); });

      return {
        show: show,
        clear: clear,
        active: function () { return activeId; }
      };
    })();

    Mosofin.guidedViews = (function () {
      var data = document.getElementById('mosofin-guided-views-data');
      var panel = document.getElementById('guided-views');
      var prev = document.getElementById('guided-view-prev');
      var next = document.getElementById('guided-view-next');
      var all = document.getElementById('guided-view-all');
      var play = document.getElementById('guided-view-play');
      var playIcon = document.getElementById('guided-view-play-icon');
      var playLabel = document.getElementById('guided-view-play-label');
      var beatLink = document.getElementById('guided-view-beat-link');
      var beatLinkLabel = document.getElementById('guided-view-beat-link-label');
      var progressBar = document.getElementById('guided-view-progress-bar');
      var count = document.getElementById('guided-view-count');
      var handoffReceipt = document.getElementById('guided-view-handoff');
      var label = document.getElementById('guided-view-label');
      var note = document.getElementById('guided-view-note');
      var trail = document.getElementById('guided-view-trail');
      var storyCaption = document.getElementById('guided-story-caption');
      var storyCaptionIndex = document.getElementById('guided-story-caption-index');
      var storyCaptionRoute = document.getElementById('guided-story-caption-route');
      var storyCaptionDetail = document.getElementById('guided-story-caption-detail');
      var storyCaptionNext = document.getElementById('guided-story-caption-next');
      var storyCaptionNextLabel = document.getElementById('guided-story-caption-next-label');
      var chapterIndex = document.getElementById('guided-view-index');
      var chapterList = document.getElementById('guided-view-chapters');
      var shareCue = document.getElementById('share-chapter-cue');
      var shareCueState = document.getElementById('share-chapter-state');
      var shareCueCount = document.getElementById('share-chapter-count');
      var shareCueLabel = document.getElementById('share-chapter-label');
      var shareCueNote = document.getElementById('share-chapter-note');
      var shareCueRoute = document.getElementById('share-chapter-route');
      var shareCueProgress = document.getElementById('share-chapter-progress-bar');
      var svg = document.querySelector('.diagram-container svg');
      var views = [];
      var activeIndex = -1;
      var playing = false;
      var storyBeatTimer = null;
      var storyBeatIndex = -1;
      var storyBeatStartedAt = 0;
      var storyBeatElapsedMs = 0;
      var storyBeatDwellMs = 0;
      var storyPlaybackGeneration = 0;
      var storyFollowGeneration = 0;
      var storyPlaybackScope = 'story';
      var storyPlaybackComplete = false;
      var beatLinkFeedbackTimer = null;
      var momentRestoreGeneration = 0;
      var storySteps = [];
      var storyPulseOwnerToken = 0;
      var storyPulseGeneration = 0;
      var autoplayPending = false;
      var VIEW_INTERVAL_MS = 3200;
      var STORY_FOLLOW_MIN_DWELL_MS = 1100;
      var STORY_FOLLOW_DURATION_MS = 320;
      var chapterButtons = [];
      var handoffGeneration = 0;
      var currentHandoff = null;
      var previewGeneration = 0;
      var pointerPreviewIntent = null;
      var focusPreviewIntent = null;
      var activePreviewIndex = -1;
      var previewOwnerToken = 0;

      try { views = JSON.parse(data.textContent || '[]'); } catch (_) { views = []; }
      if (!views.length) return { count: 0, active: function () { return null; } };
      var canonicalNodeIds = {};
      Array.prototype.forEach.call(svg.querySelectorAll('[data-node-id]'), function (node) {
        canonicalNodeIds[node.getAttribute('data-node-id')] = true;
      });
      views.forEach(function (view) {
        var seen = {};
        view.focus = (view.focus || []).filter(function (id) {
          if (!canonicalNodeIds[id] || seen[id]) return false;
          seen[id] = true;
          return true;
        });
      });
      // Establish the compact cold state before exposing the panel so the
      // full Director cannot flash during first paint. Hash restoration runs
      // in the same task and replaces this state before paint when valid.
      panel.setAttribute('data-active-view', 'all');
      panel.hidden = false;
      panel.setAttribute('data-view-interval-ms', String(VIEW_INTERVAL_MS));
      panel.setAttribute('data-story-follow-min-dwell-ms', String(STORY_FOLLOW_MIN_DWELL_MS));
      buildChapterIndex();

      function reducedMotion() {
        return !!(window.matchMedia && window.matchMedia('(prefers-reduced-motion: reduce)').matches);
      }

      function findNode(id) {
        return Array.prototype.find.call(svg.querySelectorAll('[data-node-id]'), function (node) {
          return node.getAttribute('data-node-id') === id;
        }) || null;
      }

      function chapterDelta(previous, destination) {
        var previousFocus = previous ? previous.focus : [];
        var destinationFocus = destination ? destination.focus : [];
        var previousIds = {};
        var destinationIds = {};
        previousFocus.forEach(function (id) { previousIds[id] = true; });
        destinationFocus.forEach(function (id) { destinationIds[id] = true; });
        return {
          stay: previousFocus.filter(function (id) { return destinationIds[id]; }),
          enter: destinationFocus.filter(function (id) { return !previousIds[id]; }),
          leave: previousFocus.filter(function (id) { return !destinationIds[id]; })
        };
      }

      function chapterAnchor(previous, destination, outgoingBeatIndex, delta) {
        if (!previous || !destination) return '';
        var stayIds = {};
        delta.stay.forEach(function (id) { stayIds[id] = true; });
        var activeBeat = outgoingBeatIndex >= 0 ? previous.focus[outgoingBeatIndex] : '';
        if (activeBeat && stayIds[activeBeat]) return activeBeat;
        for (var index = previous.focus.length - 1; index >= 0; index -= 1) {
          if (stayIds[previous.focus[index]]) return previous.focus[index];
        }
        return '';
      }

      function handoffMotionAllowed() {
        if (document.hidden || reducedMotion()) return false;
        if (document.documentElement.getAttribute('data-embed') === 'true') return false;
        if (window.matchMedia && window.matchMedia('print').matches) return false;
        return !(Mosofin.motionGovernor && Mosofin.motionGovernor.capable && Mosofin.motionGovernor.isPaused());
      }

      function classifyHandoff(delta, anchor) {
        var roles = {};
        delta.stay.forEach(function (id) { roles[id] = 'stay'; });
        delta.enter.forEach(function (id) { roles[id] = 'enter'; });
        delta.leave.forEach(function (id) { roles[id] = 'leave'; });
        Array.prototype.forEach.call(svg.querySelectorAll('[data-node-id]'), function (node) {
          var id = node.getAttribute('data-node-id');
          var role = roles[id] || '';
          if (role) node.setAttribute('data-chapter-role', role);
          else node.removeAttribute('data-chapter-role');
        });
        svg.setAttribute('data-chapter-handoff', anchor ? 'anchor' : 'no-anchor');
        if (anchor) svg.setAttribute('data-chapter-anchor', anchor);
        else svg.removeAttribute('data-chapter-anchor');
      }

      function drawHandoffAnchor(id) {
        var node = findNode(id);
        if (!node) return null;
        var box;
        try { box = node.getBBox(); } catch (_) { return null; }
        var overlay = document.createElementNS('http://www.w3.org/2000/svg', 'g');
        var ring = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
        overlay.setAttribute('class', 'chapter-handoff-overlay');
        overlay.setAttribute('data-chapter-handoff-overlay', '');
        overlay.setAttribute('aria-hidden', 'true');
        ring.setAttribute('class', 'chapter-handoff-anchor');
        ring.setAttribute('x', String(box.x - 8));
        ring.setAttribute('y', String(box.y - 8));
        ring.setAttribute('width', String(box.width + 16));
        ring.setAttribute('height', String(box.height + 16));
        ring.setAttribute('rx', '10');
        overlay.appendChild(ring);
        svg.appendChild(overlay);
        return overlay;
      }

      function clearHandoffPresentation() {
        svg.removeAttribute('data-chapter-handoff');
        svg.removeAttribute('data-chapter-anchor');
        Array.prototype.forEach.call(svg.querySelectorAll('[data-chapter-handoff-overlay]'), function (overlay) { overlay.remove(); });
        Array.prototype.forEach.call(svg.querySelectorAll('[data-chapter-role]'), function (node) { node.removeAttribute('data-chapter-role'); });
        if (handoffReceipt) {
          handoffReceipt.hidden = true;
          handoffReceipt.textContent = '';
        }
      }

      function completeHandoff(handoff, outcome) {
        if (!handoff || currentHandoff !== handoff) return false;
        if (handoff.holdTimer) clearTimeout(handoff.holdTimer);
        handoff.holdTimer = null;
        currentHandoff = null;
        clearHandoffPresentation();
        if (handoff.ownerToken && Mosofin.motionGovernor) Mosofin.motionGovernor.release(handoff.ownerToken);
        handoff.resolve({ id: handoff.id, state: outcome || 'complete', anchor: handoff.anchor || null });
        syncStoryControlsDisabled();
        syncChapterPreview();
        return true;
      }

      function settleHandoff(reason) {
        var handoff = currentHandoff;
        if (!handoff) return false;
        if (handoff.holdTimer) clearTimeout(handoff.holdTimer);
        handoff.holdTimer = null;
        if (handoff.camera && handoff.camera.cancel) handoff.camera.cancel(reason || 'settled', true);
        else if (Mosofin.view && Mosofin.view.reveal) Mosofin.view.reveal(handoff.focus, { instant: true, reason: reason || 'settled' });
        completeHandoff(handoff, reason || 'settled');
        return true;
      }

      function cancelHandoff(reason) {
        var handoff = currentHandoff;
        if (!handoff) return false;
        if (handoff.holdTimer) clearTimeout(handoff.holdTimer);
        handoff.holdTimer = null;
        if (handoff.camera && handoff.camera.cancel) handoff.camera.cancel(reason || 'manual', false);
        completeHandoff(handoff, reason || 'manual');
        return true;
      }

      function afterHandoff(callback) {
        if (!currentHandoff) { callback(); return; }
        currentHandoff.finished.then(callback);
      }

      function beginHandoff(previousIndex, nextIndex, previous, destination, outgoingBeatIndex, reason) {
        if (!Mosofin.view || !Mosofin.view.reveal) return null;
        if (!previous || previousIndex === nextIndex) {
          Mosofin.view.reveal(destination.focus, { reason: reason });
          return null;
        }
        var delta = chapterDelta(previous, destination);
        var anchor = chapterAnchor(previous, destination, outgoingBeatIndex, delta);
        if (!handoffMotionAllowed()) {
          Mosofin.view.reveal(destination.focus, { instant: true, reason: reason });
          return null;
        }
        var resolver;
        var handoff = {
          id: ++handoffGeneration,
          mode: anchor ? 'holding' : 'no-anchor',
          anchor: anchor,
          focus: destination.focus.slice(),
          holdTimer: null,
          camera: null,
          ownerToken: 0,
          finished: new Promise(function (resolve) { resolver = resolve; }),
          resolve: resolver
        };
        currentHandoff = handoff;
        classifyHandoff(delta, anchor);
        if (anchor) {
          drawHandoffAnchor(anchor);
          var node = findNode(anchor);
          var nodeLabel = node ? (node.getAttribute('data-node-label') || anchor) : anchor;
          handoffReceipt.textContent = viewerText('viewer.guided.handoff', {
            from: (previousIndex + 1 < 10 ? '0' : '') + (previousIndex + 1),
            to: (nextIndex + 1 < 10 ? '0' : '') + (nextIndex + 1),
            label: nodeLabel
          });
          handoffReceipt.hidden = false;
        }
        if (Mosofin.motionGovernor) {
          handoff.ownerToken = Mosofin.motionGovernor.claim('handoff', function () {
            if (currentHandoff === handoff) settleHandoff('preempted');
          });
        }
        var startCamera = function () {
          if (currentHandoff !== handoff) return;
          handoff.holdTimer = null;
          handoff.mode = 'settling';
          svg.setAttribute('data-chapter-handoff', anchor ? 'settling' : 'no-anchor');
          handoff.camera = Mosofin.view.reveal(destination.focus, { reason: reason, duration: 420 });
          if (handoff.camera && handoff.camera.finished) {
            handoff.camera.finished.then(function (outcome) {
              if (currentHandoff === handoff) completeHandoff(handoff, outcome && outcome.state || 'complete');
            });
          } else {
            completeHandoff(handoff, 'complete');
          }
        };
        if (anchor) handoff.holdTimer = setTimeout(startCamera, 110);
        else startCamera();
        return handoff;
      }

      function centerChapterButton(button) {
        if (!button || !chapterList) return false;
        var target = Math.max(0, button.offsetLeft - (chapterList.clientWidth - button.offsetWidth) / 2);
        if (typeof chapterList.scrollTo === 'function') chapterList.scrollTo({ left: target, behavior: 'auto' });
        else chapterList.scrollLeft = target;
        return true;
      }

      function focusChapterButton(index) {
        if (!chapterButtons.length) return false;
        index = Math.max(0, Math.min(chapterButtons.length - 1, index));
        chapterButtons.forEach(function (button, buttonIndex) {
          button.setAttribute('tabindex', buttonIndex === index ? '0' : '-1');
        });
        chapterButtons[index].focus();
        centerChapterButton(chapterButtons[index]);
        return true;
      }

      function buildChapterIndex() {
        chapterButtons = [];
        chapterList.textContent = '';
        views.forEach(function (view, index) {
          var item = document.createElement('li');
          var button = document.createElement('button');
          button.type = 'button';
          button.className = 'guided-view-chapter';
          button.setAttribute('data-guided-view-id', view.id);
          button.setAttribute('aria-pressed', 'false');
          button.setAttribute('tabindex', index === 0 ? '0' : '-1');
          button.setAttribute('aria-label', viewerText('viewer.guided.chapter.open', {
            index: index + 1,
            total: views.length,
            label: view.label,
            count: view.focus.length
          }));
          button.title = view.label + ' — ' + (view.note || viewerText('viewer.guided.chapter.selectedNodes', { count: view.focus.length }));
          var position = document.createElement('span');
          position.className = 'guided-view-chapter-index';
          position.setAttribute('aria-hidden', 'true');
          position.textContent = (index + 1 < 10 ? '0' : '') + (index + 1);
          var title = document.createElement('span');
          title.className = 'guided-view-chapter-title';
          title.textContent = view.label;
          var stops = document.createElement('em');
          stops.className = 'guided-view-chapter-count';
          stops.textContent = viewerCount('viewer.guided.chapter.stop', view.focus.length);
          var delta = document.createElement('span');
          delta.className = 'guided-view-chapter-delta';
          delta.hidden = true;
          delta.setAttribute('aria-hidden', 'true');
          ['stay', 'enter', 'leave'].forEach(function (kind) {
            var value = document.createElement('span');
            value.setAttribute('data-delta-kind', kind);
            delta.appendChild(value);
          });
          button.appendChild(position);
          button.appendChild(title);
          button.appendChild(stops);
          button.appendChild(delta);
          item.appendChild(button);
          chapterList.appendChild(item);
          chapterButtons.push(button);
        });
      }

      function syncChapterIndex() {
        chapterButtons.forEach(function (button, index) {
          var current = index === activeIndex;
          var stops = button.querySelector('.guided-view-chapter-count');
          var deltaLabel = button.querySelector('.guided-view-chapter-delta');
          var position = activeIndex < 0 ? 'available' : (current ? 'current' : (index < activeIndex ? 'before' : 'after'));
          button.setAttribute('data-chapter-position', position);
          button.setAttribute('aria-pressed', current ? 'true' : 'false');
          button.setAttribute('tabindex', current || (activeIndex < 0 && index === 0) ? '0' : '-1');
          if (current) {
            stops.hidden = false;
            deltaLabel.hidden = true;
            button.removeAttribute('data-chapter-delta');
            button.setAttribute('aria-label', viewerText('viewer.guided.chapter.current', {
              index: index + 1,
              total: views.length,
              label: views[index].label,
              count: views[index].focus.length
            }));
            button.title = viewerText('viewer.guided.chapter.current.title', {
              label: views[index].label,
              count: views[index].focus.length
            });
          } else {
            var delta = chapterDelta(activeIndex >= 0 ? views[activeIndex] : null, views[index]);
            var compact = '=' + delta.stay.length + ' +' + delta.enter.length + ' \u2212' + delta.leave.length;
            var expanded = viewerText('viewer.guided.chapter.delta.expanded', {
              stay: delta.stay.length,
              enter: delta.enter.length,
              leave: delta.leave.length
            });
            stops.hidden = true;
            deltaLabel.hidden = false;
            deltaLabel.querySelector('[data-delta-kind="stay"]').textContent = '=' + delta.stay.length;
            deltaLabel.querySelector('[data-delta-kind="enter"]').textContent = '+' + delta.enter.length;
            deltaLabel.querySelector('[data-delta-kind="leave"]').textContent = '\u2212' + delta.leave.length;
            button.setAttribute('data-chapter-delta', compact);
            button.setAttribute('aria-label', viewerText('viewer.guided.chapter.delta.aria', {
              index: index + 1,
              total: views.length,
              label: views[index].label,
              delta: expanded
            }));
            button.title = viewerText('viewer.guided.chapter.delta.title', {
              label: views[index].label,
              delta: compact
            });
          }
          if (current) button.setAttribute('aria-current', 'step');
          else button.removeAttribute('aria-current');
          if (button.parentElement) button.parentElement.setAttribute('data-chapter-position', position);
        });
        if (activeIndex >= 0) centerChapterButton(chapterButtons[activeIndex]);
      }

      function chapterPreviewBlocked() {
        if (document.hidden || playing || currentHandoff) return true;
        if (document.documentElement.getAttribute('data-embed') === 'true') return true;
        if (window.matchMedia && window.matchMedia('print').matches) return true;
        if (svg.hasAttribute('data-route-picking') || svg.hasAttribute('data-route-active')) return true;
        if (svg.hasAttribute('data-lens-active') || svg.hasAttribute('data-legend-preview-active')) return true;
        if (svg.hasAttribute('data-relationship-preview-active') || svg.hasAttribute('data-intent-trace-active')) return true;
        return activeIndex < 0 && svg.hasAttribute('data-focus-active');
      }

      function clearChapterPreviewPresentation() {
        svg.removeAttribute('data-chapter-preview');
        Array.prototype.forEach.call(svg.querySelectorAll('[data-chapter-preview-role]'), function (node) {
          node.removeAttribute('data-chapter-preview-role');
        });
        chapterButtons.forEach(function (button) { button.removeAttribute('data-preview-active'); });
        activePreviewIndex = -1;
        if (previewOwnerToken && Mosofin.motionGovernor) {
          var token = previewOwnerToken;
          previewOwnerToken = 0;
          Mosofin.motionGovernor.release(token);
        }
      }

      function clearChapterPreview(options) {
        options = options || {};
        previewGeneration += 1;
        if (options.clearIntents !== false) {
          pointerPreviewIntent = null;
          focusPreviewIntent = null;
        }
        clearChapterPreviewPresentation();
        return true;
      }

      function winningChapterPreviewIntent() {
        return [pointerPreviewIntent, focusPreviewIntent].filter(function (intent) {
          return intent && intent.index >= 0 && intent.index < views.length && intent.index !== activeIndex;
        }).sort(function (left, right) { return right.generation - left.generation; })[0] || null;
      }

      function showChapterPreview(index) {
        if (index < 0 || index >= views.length || index === activeIndex || chapterPreviewBlocked()) {
          clearChapterPreviewPresentation();
          return false;
        }
        if (activePreviewIndex === index && svg.getAttribute('data-chapter-preview') === views[index].id) return true;
        clearChapterPreviewPresentation();
        var delta = chapterDelta(activeIndex >= 0 ? views[activeIndex] : null, views[index]);
        var roles = {};
        delta.stay.forEach(function (id) { roles[id] = 'stay'; });
        delta.enter.forEach(function (id) { roles[id] = 'enter'; });
        delta.leave.forEach(function (id) { roles[id] = 'leave'; });
        Array.prototype.forEach.call(svg.querySelectorAll('[data-node-id]'), function (node) {
          var role = roles[node.getAttribute('data-node-id')];
          if (role) node.setAttribute('data-chapter-preview-role', role);
        });
        svg.setAttribute('data-chapter-preview', views[index].id);
        chapterButtons[index].setAttribute('data-preview-active', 'true');
        activePreviewIndex = index;
        if (Mosofin.motionGovernor && Mosofin.motionGovernor.capable) {
          previewOwnerToken = Mosofin.motionGovernor.claim('chapter-preview', function () {
            previewOwnerToken = 0;
            clearChapterPreviewPresentation();
          });
        }
        return true;
      }

      function syncChapterPreview() {
        var intent = winningChapterPreviewIntent();
        if (!intent || chapterPreviewBlocked()) {
          clearChapterPreviewPresentation();
          return false;
        }
        return showChapterPreview(intent.index);
      }

      function setChapterPreviewIntent(source, index) {
        var intent = { index: index, generation: ++previewGeneration };
        if (source === 'pointer') pointerPreviewIntent = intent;
        else focusPreviewIntent = intent;
        if (playing) pausePlayback();
        return syncChapterPreview();
      }

      function clearChapterPreviewIntent(source, index) {
        var intent = source === 'pointer' ? pointerPreviewIntent : focusPreviewIntent;
        if (intent && (typeof index !== 'number' || intent.index === index)) {
          if (source === 'pointer') pointerPreviewIntent = null;
          else focusPreviewIntent = null;
          previewGeneration += 1;
        }
        return syncChapterPreview();
      }

      function hoverCapable() {
        return !!(window.matchMedia && window.matchMedia('(hover: hover)').matches);
      }

      function sharePlaybackRequested() {
        try { return new URLSearchParams(location.search).get('play') === '1'; }
        catch (_) { return false; }
      }

      autoplayPending = sharePlaybackRequested();
      if (autoplayPending) {
        document.documentElement.setAttribute('data-share-playback', 'true');
        setAutoplayState('pending');
      }

      function setAutoplayState(state) {
        if (state) panel.setAttribute('data-autoplay', state);
        else panel.removeAttribute('data-autoplay');
        renderShareCue();
      }

      function shareCueStatus(state) {
        return {
          pending: viewerText('viewer.guided.state.ready'),
          playing: viewerText('viewer.guided.state.playing'),
          complete: viewerText('viewer.guided.state.settled'),
          interrupted: viewerText('viewer.guided.state.paused'),
          pinned: viewerText('viewer.guided.state.pinned'),
          'reduced-motion': viewerText('viewer.guided.state.still')
        }[state] || viewerText('viewer.guided.state.ready');
      }

      function shareCueBeatCopy(state, view, stops) {
        var total = stops.length;
        var activeStop = storyBeatIndex >= 0 ? stops[storyBeatIndex] : null;
        if ((state === 'playing' || state === 'interrupted') && activeStop) {
          return viewerText('viewer.guided.share.step', {
            index: (storyBeatIndex + 1 < 10 ? '0' : '') + (storyBeatIndex + 1),
            total: (total < 10 ? '0' : '') + total,
            label: activeStop.textContent
          });
        }
        if (state === 'pinned' && activeStop) {
          return storyBeatCopy(storySteps[storyBeatIndex], total);
        }
        if (state === 'reduced-motion' && activeStop && hashBeatMatchesCurrent()) {
          return viewerText('viewer.guided.share.staticMoment', {
            step: viewerText('viewer.guided.share.step', {
              index: (storyBeatIndex + 1 < 10 ? '0' : '') + (storyBeatIndex + 1),
              total: (total < 10 ? '0' : '') + total,
              label: activeStop.textContent
            })
          });
        }
        if (state === 'complete') return viewerText('viewer.guided.share.complete', {
          count: total,
          note: view.note || viewerText('viewer.guided.share.settled')
        });
        if (state === 'reduced-motion') return viewerText('viewer.guided.share.staticPath', { count: total });
        if (state === 'pending') return viewerText('viewer.guided.share.ready', { count: total });
        return view.note || viewerText('viewer.guided.chapter.selectedNodes', { count: view.focus.length });
      }

      function renderShareCue() {
        if (!shareCue) return;
        var view = activeIndex >= 0 ? views[activeIndex] : null;
        var shareMode = document.documentElement.getAttribute('data-share-playback') === 'true';
        var embedMode = document.documentElement.getAttribute('data-embed') === 'true';
        var pinnedMode = !shareMode && embedMode && hashBeatMatchesCurrent();
        if (pinnedMode) document.documentElement.setAttribute('data-share-moment', 'true');
        else document.documentElement.removeAttribute('data-share-moment');
        shareCue.hidden = !(embedMode && view && (shareMode || pinnedMode));
        if (shareCue.hidden) return;
        var state = pinnedMode ? 'pinned' : (panel.getAttribute('data-autoplay') || 'pending');
        var stops = Array.prototype.slice.call(trail.querySelectorAll('[data-story-node]'));
        var route = stops.map(function (stop, index) {
          if (index === 0) return stop.textContent;
          var kind = stop.getAttribute('data-story-link');
          var separator = kind === 'forward' ? '\u2192' : (kind === 'reverse' ? '\u2190' : (kind === 'multiple' ? '\u21c4' : '\u00b7'));
          return separator + ' ' + stop.textContent;
        }).join(' ');
        var beatCopy = shareCueBeatCopy(state, view, stops);
        shareCue.setAttribute('data-state', state);
        shareCue.setAttribute('aria-live', state === 'playing' ? 'off' : 'polite');
        shareCueState.textContent = shareCueStatus(state);
        shareCueCount.textContent = viewerText('viewer.guided.share.chapter', {
          index: (activeIndex + 1 < 10 ? '0' : '') + (activeIndex + 1),
          total: (views.length < 10 ? '0' : '') + views.length
        });
        shareCueLabel.textContent = view.label;
        shareCueNote.textContent = beatCopy;
        shareCueRoute.textContent = route;
        shareCue.setAttribute('aria-label', viewerText('viewer.guided.share.aria', {
          state: shareCueStatus(state),
          index: activeIndex + 1,
          total: views.length,
          label: view.label,
          beat: beatCopy,
          route: route
        }));
      }

      function setShareCueProgress(fraction) {
        if (!shareCueProgress) return;
        shareCueProgress.style.animation = 'none';
        shareCueProgress.style.setProperty('--guided-progress-start', String(Math.max(0, Math.min(1, fraction))));
        shareCueProgress.style.transform = 'scaleX(' + Math.max(0, Math.min(1, fraction)) + ')';
      }

      function startShareCueProgress(fraction, duration) {
        if (!shareCueProgress) return;
        setShareCueProgress(fraction || 0);
        void shareCueProgress.offsetWidth;
        shareCueProgress.style.animation = 'mosofin-guided-progress ' + Math.max(1, duration || VIEW_INTERVAL_MS) + 'ms linear forwards';
      }

      function storyNodeLabel(node, fallback) {
        if (!node) return fallback;
        return node.getAttribute('data-node-label') ||
          (node.getAttribute('aria-label') || fallback).replace(/^Focus\s+/, '');
      }

      function storyEdgeKey(edge) {
        return edge.getAttribute('data-edge-key') || (
          edge.getAttribute('data-edge-from') + '\u0000' +
          edge.getAttribute('data-edge-to') + '\u0000' +
          (edge.getAttribute('data-edge-label') || '')
        );
      }

      function uniqueStoryEdges(edgeList) {
        var positions = {};
        var unique = [];
        edgeList.forEach(function (edge) {
          var key = storyEdgeKey(edge);
          if (!Object.prototype.hasOwnProperty.call(positions, key)) {
            positions[key] = unique.length;
            unique.push(edge);
            return;
          }
          var index = positions[key];
          if (!storyGeometry(unique[index]).length && storyGeometry(edge).length) unique[index] = edge;
        });
        return unique;
      }

      function storyStep(view, index, edgeList, byId) {
        var id = view.focus[index];
        var node = byId[id];
        var previousId = index > 0 ? view.focus[index - 1] : '';
        var forward = [];
        var reverse = [];
        if (previousId) {
          edgeList.forEach(function (edge) {
            var from = edge.getAttribute('data-edge-from');
            var to = edge.getAttribute('data-edge-to');
            if (from === previousId && to === id) forward.push(edge);
            else if (from === id && to === previousId) reverse.push(edge);
          });
        }
        forward = uniqueStoryEdges(forward);
        reverse = uniqueStoryEdges(reverse);
        var edges = forward.concat(reverse).sort(function (left, right) {
          return edgeList.indexOf(left) - edgeList.indexOf(right);
        });
        var relation = index === 0 ? 'start' : (!edges.length ? 'group' : (
          edges.length === 1 && forward.length === 1 ? 'forward' :
          (edges.length === 1 && reverse.length === 1 ? 'reverse' : 'multiple')
        ));
        return {
          index: index,
          nodeId: id,
          nodeLabel: storyNodeLabel(byId[id], id),
          previousId: previousId,
          previousLabel: previousId ? storyNodeLabel(byId[previousId], previousId) : '',
          relation: relation,
          edges: edges,
          edgeKeys: edges.map(storyEdgeKey),
          edgeLabels: edges.map(function (edge) { return edge.getAttribute('data-edge-label') || ''; }).filter(function (value, edgeIndex, values) {
            return value && values.indexOf(value) === edgeIndex;
          }),
          responsibility: node ? (node.getAttribute('data-node-sublabel') || '') : '',
          context: node ? (node.getAttribute('data-node-context') || '') : ''
        };
      }

      function storyBeatCopy(step, total) {
        if (!step) return '';
        var position = (step.index + 1 < 10 ? '0' : '') + (step.index + 1);
        var countValue = (total < 10 ? '0' : '') + total;
        if (step.relation === 'start') return viewerText('viewer.guided.beat.start', { index: position, total: countValue, label: step.nodeLabel });
        if (step.relation === 'forward') return viewerText('viewer.guided.beat.forward', { index: position, total: countValue, from: step.previousLabel, to: step.nodeLabel });
        if (step.relation === 'reverse') return viewerText('viewer.guided.beat.reverse', { index: position, total: countValue, from: step.nodeLabel, to: step.previousLabel });
        if (step.relation === 'multiple') return viewerText('viewer.guided.beat.multiple', { index: position, total: countValue, from: step.previousLabel, to: step.nodeLabel, count: step.edges.length });
        return viewerText('viewer.guided.beat.group', { index: position, total: countValue, from: step.previousLabel, to: step.nodeLabel });
      }

      function storyBeatAria(step, total) {
        var prefix = viewerText('viewer.guided.beat.aria.prefix', { index: step.index + 1, total: total, label: step.nodeLabel });
        if (step.relation === 'start') return prefix + viewerText('viewer.guided.beat.aria.start');
        if (step.relation === 'forward') return prefix + viewerText('viewer.guided.beat.aria.forward', { from: step.previousLabel });
        if (step.relation === 'reverse') return prefix + viewerText('viewer.guided.beat.aria.reverse', { from: step.previousLabel, to: step.nodeLabel });
        if (step.relation === 'multiple') return prefix + viewerText('viewer.guided.beat.aria.multiple', { from: step.previousLabel, count: step.edges.length });
        return prefix + viewerText('viewer.guided.beat.aria.group', { from: step.previousLabel });
      }

      function storyCaptionRouteCopy(step) {
        if (step.relation === 'start') return step.nodeLabel + ' · ' + viewerText('viewer.guided.caption.start');
        if (step.relation === 'reverse') return step.previousLabel + ' ← ' + step.nodeLabel;
        if (step.relation === 'multiple') return step.previousLabel + ' ⇄ ' + step.nodeLabel;
        if (step.relation === 'group') return step.previousLabel + ' · ' + step.nodeLabel;
        return step.previousLabel + ' → ' + step.nodeLabel;
      }

      function storyCaptionDetailCopy(step) {
        var facts = [];
        if (step.relation === 'group') facts.push(viewerText('viewer.guided.caption.grouped'));
        else if (step.edgeLabels.length) facts.push(step.edgeLabels.slice(0, 3).join(' + ') + (step.edgeLabels.length > 3 ? viewerText('viewer.guided.caption.more', { count: step.edgeLabels.length - 3 }) : ''));
        else if (step.relation === 'reverse') facts.push(viewerText('viewer.guided.caption.reverse'));
        else if (step.relation === 'multiple') facts.push(viewerText('viewer.guided.caption.relationships', { count: step.edges.length }));
        else if (step.relation !== 'start') facts.push(viewerText('viewer.guided.caption.relationship'));
        if (step.relation === 'reverse') facts.push(viewerText('viewer.guided.caption.direction', { from: step.nodeLabel, to: step.previousLabel }));
        if (step.responsibility) facts.push(step.responsibility);
        if (step.context) facts.push(step.context);
        if (!facts.length) facts.push(viewerText('viewer.guided.caption.starting'));
        return facts.join(' · ');
      }

      function renderStoryCaption(step, total, nextStep) {
        if (!storyCaption) return;
        if (!step) {
          storyCaption.hidden = true;
          storyCaption.removeAttribute('data-story-caption');
          storyCaptionNext.hidden = true;
          storyCaptionNextLabel.textContent = '';
          return;
        }
        storyCaption.hidden = false;
        storyCaption.setAttribute('data-story-caption', step.relation);
        storyCaption.setAttribute('aria-live', playing ? 'off' : 'polite');
        storyCaptionIndex.textContent = (step.index + 1 < 10 ? '0' : '') + (step.index + 1) + ' / ' + (total < 10 ? '0' : '') + total;
        storyCaptionRoute.textContent = storyCaptionRouteCopy(step);
        storyCaptionDetail.textContent = storyCaptionDetailCopy(step);
        storyCaptionNext.hidden = !nextStep;
        storyCaptionNextLabel.textContent = nextStep
          ? ((nextStep.index + 1 < 10 ? '0' : '') + (nextStep.index + 1) + ' · ' + nextStep.nodeLabel)
          : '';
        storyCaption.style.animation = 'none';
        void storyCaption.offsetWidth;
        storyCaption.style.removeProperty('animation');
      }

      function storyMomentLink() {
        var view = activeIndex >= 0 ? views[activeIndex] : null;
        var step = storyBeatIndex >= 0 ? storySteps[storyBeatIndex] : null;
        if (!view || !step) return '';
        var url = new URL(location.href);
        url.searchParams.delete('play');
        url.hash = 'view=' + encodeURIComponent(view.id) + '&beat=' + encodeURIComponent(step.nodeId);
        return url.href;
      }

      function fallbackCopyStoryMoment(value) {
        var field = document.createElement('textarea');
        field.value = value;
        field.setAttribute('readonly', '');
        field.style.position = 'fixed';
        field.style.opacity = '0';
        document.body.appendChild(field);
        field.select();
        var copied = false;
        try { copied = document.execCommand('copy'); } catch (_) {}
        field.remove();
        return copied;
      }

      function resetBeatLinkFeedback() {
        if (beatLinkFeedbackTimer) clearTimeout(beatLinkFeedbackTimer);
        beatLinkFeedbackTimer = null;
        beatLink.removeAttribute('data-copy-state');
        beatLinkLabel.textContent = viewerText('viewer.guided.copyMoment');
      }

      function syncBeatLink() {
        var step = storyBeatIndex >= 0 ? storySteps[storyBeatIndex] : null;
        var available = !!(activeIndex >= 0 && step && !currentHandoff);
        beatLink.disabled = !available;
        if (!beatLink.hasAttribute('data-copy-state')) beatLinkLabel.textContent = viewerText('viewer.guided.copyMoment');
        var description = available
          ? viewerText('viewer.guided.beatLink', { index: step.index + 1, total: storySteps.length, label: step.nodeLabel })
          : viewerText('viewer.guided.selectBeatLink');
        beatLink.setAttribute('aria-label', description);
        beatLink.title = description;
      }

      function setBeatLinkFeedback(copied) {
        resetBeatLinkFeedback();
        beatLink.setAttribute('data-copy-state', copied ? 'copied' : 'failed');
        beatLinkLabel.textContent = viewerText(copied ? 'viewer.guided.copied' : 'viewer.guided.copyFailed');
        beatLink.setAttribute('aria-label', viewerText(copied ? 'viewer.guided.momentCopied' : 'viewer.guided.momentCopyFailed'));
        beatLinkFeedbackTimer = setTimeout(function () {
          resetBeatLinkFeedback();
          syncBeatLink();
        }, 1600);
      }

      function copyStoryMomentLink() {
        if (playing) pausePlayback();
        var value = storyMomentLink();
        if (!value) return Promise.resolve(false);
        var copy = navigator.clipboard && typeof navigator.clipboard.writeText === 'function'
          ? navigator.clipboard.writeText(value).then(function () { return true; }).catch(function () { return fallbackCopyStoryMoment(value); })
          : Promise.resolve(fallbackCopyStoryMoment(value));
        return copy.then(function (copied) {
          setBeatLinkFeedback(copied);
          return copied;
        });
      }

      function hashBeatMatchesCurrent() {
        var view = activeIndex >= 0 ? views[activeIndex] : null;
        var step = storyBeatIndex >= 0 ? storySteps[storyBeatIndex] : null;
        if (!view || !step) return false;
        try {
          var params = new URLSearchParams(location.hash.replace(/^#/, ''));
          return params.get('view') === view.id && params.get('beat') === step.nodeId;
        } catch (_) { return false; }
      }

      function storyMotionAllowed() {
        if (document.hidden || reducedMotion()) return false;
        if (document.documentElement.getAttribute('data-motion') !== 'live') return false;
        if (document.documentElement.getAttribute('data-embed') === 'true' &&
            document.documentElement.getAttribute('data-share-playback') !== 'true') return false;
        if (window.matchMedia && window.matchMedia('print').matches) return false;
        return !(Mosofin.motionGovernor && Mosofin.motionGovernor.isPaused());
      }

      function storyAutomaticPlaybackAllowed() {
        if (document.hidden || reducedMotion()) return false;
        if (window.matchMedia && window.matchMedia('print').matches) return false;
        if (Mosofin.motionGovernor && Mosofin.motionGovernor.capable) return !Mosofin.motionGovernor.isPaused();
        return true;
      }

      function clearStoryPulse(options) {
        options = options || {};
        storyPulseGeneration += 1;
        Array.prototype.forEach.call(svg.querySelectorAll('.story-trail-flow[data-story-pulse]'), function (flow) {
          flow.removeAttribute('data-story-pulse');
        });
        Array.prototype.forEach.call(svg.querySelectorAll('[data-story-carrier-overlay]'), function (overlay) {
          overlay.remove();
        });
        if (storyPulseOwnerToken && options.release !== false && Mosofin.motionGovernor) {
          var token = storyPulseOwnerToken;
          storyPulseOwnerToken = 0;
          Mosofin.motionGovernor.release(token);
        } else if (options.release === false) storyPulseOwnerToken = 0;
      }

      function pulseStoryStep(step) {
        clearStoryPulse();
        if (!step || (step.relation !== 'forward' && step.relation !== 'reverse') || step.edges.length !== 1 || !storyMotionAllowed()) return false;
        var flows = Array.prototype.slice.call(svg.querySelectorAll('.story-trail-flow[data-story-beat-step="' + step.index + '"]'));
        if (!flows.length) return false;
        var pulseGeneration = storyPulseGeneration;
        if (Mosofin.motionGovernor && Mosofin.motionGovernor.capable) {
          storyPulseOwnerToken = Mosofin.motionGovernor.claim('story', function () {
            clearStoryPulse({ release: false });
          });
        }
        flows.forEach(function (flow) { flow.setAttribute('data-story-pulse', 'true'); });
        if (Mosofin.flowTokens && typeof Mosofin.flowTokens.create === 'function') {
          var edge = step.edges[0];
          var shapes = storyGeometry(edge);
          var carrier = shapes.length ? Mosofin.flowTokens.create(edge, shapes[0], {
            className: 'story-flow-token',
            duration: '0.78s'
          }) : null;
          if (carrier) {
            var carrierOverlay = document.createElementNS('http://www.w3.org/2000/svg', 'g');
            var carrierWrapper = document.createElementNS('http://www.w3.org/2000/svg', 'g');
            carrierOverlay.setAttribute('class', 'story-carrier-overlay');
            carrierOverlay.setAttribute('data-story-carrier-overlay', '');
            carrierOverlay.setAttribute('aria-hidden', 'true');
            if (edge.hasAttribute('transform')) carrierWrapper.setAttribute('transform', edge.getAttribute('transform'));
            carrier.setAttribute('data-story-carrier-token', '');
            carrier.setAttribute('data-story-beat-step', String(step.index));
            carrierWrapper.appendChild(carrier);
            carrierOverlay.appendChild(carrierWrapper);
            var firstNode = svg.querySelector('[data-node-id]');
            while (firstNode && firstNode.parentNode !== svg) firstNode = firstNode.parentNode;
            if (firstNode) svg.insertBefore(carrierOverlay, firstNode);
            else svg.appendChild(carrierOverlay);
          }
        }
        void flows[0].getBoundingClientRect();
        flows[0].addEventListener('animationend', function () {
          if (pulseGeneration === storyPulseGeneration) clearStoryPulse();
        }, { once: true });
        return true;
      }

      function stopStoryBeatTimer(options) {
        options = options || {};
        storyPlaybackGeneration += 1;
        if (storyBeatTimer && options.preserveElapsed === true && storyBeatStartedAt) {
          storyBeatElapsedMs = Math.min(storyBeatDwellMs, storyBeatElapsedMs + Math.max(0, Date.now() - storyBeatStartedAt));
        }
        if (storyBeatTimer) clearTimeout(storyBeatTimer);
        storyBeatTimer = null;
        storyBeatStartedAt = 0;
      }

      function centerStoryStop(stop) {
        if (!stop || !trail) return false;
        var target = Math.max(0, stop.offsetLeft - (trail.clientWidth - stop.offsetWidth) / 2);
        trail.scrollLeft = target;
        return true;
      }

      function storyBeatDwell(total) {
        return Math.max(STORY_FOLLOW_MIN_DWELL_MS, VIEW_INTERVAL_MS / Math.max(1, total));
      }

      function clearStoryFollow() {
        storyFollowGeneration += 1;
        svg.removeAttribute('data-story-follow');
        panel.removeAttribute('data-story-follow');
        panel.removeAttribute('data-story-follow-node');
      }

      function storyFrameIds(step) {
        if (!step) return [];
        var ids = [];
        if (step.index > 0 && storySteps[step.index - 1]) ids.push(storySteps[step.index - 1].nodeId);
        ids.push(step.nodeId);
        if (step.index + 1 < storySteps.length) ids.push(storySteps[step.index + 1].nodeId);
        return ids;
      }

      function followStoryStep(step, options) {
        options = options || {};
        if (!step || document.hidden) return false;
        if (!Mosofin.view || typeof Mosofin.view.reveal !== 'function') {
          var deferredIndex = step.index;
          var deferredGeneration = ++storyFollowGeneration;
          requestAnimationFrame(function () {
            if (deferredGeneration !== storyFollowGeneration || storyBeatIndex !== deferredIndex) return;
            if (Mosofin.view && typeof Mosofin.view.reveal === 'function') followStoryStep(step, options);
          });
          return true;
        }
        if (window.matchMedia && window.matchMedia('print').matches) return false;
        var embed = document.documentElement.getAttribute('data-embed') === 'true';
        var explicitEmbedPlayback = document.documentElement.getAttribute('data-share-playback') === 'true';
        if (embed && !explicitEmbedPlayback && options.linked !== true) return false;
        var ids = storyFrameIds(step);
        var generation = ++storyFollowGeneration;
        svg.setAttribute('data-story-follow', step.nodeId);
        panel.setAttribute('data-story-follow', 'moving');
        panel.setAttribute('data-story-follow-node', step.nodeId);
        var receipt = Mosofin.view.reveal(ids, {
          reason: options.manual === true ? 'story-beat' : 'story-follow',
          padding: 64,
          maxScale: 1.65,
          duration: STORY_FOLLOW_DURATION_MS,
          instant: options.instant === true || reducedMotion() || document.documentElement.getAttribute('data-motion') !== 'live'
        });
        if (receipt && receipt.finished && typeof receipt.finished.then === 'function') {
          receipt.finished.then(function (result) {
            if (generation !== storyFollowGeneration || storyBeatIndex !== step.index) return;
            svg.removeAttribute('data-story-follow');
            panel.setAttribute('data-story-follow', result && result.state === 'complete' ? 'settled' : 'interrupted');
          });
        } else if (generation === storyFollowGeneration) {
          panel.setAttribute('data-story-follow', 'settled');
        }
        return receipt;
      }

      function setStoryBeat(index, options) {
        options = options || {};
        var stops = Array.prototype.slice.call(trail.querySelectorAll('[data-story-node]'));
        if (!stops.length) return false;
        resetBeatLinkFeedback();
        storyBeatIndex = Math.max(0, Math.min(stops.length - 1, index));
        var nextStep = storyBeatIndex + 1 < storySteps.length ? storySteps[storyBeatIndex + 1] : null;
        svg.setAttribute('data-story-beat', (storyBeatIndex + 1) + '/' + stops.length);
        panel.setAttribute('data-story-beat', (storyBeatIndex + 1) + '/' + stops.length);
        if (nextStep) {
          svg.setAttribute('data-story-next', nextStep.nodeId);
          panel.setAttribute('data-story-next', nextStep.nodeId);
        } else {
          svg.removeAttribute('data-story-next');
          panel.removeAttribute('data-story-next');
        }
        function storyBeatState(step) {
          if (step < storyBeatIndex) return 'past';
          if (step === storyBeatIndex) return 'active';
          if (step === storyBeatIndex + 1) return 'next';
          return 'pending';
        }
        Array.prototype.forEach.call(svg.querySelectorAll('[data-story-step]'), function (node) {
          var step = Number(node.getAttribute('data-story-step'));
          node.setAttribute('data-story-beat-state', storyBeatState(step));
        });
        Array.prototype.forEach.call(svg.querySelectorAll('[data-story-beat-step]'), function (edge) {
          var step = Number(edge.getAttribute('data-story-beat-step'));
          edge.setAttribute('data-story-beat-state', storyBeatState(step));
        });
        stops.forEach(function (stop, step) {
          stop.setAttribute('data-story-beat-state', storyBeatState(step));
          if (step === storyBeatIndex) stop.setAttribute('aria-current', 'step');
          else stop.removeAttribute('aria-current');
        });
        note.setAttribute('aria-live', 'off');
        note.textContent = storyBeatCopy(storySteps[storyBeatIndex], stops.length);
        renderStoryCaption(storySteps[storyBeatIndex], stops.length, nextStep);
        if (options.center === true) centerStoryStop(stops[storyBeatIndex]);
        if (options.pulse === true) pulseStoryStep(storySteps[storyBeatIndex]);
        else clearStoryPulse();
        if (options.follow === true) followStoryStep(storySteps[storyBeatIndex], {
          manual: options.manual === true,
          linked: options.linked === true,
          instant: options.followInstant === true
        });
        renderShareCue();
        syncBeatLink();
        return true;
      }

      function settleStoryBeats() {
        stopStoryBeatTimer();
        clearStoryPulse();
        clearStoryFollow();
        resetBeatLinkFeedback();
        storyBeatIndex = -1;
        storyBeatElapsedMs = 0;
        storyBeatDwellMs = 0;
        svg.removeAttribute('data-story-beat');
        svg.removeAttribute('data-story-next');
        panel.removeAttribute('data-story-beat');
        panel.removeAttribute('data-story-next');
        Array.prototype.forEach.call(svg.querySelectorAll('[data-story-beat-state]'), function (el) {
          el.removeAttribute('data-story-beat-state');
        });
        Array.prototype.forEach.call(trail.querySelectorAll('[data-story-beat-state]'), function (stop) {
          stop.removeAttribute('data-story-beat-state');
          stop.removeAttribute('aria-current');
        });
        note.setAttribute('aria-live', 'polite');
        if (activeIndex >= 0) note.textContent = views[activeIndex].note || viewerText('viewer.guided.chapter.selectedNodes', { count: views[activeIndex].focus.length });
        renderStoryCaption(null, 0);
        renderShareCue();
        syncBeatLink();
      }

      function clearStoryTrail() {
        stopStoryBeatTimer();
        clearStoryPulse();
        clearStoryFollow();
        resetBeatLinkFeedback();
        storyBeatIndex = -1;
        storyBeatElapsedMs = 0;
        storyBeatDwellMs = 0;
        storySteps = [];
        svg.removeAttribute('data-story-active');
        svg.removeAttribute('data-story-playing');
        svg.removeAttribute('data-story-beat');
        svg.removeAttribute('data-story-next');
        panel.removeAttribute('data-story-beat');
        panel.removeAttribute('data-story-next');
        Array.prototype.forEach.call(svg.querySelectorAll('[data-story-overlay]'), function (overlay) {
          overlay.remove();
        });
        Array.prototype.forEach.call(svg.querySelectorAll('[data-story-step], [data-story-beat-state]'), function (node) {
          node.removeAttribute('data-story-step');
          node.removeAttribute('data-story-beat-state');
          node.style.removeProperty('--story-step');
        });
        Array.prototype.forEach.call(svg.querySelectorAll('[data-edge-from][data-story-beat-step]'), function (edge) {
          edge.removeAttribute('data-story-beat-step');
          edge.removeAttribute('data-story-beat-state');
        });
        trail.textContent = '';
        trail.hidden = true;
        trail.removeAttribute('aria-label');
        renderStoryCaption(null, 0);
        syncBeatLink();
      }

      function storyGeometry(edge) {
        if (/^(path|line|polyline)$/i.test(edge.tagName)) return [edge];
        return Array.prototype.slice.call(edge.querySelectorAll('path, line, polyline'));
      }

      function renderStoryTrail(view) {
        if (Mosofin.semanticLens && typeof Mosofin.semanticLens.clearPreview === 'function') Mosofin.semanticLens.clearPreview();
        clearStoryTrail();
        if (!view) return;

        var nodeList = Array.prototype.slice.call(svg.querySelectorAll('[data-node-id]'));
        var edgeList = Array.prototype.slice.call(svg.querySelectorAll('[data-edge-from][data-edge-to]'));
        var byId = {};
        nodeList.forEach(function (node) { byId[node.getAttribute('data-node-id')] = node; });
        storySteps = view.focus.map(function (_, index) { return storyStep(view, index, edgeList, byId); });
        var labels = [];
        storySteps.forEach(function (step, index) {
          var id = step.nodeId;
          var node = byId[id];
          if (!node) return;
          node.setAttribute('data-story-step', String(index));
          node.style.setProperty('--story-step', String(index));
          var nodeLabel = step.nodeLabel;
          labels.push(nodeLabel);
          var stop = document.createElement('button');
          stop.type = 'button';
          stop.className = 'guided-view-stop';
          stop.setAttribute('data-story-node', id);
          stop.setAttribute('data-story-index', String(index));
          stop.setAttribute('data-story-relation', step.relation);
          stop.setAttribute('data-story-number', (index + 1 < 10 ? '0' : '') + (index + 1));
          if (index > 0) stop.setAttribute('data-story-link', step.relation);
          stop.style.setProperty('--story-step', String(index));
          stop.setAttribute('aria-label', storyBeatAria(step, storySteps.length));
          stop.title = storyBeatCopy(step, storySteps.length);
          stop.textContent = nodeLabel;
          trail.appendChild(stop);
        });
        trail.hidden = labels.length === 0;
        trail.setAttribute('aria-label', viewerText('viewer.guided.storyTrail', { label: view.label, count: labels.length }));
        trail.scrollLeft = 0;
        svg.setAttribute('data-story-active', view.id);

        var overlay = document.createElementNS('http://www.w3.org/2000/svg', 'g');
        overlay.setAttribute('class', 'story-trail-overlay');
        overlay.setAttribute('data-story-overlay', '');
        overlay.setAttribute('aria-hidden', 'true');
        var copied = 0;
        storySteps.forEach(function (step) {
          step.edges.forEach(function (edge) {
            var edgeBeat = step.index;
            edge.setAttribute('data-story-beat-step', String(edgeBeat));
            var wrapper = document.createElementNS('http://www.w3.org/2000/svg', 'g');
            if (edge.hasAttribute('transform')) wrapper.setAttribute('transform', edge.getAttribute('transform'));
            storyGeometry(edge).forEach(function (shape) {
              var clone = shape.cloneNode(false);
              clone.removeAttribute('id');
              clone.removeAttribute('marker-start');
              clone.removeAttribute('marker-mid');
              clone.removeAttribute('marker-end');
              clone.removeAttribute('aria-label');
              clone.removeAttribute('role');
              clone.removeAttribute('data-animate');
              clone.removeAttribute('data-edge-from');
              clone.removeAttribute('data-edge-to');
              clone.removeAttribute('data-edge-key');
              clone.removeAttribute('data-edge-id');
              clone.removeAttribute('data-edge-label');
              clone.setAttribute('class', 'story-trail-flow');
              clone.setAttribute('data-story-beat-step', String(edgeBeat));
              clone.style.setProperty('--story-step', String(edgeBeat));
              wrapper.appendChild(clone);
              copied += 1;
            });
            if (wrapper.childNodes.length) overlay.appendChild(wrapper);
          });
        });
        if (copied) {
          var firstEdge = edgeList[0];
          if (firstEdge && firstEdge.parentNode) firstEdge.parentNode.insertBefore(overlay, firstEdge);
          else svg.insertBefore(overlay, svg.firstChild);
        }
      }

      function syncStoryPlayback() {
        var shouldPlay = playing && activeIndex >= 0;
        if (shouldPlay && svg.getAttribute('data-story-playing') !== 'true') svg.setAttribute('data-story-playing', 'true');
        else if (!shouldPlay && svg.hasAttribute('data-story-playing')) svg.removeAttribute('data-story-playing');
      }

      function resetProgress(fraction) {
        fraction = Math.max(0, Math.min(1, Number(fraction) || 0));
        progressBar.style.animation = 'none';
        progressBar.style.setProperty('--guided-progress-start', String(fraction));
        progressBar.style.transform = 'scaleX(' + fraction + ')';
      }

      function startProgress(fraction, duration) {
        resetProgress(fraction);
        void progressBar.offsetWidth;
        progressBar.style.animation = 'mosofin-guided-progress ' + Math.max(1, duration || VIEW_INTERVAL_MS) + 'ms linear forwards';
      }

      function currentStoryProgress() {
        var total = storySteps.length;
        if (!total || storyBeatIndex < 0) return 0;
        var elapsed = storyBeatElapsedMs;
        if (playing && storyBeatTimer && storyBeatStartedAt) elapsed += Math.max(0, Date.now() - storyBeatStartedAt);
        var dwell = storyBeatDwellMs || storyBeatDwell(total);
        return Math.max(0, Math.min(1, (storyBeatIndex + Math.min(1, elapsed / dwell)) / total));
      }

      function renderPlayback() {
        var automaticPlaybackAllowed = storyAutomaticPlaybackAllowed();
        panel.setAttribute('data-playing', playing ? 'true' : 'false');
        syncStoryPlayback();
        play.disabled = !playing && !automaticPlaybackAllowed;
        play.setAttribute('aria-pressed', playing ? 'true' : 'false');
        play.setAttribute('aria-label', viewerText(playing
          ? 'viewer.guided.pause'
          : !automaticPlaybackAllowed
            ? 'viewer.guided.motionUnavailable'
            : storyPlaybackComplete
              ? 'viewer.guided.replay'
              : 'viewer.guided.play'));
        play.title = viewerText(playing
          ? 'viewer.guided.pause.title'
          : !automaticPlaybackAllowed
            ? 'viewer.guided.enableMotion'
            : storyPlaybackComplete
              ? 'viewer.guided.replay.title'
              : 'viewer.guided.play.title');
        playIcon.textContent = playing ? '\u2016' : '\u25b6';
        playLabel.textContent = viewerText(playing
          ? 'viewer.guided.pauseStory'
          : storyPlaybackComplete
            ? 'viewer.guided.replayStory'
            : 'viewer.guided.playStory');
        if (storyCaption && !storyCaption.hidden) storyCaption.setAttribute('aria-live', playing ? 'off' : 'polite');
      }

      function render() {
        var view = views[activeIndex];
        count.textContent = activeIndex < 0 ? '0 / ' + views.length : (activeIndex + 1) + ' / ' + views.length;
        label.textContent = view ? view.label : viewerText('viewer.guided.explore');
        note.setAttribute('aria-live', storyBeatIndex >= 0 ? 'off' : 'polite');
        note.textContent = storyBeatIndex >= 0
          ? storyBeatCopy(storySteps[storyBeatIndex], storySteps.length)
          : (view
            ? (view.note || viewerText('viewer.guided.chapter.selectedNodes', { count: view.focus.length }))
            : viewerText('viewer.guided.intro'));
        prev.disabled = activeIndex < 0;
        next.disabled = activeIndex >= views.length - 1;
        all.disabled = activeIndex < 0;
        panel.setAttribute('data-active-view', view ? view.id : 'all');
        syncChapterIndex();
        renderShareCue();
        renderPlayback();
        syncBeatLink();
      }

      function pausePlayback(options) {
        options = options || {};
        var progress = options.complete === true ? 1 : currentStoryProgress();
        if (panel.getAttribute('data-autoplay') === 'playing') {
          setShareCueProgress(progress);
          setAutoplayState(options.complete === true ? 'complete' : 'interrupted');
        }
        stopStoryBeatTimer({ preserveElapsed: options.complete !== true });
        clearStoryPulse();
        clearStoryFollow();
        playing = false;
        storyPlaybackComplete = options.complete === true;
        resetProgress(progress);
        renderPlayback();
        renderShareCue();
      }

      function finishStoryChapter() {
        if (!playing) return false;
        if (storyPlaybackScope === 'chapter') {
          pausePlayback({ complete: true });
          return true;
        }
        if (activeIndex < views.length - 1) {
          var destinationIndex = activeIndex + 1;
          activate(destinationIndex, { playback: true });
          afterHandoff(function () {
            if (!playing || activeIndex !== destinationIndex) return;
            storyBeatElapsedMs = 0;
            setStoryBeat(0, { pulse: true, follow: true });
            scheduleStoryPlayback();
          });
          return true;
        }
        pausePlayback({ complete: true });
        return true;
      }

      function scheduleStoryPlayback() {
        var total = storySteps.length;
        if (!playing || !total || reducedMotion()) {
          if (playing) pausePlayback();
          return false;
        }
        storyBeatDwellMs = storyBeatDwell(total);
        if (storyBeatIndex < 0) {
          storyBeatElapsedMs = 0;
          setStoryBeat(0, { pulse: true, follow: true });
        }
        storyBeatElapsedMs = Math.max(0, Math.min(storyBeatDwellMs, storyBeatElapsedMs));
        var remainingDwell = Math.max(1, storyBeatDwellMs - storyBeatElapsedMs);
        var progress = (storyBeatIndex + storyBeatElapsedMs / storyBeatDwellMs) / total;
        var remainingChapter = remainingDwell + Math.max(0, total - storyBeatIndex - 1) * storyBeatDwellMs;
        startProgress(progress, remainingChapter);
        if (panel.getAttribute('data-autoplay') === 'playing') startShareCueProgress(progress, remainingChapter);
        var generation = ++storyPlaybackGeneration;
        storyBeatStartedAt = Date.now();
        storyBeatTimer = setTimeout(function () {
          if (!playing || generation !== storyPlaybackGeneration) return;
          storyBeatTimer = null;
          storyBeatStartedAt = 0;
          storyBeatElapsedMs = 0;
          if (storyBeatIndex < total - 1) {
            setStoryBeat(storyBeatIndex + 1, { pulse: true, follow: true });
            scheduleStoryPlayback();
          } else finishStoryChapter();
        }, remainingDwell);
        return true;
      }

      function startPlayback() {
        if (!storyAutomaticPlaybackAllowed()) {
          renderPlayback();
          return false;
        }
        autoplayPending = false;
        setAutoplayState(null);
        setShareCueProgress(0);
        document.documentElement.removeAttribute('data-share-playback');
        storyPlaybackScope = 'story';
        if (storyPlaybackComplete) showAll({ updateUrl: false });
        storyPlaybackComplete = false;
        if (activeIndex < 0) activate(0, { playback: true });
        playing = true;
        renderPlayback();
        afterHandoff(function () {
          if (!playing) return;
          if (storyBeatIndex >= 0) followStoryStep(storySteps[storyBeatIndex]);
          scheduleStoryPlayback();
        });
        return true;
      }

      function startCurrentViewPlayback() {
        if (document.hidden) return false;
        autoplayPending = false;
        document.documentElement.setAttribute('data-share-playback', 'true');
        storyPlaybackScope = 'chapter';
        storyPlaybackComplete = false;
        if (activeIndex < 0) activate(0, { playback: true, updateUrl: false });
        if (!storyAutomaticPlaybackAllowed()) {
          var preserveMoment = hashBeatMatchesCurrent();
          setShareCueProgress(preserveMoment && storySteps.length ? (storyBeatIndex + 1) / storySteps.length : 1);
          setAutoplayState('reduced-motion');
          playing = false;
          if (preserveMoment) {
            clearStoryPulse();
            renderShareCue();
          } else settleStoryBeats();
          renderPlayback();
          return false;
        }
        playing = true;
        setAutoplayState('playing');
        renderPlayback();
        afterHandoff(function () {
          if (!playing) return;
          if (storyBeatIndex >= 0) followStoryStep(storySteps[storyBeatIndex]);
          scheduleStoryPlayback();
        });
        return true;
      }

      function maybeStartSharePlayback() {
        if (!autoplayPending || document.hidden) return false;
        return startCurrentViewPlayback();
      }

      function togglePlayback() {
        return playing ? (pausePlayback(), false) : startPlayback();
      }

      function syncStoryControlsDisabled() {
