cmake_minimum_required(VERSION 3.10)

# Projektname und Version
project(FastReader VERSION 1.0)

# Setze den Standard-C++-Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


option(FLATPAK_BUILD "Enable Flatpak-specific build settings" OFF)
option(FLATPAK_ID "Define the Flatpak ID" "")

# Füge die Executable hinzu
if(FLATPAK_BUILD)
    set(EXECUTABLE_OUTPUT_PATH "/app/bin")
else()
    set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
endif()

# Füge die Executable hinzu
if(FLATPAK_BUILD)
    set(EXECUDABLE_NAME "fast-reader")
else()
    set(EXECUDABLE_NAME "FastReader")
endif()

add_executable(${EXECUDABLE_NAME} src/FastReader.cpp)

if(FLATPAK_BUILD AND NOT FLATPAK_ID)
    message(FATAL_ERROR "FLATPAK_ID must be set!")
endif()

if(FLATPAK_BUILD)
    add_compile_definitions(LOCALEDIRR="/app/share/locale")
else()
    add_compile_definitions(LOCALEDIRR="locale")
endif()

# Finde GTK4 und libconfig und füge deren Abhängigkeiten hinzu
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(LIBCONFIG REQUIRED libconfig)

target_include_directories(${EXECUDABLE_NAME} PRIVATE ${GTK4_INCLUDE_DIRS} ${LIBCONFIG_INCLUDE_DIRS})
target_link_directories(${EXECUDABLE_NAME} PRIVATE ${GTK4_LIBRARY_DIRS} ${LIBCONFIG_LIBRARY_DIRS})
target_link_libraries(${EXECUDABLE_NAME} PRIVATE ${GTK4_LIBRARIES} ${LIBCONFIG_LIBRARIES})

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

function(copy_metainfo)
    if(FLATPAK_BUILD)
        set(ASSETS_TARGET_DIR "/app/share/metainfo")
        set(SOURCE_METAINFO "${CMAKE_SOURCE_DIR}/src/assets/metainfo.xml")
        set(DESTINATION_METAINFO "${ASSETS_TARGET_DIR}/${FLATPAK_ID}.metainfo.xml")
    
        file(MAKE_DIRECTORY ${ASSETS_TARGET_DIR})

        # Kopiere die Metainformationen mit dem neuen Namen
        file(COPY ${SOURCE_METAINFO} DESTINATION ${ASSETS_TARGET_DIR} FILES ${DESTINATION_METAINFO})
        add_custom_target(metainfo ALL DEPENDS ${DESTINATION_METAINFO})
        install(FILES ${DESTINATION_METAINFO} DESTINATION ${ASSETS_TARGET_DIR})
    endif()
endfunction()

function(generate_desktop_file)
    set(DESKTOP_FILE_IN "${CMAKE_SOURCE_DIR}/src/assets/fastreader.desktop.in")


    if(FLATPAK_BUILD)
        set(DESKTOP_FILE_DIRR "/app/share/applications")
        set(DESKTOP_FILE_TEMP "${DESKTOP_FILE_DIRR}/tmp/${FLATPAK_ID}.desktop")
        set(EXEC_COMMAND "fast-reader")
        set(ICON_NAME "${FLATPAK_ID}")
    else()
        set(DESKTOP_FILE_DIRR "${CMAKE_BINARY_DIR}")
        set(DESKTOP_FILE_TEMP "${DESKTOP_FILE_DIRR}/tmp/fastreader.desktop")
        set(EXEC_COMMAND "\"${CMAKE_BINARY_DIR}/FastReader\"")
        set(ICON_NAME "${CMAKE_BINARY_DIR}/assets/fastreader.svg")
    endif()

    configure_file(${DESKTOP_FILE_IN} ${DESKTOP_FILE_TEMP} @ONLY)
    file(
        COPY ${DESKTOP_FILE_TEMP}
        DESTINATION ${DESKTOP_FILE_DIRR}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    )

    file(REMOVE_RECURSE "${DESKTOP_FILE_DIRR}/tmp")
    add_custom_target(desktopfile ALL DEPENDS ${DESKTOP_FILE_IN})
endfunction()

# Kompiliere die Übersetzungen für Deutsch und Englisch
compile_po_files(de)
compile_po_files(en)
copy_icon()
copy_metainfo()
generate_desktop_file()
