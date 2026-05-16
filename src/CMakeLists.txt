cmake_minimum_required(VERSION 4.2)
project(PacStation)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Strict conformance, no GNU/MSVC extensions
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Svg
        SvgWidgets
        LinguistTools
        REQUIRED)

set(SOURCES
        main.cpp
        debug.h
        mainwindow.h
        mainwindow.cpp
        sidebarbutton.h
        sidebarbutton.cpp
        thememanager.h
        thememanager.cpp
        iconutils.h
        iconutils.cpp
        startupchecker.h
        startupchecker.cpp
        # Widgets
        widgets/toggleswitch.h
        widgets/toggleswitch.cpp
        widgets/togglewithstatus.h
        widgets/settingrow.h
        widgets/settingrow.cpp
        widgets/sourceprioritylist.h
        widgets/sourceprioritylist.cpp
        widgets/elidalabel.h
        widgets/infobanner.h
        widgets/toastnotification.h
        widgets/numberspinner.h
        widgets/pickerbase.h
        # Top-level pages
        pages/homepage.h
        pages/homepage.cpp
        pages/installedpage.h
        pages/installedpage.cpp
        pages/updatespage.h
        pages/updatespage.cpp
        pages/downloadspage.h
        pages/downloadspage.cpp
        pages/settingspage.h
        pages/settingspage.cpp
        # Settings sub-pages
        pages/settings/sourcessettingspage.h
        pages/settings/sourcessettingspage.cpp
        pages/settings/repossettingspage.h
        pages/settings/repossettingspage.cpp
        pages/settings/mirrorssettingspage.h
        pages/settings/mirrorssettingspage.cpp
        pages/settings/updatessettingspage.h
        pages/settings/updatessettingspage.cpp
        pages/settings/searchsettingspage.h
        pages/settings/searchsettingspage.cpp
        pages/settings/appearancesettingspage.h
        pages/settings/appearancesettingspage.cpp
        pages/settings/securitysettingspage.h
        pages/settings/securitysettingspage.cpp
        resources.qrc
)

add_executable(pacstation ${SOURCES})
target_include_directories(pacstation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(pacstation
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt6::Svg
        Qt6::SvgWidgets
)

# ---------------------------------------------------------------------------
# Translations
# ---------------------------------------------------------------------------
# Add new languages by copying translations/pacstation_en.ts and naming it
# pacstation_<locale>.ts (e.g. pacstation_fr.ts, pacstation_de.ts).
# Run `cmake --build . --target pacstation_lupdate` to refresh .ts files
# from source, then edit with Qt Linguist.
qt_add_translations(pacstation
    TS_FILES
        translations/pacstation_en.ts
)

