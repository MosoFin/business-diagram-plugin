        Array.prototype.forEach.call(trail.querySelectorAll('[data-story-node]'), function (stop) {
          stop.disabled = !!currentHandoff;
        });
        syncBeatLink();
      }

      function selectStoryBeat(index) {
        if (currentHandoff || activeIndex < 0 || index < 0 || index >= storySteps.length) return false;
        momentRestoreGeneration += 1;
        clearChapterPreview({ clearIntents: true });
        if (playing) pausePlayback();
        else {
          stopStoryBeatTimer();
          clearStoryPulse();
        }
        storyPlaybackComplete = false;
        storyPlaybackScope = 'story';
        storyBeatElapsedMs = 0;
        storyBeatDwellMs = storyBeatDwell(storySteps.length);
        var view = views[activeIndex];
        Mosofin.focus.setMany(view.focus, {
          toggle: false,
          updateUrl: false,
          mode: 'selection',
          label: view.label,
          hideChip: true
        });
        setStoryBeat(index, { manual: true, center: true, pulse: true, follow: true });
        resetProgress(index / storySteps.length);
        renderPlayback();
        return true;
      }

      function selectStoryBeatById(id, options) {
        options = options || {};
        var index = storySteps.findIndex(function (step) { return step.nodeId === id; });
        if (index < 0) return false;
        if (playing) pausePlayback();
        else {
          stopStoryBeatTimer();
          clearStoryPulse();
        }
        storyPlaybackComplete = false;
        storyPlaybackScope = 'story';
        storyBeatElapsedMs = 0;
        storyBeatDwellMs = storyBeatDwell(storySteps.length);
        setStoryBeat(index, {
          manual: false,
          center: true,
          pulse: false,
          follow: options.follow === true,
          linked: options.linked === true,
          followInstant: options.followInstant === true
        });
        resetProgress(index / storySteps.length);
        renderPlayback();
        return true;
      }

      function updateUrl(view) {
        try {
          history.replaceState(null, '', location.pathname + location.search + (view ? '#view=' + encodeURIComponent(view.id) : ''));
        } catch (_) {}
      }

      function showAll(options) {
        options = options || {};
        if (options.restore !== true) momentRestoreGeneration += 1;
        clearChapterPreview({ clearIntents: true });
        if (playing && options.playback !== true) pausePlayback();
        if (options.playback !== true) storyPlaybackComplete = false;
        cancelHandoff('overview');
        activeIndex = -1;
        renderStoryTrail(null);
        if (options.clearFocus !== false) Mosofin.focus.clear({ updateUrl: false });
        if (options.resetView !== false && Mosofin.view && typeof Mosofin.view.reset === 'function') {
          Mosofin.view.reset({ automatic: true });
        }
        render();
        if (options.updateUrl !== false) updateUrl(null);
      }

      function activate(index, options) {
        options = options || {};
        if (options.restore !== true) momentRestoreGeneration += 1;
        if (playing && options.playback !== true) pausePlayback();
        if (options.playback !== true) storyPlaybackComplete = false;
        if (index < 0) { showAll(options); return true; }
        if (index >= views.length) return false;
        clearChapterPreview({ clearIntents: true });
        var previousIndex = activeIndex;
        var previous = previousIndex >= 0 ? views[previousIndex] : null;
        var outgoingBeatIndex = storyBeatIndex;
        cancelHandoff('replaced');
        activeIndex = index;
        var view = views[index];
        Mosofin.focus.setMany(view.focus, {
          toggle: false,
          updateUrl: false,
          mode: 'selection',
          label: view.label,
          hideChip: true
        });
        renderStoryTrail(view);
        beginHandoff(previousIndex, index, previous, view, outgoingBeatIndex, options.playback === true ? 'playback' : 'guided');
        syncStoryControlsDisabled();
        render();
        if (options.updateUrl !== false) updateUrl(view);
        return true;
      }

      function activateById(id, options) {
        var index = views.findIndex(function (view) { return view.id === id; });
        return index >= 0 && activate(index, options);
      }

      prev.addEventListener('click', function () { activate(activeIndex - 1); });
      next.addEventListener('click', function () { activate(activeIndex + 1); });
      all.addEventListener('click', function () { showAll(); });
      play.addEventListener('click', togglePlayback);
      beatLink.addEventListener('click', copyStoryMomentLink);
      trail.addEventListener('focusin', function (event) {
        if (!event.target.closest('[data-story-node]')) return;
        if (playing) pausePlayback();
      });
      trail.addEventListener('click', function (event) {
        var stop = event.target.closest('[data-story-node]');
        if (!stop || stop.disabled) return;
        selectStoryBeat(Number(stop.getAttribute('data-story-index')));
      });
      trail.addEventListener('keydown', function (event) {
        if (event.key !== 'Enter' && event.key !== ' ') return;
        var stop = event.target.closest('[data-story-node]');
        if (!stop || stop.disabled) return;
        event.preventDefault();
        selectStoryBeat(Number(stop.getAttribute('data-story-index')));
      });
      chapterList.addEventListener('click', function (event) {
        var button = event.target.closest('[data-guided-view-id]');
        if (!button) return;
        activateById(button.getAttribute('data-guided-view-id'));
      });
      chapterList.addEventListener('pointerover', function (event) {
        var button = event.target.closest('[data-guided-view-id]');
        if (!button || !hoverCapable() || event.pointerType === 'touch') return;
        if (event.relatedTarget && button.contains(event.relatedTarget)) return;
        setChapterPreviewIntent('pointer', chapterButtons.indexOf(button));
      });
      chapterList.addEventListener('pointerout', function (event) {
        var button = event.target.closest('[data-guided-view-id]');
        if (!button || (event.relatedTarget && button.contains(event.relatedTarget))) return;
        clearChapterPreviewIntent('pointer', chapterButtons.indexOf(button));
      });
      chapterIndex.addEventListener('focusin', function (event) {
        var button = event.target.closest('[data-guided-view-id]');
        if (!button) return;
        setChapterPreviewIntent('focus', chapterButtons.indexOf(button));
      });
      chapterIndex.addEventListener('focusout', function (event) {
        var button = event.target.closest('[data-guided-view-id]');
        if (!button || (event.relatedTarget && button.contains(event.relatedTarget))) return;
        clearChapterPreviewIntent('focus', chapterButtons.indexOf(button));
      });
      chapterList.addEventListener('keydown', function (event) {
        var button = event.target.closest('[data-guided-view-id]');
        var index = chapterButtons.indexOf(button);
        if (index < 0) return;
        var target = null;
        if (event.key === 'ArrowRight') target = Math.min(chapterButtons.length - 1, index + 1);
        else if (event.key === 'ArrowLeft') target = Math.max(0, index - 1);
        else if (event.key === 'Home') target = 0;
        else if (event.key === 'End') target = chapterButtons.length - 1;
        if (target === null) return;
        event.preventDefault();
        focusChapterButton(target);
      });

      // A direct node exploration supersedes the curated view. Capture phase
      // releases the view before the focus explorer writes its own deep link.
      function releaseForNode(event) {
        if (activeIndex >= 0 && event.target.closest('[data-node-id]')) {
          pausePlayback();
          showAll({ clearFocus: false, updateUrl: false });
        }
      }
      svg.addEventListener('click', releaseForNode, true);
      svg.addEventListener('keydown', function (event) {
        if (event.key === 'Enter' || event.key === ' ') releaseForNode(event);
      }, true);

      document.addEventListener('keydown', function (event) {
        if (event.metaKey || event.ctrlKey || event.altKey) return;
        if (document.documentElement.getAttribute('data-guide-open') === 'true') return;
        var target = event.target;
        if (target && (target.tagName === 'INPUT' || target.tagName === 'TEXTAREA' || target.isContentEditable)) return;
        if (event.key === ']') {
          event.preventDefault();
          if (activeIndex < views.length - 1) activate(activeIndex + 1);
        } else if (event.key === '[') {
          event.preventDefault();
          if (activeIndex >= 0) activate(activeIndex - 1);
        } else if (event.key.toLowerCase() === 'p') {
          event.preventDefault();
          togglePlayback();
        } else if (event.key === 'Escape' && activePreviewIndex >= 0) {
          event.preventDefault();
          event.stopImmediatePropagation();
          clearChapterPreview({ clearIntents: true });
        } else if (event.key === 'Escape' && activeIndex >= 0) {
          event.preventDefault();
          showAll();
        }
      }, true);

      function syncViewFromHash() {
        try {
          var params = new URLSearchParams(location.hash.replace(/^#/, ''));
          var initial = params.get('view');
          var requestedBeat = params.get('beat');
          var restoreGeneration = ++momentRestoreGeneration;
          if (initial) {
            var activated = activateById(initial, { updateUrl: false, restore: true });
            if (!activated) {
              showAll({ updateUrl: false, restore: true });
              return;
            }
            if (requestedBeat) afterHandoff(function () {
              if (restoreGeneration !== momentRestoreGeneration) return;
              var latest = new URLSearchParams(location.hash.replace(/^#/, ''));
              if (latest.get('view') !== initial || latest.get('beat') !== requestedBeat) return;
              if (activeIndex < 0 || views[activeIndex].id !== initial) return;
              selectStoryBeatById(requestedBeat, { linked: true, follow: true, followInstant: true });
            });
          } else if (params.get('focus') || params.get('relation')) {
            activeIndex = -1;
            render();
          } else {
            showAll({ updateUrl: false, restore: true });
          }
        } catch (_) { showAll({ updateUrl: false, restore: true }); }
      }

      window.addEventListener('hashchange', syncViewFromHash);
      document.addEventListener('visibilitychange', function () {
        if (document.hidden) {
          clearChapterPreview({ clearIntents: true });
          if (playing) pausePlayback();
          else clearStoryPulse();
          settleHandoff('hidden');
        }
        else if (!document.hidden) maybeStartSharePlayback();
      });
      if (window.matchMedia) {
        var guidedMotionQuery = window.matchMedia('(prefers-reduced-motion: reduce)');
        var onGuidedMotionChange = function () {
          if (guidedMotionQuery.matches) {
            if (playing) pausePlayback();
            else clearStoryPulse();
            settleHandoff('reduced-motion');
          }
        };
        if (typeof guidedMotionQuery.addEventListener === 'function') guidedMotionQuery.addEventListener('change', onGuidedMotionChange);
        else if (typeof guidedMotionQuery.addListener === 'function') guidedMotionQuery.addListener(onGuidedMotionChange);
      }
      if (document.documentElement.getAttribute('data-embed') !== 'true' && typeof MutationObserver !== 'undefined' && typeof Node !== 'undefined' && svg instanceof Node && document.documentElement instanceof Node) {
        var chapterPreviewOwnerObserver = new MutationObserver(syncChapterPreview);
        chapterPreviewOwnerObserver.observe(svg, {
          attributes: true,
          attributeFilter: [
            'data-route-picking', 'data-route-active', 'data-lens-active',
            'data-legend-preview-active', 'data-relationship-preview-active',
            'data-intent-trace-active', 'data-focus-active'
          ]
        });
        var storyMotionObserver = new MutationObserver(function () {
          if (document.documentElement.getAttribute('data-motion') !== 'live') clearStoryPulse();
          renderPlayback();
        });
        storyMotionObserver.observe(document.documentElement, { attributes: true, attributeFilter: ['data-motion'] });
      }
      window.addEventListener('beforeprint', function () {
        clearChapterPreview({ clearIntents: true });
        if (playing) pausePlayback();
        else clearStoryPulse();
        settleHandoff('print');
      });
      syncViewFromHash();
      if (autoplayPending) {
        requestAnimationFrame(function () {
          requestAnimationFrame(maybeStartSharePlayback);
        });
      }

      return {
        count: views.length,
        activate: activateById,
        showAll: showAll,
        play: startPlayback,
        playCurrent: startCurrentViewPlayback,
        pause: pausePlayback,
        beatLink: storyMomentLink,
        copyBeatLink: copyStoryMomentLink,
        cancelHandoff: cancelHandoff,
        settleHandoff: settleHandoff,
        clearPreview: function () { return clearChapterPreview({ clearIntents: true }); },
        isPlaying: function () { return playing; },
        handoff: function () {
          return currentHandoff ? { id: currentHandoff.id, mode: currentHandoff.mode, anchor: currentHandoff.anchor || null } : null;
        },
        active: function () { return activeIndex < 0 ? null : views[activeIndex].id; },
        preview: function () { return activePreviewIndex < 0 ? null : views[activePreviewIndex].id; },
        delta: function (id) {
          var index = views.findIndex(function (view) { return view.id === id; });
          if (index < 0) return null;
          var value = chapterDelta(activeIndex >= 0 ? views[activeIndex] : null, views[index]);
          return { stay: value.stay.slice(), enter: value.enter.slice(), leave: value.leave.slice() };
        },
        beat: function () {
          var step = storyBeatIndex >= 0 ? storySteps[storyBeatIndex] : null;
          return step ? {
            index: step.index,
            position: step.index + 1,
            total: storySteps.length,
            nodeId: step.nodeId,
            relation: step.relation,
            edgeKeys: step.edgeKeys.slice()
          } : null;
        },
        focus: function () { return activeIndex < 0 ? [] : views[activeIndex].focus.slice(); }
      };
    })();

    /* ============================================================
       Adaptive Reader Shell — one diagram across laptop and monitor.
       The canonical SVG and viewBox never change. On ordinary desktop pages,
       wide diagrams receive only enough outer width to use the available
       height without pushing summary cards below the viewport. Mobile,
       embed, presentation, print, and non-wide diagrams retain their own
       established layout contracts.
       ============================================================ */
    Mosofin.waitForStableLayout = function (options) {
      options = options || {};
      var maximumFrames = Math.max(1, Number(options.maximumFrames) || 240);
      var fontsReady = document.fonts && document.fonts.ready
        ? document.fonts.ready.catch(function () {})
        : Promise.resolve();
      return fontsReady.then(function () {
        if (typeof options.schedule === 'function') options.schedule();
        return new Promise(function (resolve, reject) {
          var previous = '';
          var stableFrames = 0;
          var sampledFrames = 0;
          function sample() {
            sampledFrames += 1;
            if (typeof options.pending === 'function' && options.pending()) {
              previous = '';
              stableFrames = 0;
            } else {
              var current = typeof options.snapshot === 'function' ? options.snapshot() : '';
              if (current === previous) stableFrames += 1;
              else {
                previous = current;
                stableFrames = 0;
              }
              if (stableFrames >= 3) {
                resolve({ stable: true, snapshot: current, sampledFrames: sampledFrames });
                return;
              }
            }
            if (sampledFrames >= maximumFrames) {
              reject(new Error(options.timeoutMessage || 'Layout did not reach stable dimensions.'));
              return;
            }
            requestAnimationFrame(sample);
          }
          requestAnimationFrame(sample);
        });
      });
    };

    Mosofin.readerLayout = (function () {
      var html = document.documentElement;
      var body = document.body;
      var shell = document.querySelector('.container');
      var diagram = document.querySelector('.diagram-container');
      var svg = diagram && diagram.querySelector(':scope > svg');
      var header = shell && shell.querySelector('.header');
      var guided = shell && shell.querySelector('.guided-views');
      var cards = shell && shell.querySelector('.cards');
      var viewBox = svg && svg.viewBox && svg.viewBox.baseVal;
      var ratio = viewBox && viewBox.height > 0 ? viewBox.width / viewBox.height : 0;
      var frame = 0;
      var settleFrame = 0;
      var lastWidth = 0;
      var WIDE_RATIO = 1.55;
      var MIN_DESKTOP_WIDTH = 1024;
      var MIN_READER_WIDTH = 960;
      var MAX_READER_WIDTH = 1920;
      var SAFE_BOTTOM_GAP = 12;

      if (diagram && ratio >= WIDE_RATIO) {
        diagram.setAttribute('data-wide-diagram', 'true');
        html.setAttribute('data-diagram-shape', 'wide');
      }

      function number(value) {
        var parsed = parseFloat(value);
        return Number.isFinite(parsed) ? parsed : 0;
      }
      function visible(element) {
        return Boolean(element && !element.hidden && window.getComputedStyle(element).display !== 'none');
      }
      function outerHeight(element) {
        if (!visible(element)) return 0;
        var style = window.getComputedStyle(element);
        return element.getBoundingClientRect().height + number(style.marginTop) + number(style.marginBottom);
      }
      function eligible() {
        return Boolean(
          shell && diagram && svg && ratio >= WIDE_RATIO &&
          window.innerWidth >= MIN_DESKTOP_WIDTH &&
          html.getAttribute('data-embed') !== 'true' &&
          html.getAttribute('data-present') !== 'true' &&
          (!window.matchMedia || !window.matchMedia('print').matches)
        );
      }
      function clear() {
        html.style.removeProperty('--mosofin-reader-width');
        html.removeAttribute('data-reader-layout');
        html.removeAttribute('data-reader-overflow');
        lastWidth = 0;
      }
      function chromeMetrics() {
        var bodyStyle = window.getComputedStyle(body);
        var diagramStyle = window.getComputedStyle(diagram);
        return {
          bodyX: number(bodyStyle.paddingLeft) + number(bodyStyle.paddingRight),
          bodyY: number(bodyStyle.paddingTop) + number(bodyStyle.paddingBottom),
          diagramX: number(diagramStyle.paddingLeft) + number(diagramStyle.paddingRight) +
            number(diagramStyle.borderLeftWidth) + number(diagramStyle.borderRightWidth),
          diagramY: number(diagramStyle.paddingTop) + number(diagramStyle.paddingBottom) +
            number(diagramStyle.borderTopWidth) + number(diagramStyle.borderBottomWidth)
        };
      }
      function applyWidth(width) {
        var rounded = Math.round(width);
        if (Math.abs(rounded - lastWidth) < 1) return false;
        lastWidth = rounded;
        html.style.setProperty('--mosofin-reader-width', rounded + 'px');
        html.setAttribute('data-reader-layout', 'adaptive');
        return true;
      }
      function settleOverflow(minWidth) {
        if (settleFrame) cancelAnimationFrame(settleFrame);
        settleFrame = requestAnimationFrame(function () {
          settleFrame = 0;
          if (!eligible() || !lastWidth) return;
          var overflow = Math.max(
            document.documentElement.scrollHeight,
            document.body.scrollHeight
          ) - window.innerHeight;
          if (overflow > 1 && lastWidth > minWidth) {
            applyWidth(Math.max(minWidth, lastWidth - overflow * ratio - 4));
            html.setAttribute('data-reader-overflow', 'reduced');
          } else if (overflow > 1) {
            html.setAttribute('data-reader-overflow', 'authored');
          } else {
            html.removeAttribute('data-reader-overflow');
          }
        });
      }
      function measure() {
        frame = 0;
        if (!eligible()) {
          clear();
          return null;
        }
        var chrome = chromeMetrics();
        var viewportCap = Math.max(0, window.innerWidth - chrome.bodyX);
        var minWidth = Math.min(MIN_READER_WIDTH, viewportCap);
        var maxWidth = Math.min(MAX_READER_WIDTH, viewportCap);
        var fixedHeight = chrome.bodyY + chrome.diagramY + SAFE_BOTTOM_GAP +
          outerHeight(header) + outerHeight(guided) + outerHeight(cards);
        var availableSvgHeight = Math.max(1, window.innerHeight - fixedHeight);
        var desiredWidth = availableSvgHeight * ratio + chrome.diagramX;
        var width = Math.max(minWidth, Math.min(maxWidth, desiredWidth));
        applyWidth(width);
        settleOverflow(minWidth);
        return {
          ratio: ratio,
          width: Math.round(width),
          availableSvgHeight: Math.round(availableSvgHeight),
          fixedHeight: Math.round(fixedHeight)
        };
      }
      function schedule() {
        if (frame) return;
        frame = requestAnimationFrame(measure);
      }
      function stableSnapshot() {
        var shellRect = shell ? shell.getBoundingClientRect() : { width: 0, height: 0 };
        var diagramRect = diagram ? diagram.getBoundingClientRect() : { width: 0, height: 0 };
        return [
          lastWidth,
          html.getAttribute('data-reader-layout') || '',
          html.getAttribute('data-reader-overflow') || '',
          Math.ceil(document.documentElement.scrollWidth),
          Math.ceil(document.documentElement.scrollHeight),
          Math.ceil(document.body.scrollWidth),
          Math.ceil(document.body.scrollHeight),
          Math.round(shellRect.width * 100) / 100,
          Math.round(shellRect.height * 100) / 100,
          Math.round(diagramRect.width * 100) / 100,
          Math.round(diagramRect.height * 100) / 100
        ].join('|');
      }
      function whenStable() {
        return Mosofin.waitForStableLayout({
          schedule: schedule,
          pending: function () { return Boolean(frame || settleFrame); },
          snapshot: stableSnapshot,
          timeoutMessage: 'Adaptive reader layout did not reach stable dimensions.'
        });
      }

      window.addEventListener('resize', schedule, { passive: true });
      window.addEventListener('load', schedule, { once: true });
      if (document.fonts && document.fonts.ready) document.fonts.ready.then(schedule).catch(function () {});
      if (typeof ResizeObserver === 'function') {
        var resizeObserver = new ResizeObserver(schedule);
        [header, guided, cards].forEach(function (element) { if (element) resizeObserver.observe(element); });
      }
      if (typeof MutationObserver === 'function') {
        var contentObserver = new MutationObserver(schedule);
        if (guided) contentObserver.observe(guided, { attributes: true, childList: true, subtree: true });
        if (cards) contentObserver.observe(cards, { attributes: true, childList: true, subtree: true });
        contentObserver.observe(html, { attributes: true, attributeFilter: ['data-embed', 'data-present'] });
      }
      schedule();

      return {
        measure: measure,
        schedule: schedule,
        whenStable: whenStable,
        active: function () { return html.getAttribute('data-reader-layout') === 'adaptive'; },
        receipt: function () { return { ratio: ratio, width: lastWidth }; }
      };
    })();

    /* ============================================================
       Viewer Chrome Layout — keep HTML controls clear of the canonical SVG
       legend without changing authored geometry. The dock keeps its existing
       position unless its zero-reserve position would cover the legend; only
       then does a small bottom safe rail become part of the reader budget.
       ============================================================ */
    Mosofin.viewerChromeLayout = (function () {
      var html = document.documentElement;
      var container = document.querySelector('.diagram-container');
      var svg = container && container.querySelector(':scope > svg');
      var nav = container && container.querySelector('.diagram-nav');
      var legend = svg && svg.querySelector('[data-legend]');
      var frame = 0;
      var settleFrame = 0;
      var reserve = 0;
      var collisionLatched = false;
      var probingBaseline = false;
      var probePromise = null;
      var baselineIntersectionArea = 0;
      var lastReceipt = null;
      var SAFE_GAP = 10;

      function visible(element) {
        if (!element || element.hidden) return false;
        var style = window.getComputedStyle(element);
        return style.display !== 'none' && style.visibility !== 'hidden';
      }
      function usable(rect) {
        return Boolean(rect && rect.width > 0 && rect.height > 0);
      }
      function intersectionArea(a, b) {
        var width = Math.max(0, Math.min(a.right, b.right) - Math.max(a.left, b.left));
        var height = Math.max(0, Math.min(a.bottom, b.bottom) - Math.max(a.top, b.top));
        return width * height;
      }
      function eligible() {
        return Boolean(
          container && svg && nav && legend &&
          window.innerWidth > 720 &&
          html.getAttribute('data-embed') !== 'true' &&
          (!window.matchMedia || !window.matchMedia('print').matches) &&
          visible(nav) && visible(legend)
        );
      }
      function writeReserve(next, options) {
        options = options || {};
        next = Math.max(0, Math.ceil(next));
        if (Math.abs(next - reserve) < 1) return false;
        reserve = next;
        if (reserve) {
          container.style.setProperty('--mosofin-nav-reserve', reserve + 'px');
          container.setAttribute('data-nav-safe-rail', 'true');
        } else {
          container.style.removeProperty('--mosofin-nav-reserve');
          container.removeAttribute('data-nav-safe-rail');
        }
        if (Mosofin.readerLayout && typeof Mosofin.readerLayout.schedule === 'function') {
          Mosofin.readerLayout.schedule();
        }
        if (options.quiet !== true) {
          if (settleFrame) cancelAnimationFrame(settleFrame);
          settleFrame = requestAnimationFrame(function () {
            settleFrame = 0;
            schedule();
          });
        }
        return true;
      }
      function clear() {
        collisionLatched = false;
        baselineIntersectionArea = 0;
        writeReserve(0, { quiet: probingBaseline });
        lastReceipt = {
          eligible: false,
          active: false,
          reserve: 0,
          gap: SAFE_GAP,
          baselineIntersectionArea: 0,
          intersectionArea: 0
        };
        return lastReceipt;
      }
      function measure() {
        frame = 0;
        if (probingBaseline) return null;
        if (!eligible()) return clear();

        var navRect = nav.getBoundingClientRect();
        var legendRect = legend.getBoundingClientRect();
        var svgRect = svg.getBoundingClientRect();
        if (!usable(navRect) || !usable(legendRect) || !usable(svgRect)) return clear();

        var actualIntersectionArea = intersectionArea(navRect, legendRect);
        if (!collisionLatched && reserve === 0) {
          baselineIntersectionArea = actualIntersectionArea;
          if (actualIntersectionArea > 0) {
            collisionLatched = true;
            if (writeReserve(Math.max(0, svgRect.bottom + SAFE_GAP - navRect.top))) return null;
          }
        } else if (collisionLatched && reserve > 0 && actualIntersectionArea > 0) {
          /* Once zero-reserve geometry proves a collision, keep that decision
             latched while Adaptive Reader incorporates the rail. If the first
             rail is not quite tall enough (notably in Presentation flex
             layout), grow only by the remaining vertical overlap. */
          var remaining = Math.max(1, legendRect.bottom + SAFE_GAP - navRect.top);
          if (writeReserve(reserve + remaining)) return null;
        }

        navRect = nav.getBoundingClientRect();
        legendRect = legend.getBoundingClientRect();
        lastReceipt = {
          eligible: true,
          active: reserve > 0,
          reserve: reserve,
          gap: SAFE_GAP,
          baselineIntersectionArea: Math.round(baselineIntersectionArea * 100) / 100,
          intersectionArea: Math.round(intersectionArea(navRect, legendRect) * 100) / 100
        };
        return lastReceipt;
      }
      function reprobe() {
        if (probingBaseline) return probePromise || Promise.resolve(false);
        if (!reserve && !collisionLatched) {
          schedule();
          return Promise.resolve(false);
        }
        probingBaseline = true;
        collisionLatched = false;
        baselineIntersectionArea = 0;
        writeReserve(0, { quiet: true });
        var readerReady = Mosofin.readerLayout && typeof Mosofin.readerLayout.whenStable === 'function'
          ? Mosofin.readerLayout.whenStable().catch(function () {})
          : Promise.resolve();
        probePromise = readerReady.then(function () {
          probingBaseline = false;
          probePromise = null;
          schedule();
          return true;
        });
        return probePromise;
      }
      function schedule() {
        if (frame) return;
        frame = requestAnimationFrame(measure);
      }
      function stableSnapshot() {
        var containerRect = container ? container.getBoundingClientRect() : { width: 0, height: 0 };
        var navRect = nav ? nav.getBoundingClientRect() : { top: 0, left: 0 };
        var legendRect = legend ? legend.getBoundingClientRect() : { top: 0, left: 0 };
        return [
          reserve,
          Math.round(containerRect.width * 100) / 100,
          Math.round(containerRect.height * 100) / 100,
          Math.round(navRect.left * 100) / 100,
          Math.round(navRect.top * 100) / 100,
          Math.round(legendRect.left * 100) / 100,
          Math.round(legendRect.top * 100) / 100,
          collisionLatched ? 'latched' : 'clear',
          lastReceipt ? lastReceipt.intersectionArea : ''
        ].join('|');
      }
      function whenStable() {
        return Mosofin.waitForStableLayout({
          schedule: schedule,
          pending: function () { return Boolean(frame || settleFrame || probingBaseline); },
          snapshot: stableSnapshot,
          timeoutMessage: 'Viewer chrome layout did not reach stable dimensions.'
        });
      }

      window.addEventListener('resize', reprobe, { passive: true });
      window.addEventListener('load', schedule, { once: true });
      window.addEventListener('beforeprint', schedule);
      window.addEventListener('afterprint', reprobe);
      if (document.fonts && document.fonts.ready) document.fonts.ready.then(reprobe).catch(function () {});
      if (typeof ResizeObserver === 'function') {
        var resizeObserver = new ResizeObserver(schedule);
        [nav, legend].forEach(function (element) { if (element) resizeObserver.observe(element); });
      }
      if (typeof MutationObserver === 'function') {
        var contentObserver = new MutationObserver(function (records) {
          var viewerModeChanged = records.some(function (record) { return record.target === html; });
          if (viewerModeChanged) reprobe();
          else schedule();
        });
        if (legend) contentObserver.observe(legend, { attributes: true, childList: true, subtree: true });
        contentObserver.observe(html, {
          attributes: true,
          attributeFilter: ['data-embed', 'data-present', 'data-preset', 'data-theme']
        });
      }
      schedule();

      return {
        measure: measure,
        schedule: schedule,
        reprobe: reprobe,
        whenStable: whenStable,
        active: function () { return reserve > 0; },
        receipt: function () { return lastReceipt || measure(); }
      };
    })();

    Mosofin.view = (function () {
      var container = document.querySelector('.diagram-container');
      var svg = container.querySelector('svg');
      var outBtn = container.querySelector('[data-view="out"]');
      var resetBtn = container.querySelector('[data-view="reset"]');
      var resetDetailLabel = resetBtn.querySelector('[data-view-detail]');
      var resetPercentLabel = resetBtn.querySelector('[data-view-percent]');
      var inBtn = container.querySelector('[data-view="in"]');
      var state = { scale: 1, x: 0, y: 0, mode: 'overview' };
      var drag = null;
      var cameraTimer = null;
      var cameraFrame = null;
      var cameraGeneration = 0;
      var cameraTransaction = null;
      var resizeFrame = 0;
      var autoScrollUntil = 0;

      var viewBox = svg.viewBox && svg.viewBox.baseVal;

      function clamp() {
        var width = svg.clientWidth || 1;
        var height = svg.clientHeight || 1;
        state.x = Math.min(0, Math.max(width - width * state.scale, state.x));
        state.y = Math.min(0, Math.max(height - height * state.scale, state.y));
      }
      function reducedMotion() {
        return window.matchMedia && window.matchMedia('(prefers-reduced-motion: reduce)').matches;
      }
      function contentMetrics() {
        if (!viewBox || viewBox.width <= 0 || viewBox.height <= 0) return null;
        var width = svg.clientWidth || 1;
        var height = svg.clientHeight || 1;
        var scale = Math.min(width / viewBox.width, height / viewBox.height);
        return {
          width: width,
          height: height,
          scale: scale,
          offsetX: (width - viewBox.width * scale) / 2,
          offsetY: (height - viewBox.height * scale) / 2
        };
      }
      function logicalViewport() {
        var metrics = contentMetrics();
        if (!metrics) return null;
        var x;
        var y;
        var width;
        var height;
        if (window.innerWidth <= 720 && container.hasAttribute('data-wide-diagram')) {
          x = viewBox.x + container.scrollLeft / metrics.scale;
          y = viewBox.y;
          width = Math.min(viewBox.width, Math.max(1, container.clientWidth / metrics.scale));
          height = viewBox.height;
        } else {
          x = viewBox.x + ((-state.x / state.scale) - metrics.offsetX) / metrics.scale;
          y = viewBox.y + ((-state.y / state.scale) - metrics.offsetY) / metrics.scale;
          width = Math.min(viewBox.width, metrics.width / state.scale / metrics.scale);
          height = Math.min(viewBox.height, metrics.height / state.scale / metrics.scale);
        }
        width = Math.max(1, Math.min(viewBox.width, width));
        height = Math.max(1, Math.min(viewBox.height, height));
        x = Math.max(viewBox.x, Math.min(viewBox.x + viewBox.width - width, x));
        y = Math.max(viewBox.y, Math.min(viewBox.y + viewBox.height - height, y));
        return { x: x, y: y, width: width, height: height, scale: state.scale };
      }
      function detailLevel() {
        if (state.mode === 'semantic') return 'full';
        if (state.scale >= 1.75) return 'full';
        if (state.scale >= 1) return 'read';
        return 'map';
      }
      function renderControls() {
        var semantic = state.mode === 'semantic' && state.scale > 1.01;
        var detail = detailLevel();
        var percent = Math.round(state.scale * 100) + '%';
        var levelLabel = viewerText('viewer.nav.level.' + detail);
        var detailHint = detail === 'map'
          ? viewerText('viewer.nav.detail.map')
          : detail === 'read'
            ? viewerText('viewer.nav.detail.read')
            : viewerText('viewer.nav.detail.full');
        var resolvedLevel = semantic ? viewerText('viewer.nav.level.auto') : levelLabel;
        var showDetailLevel = semantic || detail !== 'read';
        if (resetDetailLabel) {
          resetDetailLabel.textContent = resolvedLevel;
          resetDetailLabel.hidden = !showDetailLevel;
        }
        if (resetPercentLabel) resetPercentLabel.textContent = percent;
        resetBtn.toggleAttribute('data-detail-visible', showDetailLevel);
        resetBtn.title = viewerText('viewer.nav.camera.title', {
          semantic: semantic ? viewerText('viewer.nav.camera.semantic') : '',
          hint: detailHint
        });
        resetBtn.setAttribute('aria-label', viewerText('viewer.nav.camera', { hint: detailHint }));
        resetBtn.setAttribute('data-detail-level', detail);
        container.setAttribute('data-detail-level', detail);
        container.setAttribute('data-camera-mode', state.mode);
        container.setAttribute('data-camera-indicator', semantic ? 'true' : 'false');
      }
      function apply() {
        clamp();
        svg.style.transform = 'translate(' + state.x + 'px,' + state.y + 'px) scale(' + state.scale + ')';
        renderControls();
        outBtn.disabled = state.scale <= 1;
        inBtn.disabled = state.scale >= 3;
        container.classList.toggle('is-pannable', state.scale > 1);
        svg.setAttribute('data-view-scale', String(state.scale));
        if (Mosofin.radar && typeof Mosofin.radar.sync === 'function') Mosofin.radar.sync();
        if (Mosofin.viewerChromeLayout && typeof Mosofin.viewerChromeLayout.schedule === 'function') {
          Mosofin.viewerChromeLayout.schedule();
        }
      }
      function sampleRenderedState() {
        var transform = '';
        try { transform = getComputedStyle(svg).transform || ''; } catch (_) {}
        var match = transform.match(/^matrix\(([^)]+)\)$/);
        if (!match) return { scale: state.scale, x: state.x, y: state.y, mode: state.mode };
        var values = match[1].split(',').map(Number);
        if (values.length !== 6 || !values.every(Number.isFinite)) {
          return { scale: state.scale, x: state.x, y: state.y, mode: state.mode };
        }
        return { scale: values[0], x: values[4], y: values[5], mode: state.mode };
      }
      function finishCameraTransaction(transaction, outcome) {
        if (!transaction || transaction.settled) return false;
        transaction.settled = true;
        transaction.state = outcome || 'complete';
        if (transaction.frame) cancelAnimationFrame(transaction.frame);
        if (transaction.timer) clearTimeout(transaction.timer);
        transaction.frame = null;
        transaction.timer = null;
        if (cameraTransaction === transaction) cameraTransaction = null;
        cameraFrame = null;
        cameraTimer = null;
        container.classList.remove('is-camera-moving');
        container.classList.remove('is-camera-transaction');
        container.removeAttribute('data-camera-transaction');
        if (Mosofin.focus && Mosofin.focus.reposition) Mosofin.focus.reposition();
        transaction.resolve({ id: transaction.id, state: transaction.state });
        return true;
      }
      function cameraReceipt(target, options) {
        var resolver;
        var transaction = {
          id: ++cameraGeneration,
          state: 'running',
          target: target,
          settled: false,
          frame: null,
          timer: null,
          finished: new Promise(function (resolve) { resolver = resolve; }),
          resolve: resolver,
          cancel: function (reason, commitTarget) {
            if (transaction.settled) return false;
            if (commitTarget && transaction.target) {
              if (Object.prototype.hasOwnProperty.call(transaction.target, 'scrollLeft')) {
                container.scrollLeft = transaction.target.scrollLeft;
              } else {
                state = {
                  scale: transaction.target.scale,
                  x: transaction.target.x,
                  y: transaction.target.y,
                  mode: transaction.target.mode
                };
                apply();
              }
            }
            return finishCameraTransaction(transaction, reason || 'cancelled');
          }
        };
        return transaction;
      }
      function stopCameraMotion(reason, commitTarget) {
        if (cameraTransaction && !cameraTransaction.settled) {
          cameraTransaction.cancel(reason || 'cancelled', commitTarget === true);
          return;
        }
        if (cameraTimer) clearTimeout(cameraTimer);
        if (cameraFrame) cancelAnimationFrame(cameraFrame);
        cameraTimer = null;
        cameraFrame = null;
        container.classList.remove('is-camera-moving');
        container.classList.remove('is-camera-transaction');
        container.removeAttribute('data-camera-transaction');
      }
      function interruptCamera(reason) {
        if (Mosofin.guidedViews && Mosofin.guidedViews.cancelHandoff) {
          Mosofin.guidedViews.cancelHandoff(reason || 'manual');
        }
        var rendered = sampleRenderedState();
        stopCameraMotion(reason || 'manual', false);
        state = rendered;
        state.mode = 'manual';
        apply();
        renderControls();
        if (Mosofin.guidedViews && Mosofin.guidedViews.isPlaying && Mosofin.guidedViews.isPlaying()) {
          Mosofin.guidedViews.pause();
        }
        if (Mosofin.routeProbe && Mosofin.routeProbe.isJourneyPlaying && Mosofin.routeProbe.isJourneyPlaying()) {
          Mosofin.routeProbe.pauseJourney({ preserveElapsed: true, reason: reason || 'manual' });
        }
      }
      function zoom(next, options) {
        options = options || {};
        if (options.manual !== false) interruptCamera();
        var previous = state.scale;
        next = Math.max(1, Math.min(3, Math.round(next * 4) / 4));
        if (next === previous) return;
        var centerX = (svg.clientWidth || 1) / 2;
        var centerY = (svg.clientHeight || 1) / 2;
        var contentX = (centerX - state.x) / previous;
        var contentY = (centerY - state.y) / previous;
        state.scale = next;
        state.x = centerX - contentX * next;
        state.y = centerY - contentY * next;
        apply();
      }
      function reset(options) {
        options = options || {};
        if (options.automatic !== true) interruptCamera();
        else stopCameraMotion('reset', false);
        state = { scale: 1, x: 0, y: 0, mode: 'overview' };
        apply();
        if (Mosofin.viewerChromeLayout && typeof Mosofin.viewerChromeLayout.reprobe === 'function') {
          Mosofin.viewerChromeLayout.reprobe();
        }
      }
      function centerAt(logicalX, logicalY, options) {
        options = options || {};
        logicalX = Number(logicalX);
        logicalY = Number(logicalY);
        var metrics = contentMetrics();
        if (!metrics || !Number.isFinite(logicalX) || !Number.isFinite(logicalY)) return false;
        interruptCamera();
        if (window.innerWidth <= 720 && container.hasAttribute('data-wide-diagram')) {
          state.scale = 1;
          state.x = 0;
          state.y = 0;
          state.mode = 'manual';
          apply();
          var mobileTarget = (logicalX - viewBox.x) * metrics.scale - container.clientWidth / 2;
          mobileTarget = Math.max(0, Math.min(svg.clientWidth - container.clientWidth, mobileTarget));
          autoScrollUntil = Date.now() + 80;
          try { container.scrollTo({ left: mobileTarget, behavior: options.instant ? 'auto' : 'smooth' }); }
          catch (_) { container.scrollLeft = mobileTarget; }
          return true;
        }
        var minimumScale = Math.max(1, Math.min(3, Number(options.minimumScale) || 1));
        var requestedScale = Number(options.scale);
        state.scale = Math.max(minimumScale, Math.min(3, Number.isFinite(requestedScale) ? requestedScale : state.scale));
        var contentX = metrics.offsetX + (logicalX - viewBox.x) * metrics.scale;
        var contentY = metrics.offsetY + (logicalY - viewBox.y) * metrics.scale;
        state.x = metrics.width / 2 - contentX * state.scale;
        state.y = metrics.height / 2 - contentY * state.scale;
        state.mode = 'manual';
        apply();
        if (Mosofin.focus && Mosofin.focus.reposition) Mosofin.focus.reposition();
        return true;
      }
      function semanticIds(ids, includeNeighbors) {
        var seeds = {};
        var wanted = {};
        (ids || []).forEach(function (id) { seeds[id] = true; wanted[id] = true; });
        if (includeNeighbors) {
          Array.prototype.forEach.call(svg.querySelectorAll('[data-edge-from][data-edge-to]'), function (edge) {
            var from = edge.getAttribute('data-edge-from');
            var to = edge.getAttribute('data-edge-to');
            if (seeds[from] || seeds[to]) { wanted[from] = true; wanted[to] = true; }
          });
        }
        return wanted;
      }
      function boxesFor(ids, includeNeighbors) {
        var wanted = semanticIds(ids, includeNeighbors);
        return Array.prototype.slice.call(svg.querySelectorAll('[data-node-id]'))
          .filter(function (node) { return wanted[node.getAttribute('data-node-id')]; })
          .map(function (node) {
            try { return node.getBBox(); } catch (_) { return null; }
          })
          .filter(Boolean);
      }
      function frameDesktop(ids, options) {
        options = options || {};
        var boxes = boxesFor(ids, options.includeNeighbors === true);
        if (!boxes.length || !viewBox || viewBox.width <= 0 || viewBox.height <= 0) return false;
        var svgWidth = svg.clientWidth || 1;
        var svgHeight = svg.clientHeight || 1;
        var contentScale = Math.min(svgWidth / viewBox.width, svgHeight / viewBox.height);
        var contentOffsetX = (svgWidth - viewBox.width * contentScale) / 2;
        var contentOffsetY = (svgHeight - viewBox.height * contentScale) / 2;
        var minX = Math.min.apply(Math, boxes.map(function (box) { return box.x; }));
        var minY = Math.min.apply(Math, boxes.map(function (box) { return box.y; }));
        var maxX = Math.max.apply(Math, boxes.map(function (box) { return box.x + box.width; }));
        var maxY = Math.max.apply(Math, boxes.map(function (box) { return box.y + box.height; }));
        var bounds = {
          x: contentOffsetX + minX * contentScale,
          y: contentOffsetY + minY * contentScale,
          width: Math.max(1, (maxX - minX) * contentScale),
          height: Math.max(1, (maxY - minY) * contentScale)
        };
        var padding = options.padding || 48;
        var left = padding;
        var right = svgWidth - padding;
        var top = padding;
        var bottom = svgHeight - Math.max(padding, 72);
        var containerRect = container.getBoundingClientRect();
        var visibleTop = Math.max(0, -containerRect.top);
        var visibleBottom = Math.min(svgHeight, window.innerHeight - containerRect.top);
        if (visibleBottom - visibleTop >= 240) {
          top = Math.max(top, visibleTop + padding);
          bottom = Math.min(bottom, visibleBottom - Math.max(padding, 72));
        }
        var chip = document.getElementById('focus-chip');
        if (chip && !chip.hidden) {
          var lensEnd = chip.offsetLeft + chip.offsetWidth + 24 - (svg.offsetLeft || 0);
          left = Math.max(left, Math.min(svgWidth * 0.42, lensEnd));
        }
        var routeReceipt = document.getElementById('route-probe');
        if (routeReceipt && !routeReceipt.hidden && routeReceipt.hasAttribute('data-route-journey')) {
          var receiptTop = routeReceipt.offsetTop;
          var receiptBottom = receiptTop + routeReceipt.offsetHeight;
          if (receiptTop < svgHeight / 2) top = Math.max(top, receiptBottom + 24);
          else bottom = Math.min(bottom, receiptTop - 24);
        }
        if (right <= left || bottom <= top) return false;
        var maxScale = options.maxScale || (options.includeNeighbors ? 1.9 : 2.15);
        var targetScale = Math.min((right - left) / bounds.width, (bottom - top) / bounds.height) * 0.9;
        targetScale = Math.max(1, Math.min(maxScale, targetScale));
        if (targetScale < 1.08) targetScale = 1;
        var target = {
          scale: Math.round(targetScale * 100) / 100,
          x: 0,
          y: 0,
          mode: 'semantic'
        };
        target.x = (left + right) / 2 - (bounds.x + bounds.width / 2) * target.scale;
        target.y = (top + bottom) / 2 - (bounds.y + bounds.height / 2) * target.scale;
        var start = sampleRenderedState();
        stopCameraMotion('replaced', false);
        var transaction = cameraReceipt(target, options);
        cameraTransaction = transaction;
        var instant = options.instant === true || reducedMotion() || document.hidden;
        if (instant) {
          state = target;
          apply();
          finishCameraTransaction(transaction, reducedMotion() ? 'reduced-motion' : (document.hidden ? 'hidden' : 'complete'));
          return transaction;
        }
        var duration = Math.max(180, Math.min(520, Number(options.duration) || 420));
        var startedAt = 0;
        state = start;
        state.mode = 'semantic';
        apply();
        container.classList.add('is-camera-moving');
        container.classList.add('is-camera-transaction');
        container.setAttribute('data-camera-transaction', String(transaction.id));
        var step = function (timestamp) {
          if (cameraTransaction !== transaction || transaction.settled) return;
          if (!startedAt) startedAt = timestamp;
          var fraction = Math.max(0, Math.min(1, (timestamp - startedAt) / duration));
          var eased = 1 - Math.pow(1 - fraction, 3);
          state = {
            scale: start.scale + (target.scale - start.scale) * eased,
            x: start.x + (target.x - start.x) * eased,
            y: start.y + (target.y - start.y) * eased,
            mode: 'semantic'
          };
          apply();
          if (fraction < 1) {
            transaction.frame = requestAnimationFrame(step);
            cameraFrame = transaction.frame;
          } else {
            state = target;
            apply();
            finishCameraTransaction(transaction, 'complete');
          }
        };
        transaction.frame = requestAnimationFrame(step);
        cameraFrame = transaction.frame;
        return transaction;
      }
      function reveal(ids, options) {
        options = options || {};
        if (window.innerWidth > 720) return frameDesktop(ids, options);
        stopCameraMotion('replaced', false);
        state.scale = 1;
        state.x = 0;
        state.y = 0;
        state.mode = 'semantic';
        apply();
        if (!container.hasAttribute('data-wide-diagram')) {
          var contained = cameraReceipt({ scale: 1, x: 0, y: 0, mode: 'semantic' }, options);
          cameraTransaction = contained;
          finishCameraTransaction(contained, 'complete');
          return contained;
        }
        var boxes = boxesFor(ids, options.includeNeighbors === true);
        if (!boxes.length || !viewBox || viewBox.width <= 0) return false;
        var minX = Math.min.apply(Math, boxes.map(function (box) { return box.x; }));
        var maxX = Math.max.apply(Math, boxes.map(function (box) { return box.x + box.width; }));
        var center = ((minX + maxX) / 2 / viewBox.width) * (svg.clientWidth || 1);
        var target = Math.max(0, Math.min(svg.clientWidth - container.clientWidth, center - container.clientWidth / 2));
        var transaction = cameraReceipt({ scrollLeft: target }, options);
        cameraTransaction = transaction;
        var instant = options.instant === true || reducedMotion() || document.hidden;
        autoScrollUntil = Date.now() + (instant ? 50 : 470);
        try { container.scrollTo({ left: target, behavior: instant ? 'auto' : 'smooth' }); }
        catch (_) { container.scrollLeft = target; }
        if (instant) finishCameraTransaction(transaction, reducedMotion() ? 'reduced-motion' : (document.hidden ? 'hidden' : 'complete'));
        else {
          transaction.timer = setTimeout(function () { finishCameraTransaction(transaction, 'complete'); }, 460);
          cameraTimer = transaction.timer;
          container.classList.add('is-camera-moving');
          container.setAttribute('data-camera-transaction', String(transaction.id));
        }
        return transaction;
      }
      function syncSemantic() {
        var guided = Mosofin.guidedViews && typeof Mosofin.guidedViews.focus === 'function'
          ? Mosofin.guidedViews.focus() : [];
        if (guided && guided.length) return reveal(guided, { reason: 'guided-sync' });
        var active = Mosofin.focus && typeof Mosofin.focus.active === 'function' ? Mosofin.focus.active() : null;
        if (typeof active === 'string') return reveal([active], { includeNeighbors: true, reason: 'focus-sync' });
        if (Array.isArray(active) && active.length) return reveal(active, { reason: 'selection-sync' });
        return false;
      }
      function pinControls() {
        container.style.setProperty('--mosofin-scroll-x', container.scrollLeft + 'px');
      }
      function onScroll() {
        pinControls();
        if (Mosofin.radar && typeof Mosofin.radar.sync === 'function') Mosofin.radar.sync();
        if (window.innerWidth <= 720 && container.hasAttribute('data-wide-diagram') && Date.now() > autoScrollUntil) {
          interruptCamera();
        }
      }
      function onPointerEnd(event) {
        if (!drag) return;
        var moved = drag.moved;
        drag = null;
        container.classList.remove('is-panning');
        try { container.releasePointerCapture(event.pointerId); } catch (_) {}
        if (moved) {
          container.setAttribute('data-just-panned', 'true');
          setTimeout(function () { container.removeAttribute('data-just-panned'); }, 80);
        }
      }

      inBtn.addEventListener('click', function () { zoom(state.scale + 0.25); });
      outBtn.addEventListener('click', function () { zoom(state.scale - 0.25); });
      resetBtn.addEventListener('click', reset);
      container.addEventListener('pointerdown', function (event) {
        if (state.scale <= 1 || event.button !== 0 || event.target.closest('.diagram-nav, .focus-chip, .node-finder, .diagram-guide, .overview-map, .route-probe, .semantic-lens') || event.target.closest('[data-node-id]') || event.target.closest('[data-relationship-hit-key]')) return;
        interruptCamera();
        drag = { startX: event.clientX, startY: event.clientY, x: state.x, y: state.y, moved: false };
        container.classList.add('is-panning');
        try { container.setPointerCapture(event.pointerId); } catch (_) {}
      });
      container.addEventListener('pointermove', function (event) {
        if (!drag) return;
        var dx = event.clientX - drag.startX;
        var dy = event.clientY - drag.startY;
        if (Math.abs(dx) + Math.abs(dy) > 3) drag.moved = true;
        state.x = drag.x + dx;
        state.y = drag.y + dy;
        apply();
      });
      container.addEventListener('pointerup', onPointerEnd);
      container.addEventListener('pointercancel', onPointerEnd);
      container.addEventListener('scroll', onScroll, { passive: true });
      window.addEventListener('resize', function () {
        if (resizeFrame) cancelAnimationFrame(resizeFrame);
        resizeFrame = requestAnimationFrame(function () {
          resizeFrame = 0;
          if (state.mode === 'semantic') syncSemantic();
          else apply();
        });
      });
      window.addEventListener('hashchange', function () { requestAnimationFrame(syncSemantic); });
      apply();
      pinControls();
      requestAnimationFrame(syncSemantic);

      return {
        zoomIn: function () { zoom(state.scale + 0.25); },
        zoomOut: function () { zoom(state.scale - 0.25); },
        reset: reset,
        reveal: reveal,
        centerAt: centerAt,
        logicalViewport: logicalViewport,
        sync: syncSemantic,
        state: function () { return { scale: state.scale, x: state.x, y: state.y, mode: state.mode }; }
      };
    })();

    /* ============================================================
       Semantic Radar — simplified semantic bounds + live viewport.
       The radar is built at runtime so the checked artifact still contains
       one canonical SVG block, and export serialization remains untouched.
       ============================================================ */
    Mosofin.radar = (function () {
      var container = document.querySelector('.diagram-container');
      var diagram = container.querySelector(':scope > svg');
      var panel = document.getElementById('overview-map');
      var panelHead = panel.querySelector('.overview-map-head');
      var surface = document.getElementById('overview-map-surface');
      var status = document.getElementById('overview-map-status');
      var trigger = document.getElementById('btn-overview-map');
      var closeBtn = document.getElementById('overview-map-close');
      var expandBtn = document.getElementById('overview-map-expand');
      var feedback = document.getElementById('overview-map-feedback');
      var navigation = container.querySelector('.diagram-nav');
      var passport = document.getElementById('focus-chip');
      var namespace = 'http://www.w3.org/2000/svg';
      var viewBox = diagram.viewBox && diagram.viewBox.baseVal;
      var mapSvg = document.createElementNS(namespace, 'svg');
      var nodeLayer = document.createElementNS(namespace, 'g');
      var viewport = document.createElementNS(namespace, 'rect');
      var nodes = [];
      var viewportDrag = null;
      var panelDrag = null;
      var manualPosition = null;
      var lastPlacement = null;
      var requestedOpen = false;
      var passportYielded = false;
      var passportPreviousAriaHidden = null;
      var syncFrame = 0;
      var spaceRetryTimer = 0;
      var spaceRetryCount = 0;
      var placementGap = 16;

      mapSvg.setAttribute('role', 'group');
      mapSvg.setAttribute('aria-label', viewerText('viewer.radar.nodes'));
      mapSvg.setAttribute('preserveAspectRatio', 'xMidYMid meet');
      nodeLayer.setAttribute('class', 'overview-map-nodes');
      viewport.setAttribute('class', 'overview-map-viewport');
      viewport.setAttribute('rx', '3');
      viewport.setAttribute('ry', '3');
      mapSvg.appendChild(nodeLayer);
      mapSvg.appendChild(viewport);
      surface.appendChild(mapSvg);

      function nodeLabel(node, fallback) {
        return node.getAttribute('data-node-label') ||
          (node.getAttribute('aria-label') || fallback).replace(/^Focus\s+/, '').split(',')[0];
      }
      function build() {
        if (!viewBox || viewBox.width <= 0 || viewBox.height <= 0) return false;
        mapSvg.setAttribute('viewBox', [viewBox.x, viewBox.y, viewBox.width, viewBox.height].join(' '));
        nodeLayer.textContent = '';
        nodes = [];
        Array.prototype.forEach.call(diagram.querySelectorAll('[data-node-id]'), function (node) {
          var box;
          try { box = node.getBBox(); } catch (_) { box = null; }
          if (!box || box.width <= 0 || box.height <= 0) return;
          var id = node.getAttribute('data-node-id');
          var rect = document.createElementNS(namespace, 'rect');
          rect.setAttribute('class', 'overview-map-node');
          rect.setAttribute('x', String(box.x));
          rect.setAttribute('y', String(box.y));
          rect.setAttribute('width', String(Math.max(3, box.width)));
          rect.setAttribute('height', String(Math.max(3, box.height)));
          rect.setAttribute('rx', String(Math.max(2, Math.min(8, Math.min(box.width, box.height) * 0.1))));
          rect.setAttribute('data-radar-node-id', id);
          rect.setAttribute('data-kind', node.getAttribute('data-node-kind') || 'neutral');
          rect.setAttribute('tabindex', '0');
          rect.setAttribute('role', 'button');
          rect.setAttribute('aria-label', viewerText('viewer.radar.focus', { label: nodeLabel(node, id) }));
          nodeLayer.appendChild(rect);
          nodes.push({ id: id, node: node, rect: rect });
        });
        status.textContent = viewerText('viewer.radar.fullMap', { count: nodes.length });
        return true;
      }
      function visibleRect(element) {
        if (!element || element.hidden) return null;
        var rect = element.getBoundingClientRect();
        if (rect.width <= 0 || rect.height <= 0 || rect.right <= 0 || rect.bottom <= 0 || rect.left >= window.innerWidth || rect.top >= window.innerHeight) return null;
        return { left: rect.left, top: rect.top, right: rect.right, bottom: rect.bottom, width: rect.width, height: rect.height };
      }
      function panelRectAt(position) {
        return {
          left: position.left,
          top: position.top,
          right: position.left + panel.offsetWidth,
          bottom: position.top + panel.offsetHeight,
          width: panel.offsetWidth,
          height: panel.offsetHeight
        };
      }
      function rectsIntersect(first, second, gap) {
        gap = Number(gap) || 0;
        return first.left < second.right + gap &&
          first.right > second.left - gap &&
          first.top < second.bottom + gap &&
          first.bottom > second.top - gap;
      }
      function intersectionArea(first, second) {
        var width = Math.max(0, Math.min(first.right, second.right) - Math.max(first.left, second.left));
        var height = Math.max(0, Math.min(first.bottom, second.bottom) - Math.max(first.top, second.top));
        return width * height;
      }
      function clamp(value, minimum, maximum) {
        if (maximum < minimum) return minimum;
        return Math.max(minimum, Math.min(maximum, value));
      }
      function placementContext() {
        var containerRect = visibleRect(container);
        if (!containerRect) return null;
        var controlRect = visibleRect(navigation);
        var left = Math.max(placementGap, containerRect.left + placementGap);
        var top = Math.max(placementGap, containerRect.top + placementGap);
        var right = Math.min(window.innerWidth - placementGap, containerRect.right - placementGap);
        var bottom = Math.min(window.innerHeight - placementGap, containerRect.bottom - placementGap);
        if (controlRect) bottom = Math.min(bottom, controlRect.top - placementGap);
        var lens = document.getElementById('focus-chip');
        var lensRect = visibleRect(lens);
        var legendRect = visibleRect(diagram.querySelector('[data-legend]'));
        var active = diagram.querySelector('[data-focus-selected]');
        var activeRect = visibleRect(active);
        return {
          bounds: { left: left, top: top, right: right, bottom: bottom },
          hardBlockers: [lensRect, controlRect, legendRect].filter(Boolean),
          softBlockers: [activeRect].filter(Boolean),
          preferredSide: !activeRect || activeRect.left + activeRect.width / 2 > window.innerWidth / 2 ? 'left' : 'right'
        };
      }
      function positionIsValid(position, context) {
        if (!position || !context) return false;
        var rect = panelRectAt(position);
        var bounds = context.bounds;
        if (rect.left < bounds.left || rect.top < bounds.top || rect.right > bounds.right || rect.bottom > bounds.bottom) return false;
        return context.hardBlockers.every(function (blocker) { return !rectsIntersect(rect, blocker, placementGap); });
      }
      function cornerCandidates(context) {
        var bounds = context.bounds;
        var left = bounds.left;
        var right = Math.max(left, bounds.right - panel.offsetWidth);
        var top = bounds.top;
        var bottom = Math.max(top, bounds.bottom - panel.offsetHeight);
        var preferredLeft = context.preferredSide === 'left' ? left : right;
        var alternateLeft = context.preferredSide === 'left' ? right : left;
        return [
          { left: preferredLeft, top: bottom },
          { left: alternateLeft, top: bottom },
          { left: preferredLeft, top: top },
          { left: alternateLeft, top: top }
        ].filter(function (candidate, index, all) {
          return all.findIndex(function (other) { return other.left === candidate.left && other.top === candidate.top; }) === index;
        });
      }
      function nearbyCandidates(context, reference) {
        var bounds = context.bounds;
        var maximumLeft = bounds.right - panel.offsetWidth;
        var maximumTop = bounds.bottom - panel.offsetHeight;
        var requested = reference ? {
          left: clamp(reference.left, bounds.left, maximumLeft),
          top: clamp(reference.top, bounds.top, maximumTop)
        } : null;
        var horizontal = [bounds.left, maximumLeft];
        var vertical = [bounds.top, maximumTop];
        if (requested) {
          horizontal.push(requested.left);
          vertical.push(requested.top);
        }
        context.hardBlockers.forEach(function (blocker) {
          horizontal.push(
            blocker.left - placementGap - panel.offsetWidth,
            blocker.right + placementGap
          );
          vertical.push(
            blocker.top - placementGap - panel.offsetHeight,
            blocker.bottom + placementGap
          );
        });
        horizontal = horizontal.map(function (left) { return clamp(left, bounds.left, maximumLeft); });
        vertical = vertical.map(function (top) { return clamp(top, bounds.top, maximumTop); });
        var candidates = [];
        horizontal.forEach(function (left) {
          vertical.forEach(function (top) { candidates.push({ left: left, top: top }); });
        });
        return candidates.filter(function (candidate, index, all) {
          return all.findIndex(function (other) { return other.left === candidate.left && other.top === candidate.top; }) === index;
        });
      }
      function placementScore(position, context, reference, softWeight) {
        var rect = panelRectAt(position);
        var referenceLeft = reference ? reference.left : (context.preferredSide === 'left' ? context.bounds.left : context.bounds.right - panel.offsetWidth);
        var referenceTop = reference ? reference.top : context.bounds.bottom - panel.offsetHeight;
        var distance = Math.pow(position.left - referenceLeft, 2) + Math.pow(position.top - referenceTop, 2);
        var softOverlap = context.softBlockers.reduce(function (total, blocker) { return total + intersectionArea(rect, blocker); }, 0);
        return distance + softOverlap * softWeight;
      }
      function chooseRadarPlacement(context, reference, options) {
        options = options || {};
        var softWeight = Number.isFinite(options.softWeight) ? options.softWeight : 100;
        var candidates = nearbyCandidates(context, reference).concat(cornerCandidates(context));
        var valid = candidates.filter(function (candidate) { return positionIsValid(candidate, context); });
        valid.sort(function (first, second) {
          return placementScore(first, context, reference, softWeight) - placementScore(second, context, reference, softWeight);
        });
        return valid.length ? valid[0] : null;
      }
      function applyPlacement(position, remember) {
        var useLeft = position.left + panel.offsetWidth / 2 <= window.innerWidth / 2;
        panel.setAttribute('data-docked', 'true');
        panel.setAttribute('data-dock-side', useLeft ? 'left' : 'right');
        if (useLeft) {
          panel.style.setProperty('--mosofin-radar-left', Math.round(position.left) + 'px');
          panel.style.removeProperty('--mosofin-radar-right');
        } else {
          panel.style.setProperty('--mosofin-radar-right', Math.round(window.innerWidth - position.left - panel.offsetWidth) + 'px');
          panel.style.removeProperty('--mosofin-radar-left');
        }
        panel.style.setProperty('--mosofin-radar-top', Math.round(position.top) + 'px');
        if (remember !== false) lastPlacement = { left: position.left, top: position.top };
      }
      function resetDockingStyles() {
        panel.removeAttribute('data-docked');
        panel.removeAttribute('data-dock-side');
        panel.removeAttribute('data-panel-dragging');
        panel.removeAttribute('data-placement-invalid');
        panel.removeAttribute('data-placement-degraded');
        panel.removeAttribute('data-placement-unavailable');
        panel.removeAttribute('data-compact');
        panel.removeAttribute('title');
        panel.style.removeProperty('visibility');
        panel.style.removeProperty('--mosofin-radar-right');
        panel.style.removeProperty('--mosofin-radar-left');
        panel.style.removeProperty('--mosofin-radar-top');
      }
      function updateDocking() {
        var options = arguments[0] || {};
        if (panel.hidden) return false;
        if (panelDrag) return true;
        panel.removeAttribute('data-compact');
        panel.removeAttribute('data-placement-degraded');
        panel.removeAttribute('data-placement-invalid');
        panel.removeAttribute('data-placement-unavailable');
        panel.removeAttribute('title');
        panel.style.removeProperty('visibility');
        var context = placementContext();
        if (!context) {
          resetDockingStyles();
          return false;
        }
        var reference = manualPosition || lastPlacement;
        var placementOptions = { softWeight: manualPosition ? 0 : 100 };
        var placement = manualPosition && positionIsValid(manualPosition, context)
          ? manualPosition
          : chooseRadarPlacement(context, reference, placementOptions);
        var compact = false;
        if (!placement && options.allowCompact !== false) {
          compact = true;
          panel.setAttribute('data-compact', 'true');
          panel.setAttribute('data-placement-degraded', 'true');
          panel.setAttribute('title', viewerText('viewer.radar.compacted'));
          context = placementContext();
          placement = chooseRadarPlacement(context, reference, placementOptions);
        }
        if (!placement) {
          resetDockingStyles();
          panel.setAttribute('data-placement-unavailable', 'true');
          return false;
        }
        if (manualPosition && !compact) manualPosition = { left: placement.left, top: placement.top };
        applyPlacement(placement, true);
        return true;
      }
      function clearSpaceRetry() {
        if (spaceRetryTimer) window.clearTimeout(spaceRetryTimer);
        spaceRetryTimer = 0;
      }
      function yieldPassport() {
        if (!passport || passport.hidden || passportYielded) return passportYielded;
        passportPreviousAriaHidden = passport.getAttribute('aria-hidden');
        passportYielded = true;
        passport.setAttribute('data-radar-yielded', 'true');
        passport.setAttribute('aria-hidden', 'true');
        return true;
      }
      function restorePassport() {
        if (!passport || !passportYielded) return;
        passportYielded = false;
        passport.removeAttribute('data-radar-yielded');
        if (passportPreviousAriaHidden === null) passport.removeAttribute('aria-hidden');
        else passport.setAttribute('aria-hidden', passportPreviousAriaHidden);
        passportPreviousAriaHidden = null;
      }
      function reflectVisible() {
        panel.hidden = false;
        panel.removeAttribute('data-placement-unavailable');
        trigger.setAttribute('aria-expanded', 'true');
        trigger.removeAttribute('data-radar-space-limited');
        trigger.setAttribute('aria-label', viewerText('viewer.radar.close'));
        trigger.title = viewerText('viewer.nav.radar.title');
        feedback.hidden = true;
      }
      function scheduleSpaceRetry() {
        if (!requestedOpen || spaceRetryTimer || spaceRetryCount >= 4) return;
        spaceRetryCount += 1;
        spaceRetryTimer = window.setTimeout(function () {
          spaceRetryTimer = 0;
          attemptRequestedOpen();
        }, 60);
      }
      function reflectUnavailable() {
        restorePassport();
        panel.hidden = true;
        panel.setAttribute('data-placement-unavailable', 'true');
        trigger.setAttribute('aria-expanded', 'false');
        trigger.setAttribute('aria-label', viewerText('viewer.radar.cancelWaiting'));
        trigger.setAttribute('data-radar-space-limited', 'true');
        trigger.title = viewerText('viewer.radar.needsSpace');
        feedback.hidden = false;
        scheduleSpaceRetry();
      }
      function attemptRequestedOpen(options) {
        options = options || {};
        if (!requestedOpen) return false;
        panel.hidden = false;
        if (!updateDocking(options)) {
          reflectUnavailable();
          return false;
        }
        clearSpaceRetry();
        spaceRetryCount = 0;
        reflectVisible();
        sync();
        if (options.focus === true && !panel.hasAttribute('data-compact')) surface.focus();
        return true;
      }
      function expandCompactRadar() {
        if (!requestedOpen || panel.hidden || !panel.hasAttribute('data-compact')) return false;
        yieldPassport();
        if (!updateDocking({ allowCompact: false })) {
          restorePassport();
          if (!updateDocking()) {
            reflectUnavailable();
            return false;
          }
        }
        reflectVisible();
        sync();
        if (!panel.hasAttribute('data-compact')) surface.focus();
        return !panel.hasAttribute('data-compact');
      }
      function syncNow() {
        syncFrame = 0;
        if (panel.hidden || !Mosofin.view || typeof Mosofin.view.logicalViewport !== 'function') return;
        if (!updateDocking()) {
          reflectUnavailable();
          return;
        }
        if (passportYielded && panel.hasAttribute('data-compact')) {
          restorePassport();
          if (!updateDocking()) {
            reflectUnavailable();
            return;
          }
        }
        reflectVisible();
        var visible = Mosofin.view.logicalViewport();
        if (!visible) return;
        viewport.setAttribute('x', String(visible.x));
        viewport.setAttribute('y', String(visible.y));
        viewport.setAttribute('width', String(visible.width));
        viewport.setAttribute('height', String(visible.height));
        var full = visible.width >= viewBox.width * 0.98 && visible.height >= viewBox.height * 0.98;
        var mobileWide = window.innerWidth <= 720 && container.hasAttribute('data-wide-diagram');
        var viewportCopy = full
          ? viewerText('viewer.radar.viewport.full')
          : (mobileWide
            ? viewerText('viewer.radar.viewport.width', { percent: Math.round(visible.width / viewBox.width * 100) })
            : viewerText('viewer.radar.viewport.scale', { percent: Math.round(visible.scale * 100) }));
        status.textContent = viewerText('viewer.radar.status', { count: nodes.length, viewport: viewportCopy });
        nodes.forEach(function (item) {
          var active = item.node.hasAttribute('data-focus-selected') ||
            item.node.getAttribute('data-story-beat-state') === 'active';
          if (active) item.rect.setAttribute('data-radar-active', 'true');
          else item.rect.removeAttribute('data-radar-active');
        });
      }
      function sync() {
        if (requestedOpen && panel.hidden) {
          attemptRequestedOpen();
          return;
        }
        if (syncFrame) return;
        syncFrame = requestAnimationFrame(syncNow);
      }
      function setOpen(next, options) {
        options = options || {};
        next = Boolean(next);
        if (next && Mosofin.semanticLens && typeof Mosofin.semanticLens.clearPreview === 'function') Mosofin.semanticLens.clearPreview();
        if (next && Mosofin.semanticLens && Mosofin.semanticLens.isOpen()) {
          Mosofin.semanticLens.close({ restoreFocus: false });
        }
        requestedOpen = next;
        if (next) {
          clearSpaceRetry();
          spaceRetryCount = 0;
          build();
          attemptRequestedOpen(options);
        } else {
          clearSpaceRetry();
          spaceRetryCount = 0;
          viewportDrag = null;
          panelDrag = null;
          container.classList.remove('is-panning');
          panel.hidden = true;
          resetDockingStyles();
          restorePassport();
          feedback.hidden = true;
          trigger.setAttribute('aria-expanded', 'false');
          trigger.removeAttribute('data-radar-space-limited');
          trigger.setAttribute('aria-label', viewerText('viewer.nav.radar'));
          trigger.title = viewerText('viewer.nav.radar.title');
          if (options.restoreFocus === true) trigger.focus();
        }
        return next;
      }
      function toggle() { return setOpen(!requestedOpen, { focus: false }); }
      function close(options) { return setOpen(false, options); }
      function bringNodeIntoWindow(node) {
        var delay = window.matchMedia && window.matchMedia('(prefers-reduced-motion: reduce)').matches ? 0 : 540;
        window.setTimeout(function () {
          var rect = node.getBoundingClientRect();
          var safeTop = 64;
          var safeBottom = window.innerHeight - 64;
          if (rect.top >= safeTop && rect.bottom <= safeBottom) return;
          var top = Math.max(0, window.scrollY + rect.top + rect.height / 2 - window.innerHeight / 2);
          try { window.scrollTo({ top: top, behavior: delay ? 'smooth' : 'auto' }); }
          catch (_) { window.scrollTo(0, top); }
        }, delay);
      }
      function focusNode(id) {
        var main = diagram.querySelector('[data-node-id="' + id + '"]');
        if (!main) return false;
        if (Mosofin.guidedViews && typeof Mosofin.guidedViews.showAll === 'function') {
          Mosofin.guidedViews.showAll({ clearFocus: false, updateUrl: false });
        }
        if (Mosofin.focus && typeof Mosofin.focus.set === 'function') {
          Mosofin.focus.set(id, { toggle: false });
        }
        if (Mosofin.view && typeof Mosofin.view.reveal === 'function') {
          Mosofin.view.reveal([id], { includeNeighbors: true, reason: 'radar' });
        }
        bringNodeIntoWindow(main);
        try { main.focus({ preventScroll: true }); } catch (_) { try { main.focus(); } catch (_) {} }
        sync();
        return true;
      }
      function diagramPoint(event) {
        var matrix = mapSvg.getScreenCTM();
        if (!matrix) return null;
        var point = mapSvg.createSVGPoint();
        point.x = event.clientX;
        point.y = event.clientY;
        return point.matrixTransform(matrix.inverse());
      }
      function navigate(event) {
        var point = diagramPoint(event);
        if (!point || !Mosofin.view || typeof Mosofin.view.centerAt !== 'function') return;
        Mosofin.view.centerAt(point.x, point.y, { minimumScale: 1.5, instant: true });
        sync();
      }
      function endViewportDrag(event) {
        if (!viewportDrag) return;
        viewportDrag = null;
        panel.removeAttribute('data-dragging');
        container.classList.remove('is-panning');
        try { surface.releasePointerCapture(event.pointerId); } catch (_) {}
      }
      function beginPanelDrag(event) {
        if (event.button !== 0 || event.target.closest('button, a, input, [role="button"]')) return;
        var context = placementContext();
        if (!context) return;
        var rect = panel.getBoundingClientRect();
        event.preventDefault();
        event.stopPropagation();
        panelDrag = {
          pointerId: event.pointerId,
          originX: event.clientX,
          originY: event.clientY,
          start: { left: rect.left, top: rect.top },
          current: { left: rect.left, top: rect.top },
          previousManual: manualPosition ? { left: manualPosition.left, top: manualPosition.top } : null
        };
        panel.setAttribute('data-panel-dragging', 'true');
        try { panelHead.setPointerCapture(event.pointerId); } catch (_) {}
      }
      function movePanelDrag(event) {
        if (!panelDrag || panelDrag.pointerId !== event.pointerId) return;
        var context = placementContext();
        if (!context) return;
        event.preventDefault();
        event.stopPropagation();
        var bounds = context.bounds;
        var position = {
          left: clamp(panelDrag.start.left + event.clientX - panelDrag.originX, bounds.left, bounds.right - panel.offsetWidth),
          top: clamp(panelDrag.start.top + event.clientY - panelDrag.originY, bounds.top, bounds.bottom - panel.offsetHeight)
        };
        panelDrag.current = position;
        if (positionIsValid(position, context)) panel.removeAttribute('data-placement-invalid');
        else panel.setAttribute('data-placement-invalid', 'true');
        applyPlacement(position, false);
      }
      function finishPanelDrag(event, cancel) {
        if (!panelDrag || panelDrag.pointerId !== event.pointerId) return;
        event.preventDefault();
        event.stopPropagation();
        var activeDrag = panelDrag;
        panelDrag = null;
        panel.removeAttribute('data-panel-dragging');
        panel.removeAttribute('data-placement-invalid');
        try { panelHead.releasePointerCapture(event.pointerId); } catch (_) {}
        var context = placementContext();
        if (!context) return;
        if (cancel) {
          manualPosition = activeDrag.previousManual;
          lastPlacement = { left: activeDrag.start.left, top: activeDrag.start.top };
          updateDocking();
          return;
        }
        var requested = activeDrag.current;
        manualPosition = { left: requested.left, top: requested.top };
        updateDocking();
      }

      trigger.addEventListener('click', toggle);
      closeBtn.addEventListener('click', function () { close({ restoreFocus: true }); });
      expandBtn.addEventListener('click', expandCompactRadar);
      panelHead.addEventListener('pointerdown', beginPanelDrag);
      panelHead.addEventListener('pointermove', movePanelDrag);
      panelHead.addEventListener('pointerup', function (event) { finishPanelDrag(event, false); });
      panelHead.addEventListener('pointercancel', function (event) { finishPanelDrag(event, true); });
      surface.addEventListener('pointerdown', function (event) {
        if (event.button !== 0 || event.target.closest('[data-radar-node-id]')) return;
        event.preventDefault();
        viewportDrag = { pointerId: event.pointerId };
        panel.setAttribute('data-dragging', 'true');
        container.classList.add('is-panning');
        try { surface.setPointerCapture(event.pointerId); } catch (_) {}
        navigate(event);
      });
      surface.addEventListener('pointermove', function (event) {
        if (!viewportDrag || viewportDrag.pointerId !== event.pointerId) return;
        navigate(event);
      });
      surface.addEventListener('pointerup', endViewportDrag);
      surface.addEventListener('pointercancel', endViewportDrag);
      surface.addEventListener('click', function (event) {
        var node = event.target.closest('[data-radar-node-id]');
        if (node) focusNode(node.getAttribute('data-radar-node-id'));
      });
      surface.addEventListener('keydown', function (event) {
        if (event.key === 'Escape') {
          event.preventDefault();
          event.stopPropagation();
          close({ restoreFocus: true });
          return;
        }
        var node = event.target.closest('[data-radar-node-id]');
        if (node && (event.key === 'Enter' || event.key === ' ')) {
          event.preventDefault();
          focusNode(node.getAttribute('data-radar-node-id'));
          return;
        }
        if (event.key !== 'ArrowLeft' && event.key !== 'ArrowRight' && event.key !== 'ArrowUp' && event.key !== 'ArrowDown') return;
        var visible = Mosofin.view && Mosofin.view.logicalViewport ? Mosofin.view.logicalViewport() : null;
        if (!visible) return;
        event.preventDefault();
        var x = visible.x + visible.width / 2;
        var y = visible.y + visible.height / 2;
        var stepX = Math.max(12, visible.width * 0.24);
        var stepY = Math.max(12, visible.height * 0.24);
        if (event.key === 'ArrowLeft') x -= stepX;
        else if (event.key === 'ArrowRight') x += stepX;
        else if (event.key === 'ArrowUp') y -= stepY;
        else y += stepY;
        Mosofin.view.centerAt(x, y, { minimumScale: 1.5, instant: true });
        sync();
      });
      document.addEventListener('keydown', function (event) {
        if (!panelDrag || event.key !== 'Escape') return;
        finishPanelDrag({
          pointerId: panelDrag.pointerId,
          preventDefault: function () { event.preventDefault(); },
          stopPropagation: function () { event.stopPropagation(); }
        }, true);
      }, true);
      function reflow() {
        if (!requestedOpen) return;
        clearSpaceRetry();
        spaceRetryCount = 0;
        sync();
      }
      window.addEventListener('resize', reflow);
      window.addEventListener('scroll', reflow, { passive: true });
      if (typeof ResizeObserver === 'function') {
        var radarResizeObserver = new ResizeObserver(function (entries) {
          if (!requestedOpen) return;
          if (panel.hidden && entries.every(function (entry) { return entry.target === panel; })) return;
          reflow();
        });
        [container, navigation, document.getElementById('focus-chip'), panel].filter(Boolean).forEach(function (element) {
          radarResizeObserver.observe(element);
        });
      }
      requestAnimationFrame(build);

      return {
        open: function () { return setOpen(true); },
        close: close,
        toggle: toggle,
        sync: sync,
        focus: focusNode,
        isOpen: function () { return requestedOpen; },
        count: function () { return nodes.length; }
      };
    })();

    /* ============================================================
       Presentation Stage — a shareable, viewport-filling live view.
       It changes only HTML layout and URL state; SVG semantics and export
       serialization remain untouched. Escape first clears an active guided
       view/focus, then exits the stage.
       ============================================================ */
    Mosofin.presentation = (function () {
      var html = document.documentElement;
      var btn = document.getElementById('btn-present');
      var label = document.getElementById('present-label');
      var previousScrollY = 0;

      function active() { return html.getAttribute('data-present') === 'true'; }

      function updateUrl(next) {
        try {
          var url = new URL(window.location.href);
          if (next) url.searchParams.set('present', '1');
          else url.searchParams.delete('present');
          history.replaceState(null, '', url.pathname + url.search + url.hash);
        } catch (_) {}
      }

      function render(next) {
        btn.setAttribute('aria-pressed', next ? 'true' : 'false');
        btn.setAttribute('aria-label', viewerText(next ? 'viewer.present.exit' : 'viewer.present.enter'));
        btn.title = viewerText(next ? 'viewer.present.exit.title' : 'viewer.present.enter.title');
        label.textContent = viewerText(next ? 'viewer.present.exit.label' : 'viewer.present.present');
      }

      function setActive(next, options) {
        options = options || {};
        next = Boolean(next);
        if (next === active()) {
          render(next);
          return next;
        }
        if (next) {
          if (Mosofin.semanticLens && typeof Mosofin.semanticLens.clearPreview === 'function') Mosofin.semanticLens.clearPreview();
          previousScrollY = window.scrollY || 0;
          html.setAttribute('data-present', 'true');
          try { window.scrollTo(0, 0); } catch (_) {}
        } else {
          html.removeAttribute('data-present');
        }
        render(next);
        if (options.updateUrl !== false) updateUrl(next);
        requestAnimationFrame(function () {
          if (Mosofin.view && typeof Mosofin.view.reset === 'function') {
            Mosofin.view.reset({ automatic: true });
            requestAnimationFrame(function () {
              if (Mosofin.view && typeof Mosofin.view.sync === 'function') Mosofin.view.sync();
            });
          }
          if (!next && previousScrollY) {
            try { window.scrollTo(0, previousScrollY); } catch (_) {}
          }
        });
        return next;
      }

      function toggle() { return setActive(!active()); }

      render(active());
      btn.addEventListener('click', toggle);

      return {
        enter: function () { return setActive(true); },
        exit: function () { return setActive(false); },
        toggle: toggle,
        active: active
      };
    })();

    /* ============================================================
       Node Finder — stable-ID search over the existing semantic SVG.
       Search never changes the IR or SVG geometry: selecting a result resets
       the viewport, releases a guided view, and delegates to semantic focus.
       ============================================================ */
    Mosofin.finder = (function () {
      var html = document.documentElement;
      var container = document.querySelector('.diagram-container');
      var svg = container.querySelector('svg');
      var trigger = document.getElementById('btn-node-finder');
      var panel = document.getElementById('node-finder');
      var heading = document.getElementById('node-finder-title');
      var closeBtn = document.getElementById('node-finder-close');
      var input = document.getElementById('node-finder-input');
      var results = document.getElementById('node-finder-results');
      var empty = document.getElementById('node-finder-empty');
      var status = document.getElementById('node-finder-status');
      var visibleItems = [];

      function defaultContext() {
        return {
          kind: 'focus',
          title: viewerText('viewer.finder.title'),
          placeholder: viewerText('viewer.finder.placeholder'),
          empty: viewerText('viewer.finder.empty'),
          resultsLabel: viewerText('viewer.finder.results'),
          availableNoun: viewerText('viewer.finder.noun.nodes'),
          allowedIds: null,
          badges: null
        };
      }

      var context = defaultContext();

      function semanticType(node) {
        var authored = node.getAttribute('data-node-kind');
        if (authored) return authored;
        var types = ['frontend', 'backend', 'database', 'cloud', 'security', 'messagebus', 'external'];
        return types.find(function (type) { return node.querySelector('.c-' + type); }) || 'node';
      }

      function connectionsFor(id) {
        var count = 0;
        var seen = {};
        Array.prototype.forEach.call(svg.querySelectorAll('[data-edge-from][data-edge-to]'), function (edge) {
          var from = edge.getAttribute('data-edge-from');
          var to = edge.getAttribute('data-edge-to');
          var key = from + '\u0000' + to;
          if ((from === id || to === id) && !seen[key]) {
            seen[key] = true;
            count += 1;
          }
        });
        return count;
      }

      var items = Array.prototype.map.call(svg.querySelectorAll('[data-node-id]'), function (node) {
        var id = node.getAttribute('data-node-id');
        var label = node.getAttribute('data-node-label') || (node.getAttribute('aria-label') || id).replace(/^Focus\s+/, '');
        var text = (node.textContent || '').replace(/\s+/g, ' ').trim();
        var sublabel = node.getAttribute('data-node-sublabel') || '';
        var context = node.getAttribute('data-node-context') || '';
        var tag = node.getAttribute('data-node-tag') || '';
        var brand = node.getAttribute('data-node-brand') || '';
        var type = semanticType(node);
        var sources = Mosofin.sourceEvidence.node(id);
        var sourceSearch = sources.map(function (source) {
          return [source.path, source.label, source.line, source.endLine].filter(Boolean).join(' ');
        }).join(' ');
        return {
          id: id,
          label: label,
          type: type,
          sublabel: sublabel,
          context: context,
          tag: tag,
          brand: brand,
          sources: sources,
          links: connectionsFor(id),
          search: (id + ' ' + label + ' ' + type + ' ' + sublabel + ' ' + context + ' ' + tag + ' ' + sourceSearch + ' ' + text).toLowerCase() + ' ' + brand.toLowerCase(),
          node: node
        };
      });

      function resultButtons() {
        return Array.prototype.slice.call(results.querySelectorAll('.node-finder-result'));
      }

      function resolveContext(options) {
        var requested = options && options.context ? options.context : null;
        if (!requested && Mosofin.routeProbe && typeof Mosofin.routeProbe.finderContext === 'function') {
          requested = Mosofin.routeProbe.finderContext();
        }
        if (!requested) return defaultContext();
        var resolved = defaultContext();
        Object.keys(requested).forEach(function (key) { resolved[key] = requested[key]; });
        return resolved;
      }

      function availableItems() {
        if (!context.allowedIds) return items.slice();
        return items.filter(function (item) { return context.allowedIds.indexOf(item.id) !== -1; });
      }

      function applyContext() {
        panel.setAttribute('data-context', context.kind);
        heading.textContent = context.title;
        input.placeholder = context.placeholder;
        empty.textContent = context.empty;
        results.setAttribute('aria-label', context.resultsLabel);
      }

      function select(id) {
        var item = items.find(function (candidate) { return candidate.id === id; });
        if (!item) return false;
        var routeSelection = context.kind === 'route-source' || context.kind === 'route-target';
        if (routeSelection) {
          if (!Mosofin.routeProbe || typeof Mosofin.routeProbe.choose !== 'function' || !Mosofin.routeProbe.choose(id)) return false;
          if (Mosofin.routeProbe.active() === 'target' && Mosofin.view && typeof Mosofin.view.reveal === 'function') {
            Mosofin.view.reveal([id], { includeNeighbors: true, reason: 'route-pick' });
          }
          close({ restoreFocus: false });
          try { item.node.focus({ preventScroll: true }); } catch (_) { try { item.node.focus(); } catch (_) {} }
          return true;
        }
        if (Mosofin.guidedViews && typeof Mosofin.guidedViews.showAll === 'function') {
          Mosofin.guidedViews.showAll({ clearFocus: false, updateUrl: false });
        }
        if (Mosofin.view && typeof Mosofin.view.reset === 'function') Mosofin.view.reset({ automatic: true });
        Mosofin.focus.set(id, { toggle: false });
        if (Mosofin.view && typeof Mosofin.view.reveal === 'function') {
          Mosofin.view.reveal([id], { includeNeighbors: true, reason: 'finder' });
        }
        close({ restoreFocus: false });
        try { item.node.focus({ preventScroll: true }); } catch (_) { try { item.node.focus(); } catch (_) {} }
        return true;
      }

      function render(query) {
        query = (query || '').trim().toLowerCase();
        var available = availableItems();
        visibleItems = available.filter(function (item) { return !query || item.search.indexOf(query) !== -1; });
        results.textContent = '';
        visibleItems.forEach(function (item) {
          var button = document.createElement('button');
          button.type = 'button';
          button.className = 'node-finder-result';
          button.setAttribute('data-node-id', item.id);
          var badge = context.badges && context.badges[item.id]
            ? context.badges[item.id]
            : context.kind === 'focus'
              ? viewerCount('viewer.finder.link', item.links)
              : String(item.links);
          var action = context.kind === 'route-source'
            ? viewerText('viewer.finder.result.routeStart', { label: item.label })
            : context.kind === 'route-target'
              ? viewerText('viewer.finder.result.routeTarget', { label: item.label, links: badge })
              : viewerCount('viewer.finder.result.focus', item.links, { label: item.label });
          button.setAttribute('aria-label', action);

          var name = document.createElement('strong');
          name.textContent = item.label;
          var links = document.createElement('em');
          links.textContent = badge;
          links.title = context.kind === 'focus' ? badge : action;
          var meta = document.createElement('small');
          meta.textContent = [viewerKindLabel(item.type), item.id, item.sublabel, item.tag].filter(Boolean).join(' \u00b7 ');
          meta.title = [viewerKindLabel(item.type), item.id, item.context, item.sublabel, item.tag].filter(Boolean).join(' \u00b7 ');
          button.appendChild(name);
          button.appendChild(links);
          button.appendChild(meta);
          button.addEventListener('click', function () { select(item.id); });
          results.appendChild(button);
        });
        empty.hidden = visibleItems.length !== 0;
        status.textContent = query
          ? viewerText('viewer.finder.status.filtered', {
              visible: visibleItems.length,
              available: available.length,
              noun: context.availableNoun
            })
          : viewerText('viewer.finder.status.all', { available: available.length, noun: context.availableNoun });
      }

      function open(options) {
        if (html.getAttribute('data-embed') === 'true') return false;
        if (Mosofin.semanticLens && typeof Mosofin.semanticLens.clearPreview === 'function') Mosofin.semanticLens.clearPreview();
        if (Mosofin.exportMenu && Mosofin.exportMenu.isOpen()) Mosofin.exportMenu.close(false);
        if (Mosofin.semanticLens && Mosofin.semanticLens.isOpen()) Mosofin.semanticLens.close({ restoreFocus: false });
        context = resolveContext(options || {});
        applyContext();
        panel.hidden = false;
        trigger.setAttribute('aria-expanded', 'true');
        if (context.kind.indexOf('route-') === 0 && Mosofin.routeProbe && typeof Mosofin.routeProbe.finderOpening === 'function') {
          Mosofin.routeProbe.finderOpening();
        }
        input.value = '';
        render('');
        requestAnimationFrame(function () { input.focus(); });
        return true;
      }

      function close(options) {
        options = options || {};
        var routeContext = context.kind.indexOf('route-') === 0;
        panel.hidden = true;
        trigger.setAttribute('aria-expanded', 'false');
        input.value = '';
        if (routeContext && Mosofin.routeProbe && typeof Mosofin.routeProbe.finderClosed === 'function') {
          Mosofin.routeProbe.finderClosed({ restoreFocus: options.restoreFocus !== false });
        } else if (options.restoreFocus !== false) {
          trigger.focus();
        }
      }

      function toggle() { return panel.hidden ? open() : (close(), false); }

      trigger.addEventListener('click', toggle);
      closeBtn.addEventListener('click', function () { close(); });
      input.addEventListener('input', function () { render(input.value); });
      input.addEventListener('keydown', function (event) {
        var buttons = resultButtons();
        if (event.key === 'ArrowDown' && buttons.length) {
          event.preventDefault();
          buttons[0].focus();
        } else if (event.key === 'Enter' && visibleItems.length) {
          event.preventDefault();
          select(visibleItems[0].id);
        }
      });
      results.addEventListener('keydown', function (event) {
        var buttons = resultButtons();
        var index = buttons.indexOf(document.activeElement);
        if (index < 0 || !buttons.length) return;
        var next = null;
        if (event.key === 'ArrowDown') next = (index + 1) % buttons.length;
        else if (event.key === 'ArrowUp') next = (index - 1 + buttons.length) % buttons.length;
        else if (event.key === 'Home') next = 0;
        else if (event.key === 'End') next = buttons.length - 1;
        if (next !== null) {
          event.preventDefault();
          buttons[next].focus();
        }
      });
      panel.addEventListener('keydown', function (event) {
        if (event.key !== 'Escape') return;
        event.preventDefault();
        event.stopPropagation();
        close();
      });
      document.addEventListener('click', function (event) {
        if (!panel.hidden && !panel.contains(event.target) && !event.target.closest('[data-node-finder-trigger]')) close({ restoreFocus: false });
      });

      render('');
      return {
        open: open,
        close: close,
        toggle: toggle,
        select: select,
        isOpen: function () { return !panel.hidden; },
        context: function () { return context.kind; },
        count: items.length
      };
    })();

    /* ============================================================
       Route Probe — shortest directed path over compiled semantics.
       The graph is read from renderer-owned stable IDs and relationship
       endpoints. BFS order follows authored DOM edge order, so equal-length
       choices are deterministic without adding weights or a graph runtime.
       ============================================================ */
    Mosofin.routeProbe = (function () {
      var html = document.documentElement;
      var container = document.querySelector('.diagram-container');
      var svg = container.querySelector(':scope > svg');
      var trigger = document.getElementById('btn-route-probe');
      var panel = document.getElementById('route-probe');
      var title = document.getElementById('route-probe-title');
      var path = document.getElementById('route-probe-path');
      var status = document.getElementById('route-probe-status');
      var findBtn = document.getElementById('route-probe-find');
      var copyBtn = document.getElementById('route-probe-copy');
      var clearBtn = document.getElementById('route-probe-clear');
      var journeyControls = document.getElementById('route-journey-controls');
      var journeyPrevBtn = document.getElementById('route-journey-prev');
      var journeyPlayBtn = document.getElementById('route-journey-play');
      var journeyPlayIcon = document.getElementById('route-journey-play-icon');
      var journeyPlayLabel = document.getElementById('route-journey-play-label');
      var journeyNextBtn = document.getElementById('route-journey-next');
      var journeyOverviewBtn = document.getElementById('route-journey-overview');
      var namespace = 'http://www.w3.org/2000/svg';
      var mode = 'idle';
      var startId = null;
      var endId = null;
      var activeNodeIds = [];
      var activeEdges = [];
      var journeyIndex = -1;
      var journeyPlaying = false;
      var journeyComplete = false;
      var journeyGeneration = 0;
      var journeyTimer = null;
      var journeyStartedAt = 0;
      var journeyElapsedMs = 0;
      var journeyOwnerToken = 0;
      var JOURNEY_DWELL_MS = 1100;

      function nodes() {
        return Array.prototype.slice.call(svg.querySelectorAll('[data-node-id]'));
      }
      function edges() {
        return Array.prototype.slice.call(svg.querySelectorAll('[data-edge-from][data-edge-to]'));
      }
      function nodesById() {
        var out = Object.create(null);
        nodes().forEach(function (node) { out[node.getAttribute('data-node-id')] = node; });
        return out;
      }
      function nodeLabel(node, fallback) {
        return node.getAttribute('data-node-label') ||
          (node.getAttribute('aria-label') || fallback).replace(/^Focus\s+/, '').split(',')[0];
      }
      function exportSnapshot() {
        if (mode !== 'result' || activeNodeIds.length < 2 || activeEdges.length !== activeNodeIds.length - 1) return null;
        var allNodes = nodes();
        var byId = nodesById();
        var seenNodeIds = Object.create(null);
        var seenEdgeKeys = Object.create(null);
        if (startId !== activeNodeIds[0] || endId !== activeNodeIds[activeNodeIds.length - 1] ||
            activeNodeIds.some(function (id) {
              if (seenNodeIds[id] || allNodes.filter(function (node) {
                return node.getAttribute('data-node-id') === id;
              }).length !== 1) return true;
              seenNodeIds[id] = true;
              return !byId[id];
            }) ||
            activeEdges.some(function (edge, index) {
              if (!edge || !svg.contains(edge)) return true;
              var edgeKey = edge.getAttribute('data-edge-key');
              var edgeId = edge.getAttribute('data-edge-id') || '';
              if (!edgeKey || seenEdgeKeys[edgeKey]) return true;
              seenEdgeKeys[edgeKey] = true;
              var fragments = Array.prototype.slice.call(svg.querySelectorAll('[data-edge-key]')).filter(function (candidate) {
                return candidate.getAttribute('data-edge-key') === edgeKey;
              });
              var drawableFragments = fragments.filter(hasDrawableGeometry);
              return !fragments.length || !fragments.every(function (fragment) {
                return fragment.getAttribute('data-edge-from') === activeNodeIds[index] &&
                  fragment.getAttribute('data-edge-to') === activeNodeIds[index + 1] &&
                  (fragment.getAttribute('data-edge-id') || '') === edgeId;
              }) || drawableFragments.length !== 1 || drawableFragments[0] !== edge ||
                edge.getAttribute('data-edge-from') !== activeNodeIds[index] ||
                edge.getAttribute('data-edge-to') !== activeNodeIds[index + 1];
            })) return null;
        return {
          source: { id: startId, label: nodeLabel(byId[startId], startId) },
          target: { id: endId, label: nodeLabel(byId[endId], endId) },
          nodeIds: activeNodeIds.slice(),
          hops: activeEdges.length,
          edges: activeEdges.map(function (edge) {
            return {
              key: edge.getAttribute('data-edge-key'),
              id: edge.getAttribute('data-edge-id') || '',
              from: edge.getAttribute('data-edge-from'),
              to: edge.getAttribute('data-edge-to'),
              label: edge.getAttribute('data-edge-label') || ''
            };
          })
        };
      }
      function edgeShapes(edge) {
        if (/^(path|line|polyline)$/i.test(edge.tagName)) return [edge];
        return Array.prototype.slice.call(edge.querySelectorAll('path, line, polyline'));
      }
      function removeOverlay() {
        Array.prototype.forEach.call(svg.querySelectorAll('[data-route-probe-overlay]'), function (overlay) {
          overlay.remove();
        });
      }
      function removeJourneyPulse(options) {
        options = options || {};
        Array.prototype.forEach.call(svg.querySelectorAll('[data-route-journey-overlay]'), function (overlay) {
          overlay.remove();
        });
        if (journeyOwnerToken && options.release !== false && Mosofin.motionGovernor) {
          var token = journeyOwnerToken;
          journeyOwnerToken = 0;
          Mosofin.motionGovernor.release(token);
        }
      }
      function stopJourneyTimer(options) {
        options = options || {};
        journeyGeneration += 1;
        if (journeyTimer) {
          if (options.preserveElapsed === true && journeyStartedAt) {
            journeyElapsedMs = Math.min(JOURNEY_DWELL_MS, journeyElapsedMs + Math.max(0, Date.now() - journeyStartedAt));
          }
          window.clearTimeout(journeyTimer);
        }
        journeyTimer = null;
        journeyStartedAt = 0;
        if (options.preserveElapsed !== true) journeyElapsedMs = 0;
      }
      function clearJourneyPresentation(options) {
        options = options || {};
        removeJourneyPulse();
        svg.removeAttribute('data-route-journey');
        panel.removeAttribute('data-route-journey');
        nodes().forEach(function (node) {
          node.removeAttribute('data-route-journey-state');
          node.removeAttribute('data-route-journey-current');
        });
        edges().forEach(function (edge) {
          edge.removeAttribute('data-route-journey-state');
          edge.removeAttribute('data-route-journey-current');
        });
        Array.prototype.forEach.call(path.querySelectorAll('[data-route-journey-index]'), function (button, index) {
          button.removeAttribute('data-route-journey-state');
          button.removeAttribute('aria-current');
          button.setAttribute('tabindex', index === 0 ? '0' : '-1');
        });
        if (options.keepControls !== true) journeyControls.hidden = true;
        journeyControls.setAttribute('data-playing', 'false');
      }
      function resetJourneyState() {
        stopJourneyTimer();
        journeyPlaying = false;
        journeyComplete = false;
        journeyIndex = -1;
        clearJourneyPresentation();
      }
      function cleanSvgState() {
        resetJourneyState();
        svg.removeAttribute('data-route-picking');
        svg.removeAttribute('data-route-active');
        removeOverlay();
        nodes().forEach(function (node) {
          node.removeAttribute('data-route-match');
          node.removeAttribute('data-route-start');
          node.removeAttribute('data-route-end');
          node.removeAttribute('data-route-step');
          node.removeAttribute('data-route-candidate');
          node.style.removeProperty('--route-step');
        });
        edges().forEach(function (edge) {
          edge.removeAttribute('data-route-match');
          edge.removeAttribute('data-route-step');
          edge.style.removeProperty('--route-step');
        });
        panel.removeAttribute('data-route-dock');
      }
      function replaceRouteHash(value) {
        try {
          history.replaceState(null, '', location.pathname + location.search + (value ? '#route=' + value : ''));
        } catch (_) {}
      }
      function renderPlaceholder(copy) {
        path.textContent = '';
        var placeholder = document.createElement('span');
        placeholder.className = 'route-probe-placeholder';
        placeholder.textContent = copy;
        path.appendChild(placeholder);
      }
      function renderPath(ids, options) {
        options = options || {};
        var byId = nodesById();
        path.textContent = '';
        ids.forEach(function (id, index) {
          if (index) {
            var arrow = document.createElement('span');
            arrow.className = 'route-probe-arrow';
            arrow.setAttribute('aria-hidden', 'true');
            arrow.textContent = '\u2192';
            path.appendChild(arrow);
          }
          var item = document.createElement(options.interactive === true ? 'button' : 'span');
          if (options.interactive === true) {
            item.type = 'button';
            item.setAttribute('data-route-journey-index', String(index));
            item.setAttribute('data-route-node-id', id);
            item.setAttribute('tabindex', index === 0 ? '0' : '-1');
            item.setAttribute('aria-label', viewerText('viewer.route.position', {
              index: index + 1,
              total: ids.length,
              label: nodeLabel(byId[id], id)
            }));
          }
          item.className = 'route-probe-node';
          item.textContent = nodeLabel(byId[id], id);
          item.title = nodeLabel(byId[id], id) + ' · ' + id;
          if (index === 0) item.setAttribute('data-endpoint', 'start');
          if (index === ids.length - 1 && ids.length > 1) item.setAttribute('data-endpoint', 'end');
          path.appendChild(item);
        });
      }
      function setTrigger(active) {
        trigger.setAttribute('aria-pressed', active ? 'true' : 'false');
        trigger.setAttribute('aria-label', viewerText(active ? 'viewer.route.trigger.clear' : 'viewer.guide.route.aria'));
      }
      function clear(options) {
        options = options || {};
        var wasActive = mode !== 'idle';
        if (Mosofin.finder && Mosofin.finder.isOpen() && typeof Mosofin.finder.context === 'function' && Mosofin.finder.context().indexOf('route-') === 0) {
          Mosofin.finder.close({ restoreFocus: false });
        }
        mode = 'idle';
        startId = null;
        endId = null;
        activeNodeIds = [];
        activeEdges = [];
        cleanSvgState();
        panel.hidden = true;
        panel.setAttribute('data-state', 'idle');
        panel.removeAttribute('data-finder-open');
        title.textContent = viewerText('viewer.route.start');
        renderPlaceholder(viewerText('viewer.route.pickTwo'));
        status.textContent = viewerText('viewer.route.instructions');
        findBtn.hidden = true;
        findBtn.textContent = viewerText('viewer.route.start.find');
        findBtn.setAttribute('aria-label', viewerText('viewer.route.start.find.aria'));
        copyBtn.hidden = true;
        copyBtn.textContent = viewerText('viewer.route.copy');
        if (Mosofin.exportMenu && typeof Mosofin.exportMenu.syncRouteShare === 'function') Mosofin.exportMenu.syncRouteShare();
        setTrigger(false);
        if (wasActive && options.preserveView !== true && Mosofin.view && typeof Mosofin.view.reset === 'function') {
          Mosofin.view.reset({ automatic: true });
        }
        if (options.updateUrl !== false) replaceRouteHash('');
        if (options.restoreFocus === true) trigger.focus();
      }
      function outgoingByNode() {
        var byId = nodesById();
        var outgoing = {};
        edges().forEach(function (edge) {
          var from = edge.getAttribute('data-edge-from');
          var to = edge.getAttribute('data-edge-to');
          if (!byId[from] || !byId[to] || from === to) return;
          if (!outgoing[from]) outgoing[from] = [];
          outgoing[from].push({ edge: edge, to: to });
        });
        return outgoing;
      }
      function reachableFrom(source) {
        var outgoing = outgoingByNode();
        var reached = {};
        var queue = [source];
        reached[source] = true;
        for (var cursor = 0; cursor < queue.length; cursor += 1) {
          var links = outgoing[queue[cursor]] || [];
          links.forEach(function (link) {
            if (reached[link.to]) return;
            reached[link.to] = true;
            queue.push(link.to);
          });
        }
        return reached;
      }
      function hopDistancesFrom(source) {
        var outgoing = outgoingByNode();
        var distances = {};
        var queue = [source];
        distances[source] = 0;
        for (var cursor = 0; cursor < queue.length; cursor += 1) {
          var links = outgoing[queue[cursor]] || [];
          links.forEach(function (link) {
            if (Object.prototype.hasOwnProperty.call(distances, link.to)) return;
            distances[link.to] = distances[queue[cursor]] + 1;
            queue.push(link.to);
          });
        }
        return distances;
      }
      function shortestDirectedPath(source, target) {
        if (source === target) return null;
        var outgoing = outgoingByNode();
        var previous = {};
        var queue = [source];
        previous[source] = null;
        for (var cursor = 0; cursor < queue.length && !Object.prototype.hasOwnProperty.call(previous, target); cursor += 1) {
          var links = outgoing[queue[cursor]] || [];
          links.some(function (link) {
            if (Object.prototype.hasOwnProperty.call(previous, link.to)) return false;
            previous[link.to] = { from: queue[cursor], edge: link.edge };
            queue.push(link.to);
            return link.to === target;
          });
        }
        if (!Object.prototype.hasOwnProperty.call(previous, target)) return null;
        var nodeIds = [target];
        var routeEdges = [];
        var current = target;
        while (current !== source) {
          var step = previous[current];
          if (!step) return null;
          routeEdges.unshift(step.edge);
          nodeIds.unshift(step.from);
          current = step.from;
        }
        return { nodes: nodeIds, edges: routeEdges };
      }
      function traceGeometry(shape, step) {
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
        clone.removeAttribute('data-route-match');
        clone.removeAttribute('data-route-step');
        clone.setAttribute('class', 'route-probe-flow');
        clone.setAttribute('pathLength', '1');
        clone.style.setProperty('--route-step', String(step));
        return clone;
      }
      function renderOverlay(routeEdges) {
        removeOverlay();
        if (!routeEdges.length) return;
        var overlay = document.createElementNS(namespace, 'g');
        overlay.setAttribute('class', 'route-probe-overlay');
        overlay.setAttribute('data-route-probe-overlay', '');
        overlay.setAttribute('aria-hidden', 'true');
        routeEdges.forEach(function (edge, step) {
          var wrapper = document.createElementNS(namespace, 'g');
          if (edge.hasAttribute('transform')) wrapper.setAttribute('transform', edge.getAttribute('transform'));
          edgeShapes(edge).forEach(function (shape) { wrapper.appendChild(traceGeometry(shape, step)); });
          if (wrapper.childNodes.length) overlay.appendChild(wrapper);
        });
        var firstNode = svg.querySelector('[data-node-id]');
        if (firstNode) svg.insertBefore(overlay, firstNode);
        else svg.appendChild(overlay);
      }
      function journeyButtons() {
        return Array.prototype.slice.call(path.querySelectorAll('[data-route-journey-index]'));
      }
      function journeyMotionAllowed() {
        return html.getAttribute('data-embed') !== 'true' &&
          !document.hidden &&
          Mosofin.motionGovernor &&
          Mosofin.motionGovernor.capable === true &&
          !Mosofin.motionGovernor.isPaused();
      }
      function routeOverviewStatus() {
        return viewerText('viewer.route.overview.status', {
          nodes: viewerCount('viewer.route.overview.node', activeNodeIds.length),
          hops: viewerCount('viewer.route.overview.hop', activeEdges.length)
        });
      }
      function centerJourneyButton(button) {
        if (!button) return;
        var target = button.offsetLeft - Math.max(0, (path.clientWidth - button.offsetWidth) / 2);
        path.scrollLeft = Math.max(0, target);
      }
      function focusJourneyButton(index) {
        var buttons = journeyButtons();
        if (!buttons.length) return false;
        var next = Math.max(0, Math.min(buttons.length - 1, index));
        buttons.forEach(function (button, buttonIndex) {
          button.setAttribute('tabindex', buttonIndex === next ? '0' : '-1');
        });
        try { buttons[next].focus({ preventScroll: true }); } catch (_) { buttons[next].focus(); }
        centerJourneyButton(buttons[next]);
        return true;
      }
      function renderJourneyControls() {
        var hasRoute = mode === 'result' && activeNodeIds.length > 1;
        var canPlay = hasRoute && journeyMotionAllowed();
        journeyControls.hidden = !hasRoute;
        journeyControls.setAttribute('data-playing', journeyPlaying ? 'true' : 'false');
        journeyPrevBtn.disabled = !hasRoute || journeyIndex <= 0;
        journeyNextBtn.disabled = !hasRoute || journeyIndex >= activeNodeIds.length - 1;
        journeyOverviewBtn.disabled = !hasRoute || journeyIndex < 0;
        journeyPlayBtn.disabled = !canPlay;
        journeyPlayBtn.setAttribute('aria-pressed', journeyPlaying ? 'true' : 'false');
        if (journeyPlaying) {
          journeyPlayIcon.textContent = '\u275a\u275a';
          journeyPlayLabel.textContent = viewerText('viewer.route.pause.label');
          journeyPlayBtn.setAttribute('aria-label', viewerText('viewer.route.pause'));
          journeyPlayBtn.title = viewerText('viewer.route.pause');
        } else if (journeyComplete || journeyIndex === activeNodeIds.length - 1) {
          journeyPlayIcon.textContent = '\u21bb';
          journeyPlayLabel.textContent = viewerText('viewer.route.replay.label');
          journeyPlayBtn.setAttribute('aria-label', viewerText('viewer.route.replay'));
          journeyPlayBtn.title = viewerText(canPlay ? 'viewer.route.replay' : 'viewer.route.motionRequired');
        } else {
          journeyPlayIcon.textContent = '\u25b6';
          journeyPlayLabel.textContent = viewerText('viewer.route.journey');
          journeyPlayBtn.setAttribute('aria-label', viewerText('viewer.route.play'));
          journeyPlayBtn.title = viewerText(canPlay ? 'viewer.route.play' : 'viewer.route.motionRequired');
        }
      }
      function journeyGeometry(shape) {
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
        clone.removeAttribute('data-route-match');
        clone.removeAttribute('data-route-step');
        clone.removeAttribute('data-route-journey-state');
        clone.removeAttribute('data-route-journey-current');
        clone.setAttribute('class', 'route-journey-flow');
        clone.setAttribute('pathLength', '1');
        return clone;
      }
      function renderJourneyPulse(edge) {
        removeJourneyPulse();
        if (!edge || !journeyMotionAllowed()) return false;
        var overlay = document.createElementNS(namespace, 'g');
        overlay.setAttribute('class', 'route-journey-overlay');
        overlay.setAttribute('data-route-journey-overlay', '');
        overlay.setAttribute('aria-hidden', 'true');
        var wrapper = document.createElementNS(namespace, 'g');
        if (edge.hasAttribute('transform')) wrapper.setAttribute('transform', edge.getAttribute('transform'));
        edgeShapes(edge).forEach(function (shape) { wrapper.appendChild(journeyGeometry(shape)); });
        if (!wrapper.childNodes.length) return false;
        overlay.appendChild(wrapper);
        var firstNode = svg.querySelector('[data-node-id]');
        if (firstNode) svg.insertBefore(overlay, firstNode);
        else svg.appendChild(overlay);
        if (Mosofin.motionGovernor && Mosofin.motionGovernor.capable) {
          var token = 0;
          token = Mosofin.motionGovernor.claim('route', function () {
            if (journeyOwnerToken === token) journeyOwnerToken = 0;
            removeJourneyPulse({ release: false });
          });
          journeyOwnerToken = token;
        }
        overlay.addEventListener('animationend', function () {
          if (overlay.isConnected) removeJourneyPulse();
        }, { once: true });
        window.setTimeout(function () {
          if (overlay.isConnected) removeJourneyPulse();
        }, 860);
        return true;
      }
      function applyJourneyState(index, options) {
        options = options || {};
        if (mode !== 'result' || !activeNodeIds.length) return false;
        var byId = nodesById();
        journeyIndex = Math.max(0, Math.min(activeNodeIds.length - 1, index));
        removeJourneyPulse();
        svg.setAttribute('data-route-journey', (journeyIndex + 1) + '/' + activeNodeIds.length);
        panel.setAttribute('data-route-journey', String(journeyIndex));
        activeNodeIds.forEach(function (id, step) {
          var node = byId[id];
          if (!node) return;
          var state = step < journeyIndex ? 'past' : (step === journeyIndex ? 'current' : 'future');
          node.setAttribute('data-route-journey-state', state);
          if (step === journeyIndex) node.setAttribute('data-route-journey-current', '');
          else node.removeAttribute('data-route-journey-current');
        });
        activeEdges.forEach(function (edge, step) {
          var destination = step + 1;
          var state = destination < journeyIndex ? 'past' : (destination === journeyIndex ? 'current' : 'future');
          edge.setAttribute('data-route-journey-state', state);
          if (destination === journeyIndex) edge.setAttribute('data-route-journey-current', '');
          else edge.removeAttribute('data-route-journey-current');
        });
        var buttons = journeyButtons();
        buttons.forEach(function (button, step) {
          var state = step < journeyIndex ? 'past' : (step === journeyIndex ? 'current' : 'future');
          button.setAttribute('data-route-journey-state', state);
          button.setAttribute('tabindex', step === journeyIndex ? '0' : '-1');
          if (step === journeyIndex) button.setAttribute('aria-current', 'step');
          else button.removeAttribute('aria-current');
        });
        if (options.center !== false) centerJourneyButton(buttons[journeyIndex]);
        var phase = viewerText(journeyPlaying
          ? 'viewer.route.phase.playing'
          : journeyComplete
            ? 'viewer.route.phase.complete'
            : 'viewer.route.phase.inspecting');
        status.textContent = viewerText('viewer.route.step', {
          index: journeyIndex + 1,
          total: activeNodeIds.length,
          phase: phase,
          label: nodeLabel(byId[activeNodeIds[journeyIndex]], activeNodeIds[journeyIndex])
        });
        if (options.pulse === true && journeyIndex > 0) renderJourneyPulse(activeEdges[journeyIndex - 1]);
        if (options.reveal !== false && Mosofin.view && typeof Mosofin.view.reveal === 'function') {
          Mosofin.view.reveal(activeNodeIds.slice(Math.max(0, journeyIndex - 1), Math.min(activeNodeIds.length, journeyIndex + 2)), {
            includeNeighbors: false,
            reason: 'route-journey',
            maxScale: 1.65,
            padding: 64,
            duration: 360,
            instant: !journeyMotionAllowed()
          });
        }
        renderJourneyControls();
        requestDocking();
        return true;
      }
      function pauseJourney(options) {
        options = options || {};
        if (!journeyPlaying && options.complete !== true) {
          renderJourneyControls();
          return false;
        }
        stopJourneyTimer({ preserveElapsed: options.complete !== true && options.preserveElapsed !== false });
        journeyPlaying = false;
        journeyComplete = options.complete === true;
        if (journeyComplete) journeyElapsedMs = 0;
        removeJourneyPulse();
        if (journeyIndex >= 0) applyJourneyState(journeyIndex, { center: false, pulse: false, reveal: false });
        else renderJourneyControls();
        return true;
      }
      function selectJourneyIndex(index) {
        if (mode !== 'result') return false;
        stopJourneyTimer();
        journeyPlaying = false;
        journeyComplete = false;
        return applyJourneyState(index, { pulse: true, reveal: true });
      }
      function showJourneyOverview(options) {
        options = options || {};
        if (mode !== 'result') return false;
        stopJourneyTimer();
        journeyPlaying = false;
        journeyComplete = false;
        journeyIndex = -1;
        clearJourneyPresentation({ keepControls: true });
        status.textContent = routeOverviewStatus();
        renderJourneyControls();
        if (options.reveal !== false && Mosofin.view && typeof Mosofin.view.reveal === 'function') {
          Mosofin.view.reveal(activeNodeIds, { includeNeighbors: false, reason: 'route', instant: !journeyMotionAllowed() });
        }
        requestDocking();
        return true;
      }
      function scheduleJourney() {
        if (!journeyPlaying || !journeyMotionAllowed()) {
          pauseJourney({ preserveElapsed: true });
          return false;
        }
        var generation = ++journeyGeneration;
        var remaining = Math.max(0, JOURNEY_DWELL_MS - journeyElapsedMs);
        journeyStartedAt = Date.now();
        journeyTimer = window.setTimeout(function () {
          if (generation !== journeyGeneration || !journeyPlaying) return;
          journeyTimer = null;
          journeyStartedAt = 0;
          journeyElapsedMs = 0;
          if (journeyIndex >= activeNodeIds.length - 1) {
            pauseJourney({ complete: true, preserveElapsed: false });
            return;
          }
          applyJourneyState(journeyIndex + 1, { pulse: true, reveal: true });
          // Camera and motion-owner handoff are allowed to synchronously
          // settle the previous step. The new step always starts with a fresh
          // dwell; never carry cleanup time into the next scheduler lease.
          journeyElapsedMs = 0;
          journeyStartedAt = 0;
          scheduleJourney();
        }, remaining);
        return true;
      }
      function playJourney() {
        if (mode !== 'result' || activeNodeIds.length < 2 || !journeyMotionAllowed()) {
          renderJourneyControls();
          return false;
        }
        if (journeyPlaying) return true;
        if (journeyComplete || journeyIndex >= activeNodeIds.length - 1) {
          journeyIndex = -1;
          journeyElapsedMs = 0;
          journeyComplete = false;
        }
        journeyPlaying = true;
        if (journeyIndex < 0) applyJourneyState(0, { pulse: false, reveal: true });
        else applyJourneyState(journeyIndex, { center: false, pulse: false, reveal: false });
        scheduleJourney();
        return true;
      }
      function toggleJourney() {
        return journeyPlaying ? pauseJourney({ preserveElapsed: true }) : playJourney();
      }
      function previousJourney() {
        return selectJourneyIndex(Math.max(0, journeyIndex < 0 ? 0 : journeyIndex - 1));
      }
      function nextJourney() {
        return selectJourneyIndex(Math.min(activeNodeIds.length - 1, journeyIndex + 1));
      }
      function overlapArea(a, b) {
        var width = Math.max(0, Math.min(a.right, b.right) - Math.max(a.left, b.left));
        var height = Math.max(0, Math.min(a.bottom, b.bottom) - Math.max(a.top, b.top));
        return width * height;
      }
      function updateDocking() {
        if (panel.hidden) {
          panel.removeAttribute('data-route-dock');
          return;
        }
        var containerRect = container.getBoundingClientRect();
        var width = panel.offsetWidth;
        var height = panel.offsetHeight;
        if (!width || !height) return;
        var currentRect = panel.getBoundingClientRect();
        var left = currentRect.left;
        var topCandidate = {
          left: left,
          right: left + width,
          top: containerRect.top + 8,
          bottom: containerRect.top + 8 + height
        };
        var nav = container.querySelector('.diagram-nav');
        var navRect = nav ? nav.getBoundingClientRect() : null;
        var bottom = navRect ? navRect.top - 9 : containerRect.bottom - 8;
        var bottomCandidate = {
          left: left,
          right: left + width,
          top: bottom - height,
          bottom: bottom
        };
        var byId = nodesById();
        var relevant = activeNodeIds.length ? activeNodeIds : (startId ? [startId] : []);
        var blockers = relevant.map(function (id) { return byId[id]; }).filter(Boolean).map(function (node) {
          return node.getBoundingClientRect();
        });
        function score(candidate) {
          var value = blockers.reduce(function (sum, rect) { return sum + overlapArea(candidate, rect); }, 0);
          if (navRect) value += overlapArea(candidate, navRect) * 4;
          if (candidate.top < containerRect.top || candidate.bottom > containerRect.bottom) value += 1000000;
          return value;
        }
        if (score(topCandidate) <= score(bottomCandidate)) panel.setAttribute('data-route-dock', 'top');
        else panel.setAttribute('data-route-dock', 'bottom');
      }
      function requestDocking() {
        requestAnimationFrame(updateDocking);
        window.setTimeout(updateDocking, 120);
        window.setTimeout(updateDocking, 560);
      }
      function chooseStart(id) {
        var byId = nodesById();
        if (!byId[id]) return false;
        cleanSvgState();
        startId = id;
        endId = null;
        mode = 'target';
        var reached = reachableFrom(id);
        byId[id].setAttribute('data-route-start', '');
        byId[id].setAttribute('data-route-match', '');
        Object.keys(reached).forEach(function (candidateId) {
          if (candidateId !== id && byId[candidateId]) byId[candidateId].setAttribute('data-route-candidate', '');
        });
        svg.setAttribute('data-route-picking', 'target');
        panel.setAttribute('data-state', 'target');
        title.textContent = viewerText('viewer.route.destination', { label: nodeLabel(byId[id], id) });
        renderPath([id]);
        var count = Math.max(0, Object.keys(reached).length - 1);
        status.textContent = count
          ? viewerCount('viewer.route.destination.count', count)
          : viewerText('viewer.route.noOutgoing');
        findBtn.hidden = false;
        findBtn.textContent = viewerText('viewer.route.destination.find');
        findBtn.setAttribute('aria-label', viewerText('viewer.route.destination.find.aria'));
        requestDocking();
        return true;
      }
      function showResult(result, options) {
        options = options || {};
        var byId = nodesById();
        cleanSvgState();
        mode = 'result';
        startId = result.nodes[0];
        endId = result.nodes[result.nodes.length - 1];
        activeNodeIds = result.nodes.slice();
        activeEdges = result.edges.slice();
        result.nodes.forEach(function (id, step) {
          var node = byId[id];
          if (!node) return;
          node.setAttribute('data-route-match', '');
          node.setAttribute('data-route-step', String(step));
          node.style.setProperty('--route-step', String(step));
          if (step === 0) node.setAttribute('data-route-start', '');
          if (step === result.nodes.length - 1) node.setAttribute('data-route-end', '');
        });
        result.edges.forEach(function (edge, step) {
          edge.setAttribute('data-route-match', '');
          edge.setAttribute('data-route-step', String(step));
          edge.style.setProperty('--route-step', String(step));
        });
        svg.setAttribute('data-route-active', startId + '~' + endId);
        renderOverlay(result.edges);
        renderPath(result.nodes, { interactive: true });
        panel.setAttribute('data-state', 'result');
        panel.hidden = false;
        title.textContent = viewerText('viewer.route.result.title', {
          source: nodeLabel(byId[startId], startId),
          target: nodeLabel(byId[endId], endId)
        });
        findBtn.hidden = true;
        copyBtn.hidden = false;
        setTrigger(true);
        if (Mosofin.exportMenu && typeof Mosofin.exportMenu.syncRouteShare === 'function') Mosofin.exportMenu.syncRouteShare();
        if (options.updateUrl !== false) {
          replaceRouteHash(encodeURIComponent(startId) + '~' + encodeURIComponent(endId));
        }
        showJourneyOverview({ reveal: false });
        if (Mosofin.view && typeof Mosofin.view.reveal === 'function') {
          Mosofin.view.reveal(result.nodes, { includeNeighbors: false, reason: 'route' });
        }
        return true;
      }
      function choose(id, options) {
        options = options || {};
        var byId = nodesById();
        if (!byId[id]) return false;
        if (mode === 'source') return chooseStart(id);
        if (mode !== 'target') return false;
        if (id === startId) {
          panel.setAttribute('data-state', 'error');
          title.textContent = viewerText('viewer.route.differentDestination');
          status.textContent = viewerText('viewer.route.distinct');
          return false;
        }
        var result = shortestDirectedPath(startId, id);
        if (!result) {
          panel.setAttribute('data-state', 'error');
          title.textContent = viewerText('viewer.route.unreachable', { label: nodeLabel(byId[id], id) });
          status.textContent = viewerText('viewer.route.unreachable.detail', {
            target: nodeLabel(byId[id], id),
            source: nodeLabel(byId[startId], startId)
          });
          return false;
        }
        return showResult(result, options);
      }
      function begin(options) {
        options = options || {};
        if (html.getAttribute('data-embed') === 'true') return false;
        if (Mosofin.semanticLens && typeof Mosofin.semanticLens.clearPreview === 'function') Mosofin.semanticLens.clearPreview();
        if (Mosofin.semanticLens && Mosofin.semanticLens.active()) {
          Mosofin.semanticLens.clear({ updateUrl: false, preserveView: true, closePanel: true });
        }
        var focused = options.source || (Mosofin.focus && typeof Mosofin.focus.active === 'function' ? Mosofin.focus.active() : null);
        if (Array.isArray(focused)) focused = null;
        clear({ updateUrl: false, preserveView: true, restoreFocus: false });
        if (Mosofin.intentTrace && typeof Mosofin.intentTrace.clear === 'function') Mosofin.intentTrace.clear({ announce: false });
        if (Mosofin.guidedViews && typeof Mosofin.guidedViews.showAll === 'function') {
          Mosofin.guidedViews.showAll({ clearFocus: false, updateUrl: false });
        }
        if (Mosofin.focus && typeof Mosofin.focus.clear === 'function') {
          Mosofin.focus.clear({ updateUrl: false, preserveView: true });
        }
        if (Mosofin.finder && Mosofin.finder.isOpen()) Mosofin.finder.close({ restoreFocus: false });
        if (Mosofin.radar && Mosofin.radar.isOpen()) Mosofin.radar.close({ restoreFocus: false });
        mode = 'source';
        panel.hidden = false;
        panel.setAttribute('data-state', 'source');
        svg.setAttribute('data-route-picking', 'source');
        title.textContent = viewerText('viewer.route.start');
        renderPlaceholder(viewerText('viewer.route.pickOne'));
        status.textContent = viewerText('viewer.route.start.instructions');
        findBtn.hidden = false;
        findBtn.textContent = viewerText('viewer.route.start.find');
        findBtn.setAttribute('aria-label', viewerText('viewer.route.start.find.aria'));
        copyBtn.hidden = true;
        setTrigger(true);
        if (focused && nodesById()[focused]) chooseStart(focused);
        if (options.focusNode === true && !focused) {
          var first = nodes()[0];
          if (first) {
            try { first.focus({ preventScroll: true }); } catch (_) { try { first.focus(); } catch (_) {} }
          }
        }
        return true;
      }
      function toggle(options) {
        if (mode !== 'idle') {
          clear({ restoreFocus: true });
          return false;
        }
        return begin(options);
      }
      function finderContext() {
        var byId = nodesById();
        if (mode === 'source') {
          var outgoing = outgoingByNode();
          var sourceIds = nodes().map(function (node) { return node.getAttribute('data-node-id'); }).filter(function (id) {
            return outgoing[id] && outgoing[id].length;
          });
          var sourceBadges = {};
          sourceIds.forEach(function (id) { sourceBadges[id] = viewerText('viewer.route.finder.source.badge'); });
          return {
            kind: 'route-source',
            title: viewerText('viewer.route.finder.source.title'),
            placeholder: viewerText('viewer.route.finder.source.placeholder'),
            empty: viewerText('viewer.route.finder.source.empty'),
            resultsLabel: viewerText('viewer.route.finder.source.results'),
            availableNoun: viewerText('viewer.route.finder.source.noun'),
            allowedIds: sourceIds,
            badges: sourceBadges
          };
        }
        if (mode === 'target' && startId && byId[startId]) {
          var distances = hopDistancesFrom(startId);
          var targetIds = Object.keys(distances).filter(function (id) { return id !== startId && byId[id]; });
          var targetBadges = {};
          targetIds.forEach(function (id) {
            targetBadges[id] = viewerCount('viewer.route.hop', distances[id]);
          });
          return {
            kind: 'route-target',
            title: viewerText('viewer.route.finder.target.title', { label: nodeLabel(byId[startId], startId) }),
            placeholder: viewerText('viewer.route.finder.target.placeholder'),
            empty: viewerText('viewer.route.finder.target.empty'),
            resultsLabel: viewerText('viewer.route.finder.target.results'),
            availableNoun: viewerText('viewer.route.finder.target.noun'),
            allowedIds: targetIds,
            badges: targetBadges
          };
        }
        return null;
      }
      function finderOpening() {
        panel.setAttribute('data-finder-open', 'true');
      }
      function finderClosed(options) {
        options = options || {};
        panel.removeAttribute('data-finder-open');
        requestDocking();
        if (options.restoreFocus === true && !findBtn.hidden) findBtn.focus();
      }
      function openFinder() {
        var context = finderContext();
        if (!context || !Mosofin.finder) return false;
        return Mosofin.finder.open({ context: context });
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
      function copyLink() {
        if (mode !== 'result') return Promise.resolve(false);
        var value = location.href.replace(/#.*$/, '') + '#route=' + encodeURIComponent(startId) + '~' + encodeURIComponent(endId);
        var copy = navigator.clipboard && typeof navigator.clipboard.writeText === 'function'
          ? navigator.clipboard.writeText(value).then(function () { return true; }).catch(function () { return fallbackCopy(value); })
          : Promise.resolve(fallbackCopy(value));
        return copy.then(function (copied) {
          copyBtn.textContent = viewerText(copied ? 'viewer.common.copied' : 'viewer.common.copyFailed');
          copyBtn.setAttribute('aria-label', viewerText(copied ? 'viewer.route.copy.success' : 'viewer.route.copy.failed'));
          window.setTimeout(function () {
            copyBtn.textContent = viewerText('viewer.route.copy');
            copyBtn.setAttribute('aria-label', viewerText('viewer.route.copy.aria'));
          }, 1600);
          return copied;
        });
      }
      function interceptSelection(event) {
        if (mode !== 'source' && mode !== 'target') return;
        if (container.getAttribute('data-just-panned') === 'true') return;
        var node = event.target.closest('[data-node-id]');
        if (!node) return;
        if (event.type === 'keydown' && event.key !== 'Enter' && event.key !== ' ') return;
        event.preventDefault();
        event.stopImmediatePropagation();
        choose(node.getAttribute('data-node-id'));
      }
      function syncFromHash() {
        try {
          var params = new URLSearchParams(location.hash.replace(/^#/, ''));
          var route = params.get('route');
          if (!route) {
            if (mode !== 'idle') clear({ updateUrl: false, restoreFocus: false });
            return;
          }
          var parts = route.split('~');
          if (parts.length !== 2) return;
          var byId = nodesById();
          if (!byId[parts[0]] || !byId[parts[1]]) return;
          begin({ source: parts[0], focusNode: false });
          choose(parts[1], { updateUrl: false });
        } catch (_) {}
      }
      function escapeRoute(options) {
        options = options || {};
        if (journeyPlaying) {
          pauseJourney({ preserveElapsed: true });
          if (options.restoreFocus === true) journeyPlayBtn.focus();
          return 'paused';
        }
        if (journeyIndex >= 0) {
          showJourneyOverview({ reveal: true });
          if (options.restoreFocus === true) journeyOverviewBtn.focus();
          return 'overview';
        }
        clear({ restoreFocus: options.restoreFocus === true });
        return 'cleared';
      }
      function syncJourneyMotion() {
        if (journeyPlaying && !journeyMotionAllowed()) pauseJourney({ preserveElapsed: true });
        renderJourneyControls();
        return journeyMotionAllowed();
      }

      trigger.addEventListener('click', function () { toggle({ focusNode: false }); });
      findBtn.addEventListener('click', openFinder);
      clearBtn.addEventListener('click', function () { clear({ restoreFocus: true }); });
      copyBtn.addEventListener('click', copyLink);
      journeyPrevBtn.addEventListener('click', previousJourney);
      journeyPlayBtn.addEventListener('click', toggleJourney);
      journeyNextBtn.addEventListener('click', nextJourney);
      journeyOverviewBtn.addEventListener('click', function () { showJourneyOverview({ reveal: true }); });
      path.addEventListener('click', function (event) {
        var button = event.target.closest('[data-route-journey-index]');
        if (!button) return;
        selectJourneyIndex(Number(button.getAttribute('data-route-journey-index')));
      });
      path.addEventListener('focusin', function (event) {
        if (journeyPlaying && event.target.closest('[data-route-journey-index]')) pauseJourney({ preserveElapsed: true });
      });
      path.addEventListener('keydown', function (event) {
        var button = event.target.closest('[data-route-journey-index]');
        if (!button) return;
        var index = Number(button.getAttribute('data-route-journey-index'));
        var next = null;
        if (event.key === 'ArrowRight') next = Math.min(activeNodeIds.length - 1, index + 1);
        else if (event.key === 'ArrowLeft') next = Math.max(0, index - 1);
        else if (event.key === 'Home') next = 0;
        else if (event.key === 'End') next = activeNodeIds.length - 1;
        if (next !== null) {
          event.preventDefault();
          focusJourneyButton(next);
          return;
        }
        if (event.key === 'Enter' || event.key === ' ') {
          event.preventDefault();
          selectJourneyIndex(index);
        }
      });
      svg.addEventListener('click', interceptSelection, true);
      svg.addEventListener('keydown', interceptSelection, true);
      container.addEventListener('scroll', updateDocking, { passive: true });
      window.addEventListener('resize', requestDocking);
      window.addEventListener('beforeprint', function () {
        if (journeyPlaying) pauseJourney({ preserveElapsed: true, reason: 'print' });
        else removeJourneyPulse();
      });
      window.addEventListener('hashchange', function () { requestAnimationFrame(syncFromHash); });
      syncFromHash();

      return {
        begin: begin,
        choose: choose,
        clear: clear,
        toggle: toggle,
        escape: escapeRoute,
        copyLink: copyLink,
        playJourney: playJourney,
        pauseJourney: pauseJourney,
        showOverview: showJourneyOverview,
        selectJourneyIndex: selectJourneyIndex,
        syncMotion: syncJourneyMotion,
        isJourneyPlaying: function () { return journeyPlaying; },
        finderContext: finderContext,
        finderOpening: finderOpening,
        finderClosed: finderClosed,
        openFinder: openFinder,
        exportSnapshot: exportSnapshot,
        active: function () { return mode === 'idle' ? null : mode; },
        result: function () {
          return mode === 'result'
            ? { source: startId, target: endId, nodes: activeNodeIds.slice(), hops: activeEdges.length, journey: journeyIndex, playing: journeyPlaying }
            : null;
        }
      };
    })();

    /* ============================================================
       Semantic Lens — a counted, viewer-only legend over data-node-kind.
       One selected kind reveals every authored relationship touching it.
       Two selected kinds compare only direct cross-kind relationships while
       preserving the full diagram as a dimmed spatial reference.
       ============================================================ */
    Mosofin.semanticLens = (function () {
      var html = document.documentElement;
      var container = document.querySelector('.diagram-container');
      var svg = container.querySelector(':scope > svg');
      var trigger = document.getElementById('btn-semantic-lens');
      var panel = document.getElementById('semantic-lens');
      var closeBtn = document.getElementById('semantic-lens-close');
      var kindsRoot = document.getElementById('semantic-lens-kinds');
      var status = document.getElementById('semantic-lens-status');
      var copyBtn = document.getElementById('semantic-lens-copy');
      var clearBtn = document.getElementById('semantic-lens-clear');
      var legendBridge = svg.querySelector('[data-legend-bridge]');
      var legendEntries = [];
      var hoveredLegendEntry = null;
      var focusedLegendEntry = null;
      var activeLegendPreview = null;
      var lensOpener = trigger;
      var selectedKinds = [];
      var namespace = 'http://www.w3.org/2000/svg';
      var finePointerQuery = window.matchMedia ? window.matchMedia('(hover: hover) and (pointer: fine)') : null;
      var MAX_LENS_FLOW_EDGES = 24;

      function nodesById() {
        var byId = {};
        Array.prototype.forEach.call(svg.querySelectorAll('[data-node-id][data-node-kind]'), function (node) {
          var id = node.getAttribute('data-node-id');
          if (id && !byId[id]) byId[id] = node;
        });
        return byId;
      }
      function collectKinds() {
        var kinds = {};
        var byId = nodesById();
        Object.keys(byId).forEach(function (id) {
          var node = byId[id];
          var value = node.getAttribute('data-node-kind') || 'neutral';
          var kind = kinds[value] || (kinds[value] = { id: value, label: viewerKindLabel(value), nodes: [] });
          kind.nodes.push(node);
        });
        return Object.keys(kinds).map(function (key) { return kinds[key]; }).sort(function (a, b) {
          return b.nodes.length - a.nodes.length || a.label.localeCompare(b.label);
        });
      }
      function edgeGroups() {
        var groups = {};
        Array.prototype.forEach.call(svg.querySelectorAll('[data-edge-from][data-edge-to]'), function (edge) {
          var from = edge.getAttribute('data-edge-from');
          var to = edge.getAttribute('data-edge-to');
          var key = edge.getAttribute('data-edge-key') || [from, to, edge.getAttribute('data-edge-label') || ''].join('\u0000');
          if (!groups[key]) groups[key] = { key: key, from: from, to: to, members: [] };
          groups[key].members.push(edge);
        });
        return Object.keys(groups).map(function (key) { return groups[key]; });
      }
      function edgeShapes(edge) {
        if (!edge) return [];
        if (/^(path|line|polyline)$/i.test(edge.tagName || '')) return [edge];
        return Array.prototype.slice.call(edge.querySelectorAll('path, line, polyline'));
      }
      function legendSourceBounds(entry) {
        var bounds = null;
        Array.prototype.forEach.call(entry.children, function (child) {
          if (child.hasAttribute('data-legend-bridge-runtime')) return;
          var box;
          try { box = child.getBBox(); } catch (_) { return; }
          if (!box || !Number.isFinite(box.x) || !Number.isFinite(box.y)) return;
          var next = { left: box.x, top: box.y, right: box.x + box.width, bottom: box.y + box.height };
          if (!bounds) bounds = next;
          else {
            bounds.left = Math.min(bounds.left, next.left);
            bounds.top = Math.min(bounds.top, next.top);
            bounds.right = Math.max(bounds.right, next.right);
            bounds.bottom = Math.max(bounds.bottom, next.bottom);
          }
        });
        return bounds;
      }
      function layoutLegendEntry(entry) {
        var bounds = legendSourceBounds(entry);
        var hit = entry.querySelector('[data-legend-hit]');
        var badge = entry.querySelector('[data-legend-count-badge]');
        if (!bounds || !hit || !badge) return false;
        var count = entry.getAttribute('data-legend-count') || '0';
        var centerY = (bounds.top + bounds.bottom) / 2;
        var badgeWidth = Math.max(14, count.length * 5 + 8);
        var badgeX = bounds.right + 3;
        var hitX = bounds.left - 5;
        var hitRight = badgeX + badgeWidth + 4;
        hit.setAttribute('x', hitX.toFixed(2));
        hit.setAttribute('y', (centerY - 12).toFixed(2));
        hit.setAttribute('width', Math.max(24, hitRight - hitX).toFixed(2));
        hit.setAttribute('height', '24');
        hit.setAttribute('rx', '4');
        var badgeRect = badge.querySelector('rect');
        var badgeText = badge.querySelector('text');
        badgeRect.setAttribute('x', badgeX.toFixed(2));
        badgeRect.setAttribute('y', (centerY - 7).toFixed(2));
        badgeRect.setAttribute('width', badgeWidth.toFixed(2));
        badgeRect.setAttribute('height', '14');
        badgeRect.setAttribute('rx', '7');
        badgeText.setAttribute('x', (badgeX + badgeWidth / 2).toFixed(2));
        badgeText.setAttribute('y', (centerY + 2.5).toFixed(2));
        return true;
      }
      function layoutLegendBridge() {
        legendEntries.forEach(layoutLegendEntry);
        if (!legendBridge) return;
        Array.prototype.forEach.call(legendBridge.querySelectorAll('[data-legend-zero]'), layoutLegendEntry);
      }
      function decorateLegendBridge() {
        legendEntries = [];
        if (!legendBridge || html.getAttribute('data-embed') === 'true') return false;
        var facts = {};
        collectKinds().forEach(function (kind) { facts[kind.id] = kind; });
        Array.prototype.forEach.call(legendBridge.querySelectorAll('[data-legend-kind]'), function (entry) {
          var kind = entry.getAttribute('data-legend-kind');
          var fact = facts[kind];
          var count = fact ? fact.nodes.length : 0;
          entry.setAttribute('data-legend-count', String(count));
          var hit = document.createElementNS(namespace, 'rect');
          hit.setAttribute('data-legend-bridge-runtime', '');
          hit.setAttribute('data-legend-hit', '');
          hit.setAttribute('aria-hidden', 'true');
          entry.insertBefore(hit, entry.firstChild);
          var badge = document.createElementNS(namespace, 'g');
          badge.setAttribute('data-legend-bridge-runtime', '');
          badge.setAttribute('data-legend-count-badge', '');
          badge.setAttribute('aria-hidden', 'true');
          badge.appendChild(document.createElementNS(namespace, 'rect'));
          var countText = document.createElementNS(namespace, 'text');
          countText.textContent = String(count);
          badge.appendChild(countText);
          entry.appendChild(badge);
          if (!fact || !count) {
            entry.setAttribute('data-legend-zero', '');
            return;
          }
          entry.setAttribute('role', 'button');
          entry.setAttribute('tabindex', legendEntries.length ? '-1' : '0');
          var visibleLabel = entry.getAttribute('data-legend-label') || fact.label;
          entry.setAttribute('aria-label', viewerCount('viewer.lens.legend.inspect', count, { label: visibleLabel }));
          entry.setAttribute('aria-pressed', 'false');
          entry.setAttribute('aria-haspopup', 'dialog');
          entry.setAttribute('aria-controls', 'semantic-lens');
          entry.setAttribute('aria-expanded', 'false');
          legendEntries.push(entry);
        });
        if (!legendEntries.length) return false;
        legendBridge.setAttribute('role', legendEntries.length >= 3 ? 'toolbar' : 'group');
        legendBridge.setAttribute('aria-label', viewerText('viewer.lens.legend'));
        syncLegendBridge();
        layoutLegendBridge();
        try {
          if (document.fonts && document.fonts.ready) document.fonts.ready.then(layoutLegendBridge);
        } catch (_) {}
        return true;
      }
      function syncLegendBridge() {
        legendEntries.forEach(function (entry) {
          var selected = selectedKinds.indexOf(entry.getAttribute('data-legend-kind')) >= 0;
          entry.setAttribute('aria-pressed', selected ? 'true' : 'false');
          entry.setAttribute('aria-expanded', !panel.hidden && lensOpener === entry ? 'true' : 'false');
          if (selected) entry.setAttribute('data-legend-selected', '');
          else entry.removeAttribute('data-legend-selected');
        });
      }
      function clearLegendPreview() {
        activeLegendPreview = null;
        svg.removeAttribute('data-legend-preview-active');
        Array.prototype.forEach.call(svg.querySelectorAll('[data-legend-preview-match], [data-legend-preview-selected], [data-legend-preview-peer]'), function (element) {
          element.removeAttribute('data-legend-preview-match');
          element.removeAttribute('data-legend-preview-selected');
          element.removeAttribute('data-legend-preview-peer');
        });
      }
      function strongerLegendOwnerActive() {
        return selectedKinds.length > 0 || !panel.hidden || html.getAttribute('data-present') === 'true' ||
          svg.hasAttribute('data-focus-active') || svg.hasAttribute('data-intent-trace-active') ||
          svg.hasAttribute('data-route-picking') || svg.hasAttribute('data-route-active') ||
          svg.hasAttribute('data-story-active') || svg.hasAttribute('data-relationship-preview-active');
      }
      function previewLegendKind(entry) {
        clearLegendPreview();
        if (!entry || strongerLegendOwnerActive()) return false;
        var kind = entry.getAttribute('data-legend-kind');
        var byId = nodesById();
        var matches = Object.keys(byId).filter(function (id) {
          return (byId[id].getAttribute('data-node-kind') || 'neutral') === kind;
        });
        if (!matches.length) return false;
        var chosen = {};
        matches.forEach(function (id) {
          chosen[id] = true;
          byId[id].setAttribute('data-legend-preview-match', '');
          byId[id].setAttribute('data-legend-preview-selected', '');
        });
        edgeGroups().forEach(function (edge) {
          if (!chosen[edge.from] && !chosen[edge.to]) return;
          edge.members.forEach(function (member) { member.setAttribute('data-legend-preview-match', ''); });
          [edge.from, edge.to].forEach(function (id) {
            if (!byId[id] || chosen[id]) return;
            byId[id].setAttribute('data-legend-preview-match', '');
            byId[id].setAttribute('data-legend-preview-peer', '');
          });
        });
        svg.setAttribute('data-legend-preview-active', kind);
        activeLegendPreview = entry;
        return true;
      }
      function syncLegendPreview() {
        var next = focusedLegendEntry || hoveredLegendEntry;
        if (next === activeLegendPreview) return;
        clearLegendPreview();
        if (next) previewLegendKind(next);
      }
      function activateLegendEntry(entry) {
        if (!entry || entry.getAttribute('role') !== 'button') return false;
        clearLegendPreview();
        select(entry.getAttribute('data-legend-kind'));
        return open({ opener: entry });
      }
      function removeFlowOverlay() {
        Array.prototype.forEach.call(svg.querySelectorAll('[data-semantic-lens-overlay]'), function (element) {
          element.remove();
        });
        svg.removeAttribute('data-lens-flow-count');
        svg.removeAttribute('data-lens-flow-density');
      }
      function flowGeometry(shape, direction, step) {
        var clone = shape.cloneNode(false);
        clone.removeAttribute('id');
        clone.removeAttribute('class');
        clone.removeAttribute('style');
        clone.removeAttribute('marker-start');
        clone.removeAttribute('marker-mid');
        clone.removeAttribute('marker-end');
        clone.removeAttribute('role');
        clone.removeAttribute('aria-label');
        clone.removeAttribute('aria-hidden');
        clone.removeAttribute('data-animate');
        clone.removeAttribute('data-edge-from');
        clone.removeAttribute('data-edge-to');
        clone.removeAttribute('data-edge-key');
        clone.removeAttribute('data-edge-id');
        clone.removeAttribute('data-edge-label');
        clone.removeAttribute('data-lens-match');
        clone.setAttribute('class', 'semantic-lens-flow');
        clone.setAttribute('data-direction', direction);
        clone.setAttribute('pathLength', '1');
        clone.style.setProperty('--lens-flow-delay', (step * 0.08).toFixed(2) + 's');
        return clone;
      }
      function renderFlowOverlay(entries) {
        removeFlowOverlay();
        svg.setAttribute('data-lens-flow-count', String(entries.length));
        if (!entries.length || html.getAttribute('data-embed') === 'true') return false;
        if (entries.length > MAX_LENS_FLOW_EDGES) {
          svg.setAttribute('data-lens-flow-density', 'quiet');
          return false;
        }
        var overlay = document.createElementNS(namespace, 'g');
        overlay.setAttribute('class', 'semantic-lens-overlay');
        overlay.setAttribute('data-semantic-lens-overlay', '');
        overlay.setAttribute('aria-hidden', 'true');
        entries.forEach(function (entry, step) {
          var wrapper = document.createElementNS(namespace, 'g');
          if (entry.edge.members[0].hasAttribute('transform')) {
            wrapper.setAttribute('transform', entry.edge.members[0].getAttribute('transform'));
          }
          edgeShapes(entry.edge.members[0]).forEach(function (shape) {
            wrapper.appendChild(flowGeometry(shape, entry.direction, step));
          });
          if (wrapper.childNodes.length) overlay.appendChild(wrapper);
        });
        if (!overlay.childNodes.length) return false;
        var firstNode = svg.querySelector('[data-node-id]');
        if (firstNode) svg.insertBefore(overlay, firstNode);
        else svg.appendChild(overlay);
        return true;
      }
      function cleanSvgState() {
        removeFlowOverlay();
        svg.removeAttribute('data-lens-active');
        Array.prototype.forEach.call(svg.querySelectorAll('[data-lens-match], [data-lens-selected], [data-lens-peer]'), function (element) {
          element.removeAttribute('data-lens-match');
          element.removeAttribute('data-lens-selected');
          element.removeAttribute('data-lens-peer');
        });
      }
      function renderKinds() {
        kindsRoot.textContent = '';
        collectKinds().forEach(function (kind) {
          var button = document.createElement('button');
          button.type = 'button';
          button.className = 'semantic-lens-kind';
          button.setAttribute('data-kind', kind.id);
          button.setAttribute('aria-pressed', selectedKinds.indexOf(kind.id) >= 0 ? 'true' : 'false');
          button.setAttribute('aria-label', viewerCount('viewer.lens.kind.count', kind.nodes.length, { label: kind.label }));
          button.disabled = selectedKinds.length >= 2 && selectedKinds.indexOf(kind.id) === -1;
          var swatch = document.createElement('span');
          swatch.className = 'semantic-lens-swatch';
          swatch.setAttribute('aria-hidden', 'true');
          var label = document.createElement('strong');
          label.textContent = kind.label;
          var count = document.createElement('em');
          count.textContent = String(kind.nodes.length);
          button.appendChild(swatch);
          button.appendChild(label);
          button.appendChild(count);
          kindsRoot.appendChild(button);
        });
      }
      function updateHash() {
        try {
          var hash = selectedKinds.length
            ? '#lens=' + selectedKinds.map(encodeURIComponent).join('~')
            : '';
          history.replaceState(null, '', location.pathname + location.search + hash);
        } catch (_) {}
      }
      function updateTrigger() {
        var active = selectedKinds.length > 0;
        trigger.setAttribute('aria-pressed', active ? 'true' : 'false');
        trigger.setAttribute('aria-label', viewerText(active ? 'viewer.lens.openActive' : 'viewer.lens.open'));
        copyBtn.hidden = !active;
        syncLegendBridge();
      }
      function overlapArea(a, b) {
        var width = Math.max(0, Math.min(a.right, b.right) - Math.max(a.left, b.left));
        var height = Math.max(0, Math.min(a.bottom, b.bottom) - Math.max(a.top, b.top));
        return width * height;
      }
      function dockPanel(byId) {
        panel.removeAttribute('data-dock-side');
        if (panel.hidden || window.innerWidth <= 720) return 'right';
        var panelRect = panel.getBoundingClientRect();
        var containerRect = container.getBoundingClientRect();
        if (!panelRect.width || !panelRect.height) return 'right';
        var selectedRects = Object.keys(byId).filter(function (id) {
          return selectedKinds.indexOf(byId[id].getAttribute('data-node-kind') || 'neutral') >= 0;
        }).map(function (id) { return byId[id].getBoundingClientRect(); });
        var top = panelRect.top;
        var width = panelRect.width;
        var leftCandidate = { left: containerRect.left + 16, right: containerRect.left + 16 + width, top: top, bottom: panelRect.bottom };
        var rightCandidate = { left: containerRect.right - 16 - width, right: containerRect.right - 16, top: top, bottom: panelRect.bottom };
        var leftScore = selectedRects.reduce(function (score, rect) { return score + overlapArea(leftCandidate, rect); }, 0);
        var rightScore = selectedRects.reduce(function (score, rect) { return score + overlapArea(rightCandidate, rect); }, 0);
        var legend = svg.querySelector('[data-legend]');
        var nav = container.querySelector('.diagram-nav');
        var protectedRects = [legend, nav].filter(function (element) {
          return element && !element.hidden && window.getComputedStyle(element).display !== 'none';
        }).map(function (element) { return element.getBoundingClientRect(); });
        leftScore += protectedRects.reduce(function (score, rect) {
          return score + overlapArea(leftCandidate, rect) * 1000;
        }, 0);
        rightScore += protectedRects.reduce(function (score, rect) {
          return score + overlapArea(rightCandidate, rect) * 1000;
        }, 0);
        var side = leftScore < rightScore ? 'left' : 'right';
        panel.setAttribute('data-dock-side', side);
        return side;
      }
      function applySelection(options) {
        options = options || {};
        cleanSvgState();
        var byId = nodesById();
        var nodeKind = {};
        Object.keys(byId).forEach(function (id) { nodeKind[id] = byId[id].getAttribute('data-node-kind') || 'neutral'; });
        if (!selectedKinds.length) {
          status.textContent = viewerText('viewer.lens.choose');
          updateTrigger();
          renderKinds();
          dockPanel(byId);
          if (options.updateUrl !== false) updateHash();
          return false;
        }

        var chosen = {};
        selectedKinds.forEach(function (kind) { chosen[kind] = true; });
        Object.keys(byId).forEach(function (id) {
          if (!chosen[nodeKind[id]]) return;
          byId[id].setAttribute('data-lens-match', '');
          byId[id].setAttribute('data-lens-selected', '');
        });

        var touching = 0;
        var forward = 0;
        var reverse = 0;
        var crossKind = selectedKinds.length === 2;
        var matchedFlow = [];
        edgeGroups().forEach(function (edge) {
          var fromKind = nodeKind[edge.from];
          var toKind = nodeKind[edge.to];
          var match = false;
          if (crossKind) {
            if (fromKind === selectedKinds[0] && toKind === selectedKinds[1]) { match = true; forward += 1; }
            if (fromKind === selectedKinds[1] && toKind === selectedKinds[0]) { match = true; reverse += 1; }
          } else if (fromKind === selectedKinds[0] || toKind === selectedKinds[0]) {
            match = true;
            touching += 1;
          }
          if (!match) return;
          var direction = 'within';
          if (crossKind) {
            direction = fromKind === selectedKinds[0] ? 'forward' : 'reverse';
          } else {
            direction = fromKind === selectedKinds[0] && toKind !== selectedKinds[0] ? 'out'
              : (toKind === selectedKinds[0] && fromKind !== selectedKinds[0] ? 'in' : 'within');
          }
          matchedFlow.push({ edge: edge, direction: direction });
          edge.members.forEach(function (member) { member.setAttribute('data-lens-match', ''); });
          if (!crossKind) {
            [edge.from, edge.to].forEach(function (id) {
              if (!byId[id]) return;
              byId[id].setAttribute('data-lens-match', '');
              if (!chosen[nodeKind[id]]) byId[id].setAttribute('data-lens-peer', '');
            });
          }
        });

        svg.setAttribute('data-lens-active', selectedKinds.join(' '));
        renderFlowOverlay(matchedFlow);
        if (crossKind) {
          var total = forward + reverse;
          status.textContent = viewerCount('viewer.lens.compare', total, {
            first: viewerKindLabel(selectedKinds[0]),
            second: viewerKindLabel(selectedKinds[1]),
            forward: forward,
            reverse: reverse
          });
        } else {
          var nodeCount = collectKinds().filter(function (kind) { return kind.id === selectedKinds[0]; })[0].nodes.length;
          status.textContent = viewerText('viewer.lens.single', {
            nodes: viewerCount('viewer.lens.node', nodeCount, { label: viewerKindLabel(selectedKinds[0]) }),
            relationships: viewerCount('viewer.lens.relationship', touching)
          });
        }
        updateTrigger();
        renderKinds();
        dockPanel(byId);
        if (options.updateUrl !== false) updateHash();
        return true;
      }
      function prepareForLens() {
        clearLegendPreview();
        if (Mosofin.focus && typeof Mosofin.focus.clear === 'function') {
          Mosofin.focus.clear({ updateUrl: false, preserveView: true });
        }
        if (Mosofin.routeProbe && typeof Mosofin.routeProbe.clear === 'function') {
          Mosofin.routeProbe.clear({ updateUrl: false, restoreFocus: false });
        }
        if (Mosofin.guidedViews && typeof Mosofin.guidedViews.showAll === 'function') {
          Mosofin.guidedViews.showAll({ clearFocus: false, updateUrl: false });
        }
        if (Mosofin.intentTrace && typeof Mosofin.intentTrace.clear === 'function') {
          Mosofin.intentTrace.clear({ announce: false });
        }
      }
      function select(kind, options) {
        options = options || {};
        clearLegendPreview();
        var exists = collectKinds().some(function (entry) { return entry.id === kind; });
        if (!exists) return false;
        var index = selectedKinds.indexOf(kind);
        if (index >= 0) selectedKinds.splice(index, 1);
        else {
          if (selectedKinds.length >= 2) return false;
          if (!selectedKinds.length) prepareForLens();
          selectedKinds.push(kind);
        }
        return applySelection(options);
      }
      function close(options) {
        options = options || {};
        panel.hidden = true;
        trigger.setAttribute('aria-expanded', 'false');
        updateTrigger();
        if (options.restoreFocus !== false && lensOpener && typeof lensOpener.focus === 'function') lensOpener.focus();
        return false;
      }
      function open(options) {
        options = options || {};
        if (html.getAttribute('data-embed') === 'true') return false;
        lensOpener = options.opener || trigger;
        if (Mosofin.exportMenu && Mosofin.exportMenu.isOpen()) Mosofin.exportMenu.close(false);
        if (Mosofin.finder && Mosofin.finder.isOpen()) Mosofin.finder.close({ restoreFocus: false });
        if (Mosofin.radar && Mosofin.radar.isOpen()) Mosofin.radar.close({ restoreFocus: false });
        if (Mosofin.guide && Mosofin.guide.isOpen()) Mosofin.guide.close({ restoreFocus: false });
        renderKinds();
        panel.hidden = false;
        trigger.setAttribute('aria-expanded', 'true');
        updateTrigger();
        requestAnimationFrame(function () {
          dockPanel(nodesById());
          var activeButton = kindsRoot.querySelector('[aria-pressed="true"]');
          var firstButton = activeButton || kindsRoot.querySelector('button:not(:disabled)');
          if (firstButton) firstButton.focus();
        });
        return true;
      }
      function toggle() { return panel.hidden ? open({ opener: trigger }) : close(); }
      function clear(options) {
        options = options || {};
        selectedKinds = [];
        cleanSvgState();
        panel.removeAttribute('data-dock-side');
        updateTrigger();
        renderKinds();
        status.textContent = viewerText('viewer.lens.choose');
        if (options.updateUrl !== false) updateHash();
        if (options.preserveView !== true && Mosofin.view && typeof Mosofin.view.reset === 'function') {
          Mosofin.view.reset({ automatic: true });
        }
        if (options.closePanel === true) close({ restoreFocus: false });
        return false;
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
      function copyLink() {
        if (!selectedKinds.length) return Promise.resolve(false);
        var value = location.href.replace(/#.*$/, '') + '#lens=' + selectedKinds.map(encodeURIComponent).join('~');
        var copy = navigator.clipboard && typeof navigator.clipboard.writeText === 'function'
          ? navigator.clipboard.writeText(value).then(function () { return true; }).catch(function () { return fallbackCopy(value); })
          : Promise.resolve(fallbackCopy(value));
        return copy.then(function (copied) {
          copyBtn.textContent = viewerText(copied ? 'viewer.common.copied' : 'viewer.common.copyFailed');
          window.setTimeout(function () { copyBtn.textContent = viewerText('viewer.common.copyLink'); }, 1600);
          return copied;
        });
      }
      function syncFromHash() {
        try {
          var params = new URLSearchParams(location.hash.replace(/^#/, ''));
          var value = params.get('lens');
          if (!value) {
            if (selectedKinds.length) clear({ updateUrl: false, preserveView: true });
            return;
          }
          var available = collectKinds().map(function (kind) { return kind.id; });
          var requested = value.split('~').filter(function (kind, index, list) {
            return available.indexOf(kind) >= 0 && list.indexOf(kind) === index;
          }).slice(0, 2);
          if (!requested.length) return;
          prepareForLens();
          selectedKinds = requested;
          applySelection({ updateUrl: false });
        } catch (_) {}
      }

      trigger.addEventListener('click', toggle);
      if (decorateLegendBridge()) {
        legendBridge.addEventListener('click', function (event) {
          var entry = event.target.closest('[data-legend-kind][role="button"]');
          if (entry) activateLegendEntry(entry);
        });
        legendBridge.addEventListener('pointerover', function (event) {
          if (event.pointerType === 'touch' || (finePointerQuery && !finePointerQuery.matches)) return;
          var entry = event.target.closest('[data-legend-kind][role="button"]');
          if (!entry || (event.relatedTarget && entry.contains(event.relatedTarget))) return;
          hoveredLegendEntry = entry;
          syncLegendPreview();
        });
        legendBridge.addEventListener('pointerout', function (event) {
          var entry = event.target.closest('[data-legend-kind][role="button"]');
          if (!entry || (event.relatedTarget && entry.contains(event.relatedTarget))) return;
          if (hoveredLegendEntry === entry) hoveredLegendEntry = null;
          syncLegendPreview();
        });
        legendBridge.addEventListener('focusin', function (event) {
          var entry = event.target.closest('[data-legend-kind][role="button"]');
          if (!entry) return;
          focusedLegendEntry = entry;
          legendEntries.forEach(function (candidate) {
            candidate.setAttribute('tabindex', candidate === entry ? '0' : '-1');
          });
          syncLegendPreview();
        });
        legendBridge.addEventListener('focusout', function (event) {
          var entry = event.target.closest('[data-legend-kind][role="button"]');
          if (!entry || (event.relatedTarget && entry.contains(event.relatedTarget))) return;
          if (focusedLegendEntry === entry) focusedLegendEntry = null;
          syncLegendPreview();
        });
        legendBridge.addEventListener('keydown', function (event) {
          var entry = event.target.closest('[data-legend-kind][role="button"]');
          if (!entry) return;
          if (event.key === 'Enter' || event.key === ' ') {
            event.preventDefault();
            activateLegendEntry(entry);
            return;
          }
          var index = legendEntries.indexOf(entry);
          var next = null;
          if (event.key === 'ArrowRight') next = (index + 1) % legendEntries.length;
          else if (event.key === 'ArrowLeft') next = (index - 1 + legendEntries.length) % legendEntries.length;
          else if (event.key === 'Home') next = 0;
          else if (event.key === 'End') next = legendEntries.length - 1;
          if (next === null) return;
          event.preventDefault();
          legendEntries.forEach(function (candidate, candidateIndex) {
            candidate.setAttribute('tabindex', candidateIndex === next ? '0' : '-1');
          });
          legendEntries[next].focus();
        });
      }
      closeBtn.addEventListener('click', function () { close(); });
      clearBtn.addEventListener('click', function () { clear({ preserveView: true }); });
      copyBtn.addEventListener('click', copyLink);
      kindsRoot.addEventListener('click', function (event) {
        var button = event.target.closest('[data-kind]');
        if (!button || button.disabled) return;
        select(button.getAttribute('data-kind'));
      });
      kindsRoot.addEventListener('keydown', function (event) {
        var buttons = Array.prototype.slice.call(kindsRoot.querySelectorAll('button:not(:disabled)'));
        var index = buttons.indexOf(document.activeElement);
        var next = null;
        if (index >= 0 && (event.key === 'ArrowRight' || event.key === 'ArrowDown')) next = (index + 1) % buttons.length;
        else if (index >= 0 && (event.key === 'ArrowLeft' || event.key === 'ArrowUp')) next = (index - 1 + buttons.length) % buttons.length;
        else if (event.key === 'Home' && buttons.length) next = 0;
        else if (event.key === 'End' && buttons.length) next = buttons.length - 1;
        if (next !== null) { event.preventDefault(); buttons[next].focus(); }
      });
      panel.addEventListener('keydown', function (event) {
        if (event.key !== 'Escape') return;
        event.preventDefault();
        event.stopPropagation();
        close();
      });
      document.addEventListener('click', function (event) {
        var eventPath = typeof event.composedPath === 'function' ? event.composedPath() : [];
        var clickedInside = eventPath.indexOf(panel) >= 0 || panel.contains(event.target);
        var clickedLauncher = event.target === trigger || legendEntries.some(function (entry) {
          return event.target === entry || entry.contains(event.target);
        });
        if (!panel.hidden && !clickedInside && !clickedLauncher) close({ restoreFocus: false });
      });
      window.addEventListener('hashchange', syncFromHash);
      window.addEventListener('resize', function () {
        if (!panel.hidden && selectedKinds.length) dockPanel(nodesById());
        layoutLegendBridge();
      });
      renderKinds();
      syncFromHash();

      return {
        open: open,
        close: close,
        toggle: toggle,
        clear: clear,
        clearPreview: clearLegendPreview,
        select: select,
        copyLink: copyLink,
        isOpen: function () { return !panel.hidden; },
        active: function () { return selectedKinds.length ? selectedKinds.slice() : null; },
        kinds: function () { return collectKinds().map(function (kind) { return { id: kind.id, count: kind.nodes.length }; }); }
      };
    })();

    /* ============================================================
       Diagram Guide — a factual command deck over existing interactions.
       Counts come from compiled semantics; actions delegate to Finder, Route
       Probe, Semantic Radar, Semantic Lens, Story Trail, Presentation, theme, export, and
       camera without adding a second command implementation or SVG state.
       ============================================================ */
    Mosofin.guide = (function () {
      var html = document.documentElement;
      var container = document.querySelector('.diagram-container');
      var svg = container.querySelector(':scope > svg');
      var trigger = document.getElementById('btn-diagram-guide');
      var panel = document.getElementById('diagram-guide');
      var closeBtn = document.getElementById('diagram-guide-close');
      var stats = document.getElementById('diagram-guide-stats');
      var actions = document.getElementById('diagram-guide-actions');
      var feedback = document.getElementById('diagram-guide-feedback');
      var storyBtn = actions.querySelector('[data-guide-action="story"]');
      var storyCopy = document.getElementById('diagram-guide-story-copy');
      var routePanel = document.getElementById('route-probe');

      function viewCount() {
        return Mosofin.guidedViews && Number(Mosofin.guidedViews.count) || 0;
      }
      function relationshipCount() {
        var seen = {};
        Array.prototype.forEach.call(svg.querySelectorAll('[data-edge-from][data-edge-to]'), function (edge) {
          var key = edge.getAttribute('data-edge-key') || [
            edge.getAttribute('data-edge-from'),
            edge.getAttribute('data-edge-to'),
            edge.getAttribute('data-edge-label') || ''
          ].join('\u0000');
          seen[key] = true;
        });
        return Object.keys(seen).length;
      }
      function renderFacts() {
        var nodes = svg.querySelectorAll('[data-node-id]').length;
        var relationships = relationshipCount();
        var views = viewCount();
        stats.textContent = viewerText('viewer.guide.facts', {
          nodes: viewerCount('viewer.guide.fact.node', nodes),
          relationships: viewerCount('viewer.guide.fact.relationship', relationships),
          views: viewerCount('viewer.guide.fact.view', views)
        });
        storyBtn.disabled = views === 0;
        storyBtn.setAttribute('aria-disabled', views === 0 ? 'true' : 'false');
        storyCopy.textContent = views
          ? viewerCount('viewer.guide.story.available', views)
          : viewerText('viewer.guide.story.unavailable');
      }
      function actionButtons() {
        return Array.prototype.slice.call(actions.querySelectorAll('.diagram-guide-action:not(:disabled)'));
      }
      function close(options) {
        options = options || {};
        panel.hidden = true;
        html.removeAttribute('data-guide-open');
        routePanel.removeAttribute('data-guide-open');
        trigger.setAttribute('aria-expanded', 'false');
        trigger.setAttribute('aria-label', viewerText('viewer.guide.open'));
        feedback.textContent = '';
        if (options.restoreFocus !== false) trigger.focus();
        return false;
      }
      function open() {
        if (html.getAttribute('data-embed') === 'true') return false;
        if (Mosofin.semanticLens && typeof Mosofin.semanticLens.clearPreview === 'function') Mosofin.semanticLens.clearPreview();
        if (Mosofin.exportMenu && Mosofin.exportMenu.isOpen()) Mosofin.exportMenu.close(false);
        if (Mosofin.finder && Mosofin.finder.isOpen()) Mosofin.finder.close({ restoreFocus: false });
        if (Mosofin.radar && Mosofin.radar.isOpen()) Mosofin.radar.close({ restoreFocus: false });
        if (Mosofin.semanticLens && Mosofin.semanticLens.isOpen()) Mosofin.semanticLens.close({ restoreFocus: false });
        if (Mosofin.guidedViews && Mosofin.guidedViews.isPlaying && Mosofin.guidedViews.isPlaying()) {
          Mosofin.guidedViews.pause();
        }
        if (Mosofin.routeProbe && Mosofin.routeProbe.isJourneyPlaying && Mosofin.routeProbe.isJourneyPlaying()) {
          Mosofin.routeProbe.pauseJourney({ preserveElapsed: true, reason: 'guide' });
        }
        renderFacts();
        panel.hidden = false;
        html.setAttribute('data-guide-open', 'true');
        routePanel.setAttribute('data-guide-open', 'true');
        trigger.setAttribute('aria-expanded', 'true');
        trigger.setAttribute('aria-label', viewerText('viewer.guide.close'));
        feedback.textContent = '';
        requestAnimationFrame(function () {
          var first = actionButtons()[0];
          if (first) first.focus();
        });
        return true;
      }
      function toggle() { return panel.hidden ? open() : close(); }
      function execute(action) {
        feedback.textContent = '';
        if (action === 'story' && !viewCount()) {
          feedback.textContent = viewerText('viewer.guide.noStory');
          return false;
        }
        close({ restoreFocus: false });
        if (action === 'find') return Mosofin.finder.open();
        if (action === 'route') return Mosofin.routeProbe.begin({ focusNode: true });
        if (action === 'map') return Mosofin.radar.open();
        if (action === 'lens') return Mosofin.semanticLens.open();
        if (action === 'story') return Mosofin.guidedViews.play();
        if (action === 'present') return Mosofin.presentation.enter();
        if (action === 'export') return Mosofin.exportMenu.open();
        if (action === 'theme') return Mosofin.theme.toggle();
        if (action === 'preset') return Mosofin.preset.cycle();
        if (action === 'reset') return Mosofin.view.reset();
        if (action === 'zoom-in') return Mosofin.view.zoomIn();
        if (action === 'zoom-out') return Mosofin.view.zoomOut();
        return false;
      }
      function actionForKey(key) {
        return {
          '/': 'find',
          r: 'route',
          m: 'map',
          l: 'lens',
          p: 'story',
          f: 'present',
          e: 'export',
          t: 'theme',
          s: 'preset',
          '0': 'reset',
          '+': 'zoom-in',
          '=': 'zoom-in',
          '-': 'zoom-out'
        }[key] || null;
      }

      trigger.addEventListener('click', toggle);
      closeBtn.addEventListener('click', function () { close(); });
      actions.addEventListener('click', function (event) {
        var button = event.target.closest('[data-guide-action]');
        if (!button || button.disabled) return;
        event.stopPropagation();
        execute(button.getAttribute('data-guide-action'));
      });
      panel.addEventListener('keydown', function (event) {
        if (event.key === 'Escape' || event.key === '?') {
          event.preventDefault();
          event.stopPropagation();
          close();
          return;
        }
        var buttons = actionButtons();
        var index = buttons.indexOf(document.activeElement);
        var next = null;
        if (index >= 0 && event.key === 'ArrowRight') next = (index + 1) % buttons.length;
        else if (index >= 0 && event.key === 'ArrowDown') next = (index + 1) % buttons.length;
        else if (index >= 0 && event.key === 'ArrowLeft') next = (index - 1 + buttons.length) % buttons.length;
        else if (index >= 0 && event.key === 'ArrowUp') next = (index - 1 + buttons.length) % buttons.length;
        else if (event.key === 'Home' && buttons.length) next = 0;
        else if (event.key === 'End' && buttons.length) next = buttons.length - 1;
        if (next !== null) {
          event.preventDefault();
          event.stopPropagation();
          buttons[next].focus();
          return;
        }
        var action = actionForKey(event.key.length === 1 ? event.key.toLowerCase() : event.key);
        if (!action) return;
        event.preventDefault();
        event.stopPropagation();
        execute(action);
      });
      document.addEventListener('click', function (event) {
        if (!panel.hidden && !panel.contains(event.target) && event.target !== trigger) close({ restoreFocus: false });
      });

      renderFacts();
      return {
        open: open,
        close: close,
        toggle: toggle,
        execute: execute,
        isOpen: function () { return !panel.hidden; },
        facts: function () {
          return {
            nodes: svg.querySelectorAll('[data-node-id]').length,
            relationships: relationshipCount(),
            views: viewCount()
          };
        }
      };
    })();

    /* ============================================================
       Global keyboard shortcuts
       ? -> open Diagram Guide
       T -> toggle theme
       S -> cycle visual style
       E -> open export menu
         F -> toggle presentation stage
         M -> toggle Semantic Radar
         L -> toggle Semantic Lens
         R -> start/clear Route Probe
         / -> open node finder or the active route endpoint picker
         + / - -> zoom, 0 -> reset view
         Escape -> clear temporary trace/focus/view first, then exit presentation
       Ignored when focus is inside an input/textarea/contenteditable.
       ============================================================ */
    document.addEventListener('keydown', function (e) {
      if (e.metaKey || e.ctrlKey || e.altKey) return;
      if (e.defaultPrevented) return;
      var t = e.target;
      if (t && (t.tagName === 'INPUT' || t.tagName === 'TEXTAREA' || t.isContentEditable)) return;
      if (Mosofin.preset && Mosofin.preset.isOpen() && e.key !== 'Escape' && e.key !== 's' && e.key !== 'S') {
        Mosofin.preset.close(false);
      }
      if (e.key === '?') {
        e.preventDefault();
        Mosofin.guide.toggle();
      } else if (e.key === '/') {
        e.preventDefault();
        Mosofin.finder.open();
      } else if (e.key === 't' || e.key === 'T') {
        e.preventDefault();
        Mosofin.theme.toggle();
      } else if (e.key === 's' || e.key === 'S') {
        e.preventDefault();
        Mosofin.preset.cycle();
      } else if (e.key === 'e' || e.key === 'E') {
        e.preventDefault();
        if (!Mosofin.exportMenu.isOpen()) Mosofin.exportMenu.open();
      } else if (e.key === 'f' || e.key === 'F') {
        e.preventDefault();
        Mosofin.presentation.toggle();
      } else if (e.key === 'm' || e.key === 'M') {
        e.preventDefault();
        Mosofin.radar.toggle();
      } else if (e.key === 'l' || e.key === 'L') {
        e.preventDefault();
        Mosofin.semanticLens.toggle();
      } else if (e.key === 'r' || e.key === 'R') {
        e.preventDefault();
        Mosofin.routeProbe.toggle({ focusNode: true });
      } else if (e.key === '+' || e.key === '=') {
        e.preventDefault();
        Mosofin.view.zoomIn();
      } else if (e.key === '-') {
        e.preventDefault();
        Mosofin.view.zoomOut();
      } else if (e.key === '0') {
        e.preventDefault();
        Mosofin.view.reset();
      } else if (e.key === 'Escape' && Mosofin.preset.isOpen()) {
        e.preventDefault();
        Mosofin.preset.close(true);
      } else if (e.key === 'Escape' && Mosofin.semanticLens.isOpen()) {
        e.preventDefault();
        Mosofin.semanticLens.close({ restoreFocus: true });
      } else if (e.key === 'Escape' && Mosofin.semanticLens.active()) {
        e.preventDefault();
        Mosofin.semanticLens.clear({ preserveView: true });
      } else if (e.key === 'Escape' && Mosofin.guide.isOpen()) {
        e.preventDefault();
        Mosofin.guide.close({ restoreFocus: true });
      } else if (e.key === 'Escape' && Mosofin.radar.isOpen()) {
        e.preventDefault();
        Mosofin.radar.close({ restoreFocus: true });
      } else if (e.key === 'Escape' && Mosofin.routeProbe.active()) {
        e.preventDefault();
        Mosofin.routeProbe.escape({ restoreFocus: true });
      } else if (e.key === 'Escape' && Mosofin.intentTrace.active()) {
        e.preventDefault();
        Mosofin.intentTrace.clear();
      } else if (e.key === 'Escape' && Mosofin.focus.active()) {
        e.preventDefault();
        Mosofin.focus.clear({ restoreFocus: true });
      } else if (e.key === 'Escape' && Mosofin.presentation.active()) {
        e.preventDefault();
        Mosofin.presentation.exit();
      }
    });
  </script>
</body>
</html>
