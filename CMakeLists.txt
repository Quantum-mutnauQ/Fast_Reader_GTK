cmake_minimum_required(VERSION 3.10)

# Projektname und Version
project(FastReader VERSION 1.0)

# Setze den Standard-C++-Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Füge die Executable hinzu
add_executable(FastReader src/FastReader.cpp)

option(FLATPAK_BUILD "Enable Flatpak-specific build settings" OFF)

if(FLATPAK_BUILD)
    add_compile_definitions(LOCALEDIRR="/app/share/locale")
else()
    add_compile_definitions(LOCALEDIRR="locale")
endif()

# Finde GTK4 und libconfig und füge deren Abhängigkeiten hinzu
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(LIBCONFIG REQUIRED libconfig)

target_include_directories(FastReader PRIVATE ${GTK4_INCLUDE_DIRS} ${LIBCONFIG_INCLUDE_DIRS})
target_link_directories(FastReader PRIVATE ${GTK4_LIBRARY_DIRS} ${LIBCONFIG_LIBRARY_DIRS})
target_link_libraries(FastReader PRIVATE ${GTK4_LIBRARIES} ${LIBCONFIG_LIBRARIES})

# Nachrichtenkompilierung
find_program(MSGFMT_EXECUTABLE msgfmt REQUIRED)

# Übersetzungsverzeichnisse definieren
if(FLATPAK_BUILD)
    set(LOCALE_DIR "/app/share/locale")
else()
    set(LOCALE_DIR "${CMAKE_BINARY_DIR}/locale")
endif()

# Funktion zum Kompilieren der Übersetzungen
function(compile_po_files lang)
    file(MAKE_DIRECTORY "${LOCALE_DIR}/${lang}/LC_MESSAGES")
    set(PO_FILE "${CMAKE_SOURCE_DIR}/src/locale/${lang}/LC_MESSAGES/FastReader.po")
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
    # Installation der .mo-Dateien
    install(FILES ${MO_FILE}
            DESTINATION "${LOCALE_DIR}/${lang}/LC_MESSAGES")
endfunction()

function(copy_icon)
    if(FLATPAK_BUILD)
        set(ASSETS_TARGET_DIR "/app/share/icons/hicolor/scalable/apps")
    else()
        set(ASSETS_TARGET_DIR "${CMAKE_BINARY_DIR}/assets")
    endif()
    
    file(MAKE_DIRECTORY ${ASSETS_TARGET_DIR})

    set(SOURCE_ICON "${CMAKE_SOURCE_DIR}/src/assets/fastreader.svg")
    
    file(COPY ${SOURCE_ICON} DESTINATION ${ASSETS_TARGET_DIR})
    add_custom_target(icon ALL DEPENDS ${SOURCE_ICON})
    install(FILES ${SOURCE_ICON} DESTINATION ${ASSETS_TARGET_DIR})
endfunction()

# Kompiliere die Übersetzungen für Deutsch und Englisch
compile_po_files(de)
compile_po_files(en)
copy_icon()
