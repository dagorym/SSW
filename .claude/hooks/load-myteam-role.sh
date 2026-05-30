#!/usr/bin/env bash
# SessionStart hook: load the myteam role catalog into the agent's context.
#
# Purpose: make reading AGENTS.md and loading the initial myteam role a
# HARNESS-ENFORCED precondition rather than something the model may treat as a
# suggestion. The stdout of this script is injected into the session context.
#
# Behavior:
#   - Locate the `myteam` command robustly (PATH first, then known fallbacks).
#   - Run `myteam get role` (the no-arg default: the full role + shared-skill
#     catalog) and emit it into context, wrapped in mandatory framing.
#   - If `myteam` cannot be found or the command fails, emit a loud STOP notice
#     instead of failing silently, so the agent surfaces the problem.

set -uo pipefail

# --- Locate myteam -----------------------------------------------------------
myteam_bin=""
if command -v myteam >/dev/null 2>&1; then
  myteam_bin="$(command -v myteam)"
else
  for candidate in \
    "$HOME/miniconda3/bin/myteam" \
    "$HOME/anaconda3/bin/myteam" \
    "/opt/conda/bin/myteam" \
    "/usr/local/bin/myteam"; do
    if [ -x "$candidate" ]; then
      myteam_bin="$candidate"
      break
    fi
  done
fi

# --- Emit framing + role catalog (or a STOP notice) --------------------------
echo "=============================================================="
echo "MANDATORY SESSION SETUP — NOT ADVISORY"
echo "=============================================================="
echo
echo "Before doing ANY work in this repository you MUST read AGENTS.md in"
echo "full and follow it exactly. The myteam role catalog is loaded below as"
echo "first-class, binding instructions. Load your specific role with"
echo "'myteam get role <role>' and read its complete output before acting."
echo

if [ -z "$myteam_bin" ]; then
  echo "!! STOP: the 'myteam' command could not be located by the SessionStart"
  echo "!! hook. Do NOT proceed with guesses. Try 'myteam get role' yourself;"
  echo "!! if it fails, check for a local venv ('venv/bin/python -m myteam ...')"
  echo "!! per AGENTS.md, and if it still fails, alert the user immediately."
  exit 0
fi

echo "----- output of: myteam get role -----"
if ! "$myteam_bin" get role 2>&1; then
  echo
  echo "!! STOP: 'myteam get role' exited non-zero. Do NOT proceed with"
  echo "!! guesses. Investigate and alert the user per AGENTS.md."
fi

exit 0
