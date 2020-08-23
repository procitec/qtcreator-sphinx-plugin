DEFINES += QTCREATORSPHINXPLUGIN_LIBRARY

## Either set the IDE_SOURCE_TREE when running qmake,
## or set the QTC_SOURCE environment variable, to override the default setting
isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = $$(QTC_SOURCE)

## Either set the IDE_BUILD_TREE when running qmake,
## or set the QTC_BUILD environment variable, to override the default setting
isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = $$(QTC_BUILD)

isEmpty(IDE_SOURCE_TREE):error(QTC_SOURCE must be set)
isEmpty(IDE_BUILD_TREE):error(QTC_BUILD must be set)

TEST=1


#QT+= webengine

# qtcreator-sphinx-plugin files

SOURCES += \
        editor/SphinxCodeModel.cpp \
        qtcreator-sphinx-plugin.cpp \
        qtcreator-sphinx-plugin-projectsettings.cpp \
        editor/SphinxOutputParser.cpp \
        editor/SphinxPreviewPage.cpp \
        editor/SphinxDocument.cpp \
        editor/SphinxEditor.cpp \
        editor/SphinxEditorFactory.cpp \
        editor/SphinxEditorWidget.cpp \
        editor/SphinxHighlighter.cpp \
        editor/SphinxFormatter.cpp \
        editor/SphinxFormatActions.cpp \
        editor/SphinxIndenter.cpp \
        editor/SphinxCodeStylePreferencesFactory.cpp \
        editor/SphinxCompletionAssist.cpp \
        editor/SphinxRstcheckHighlighter.cpp \
        options/SphinxOptionsPage.cpp \
        options/SphinxOptionsWidget.cpp \
        options/SphinxSettings.cpp

equals( TEST, 1) {
    SOURCES += \
            tests/tst_formatter.cpp \
            tests/tst_completion.cpp
}


HEADERS += \
    editor/SphinxCodeModel.h \
        qtcreator-sphinx-plugin.h \
        qtcreator-sphinx-plugin_global.h \
        qtcreator-sphinx-pluginconstants.h \
        qtcreator-sphinx-plugin-projectsettings.h \
        editor/SphinxOutputParser.h \
        editor/SphinxPreviewPage.h \
        editor/SphinxDocument.h \
        editor/SphinxEditor.h \
        editor/SphinxEditorFactory.h \
        editor/SphinxHighlighter.h \
        editor/SphinxEditorWidget.h \
        editor/SphinxFormatter.h \
        editor/SphinxFormatActions.h \
        editor/SphinxIndenter.h \
        editor/SphinxCodeStylePreferencesFactory.h \
        editor/SphinxCompletionAssist.h \
        editor/SphinxRstcheckHighlighter.h \
        options/SphinxOptionsPage.h \
        options/SphinxOptionsWidget.h \
        options/SphinxSettings.h \


equals( TEST, 1) {
    HEADERS += \
            tests/tst_formatter.h \
            tests/tst_completion.h
}


DISTFILES += \
        .github/workflow/build_qmake.yml \
        .github/workflow/README.md

RESOURCES += \
    qtcreator-sphinx-plugin.qrc


# Qt Creator linking

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%QtProjectqtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on OS X
# USE_USER_DESTDIR = yes

###### If the plugin can be depended upon by other plugins, this code needs to be outsourced to
###### <dirname>_dependencies.pri, where <dirname> is the name of the directory containing the
###### plugin's sources.

QTC_PLUGIN_NAME = Sphinx
QTC_LIB_DEPENDS += \


QTC_PLUGIN_DEPENDS += \
    coreplugin \
    texteditor \
    projectexplorer \

QTC_PLUGIN_RECOMMENDS += \
    # optional plugin dependencies. nothing here at this time

QT += xmlpatterns

###### End _dependencies.pri contents ######

include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)


snippets.path = /share/qtcreator/snippets
snippets.files += share/qtcreator/snippets/sphinx.xml \

rstcheck.path = /share/qtcreator/sphinx/rstcheck
rstcheck.files += share/qtcreator/sphinx/rstcheck/protocol_data_parser.py \
                share/qtcreator/sphinx/rstcheck/rest_check.py \

model.path = /share/qtcreator/sphinx/model
model.files += share/qtcreator/sphinx/model/*.xml \
                share/qtcreator/sphinx/model/*.xsd \

INSTALLS += snippets rstcheck model

OTHER_FILES = \
    share/qtcreator/snippets/sphinx.xml \
    share/qtcreator/sphinx/rstcheck/rest_check.py \
    share/qtcreator/sphinx/rstcheck/protocol_data_parser.py \
    share/qtcreator/sphinx/model/sphinx.xml \



