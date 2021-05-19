cmake_minimum_required(VERSION 3.10)

project(qtcreator-plugin-sphinx VERSION "5.0.0")

set( QTC_PLUGIN_NAME Sphinx )
set( QTC_PLUGIN_REVISION ${PROJECT_VERSION})

option(WITH_TESTS "Build Tests" ON)
option(WITH_DEBUG_CMAKE "Enabled CMake project debugging functionality (e.g. source file disk checking)" OFF)
option(BUILD_WITH_PCH "Build with precompiled headers" OFF)

# Force C++ standard, do not fall back, do not use compiler extensions
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up Qt stuff:
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

set( CMAKE_INSTALL_MESSAGE "LAZY" ) # one of NEVER, LAZY, ALWAYS

find_package(QtCreator COMPONENTS Core TextEditor ProjectExplorer REQUIRED)
find_package( Qt5 COMPONENTS Test XmlPatterns LinguistTools Widgets REQUIRED)
set(QtX Qt${QT_VERSION_MAJOR})

add_feature_info("Build tests" ${WITH_TESTS} "")

add_qtc_plugin( ${QTC_PLUGIN_NAME} VERSION ${qtcreator-plugin-sphinx_VERSION}
    PLUGIN_DEPENDS QtCreator::Core QtCreator::TextEditor QtCreator::ProjectExplorer
    DEPENDS  ${QtX}::Core ${QtX}::Gui ${QtX}::Widgets ${QtX}::XmlPatterns QtCreator::qlitehtml
    SOURCES
    editor/SphinxCodeModel.cpp
    qtcreator-sphinx-plugin.cpp
    qtcreator-sphinx-plugin-projectsettings.cpp
    editor/SphinxOutputParser.cpp
    editor/SphinxHtmlPage.cpp
    editor/SphinxPreviewPage.cpp
    editor/SphinxRightPaneWidget.cpp
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
    editor/SphinxRest2Html.cpp
    options/SphinxOptionsPage.cpp
    options/SphinxOptionsWidget.cpp
    options/SphinxSettings.cpp
    editor/SphinxCodeModel.h
    editor/SphinxWidgetHelpers.h
    editor/SphinxWidgetHelpers.cpp
    qtcreator-sphinx-plugin.h
    qtcreator-sphinx-plugin_global.h
    qtcreator-sphinx-pluginconstants.h
    qtcreator-sphinx-plugin-projectsettings.h
    editor/SphinxOutputParser.h
    editor/SphinxHtmlPage.h
    editor/SphinxPreviewPage.h
    editor/SphinxRightPaneWidget.h
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
    editor/SphinxRest2Html.h
    editor/SphinxRegularExpressions.h
    editor/SphinxRegularExpressions.cpp
    options/SphinxOptionsPage.h
    options/SphinxOptionsWidget.h
    options/SphinxSettings.h
    qtcreator-sphinx-plugin.qrc
)

add_translation_targets( ${QTC_PLUGIN_NAME} INSTALL_DESTINATION "${IDE_DATA_PATH}/translations" LANGUAGES "de"  TARGETS ${QTC_PLUGIN_NAME} )


extend_qtc_plugin( ${QTC_PLUGIN_NAME}
  CONDITION WITH_TESTS
  SOURCES
      tests/tst_formatter.cpp
      tests/tst_completion.cpp
      tests/tst_formatter.h
      tests/tst_completion.h
  DEPENDS  ${QtX}::Test
)

enable_testing()

INSTALL( DIRECTORY "share/qtcreator/sphinx" DESTINATION "${IDE_DATA_PATH}" PATTERN "__pycache__" EXCLUDE  )
INSTALL( FILES "share/qtcreator/snippets/sphinx.xml"  DESTINATION "${IDE_DATA_PATH}/snippets" )

string( REPLACE "${IDE_CMAKE_INSTALL_PATH}/${IDE_CASED_ID}" "" qtc_DIR "${QtCreator_DIR}" )
message("qt creator dir ${qtc_DIR}")

function( qtc_add_plugin_test plugin)
    string( REPLACE "${IDE_CMAKE_INSTALL_PATH}/${IDE_CASED_ID}" "" qtc_DIR "${QtCreator_DIR}" )

    add_test (NAME test_plugin_${plugin}_install
              COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${qtc_DIR} -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
    )

    set( qtc_app  "${qtc_DIR}/${IDE_APP_PATH}/${IDE_APP_TARGET}" )

    add_test( NAME test_plugin_${plugin} COMMAND ${qtc_app} -test ${plugin} )
    set_tests_properties( test_plugin_${plugin} PROPERTIES FIXTURES_REQUIRED test_plugin_${plugin}_install )
endfunction()

qtc_add_plugin_test( ${QTC_PLUGIN_NAME} )
