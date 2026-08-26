# Project Status

## Current
FEN-Parser und Serializer sind vorhanden. Das Test-Target `chess_tests` ist
verkabelt und läuft, enthält bisher aber nur Smoke-Tests.

## Next
- Bestehendes CMake-/Test-Setup und `set_fen`/`to_fen` noch einmal in Ruhe lesen.
- Echte FEN-Round-Trip-Tests anlegen: Startstellung, Kiwipete,
  En-passant- und Promotion-Stellungen sowie verschiedene Rochaderechte.
- FEN-Fehlerfälle testen und dabei immer auch `FenErrorCode` und `position`
  prüfen.
- Danach die fehlende FEN-Validierung planen bzw. umsetzen: genau ein König
  pro Farbe, keine Bauern auf dem ersten/letzten Rang und inkonsistente
  Rochade-/En-passant-Angaben behandeln.
- Erst wenn diese Tests grün sind, mit Legalitätsfilter und Perft anfangen.

## Blocked
- Nichts

## Notes
Nicht direkt in die Perft-Implementierung springen: Der aktuelle Blocker ist
erst einmal die fehlende FEN-Validierung und eine belastbare Testbasis.

## Last updated
2026-08-26
