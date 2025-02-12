HEADERS       = mandelbrotwidget.h \
                renderthread.h
SOURCES       = main.cpp \
                mandelbrotwidget.cpp \
                renderthread.cpp

unix:!mac:LIBS += -lm

# install
target.path = $$[QT_INSTALL_EXAMPLES]/threads/mandelbrot
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS mandelbrot.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/threads/mandelbrot
INSTALLS += target sources
