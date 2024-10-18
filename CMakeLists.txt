cmake_minimum_required(VERSION 3.10)

# Projektname und Version
project(FastReader VERSION 1.0)

# Setze den Standard-C++-Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Füge die Executable hinzu
add_executable(FastReader src/FastReader.cpp)

# Finde GTK4 und libconfig und füge deren Abhängigkeiten hinzu
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
find_package(LibConfig REQUIRED)

target_include_directories(FastReader PRIVATE ${GTK4_INCLUDE_DIRS} ${LIBCONFIG_INCLUDE_DIRS})
target_link_directories(FastReader PRIVATE ${GTK4_LIBRARY_DIRS} ${LIBCONFIG_LIBRARY_DIRS})
target_link_libraries(FastReader PRIVATE ${GTK4_LIBRARIES} ${LIBCONFIG_LIBRARIES})

# Nachrichtenkompilierung
find_program(MSGFMT_EXECUTABLE msgfmt REQUIRED)

# Übersetzungsverzeichnisse definieren
set(LOCALE_DIR "${CMAKE_BINARY_DIR}/locale")
set(LOCALE_DE_DIR "${LOCALE_DIR}/de/LC_MESSAGES")
set(LOCALE_EN_DIR "${LOCALE_DIR}/en/LC_MESSAGES")

# Erstelle Verzeichnisse für die Übersetzungen im Build-Verzeichnis
file(MAKE_DIRECTORY ${LOCALE_DE_DIR})
file(MAKE_DIRECTORY ${LOCALE_EN_DIR})

# Funktion zum Kompilieren der Übersetzungen
function(compile_po_files lang)
    set(PO_FILE "${CMAKE_SOURCE_DIR}/src//locale/${lang}/LC_MESSAGES/FastReader.po")
    set(MO_FILE "${LOCALE_DIR}/${lang}/LC_MESSAGES/FastReader.mo")

    # Füge die Kompilierung als benutzerdefinierten Befehl hinzu
    add_custom_command(
        OUTPUT ${MO_FILE}
        COMMAND ${MSGFMT_EXECUTABLE} ${PO_FILE} -o ${MO_FILE}
        DEPENDS ${PO_FILE}
        COMMENT "Compiling ${lang} translations"
        VERBATIM
    )

    # Füge die .mo-Dateien als Abhängigkeiten für das Ziel hinzu
    add_custom_target("${lang}_mo" ALL DEPENDS ${MO_FILE})
endfunction()

# Kompiliere die Übersetzungen für Deutsch und Englisch
compile_po_files(de)
compile_po_files(en)

# Installation der .mo-Dateien
install(FILES ${LOCALE_DE_DIR}/FastReader.mo
        DESTINATION locale/de/LC_MESSAGES)

install(FILES ${LOCALE_EN_DIR}/FastReader.mo
        DESTINATION locale/en/LC_MESSAGES)

