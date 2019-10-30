
include(../terrasys.pri)

DEFINES += QT_NO_DEPRECATED_WARNINGS
QT += widgets sql xml
#testlib concurrent network

SOURCES += \
	T_MainWindow.cpp \
	T_Configuration.cpp \
	T_TaskTreeWidget.cpp \
	T_AllowedTaskTreeWidget.cpp \
	T_EditTaskDlg.cpp \
	main.cpp

HEADERS += \
	T_MainWindow.hpp \
	T_Configuration.hpp \
	T_TaskTreeWidget.hpp \
	T_AllowedTaskTreeWidget.hpp \
	T_EditTaskDlg.hpp
	
FORMS += \
	forms/mainWindow.ui \
	forms/taskTreeWidget.ui \
	forms/allowedTaskTreeWidget.ui \
	forms/editTaskDlg.ui
	
RESOURCES += \
	resources/resources.qrc