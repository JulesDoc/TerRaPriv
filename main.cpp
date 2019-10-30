#include <QApplication>

#include "T_Logger.hpp"
#include "T_DBTarget.hpp"
#include "T_Database.hpp"
#include "T_MainWindow.hpp"
#include "T_Configuration.hpp"

int main(int argc, char *argv[]){

	QString fileName; // File name where all logged messages would be generated
	QString sqlFileName; // File name where sql would be generated
	bool bGenerateDebugMessages = false;
	T_Logger::installMessageHandler(fileName, sqlFileName, bGenerateDebugMessages);

	QApplication app(argc, argv);

	T_DBName aDBName(T_DBName::TRS_DB);
	T_DBTarget aDBTarget(T_DBTarget::DEVL);
	T_Database db(T_DBName::TRS_DB, T_DBTarget::DEVL);

	T_String text; text << db.getConnectionInfo();
	T_MainWindow mainWindow(db);
	mainWindow.setWindowTitle(QObject::tr("Connected to %1").arg(text));
	mainWindow.showMaximized();

	return app.exec();

}