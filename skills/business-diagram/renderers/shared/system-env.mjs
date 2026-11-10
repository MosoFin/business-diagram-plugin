// The only file in this plugin that reads the process environment.
//
// Every read below is a fixed, named variable that locates a program, a temp
// folder, or a locale, or carries one of this CLI's own options from the parent
// command to the renderer it starts. Nothing here is forwarded anywhere except
// to the local child processes this CLI starts (node, git, Chrome), and the
// plugin opens no network connection.

function pick(target, name, value) {
  if (value != null) target[name] = value;
}

// Variables a local child process needs to start and find its programs.
export function childBaseEnv() {
  const env = {};
  pick(env, 'PATH', process.env.PATH);
  pick(env, 'PATHEXT', process.env.PATHEXT);
  pick(env, 'SYSTEMROOT', process.env.SYSTEMROOT);
  pick(env, 'WINDIR', process.env.WINDIR);
  pick(env, 'TMP', process.env.TMP);
  pick(env, 'TEMP', process.env.TEMP);
  pick(env, 'TMPDIR', process.env.TMPDIR);
  pick(env, 'LANG', process.env.LANG);
  pick(env, 'LC_ALL', process.env.LC_ALL);
  pick(env, 'LC_CTYPE', process.env.LC_CTYPE);
  pick(env, 'TERM', process.env.TERM);
  pick(env, 'COLORTERM', process.env.COLORTERM);
  return env;
}

// Where to look for an installed program (git, Chrome, Chromium).
export function programLookupEnv() {
  const env = {};
  pick(env, 'PATH', process.env.PATH);
  pick(env, 'PATHEXT', process.env.PATHEXT);
  pick(env, 'SYSTEMROOT', process.env.SYSTEMROOT);
  pick(env, 'PROGRAMFILES', process.env.PROGRAMFILES);
  pick(env, 'PROGRAMFILES(X86)', process.env['PROGRAMFILES(X86)']);
  return env;
}

// Options the parent `mosofin` command sets for the renderer it starts, from
// its own --quality, --repo-root, and --json flags.
export const RUN_OPTIONS = Object.freeze({
  qualityProfile: process.env.MOSOFIN_QUALITY_PROFILE || '',
  repoRoot: process.env.MOSOFIN_REPO_ROOT || '',
  diagnosticJson: process.env.MOSOFIN_DIAGNOSTIC_FORMAT === 'json',
});
