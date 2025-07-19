# Fast Reader

Fast Reader helps you to read more quickly, by showing only one word after the other. Therefor the eyes don’t have to move from word to word, that reduce the speed of reading. To read the text you only have to copy the text into the big field in the middle and to start it.
You can choose two different basic settings. Either you can choose the next word bei using the button „→“ or you can use the „Time based next word“. By this setting can be the duration of the word chosen. The selection is processed with a factor so that longer words are not displayed too short. This dynamic setting ensures that longer and more complex words are given more time to provide a natural and fluid reading experience. Reading can be started via the “Read” button. Experienced readers can even have more than one word displayed at once. This increases the reading speed even further.

This program offers individual settings to improve reading:
- Background colour
- Font colour
- Font and size
- Reading progress in the text

used to highlight which words took a particularly long time and which were read quickly.

In the "Advanced settings for time-based next word", you can extend the display time for longer words. Here, you can set the duration of the time and from how many characters this time should be used. Additionally, you can also specify a delay at the start, so that the first word remains on the screen for a longer period.

- **Customizable Settings**: Tailor your reading experience by adjusting settings like background color, text color, font size, and display of progress indicators.
- **Text Input**: Easily input text into the application through a dedicated text entry field.
- **Word-by-Word Display**: The text is broken down into individual words, presented one at a time, facilitating focused reading.
- **Navigation Options**: Seamlessly navigate through the text using intuitive navigation buttons or convenient keyboard shortcuts.
- **Progress Indicators**: Keep track of your reading progress with helpful indicators displaying your current position in the text and overall progress.
- **Time-Based Word predictions**: You can set a variable time calculate by a formal for the display of each word. The formula for calculating the word display time is: "Time-Per-Word × (Cube Root of Word-Length)". This dynamic adjustment ensures that longer, more complex words are allotted more time, providing a natural and responsive reading experience.
- **Statistic Collection**: Generates statistics for you, showing how long you took to read each word.
- **Colorize Statistics**: Makes the statistics colorful (optional), allowing you to quickly see how long you took per word and identify which words were very slow or fast.


 ## Screenshots

**Version 4(Light theme Version 6) images**:

***Dark Theme***

