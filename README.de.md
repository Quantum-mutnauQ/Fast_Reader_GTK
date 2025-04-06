# Geschwindigkeitsleser

Fast Reader hilft Ihnen dabei schnell zu lesen, indem es nur ein Wort auf einmal anzeigt. Dadurch muss das Auge nicht von Wort zu Wort zu Wort springen, dass die Lesegeschwindigkeit reduziert. Um den Text zu lesen, muss er in das große Feld in der Mitte kopiert werden. 
Es gibt zwei unterschiedliche Grundeinstellung. Entweder wird das nächste Wort durch die → Taste angezeigt, oder eine „Zeitbasiert nächstes Wort“ ausgewählt. Dort kann die Verweildauer eines Wortes gewählt werden. Die Angabe wird mit einen Faktor verarbeitet, damit längere Wörter nicht zu kurz angezeigt werden. Diese dynamische Einstellung sichert, dass längere und komplexere Wörter mehr Zeit erhalten, um eine natürliche und flüssiges Lesen zu bieten.
Das lesen kann über die „Lesen“ Taste gestartet werden.
Geübten Lesern ist es sogar möglich mehrere Wörter auf einmal anzeigen zu lassen. Dadurch erhöht sich sie Lesegeschwindigkeit um ein weiteres.

Dieses Programm bietet individuelle Einstellungen an, um das Lesen zu verbessern:
- Hintergrundfarbe
- Schriftfarbe
- Schriftart und Größe
- Lesefortschritt im Text

Über die Aktivierung der Statistik erhält man sein persönliche Lesegeschwindigkeit der Worte. Diese Statistik ist interessant, wenn man nicht „Zeitbasiert“ liest. Dort kann über Farben hervorgehoben werden, welche Worte besonders viel Zeit benötigten und welche schnell erfasst wurden.

In den "Erweiterten Einstellungen für zeitbasiertes nächstes Wort" können Sie die Standzeit für längere Wörter verlängern. Hier können Sie festlegen, wie lange die Zeit dauern soll und ab wie vielen Buchstaben diese Zeit genutzt werden soll. Zusätzlich können Sie auch eine Verzögerung beim Start angeben, sodass das erste Wort länger auf dem Bildschirm bleibt.

## Screenshots

**Version 4 (Helles Thema Version 6) Bilder**:

***Dunkles Thema***

![Bildschirmfoto vom 2024-07-05 21-01-55](https://github.com/Quantum-mutnauQ/Fast-Reader-GTK/assets/141065355/48d57ddf-fe5d-4209-aaed-1b30403e1816)
![Bildschirmfoto vom 2024-07-05 21-08-48](https://github.com/Quantum-mutnauQ/Fast-Reader-GTK/assets/141065355/cc7e675b-7642-43a6-a2a9-a5eecfc2e72d)

***Helles Thema:***

![Bildschirmfoto vom 2024-12-29 13-24-42](https://github.com/user-attachments/assets/6de4cee5-b628-42c4-bea0-ede9130b102d)
![Bildschirmfoto vom 2024-12-29 13-25-03](https://github.com/user-attachments/assets/a8433539-2451-4863-a260-e844d4bc868e)

**Version 5 (Helles Thema Version 6) Bilder:**

***Dunkles Thema:***

![Bildschirmfoto vom 2024-07-11 15-30-05](https://github.com/Quantum-mutnauQ/Fast-Reader-GTK/assets/141065355/0e4b847a-79a7-496a-a9cc-8c63dbe68507)
![Bildschirmfoto vom 2024-07-11 15-30-10](https://github.com/Quantum-mutnauQ/Fast-Reader-GTK/assets/141065355/6131c3e0-7c8b-479e-95de-31b312c993b4)

***Helles Thema:***

![Bildschirmfoto vom 2024-12-29 13-25-26](https://github.com/user-attachments/assets/6195e5e6-5c20-4446-a75b-6a44a4235514)
![Bildschirmfoto vom 2024-12-29 13-25-36](https://github.com/user-attachments/assets/044fc9d1-aa70-4128-80b3-fb7554f04750)

## Übersetzungen von:
- Italienisch: [albanobattistella](https://github.com/albanobattistella)

## Erste Schritte

Es gibt zwei Installationsmethoden, aus denen Sie wählen können:

**1. Installation über [Flatpak auf Flathub](https://flathub.org/apps/io.github.quantum_mutnauq.fast_reader_gtk)**

**2. Aus der Binärdatei ausführen**

Wenn Sie Fast Reader aus der Binärdatei ausführen möchten, folgen Sie einfach diesen einfachen Schritten:

1. **Installation**: Laden Sie die [neueste Version](https://github.com/Quantum-mutnauQ/Fast_Reader_GTK/releases) herunter und entpacken Sie den Inhalt der Zip-Datei.
2. **Ausführung**: Führen Sie die kompilierte "FastReader"-Executable aus. Sie können sie ausführen, indem Sie `./FastReader` im Ordner eingeben, in dem das Programm gespeichert ist, oder indem Sie einfach doppelt darauf klicken.
3. **Fehlerbehebung**: Sie müssen `libgtk-4-1` und `libconfig9` installieren.

## Selbst kompilieren
### Build-Anweisungen

1. **Code herunterladen**: Beginnen Sie mit dem Herunterladen der neuesten Version des Codes.
2. **Vorbereitung**: Erstellen Sie ein Build-Verzeichnis im Projektordner.
3. **Build-Programm erstellen**: Navigieren Sie zum Build-Verzeichnis und führen Sie den folgenden Befehl aus: `cmake ..`
4. **Build ausführen**: Führen Sie im Build-Verzeichnis den folgenden Befehl aus, um den Code zu kompilieren:
       `make -j[Thread-Anzahl]` Ersetzen Sie `[Thread-Anzahl]` durch die Anzahl der Threads, die Sie für den Build-Prozess nutzen möchten.
