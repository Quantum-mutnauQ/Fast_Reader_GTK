# Fast Reader

Fast Reader is a versatile application designed to assist users in reading text swiftly and effectively. Developed using the GTK library in C, this program offers a customizable interface to enhance the reading experience.

## Features

- **Customizable Settings**: Tailor your reading experience by adjusting settings like background color, text color, font size, and display of progress indicators.
- **Text Input**: Easily input text into the application through a dedicated text entry field.
- **Word-by-Word Display**: The text is broken down into individual words, presented one at a time, facilitating focused reading.
- **Navigation Options**: Seamlessly navigate through the text using intuitive navigation buttons or convenient keyboard shortcuts.
- **Progress Indicators**: Keep track of your reading progress with helpful indicators displaying your current position in the text and overall progress.
- 
## Getting Started

To begin using Fast Reader, follow these simple steps:

1. **Installation**: Download the latest release and extract the contents from the zip file.
2. **Execution**: Run the compiled "SpeedReader" executable. You can execute it by running `./FastReader` in the folder where the program is saved, or simply double-click on it.

## Adding a New Language

1. **Create Files**: Within the "locales" folder, create a subfolder named with the short of your language. Inside this folder, create another folder named `LC_MESSAGES`. Then, within this folder, create a file named `speedreader.po`.
2. **Add Translations to File**: Copy the provided template and modify the English phrases to their equivalents in your language in the speedreader.po file.
```
msgid ""
msgstr ""
"Content-Type: text/plain; charset=UTF-8\n"

msgid "Fast Reader"
msgstr "Fast Reader"

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
msgstr "Displayed Words:"

msgid "Lesen"
msgstr "Read"

msgid "Zurück"
msgstr "Back"
 ```
3. **Compile**: Use the `msgfmt` command to compile the language file. For example: `msgfmt locale/<YourLanguage>/LC_MESSAGES/FastReader.po -o locale/<YourLanguage>/LC_MESSAGES/FastReader.mo`.
4. **Testing**: Launch Fast Reader and verify that the new language is working correctly. If any issues arise, please open an issue and provide your `FastReader.po` file and language details.
5. **Support the Project (Optional)**: Open an issue and share your `FastReader.po` file along with your language details, and we will add it to the project.

## Compiling It Yourself

1. **Download the Code**: Download the latest code.
2. **Compilation**: Compile it using the following command:
3. ```gcc -o FastReader FastReader.c `pkg-config --cflags --libs gtk4 libconfig` && msgfmt locale/de/LC_MESSAGES/FastReader.po -o locale/de/LC_MESSAGES/FastReader.mo && msgfmt locale/en/LC_MESSAGES/FastReader.po -o locale/en/LC_MESSAGES/FastReader.mo && ./FastReader```. If any issues arise, ensure you have `GCC`, `msgfmt`, `libconfig-dev`,and `libgtk-4-dev` installed.

## Plans for the future 
 1. Implement time-based word predictions.
 2. Include statistics like time per word.
 3. Save last settings with configuration
