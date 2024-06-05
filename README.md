# Speed Reader

Speed Reader is a simple yet powerful application designed to help users read text rapidly and efficiently. Built using the GTK library in C, this program offers a customizable interface for optimizing the reading experience.


## Features

- **Customizable Settings**: Users can configure various aspects of the reading experience, including background color, text color, font size, and whether to show progress indicators.
- **Text Input**: Users can input text into the application using a text entry field.
- **Word-by-Word Display**: The entered text is split into words, and each word is displayed individually, allowing users to focus on one word at a time.
- **Navigation Options**: Users can navigate through the text using navigation buttons (previous and next) or keyboard shortcuts.
- **Progress Indicators**: The application provides progress indicators to show the current position in the text and overall progress.
- **Internationalization Support**: Speed Reader supports internationalization, allowing for localization of user-facing strings in different languages.

## Getting Started

To use Speed Reader, follow these steps:

1. **Installation**: Downloda the latest relase and upack the zip file.
2. **RUN** run the Copiled "SpeedReader", you can execute ./SpeedReader in the folder where the programm is safen of you can dobble click it.

## Adding new laguage
1. **Ceate files** Ceate a folder in the locales folader named in the short form of you language and in this foalder creat an other foalder named ```LC_MESSAGES```. Than in this folder create a flie named ```speedreader.po```.
2.  **Add Translation to file** copy this template, and modify the Englisch names to the maes in your laguage.
```
msgid ""
msgstr ""
"Content-Type: text/plain; charset=UTF-8\n"

msgid "Speed Reader"
msgstr "Speed Reader"

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
4. **Compile** use the command msgfmt to compile the laguage.  ``` locale/<Your language>/LC_MESSAGES/speedreader.po -o locale/<Your language>/LC_MESSAGES/speedreader.m ```
5. **Testing** star up the Spead reader an look if it works. if it dosen pleas open a issuse and send you speedreader.po and you laguang an I will add it.
6. **Support the prgekt(Optional)** Open a issuse and schare you speedreader.po and you laguang an I will add it.

##Compile it your self
1. **Download the code** Downloat the leates code.
2. **Compile it** use this command to comile it: ``` gcc -o SpeedReader SpeedReader.c `pkg-config --cflags --libs gtk4` && msgfmt locale/de/LC_MESSAGES/speedreader.po -o locale/de/LC_MESSAGES/speedreader.mo && msgfmt locale/en/LC_MESSAGES/speedreader.po -o locale/en/LC_MESSAGES/speedreader.mo```. If it dosn ´t work you have to install gcc, msgfmt and libgtk-4-dev. 
   