![Bildschirmfoto vom 2024-07-05 21-01-55](https://github.com/Quantum-mutnauQ/Fast-Reader-GTK/assets/141065355/48d57ddf-fe5d-4209-aaed-1b30403e1816)
![Bildschirmfoto vom 2024-07-05 21-08-48](https://github.com/Quantum-mutnauQ/Fast-Reader-GTK/assets/141065355/cc7e675b-7642-43a6-a2a9-a5eecfc2e72d)

***Light Theme:***

![Bildschirmfoto vom 2024-12-29 13-24-42](https://github.com/user-attachments/assets/6de4cee5-b628-42c4-bea0-ede9130b102d)
![Bildschirmfoto vom 2024-12-29 13-25-03](https://github.com/user-attachments/assets/a8433539-2451-4863-a260-e844d4bc868e)

**Version 5(Light theme Version 6) images:**

***Dark Theme:***

![Bildschirmfoto vom 2024-07-11 15-30-05](https://github.com/Quantum-mutnauQ/Fast-Reader-GTK/assets/141065355/0e4b847a-79a7-496a-a9cc-8c63dbe68507)
![Bildschirmfoto vom 2024-07-11 15-30-10](https://github.com/Quantum-mutnauQ/Fast-Reader-GTK/assets/141065355/6131c3e0-7c8b-479e-95de-31b312c993b4)

***Light Theme:***

![Bildschirmfoto vom 2024-12-29 13-25-26](https://github.com/user-attachments/assets/6195e5e6-5c20-4446-a75b-6a44a4235514)
![Bildschirmfoto vom 2024-12-29 13-25-36](https://github.com/user-attachments/assets/044fc9d1-aa70-4128-80b3-fb7554f04750)

## Translations by:
- Italian:  [albanobattistella](https://github.com/albanobattistella)

## Getting Started

There are two installation methods available for you to choose from:

**1. Install via [Flatpak on Flathub](https://flathub.org/apps/io.github.quantum_mutnauq.fast_reader_gtk)**

**2. Run from the Binary**

If you prefer to run Fast Reader from the binary, just follow these simple steps:

1. **Installation**: Download the [latest release](https://github.com/Quantum-mutnauQ/Fast_Reader_GTK/releases) and extract the contents from the zip file.
2. **Execution**: Run the compiled "FastReader" executable. You can execute it by running `./FastReader` in the folder where the program is saved, or simply double-click on it.
3. **Troubleshooting** You need to install `libgtk-4-1` and `libconfig9`.

## Adding a New Language

1. **Create Files**: Within the "src/locales" folder, create a subfolder named with the short of your language. Inside this folder, create another folder named `LC_MESSAGES`. Then, within this folder, create a file named `FastReader.po`.
2. **Add Translations to File**: Copy the provided template and modify the English phrases to their equivalents in your language in the speedreader.po file.
```po
msgid ""
msgstr ""
"Content-Type: text/plain; charset=UTF-8\n"

msgid "Error while writing file.\n"
msgstr "Error while writing the file.\n"

msgid "Error while reading file: %s:%d - %s\n"
msgstr "Error while reading the file: %s:%d - %s\n"

msgid "Trye to corrct\n"
msgstr "Try to correct it\n"

msgid "Statistiken:"
msgstr "Statistics:"

msgid "Lese Zeit Ergebnisse"
msgstr "Reading Time Results"

msgid "Gesamtzeit: "
msgstr "Total Time: "

msgid "Farbe:"
msgstr "Color:"

msgid "Wort"
msgstr "Word"

msgid "Zeit (s)"
msgstr "Time (s)"

msgid "Prozentuale Zeit (%)"
msgstr "Percentage Time (%)"

msgid "Punkte"
msgstr "Points"

msgid "Fehler beim Abrufen der Farben.\n"
msgstr "Error while retrieving the colors.\n"

msgid "Möchten Sie zur letzten Stelle springen?"
msgstr "Would you like to jump to the last position?"

msgid "Ja"
msgstr "yes"

msgid "Nein"
msgstr "no"

msgid "Fehler beim Abrufen des Textes.\n"
msgstr "Error while retrieving the text.\n"

msgid "Fehler beim Abrufen des Textpuffers.\n"
msgstr "Error while retrieving the text buffer.\n"

msgid ""
"Fast Reader helps you read quickly by displaying only one word at a time"
msgstr "Fast Reader helps you read quickly by displaying only one word at a time"

msgid "Fehler beim Erstellen der Aktion.\n"
msgstr "Error while creating the action.\n"

msgid "Schalter"
msgstr "Switch"

msgid "Farbe"
msgstr "Color"

msgid "Hintergrund:"
msgstr "Background:"

msgid "Schrift:"
msgstr "Font:"

msgid "Schrift Größe:"
msgstr "Font Size:"

msgid "Fortschritt Zeigen:"
msgstr "Show Progress:"

msgid "Angezeigte wörter:"
msgstr "Displayed words:"

msgid "Zeitbassirt nechstes Wort:"
msgstr "Time-based next word:"

msgid "Zeit (sec):"
msgstr "Time (sec):"

msgid "Längere Zeit Bei sehrlangen Wörtern:"
msgstr "Longer time for very long words:"

msgid "→ Wortlänge in Bustaben:"
msgstr "→ Word length in characters:"

msgid "→ + Zeit: "
msgstr "→ + Time: "

msgid "Exterzeit für erstes Wort:"
msgstr "Extra time for the first word:"

msgid "→ dazu addierte Zeit(Sekunden):"
msgstr "→ Time added (seconds):"

msgid "Statistiken erheben:"
msgstr "Collect statistics:"

msgid "Text Box"
msgstr "Text Box"

msgid "Zurücksetzen"
msgstr "Reset"

msgid "Schließen"
msgstr "Close"

msgid "Datei"
msgstr "File"

msgid "Vollbild"
msgstr "Fullscreen"

msgid "Ansicht"
msgstr "View"

msgid "Über"
msgstr "About"

msgid "Hilfe"
msgstr "Help"

msgid "Einstellungen:"
msgstr "Settings:"

msgid "Erweiterte Einstellugen für Zeitbassiertes nächstes Wort"
msgstr "Advanced settings for time-based next word"

msgid ""
"Hier kann die gewünschte zusätzliche Zeit angegeben werden, die das Wort "
"länger bleiben soll (in Sekunden)"
msgstr "Here you can specify the desired additional time for which the"
" word should remain longer (in seconds)"


msgid ""
"Wartezeit, bis das Lese Programm startet. Ermöglicht so, dass das erste "
"kurze Worte nicht zu schnell verschwinden."
msgstr "Waiting time before the reading program starts."
"This allows the first short words not to disappear too quickly."

msgid ""
"Die Zeit wird berechnet durch:\n"
"Zeit−Pro−Wort×(₃√Wort−Länge )"
msgstr ""
"The time will be calculated by:\n"
"Time-Per-Word×(₃√Word-Length)"

msgid "Lesen"
msgstr "Read"

msgid "Copy"
msgstr "Copy"

msgid "Paste"
msgstr "Paste"

msgid "Clear"
msgstr "Clear"

msgid "Undo"
msgstr "Undo"

msgid "Ergebnissezeigen"
msgstr "Show Results"

msgid "Zurück"
msgstr "Back"

msgid "Icon mit standart Namen gefunden.\n"
msgstr "Icon found with standard name.\n"

msgid "Icon nicht gefunden.\n"
msgstr "Icon not found.\n"

msgid "ICON_NAME ist definiert: %s\n"
msgstr "ICON_NAME is defined: %s\n"

msgid "Schema nicht gefunden!\n"
msgstr "Schema not found!\n"

msgid "Fehler beim Erstellen des Fensters.\n"
msgstr "Error while creating the window.\n"

msgid "Fast Reader"
msgstr "Fast Reader"

msgid "Konnte das Standard-Konfigurationsverzeichnis nicht abrufen.\n"
msgstr "Could not retrieve default configuration directory.\n"

msgid "Fehler beim Erstellen des Verzeichnisses FastReader.\n"
msgstr "Error while creating FastReader directory.\n"

msgid "Fehler beim Erstellen der Anwendung.\n"
msgstr "Error while creating the application.\n"
 ```
3. **Compile**: Use the `msgfmt` command to compile the language file. For example: `msgfmt src/locale/<YourLanguage>/LC_MESSAGES/FastReader.po -o build/locale/<YourLanguage>/LC_MESSAGES/FastReader.mo`.
4. **Testing**: Launch Fast Reader and verify that the new language is working correctly. If any issues arise, please open an issue and provide your `FastReader.po` file and language details.
5. **Support the Project (Optional)**: Open an issue and share your `FastReader.po` file along with your language details, and we will add it to the project.
6. **Add Language Support for CMake (Optional)**: To include support for an additional language, add the following line under the existing `compile_po_files(en)` entry in your CMake configuration: `compile_po_files([your_language_short_name])` After making this change, reconfigure CMake to apply the updates.

## Compiling It Yourself
### Build Instructions

1. **Download the Code**: Begin by downloading the latest version of the code.
2. **Preparation**: Create a build directory within the project folder.
3. **Create Build Program**: Navigate to the build directory and run the following command: `cmake ..`
4. **Run the Build**: In the build directory, execute the following command to compile the code:
       `make -j[thread count]` Replace `[thread count]` with the number of threads you wish to utilize for the build process.


Feel free to let me know if you need any further modifications!
## Plans for the future 
- [ ] Optimize RAM usage and clean up code (Version 8)  
- [ ] Add icon (Version ?)
