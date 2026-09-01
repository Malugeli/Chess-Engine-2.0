# Morgenbriefing: Chess Engine

> **Nur Briefing-Quelle.** Diese Datei wird vom taeglichen Cloud-Briefing
> gelesen. Fuer normale Arbeit ist sie keine Pflichtlektüre und keine
> Projektanweisung; dafuer gelten der konkrete Auftrag, `AGENTS.md` und der
> aktuelle Code. Oeffne oder aktualisiere diese Datei nur, wenn du das
> Morgenbriefing vorbereitest oder sich der naechste Arbeitsblock geaendert hat.
>
> **Pflegekriterium:** Nur bestaetigte Ergebnisse eintragen. Nach einer Session,
> die den naechsten Schritt veraendert, `Current`, `Next` und `Last updated`
> aktualisieren und die Datei nach `main` pushen.

## Current
FEN-Parser und Serializer sind vorhanden. Move-Generation, Legalitätsfilter,
`do_move`/`undo_move` und `perft` bestehen die sechs kanonischen
Chess-Programming-Wiki-Stellungen.

Die Perft-Tests in `tests/perft.cpp` sind in zwei Gruppen getrennt:

- `[perft][fast]`: alle sechs Stellungen bis Tiefe 3.
- `[perft][slow]`: je Stellung eine tiefere, nach Knotenzahl ausgewählte
  Referenzprüfung (Tiefe 4 bis 6).

Beide Gruppen wurden am 31.08.2026 im Debug-Build ausgeführt und sind grün. Die
schnellen Tests umfassen 50 Assertions, die langsamen 24 Assertions. Jeder
Perft-Test prüft außerdem, dass das Board nach der Berechnung wieder exakt als
ursprüngliche FEN serialisiert wird.

`catch_discover_tests` übernimmt die Catch2-Tags mit `ADD_TAGS_AS_LABELS` als
CTest-Labels. Dadurch kann CTest Fast und Slow filtern.

Ein getrackter Hook liegt unter `.githooks/pre-commit`. Das Repository ist lokal
über `core.hooksPath=.githooks` damit verbunden. Vor jedem Commit baut der Hook
die Debug-Konfiguration und führt mit `ctest --preset Debug -LE slow` alle
nicht-langsamen Tests aus. Ein Build- oder Testfehler bricht den Commit ab.

Noch offen:

- `tests/fen.cpp` und `tests/board.cpp` sind noch leer.
- Tests für 0, 1 und 2 Könige pro Farbe fehlen weiterhin.
- `InvalidKing` meldet derzeit immer Position 0; das gewünschte Verhalten für
  einen zweiten König ist noch zu entscheiden und zu testen.
- Bauern auf Rang 1 oder 8 werden noch nicht validiert.

## Next 90-minute block
- Mit gezielten FEN-Tests beginnen, nicht mit weiterer Perft-Infrastruktur.
- Zuerst festlegen und testen, wie 0, 1 und 2 Könige pro Farbe behandelt werden.
  Neben Erfolg/Misserfolg auch `FenErrorCode` und Fehlerposition prüfen.
- Danach ungültige Bauern auf Rang 1 und 8 testgetrieben behandeln.
- Prüfen, dass ein fehlgeschlagenes `set_fen` das vorherige Board nicht
  verändert.
- Anschließend gültige Round-Trips für Rochaderechte, En-passant und
  Promotionsstellungen ergänzen.

## First step
Eine einzelne, klar benannte FEN-Anforderung auswählen und zuerst den
zugehörigen Test in `tests/fen.cpp` schreiben. Der Pre-Commit-Hook übernimmt den
schnellen Basischeck automatisch.

## Blocked
- Nichts.

## Briefing notes
Perft ist für den aktuellen Stand ausreichend abgesichert. Weitere Tiefe oder
weitere Perft-Stellungen bringen momentan weniger als gezielte Parser- und
Board-Tests.

Der lokale Hook ist ein Komfort- und Sicherheitsnetz, aber überspringbar. Eine
GitHub-Actions-Prüfung und ein verpflichtender Status-Check für `main` sind noch
nicht eingerichtet und können später als unabhängige CI-Sicherung folgen.

## Last updated
2026-08-31
