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
FEN-Parser und Serializer sind vorhanden. Das Test-Target `chess_tests` ist
verkabelt und läuft. Maher hat selbst einen ersten Round-Trip-Test für die
Startstellung geschrieben (`set_fen(fen)` gefolgt von `to_fen() == fen`). Beide
aktuellen Tests laufen grün. Der Test beweist den Weg Parser -> Board ->
Serializer für genau diese eine FEN; eine breitere Testbasis fehlt noch.

Die nächste Validierungsregel wurde bereits durchdacht: Ein `bool` pro König
reicht nicht, weil damit ein und mehrere Könige nicht unterschieden werden
können. Stattdessen beim Parsen lokale Zähler für weiße und schwarze Könige
verwenden und nach dem Figurenfeld jeweils exakt einen verlangen.

## Next 90-minute block
- Morgen mit einer kleinen, isolierten Aufgabe anfangen: Tests für fehlende und
  mehrfache Könige schreiben (0, 1 und 2 Könige pro Farbe). Bei Fehlerfällen
  auch `FenErrorCode` und `position` prüfen.
- Passende neue `FenErrorCode`-Werte festlegen und beim Parsen lokale
  Königszähler verwenden. Einen zweiten König kann der Parser an dessen genauer
  Stringposition ablehnen; einen fehlenden König am Ende des Figurenfeldes.
- Danach Bauern auf Rang 1/8 mit eigenen Tests und Validierung behandeln.
- Anschließend weitere echte FEN-Round-Trip-Tests anlegen: Kiwipete,
  En-passant- und Promotion-Stellungen sowie verschiedene Rochaderechte.
- Erst danach feldübergreifende Plausibilitätsregeln bearbeiten:
  Rochaderechte gegen König-/Turmpositionen und En-passant gegen die Stellung.
- Erst wenn diese Tests grün sind, mit Legalitätsfilter und Perft anfangen.

## Blocked
- Nichts

## Briefing notes
Nicht direkt in die Perft-Implementierung springen: Der aktuelle Blocker ist
erst einmal die fehlende FEN-Validierung und eine belastbare Testbasis.

Aufteilung der Validierung:
- Beim Figurenparsen kostenlos prüfen, was dort bereits bekannt ist:
  Königszahl und Bauern auf der ersten/letzten Reihe.
- Nach vollständigem Parsen, aber vor dem Commit in das echte `Board`,
  Zusammenhänge anhand von `new_mailbox` und `new_state` prüfen. Dafür muss der
  FEN-String nicht erneut durchlaufen werden.

Der spätere erste Legalitätsfilter soll noch keine speziellen Pin-Masken
berechnen. Zuerst korrekt und einfach: pseudo-legalen Zug ausführen, prüfen ob
der eigene König angegriffen ist, Zug zurücknehmen und illegale Züge verwerfen.
Direkte Pin-Erkennung kommt erst als Optimierung nach korrektem Perft.

## Last updated
2026-08-27
