HEADERS       = mainwindow.h \
                xbeltree.h
SOURCES       = main.cpp \
                mainwindow.cpp \
                xbeltree.cpp
QT           += xml

# install
target.path = $$[QT_INSTALL_EXAMPLES]/xml/dombookmarks
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS dombookmarks.pro *.xbel
sources.path = $$[QT_INSTALL_EXAMPLES]/xml/dombookmarks
INSTALLS += target sources

wince*: {
     addFiles.sources = frank.xbel jennifer.xbel
     addFiles.path = \My Documents
     DEPLOYMENT += addFiles
}
