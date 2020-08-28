cmake_minimum_required(VERSION 3.10)

project(qtcreator-plugin-sphinx VERSION "0.2.0")

set( IDE_SOURCE_TREE $ENV{QTC_SOURCE} CACHE PATH "QTC Source Tree" )
set( IDE_BUILD_TREE  $ENV{QTC_BUILD} CACHE PATH "QTC Build Tree" )

if( "${IDE_SOURCE_TREE}" STREQUAL "" OR NOT EXISTS "${IDE_SOURCE_TREE}" )
    message( FATAL_ERROR "QTC_SOURCE must be configured" )
endif()

if( "${IDE_BUILD_TREE}" STREQUAL "" OR NOT EXISTS "${IDE_BUILD_TREE}")
    message( FATAL_ERROR "QTC_BUILD must be configured" )
endif()


set( QTC_PLUGIN_NAME Sphinx )
set( QTC_PLUGIN_REVISION ${PROJECT_VERSION})


## Add paths to check for cmake modules:
list(APPEND CMAKE_MODULE_PATH "${IDE_SOURCE_TREE}/lib/cmake/QtCreator")

# Force C++ standard, do not fall back, do not use compiler extensions
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(WITH_TESTS "Build Tests" ON)
option(WITH_DEBUG_CMAKE "Enabled CMake project debugging functionality (e.g. source file disk checking)" OFF)
option(BUILD_WITH_PCH "Build with precompiled headers" OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up Qt stuff:
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)



set(CMAKE_INSTALL_PREFIX "" )

include(QtCreatorConfig)


find_dependency(Qt5
  COMPONENTS Test XmlPatterns REQUIRED
)

add_feature_info("Build tests" ${WITH_TESTS} "")

if (WITH_TESTS)
  set(QT_TEST_COMPONENT Test)
  set(IMPLICIT_DEPENDS Qt5::Test)
endif()

add_qtc_plugin( Sphinx VERSION "0.2.0" PLUGIN_DEPENDS QtCreator::Core QtCreator::TextEditor QtCreator::ProjectExplorer DEPENDS Qt5::Core Qt5::Gui Qt5::Widgets Qt5::XmlPatterns)

qt5_add_resources( QRCS ${CMAKE_CURRENT_LIST_DIR}/qtcreator-sphinx-plugin.qrc )

# workaround QTCREATORBUG-24413
if( NOT EXISTS "${IDE_SOURCE_TREE}/include/src/libs/3rdparty/syntax-highlighting/autogenerated/src/lib/State" )
    message( WARNING "Fix for QTCREATORBUG-24413 applied")
    execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink "${IDE_SOURCE_TREE}/include/src/libs/3rdparty/syntax-highlighting/src/lib/state.h" "${IDE_SOURCE_TREE}/include/src/libs/3rdparty/syntax-highlighting/autogenerated/src/lib/State"
                     WORKING_DIRECTORY "${IDE_SOURCE_TREE}/include/src/libs/3rdparty/syntax-highlighting/autogenerated/src/lib"
    )
endif()

set( SOURCES
            editor/SphinxCodeModel.cpp
            qtcreator-sphinx-plugin.cpp
            qtcreator-sphinx-plugin-projectsettings.cpp
            editor/SphinxOutputParser.cpp
            editor/SphinxPreviewPage.cpp
            editor/SphinxDocument.cpp
            editor/SphinxEditor.cpp
            editor/SphinxEditorFactory.cpp
            editor/SphinxEditorWidget.cpp
            editor/SphinxHighlighter.cpp
            editor/SphinxFormatter.cpp
            editor/SphinxFormatActions.cpp
            editor/SphinxIndenter.cpp
            editor/SphinxCodeStylePreferencesFactory.cpp
            editor/SphinxCompletionAssist.cpp
            editor/SphinxRstcheckHighlighter.cpp
            options/SphinxOptionsPage.cpp
            options/SphinxOptionsWidget.cpp
            options/SphinxSettings.cpp
            editor/SphinxCodeModel.h
            qtcreator-sphinx-plugin.h
            qtcreator-sphinx-plugin_global.h
            qtcreator-sphinx-pluginconstants.h
            qtcreator-sphinx-plugin-projectsettings.h
            editor/SphinxOutputParser.h
            editor/SphinxPreviewPage.h
            editor/SphinxDocument.h
            editor/SphinxEditor.h
            editor/SphinxEditorFactory.h
            editor/SphinxHighlighter.h
            editor/SphinxEditorWidget.h
            editor/SphinxFormatter.h
            editor/SphinxFormatActions.h
            editor/SphinxIndenter.h
            editor/SphinxCodeStylePreferencesFactory.h
            editor/SphinxCompletionAssist.h
            editor/SphinxRstcheckHighlighter.h
            options/SphinxOptionsPage.h
            options/SphinxOptionsWidget.h
            options/SphinxSettings.h
            ${QRCS}
)


if( WITH_TESTS)
    list( APPEND SOURCES
            tests/tst_formatter.cpp
            tests/tst_completion.cpp
            tests/tst_formatter.h
            tests/tst_completion.h
    )
endif()

target_sources( Sphinx PRIVATE
    ${SOURCES}
)

INSTALL( DIRECTORY "share/qtcreator/sphinx" DESTINATION "share/qtcreator" )
INSTALL( FILES "share/qtcreator/snippets/sphinx.xml"  DESTINATION "share/qtcreator/snippets" )