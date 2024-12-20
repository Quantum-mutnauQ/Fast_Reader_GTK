# Fast Reader

Reader is a versatile application designed to assist users in reading text swiftly and effectively. Developed using the GTK library in C++, this program offers a customizable interface to enhance the reading experience.

**Version 4 images**:

![Bildschirmfoto vom 2024-07-05 21-01-55](https://github.com/Quantum-mutnauQ/Fast-Reader-GTK/assets/141065355/48d57ddf-fe5d-4209-aaed-1b30403e1816) ![Bildschirmfoto vom 2024-07-05 21-08-48](https://github.com/Quantum-mutnauQ/Fast-Reader-GTK/assets/141065355/cc7e675b-7642-43a6-a2a9-a5eecfc2e72d)

**Version 5 images:**

![Bildschirmfoto vom 2024-07-11 15-30-05](https://github.com/Quantum-mutnauQ/Fast-Reader-GTK/assets/141065355/0e4b847a-79a7-496a-a9cc-8c63dbe68507)![Bildschirmfoto vom 2024-07-11 15-30-10](https://github.com/Quantum-mutnauQ/Fast-Reader-GTK/assets/141065355/6131c3e0-7c8b-479e-95de-31b312c993b4)



## Features

- **Customizable Settings**: Tailor your reading experience by adjusting settings like background color, text color, font size, and display of progress indicators.
- **Text Input**: Easily input text into the application through a dedicated text entry field.
- **Word-by-Word Display**: The text is broken down into individual words, presented one at a time, facilitating focused reading.
- **Navigation Options**: Seamlessly navigate through the text using intuitive navigation buttons or convenient keyboard shortcuts.
- **Progress Indicators**: Keep track of your reading progress with helpful indicators displaying your current position in the text and overall progress.
- **Time-Based Word predictions**: You can set a variable time calculate by a formal for the display of each word. The formula for calculating the word display time is: "Time-Per-Word × (Cube Root of Word-Length)". This dynamic adjustment ensures that longer, more complex words are allotted more time, providing a natural and responsive reading experience.
- **Statistic Collection**: Generates statistics for you, showing how long you took to read each word.
- **Colorize Statistics**: Makes the statistics colorful (optional), allowing you to quickly see how long you took per word and identify which words were very slow or fast.

 You can set a variable time limit for the display of each word. The formula for calculating the word display time is: "Time-Per-Word × (Cube Root of Word-Length)" (Time−Per−Word×(₃√Word−Length )). This dynamic adjustment ensures that longer, more complex words are allotted more time, providing a natural and responsive reading experience.
  
## Getting Started

To begin using Fast Reader, follow these simple steps:

1. **Installation**: Download the latest release and extract the contents from the zip file.
2. **Execution**: Run the compiled "SpeedReader" executable. You can execute it by running `./FastReader` in the folder where the program is saved, or simply double-click on it.
3. **Troubleshooting** You need to install `libgtk-4-1` and `libconfig9`.

## Adding a New Language

1. **Create Files**: Within the "src/locales" folder, create a subfolder named with the short of your language. Inside this folder, create another folder named `LC_MESSAGES`. Then, within this folder, create a file named `FastReader.po`.
2. **Add Translations to File**: Copy the provided template and modify the English phrases to their equivalents in your language in the speedreader.po file.
```
msgid ""
msgstr ""
"Content-Type: text/plain; charset=UTF-8\n"

msgid "Error while writing file.\n"
msgstr "Error while writing file.\n"

msgid "Error while reading file: %s:%d - %s\n"
msgstr "Error while reading file: %s:%d - %s\n"

msgid "Trye to corrct"
msgstr "Try to correct"

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

msgid "Einstellungen:"
msgstr "Settings:"

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
msgstr "Time based next word:"

msgid "Zeit (sec):"
msgstr "Time (sec):"

msgid "Statistiken erheben:"
msgstr "Collect Statistics:"

msgid ""
"Die Zeit wird berechnet durch:\n"
"Zeit-Pro-Wort  * Wort-Länge * 0.5"
msgstr ""
"The time will be calculated by:\n"
"Time-Per-Word * Word-Length * 0.5"

msgid "Zurücksetzen"
msgstr "Reset"

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
msgstr "Show results"

msgid "Zurück"
msgstr "Back"

msgid "Konnte das Standard-Konfigurationsverzeichnis nicht abrufen.\n"
msgstr "Could not retrieve default configuration directory.\n"

msgid "Fehler beim Erstellen des Verzeichnisses FastReader.\n"
msgstr "Error while creating FastReader directory.\n"

msgid "Fast Reader"
msgstr "Fast Reader"
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
1. Optimize RAM usage and clean up code (Version 8)
2. Move to GTK 4.14.2 version (Version 7)
3. add flatpak support (Version 6)
