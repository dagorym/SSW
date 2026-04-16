## Documenter Report

### Files Updated
- **`AGENTS.md`** — Recorded that live GUI centering assertions now allow brief geometry-settle retries, narrow fallback centering targets for transient windows, and selectively longer modal fallback timeouts where wxGTK close paths settle slowly.

### Summary
Updated the repository guidance for GUI live-test coverage so the documented regression contract matches the shipped test helpers: centering checks remain enforced, but they now tolerate short geometry-settle races and explicit transient fallback targets instead of failing intermittently. The documentation also notes that modal fallback timeouts should only be increased where repository evidence shows the close path settles slowly.

### Commit Message
`docs: capture GUI centering test stability guidance`
