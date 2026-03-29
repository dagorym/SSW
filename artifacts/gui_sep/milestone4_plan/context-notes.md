# Milestone 4 Context Notes

## Confirmed Current State

- `FGame` still owns strategic rendering in `src/strategic/FGame.cpp` through:
  - `draw()`
  - `draw(wxDC&)`
  - `drawTurnCounter(wxDC&)`
- `FGame` still owns wx event translation in `src/strategic/FGame.cpp` through:
  - `onLeftDClick(wxMouseEvent&)`
  - `onLeftUp(wxMouseEvent&)`
- `FGamePanel::onPaint()` currently calls `m_game->draw(dc)`.
- `FMainFrame::onLeftDClick()` and `FMainFrame::onLeftUp()` currently forward raw wx events to `FGame`.
- `WXGameDisplay` exists but has no draw API yet.
- `WXMapDisplay::getScale(wxDC&)` already provides the scale needed to translate screen coordinates to strategic map coordinates.

## Key Migration Constraints

- Logical map coordinates may cross into `FGame`; screen coordinates must not.
- End-turn button hit testing is currently pixel-space logic and should move to the GUI layer.
- Menu-state toggling currently lives in `FMainFrame::onLeftUp()` after interpreting the integer result from `FGame::onLeftUp()`.
- Milestone 4 should preserve current functionality while tightening the model/view boundary.
