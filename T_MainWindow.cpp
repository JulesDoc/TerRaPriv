#include <QMessageBox>
#include <QPlainTextEdit>

#include "TerRaWidgetsLib.hpp"
#include "T_Configuration.hpp"
#include "T_PrivDBTransaction.hpp"
#include "T_QtProgramAboutDlg.hpp"
#include "T_MainWindow.hpp"
#include "ui_mainWindow.h"

T_MainWindow::T_MainWindow(T_Database& rDatabase, QWidget *parent)
	:QMainWindow(parent), m_db(rDatabase)
{
	initializeGUI();
}

T_MainWindow::~T_MainWindow()
{
	delete m_ui;
}

void T_MainWindow::initializeGUI()
{
	m_ui = new Ui_MainWindow;
	m_ui->setupUi(this);
	loadListOfUsers();

	m_ui->taskTreeWidget->initialize(m_db);
	m_ui->allowedTaskTreeWidget->initialize(m_db);

	connect(m_ui->actionAbout_TerRaPriv, &QAction::triggered, this, &T_MainWindow::showAboutMessage);
	connect(m_ui->actionQuit, &QAction::triggered, this, &T_MainWindow::close);
	connect(m_ui->usersListWidget, &QListWidget::itemClicked, m_ui->allowedTaskTreeWidget, &T_AllowedTaskTreeWidget::handleUserClicked);
	connect(m_ui->usersListWidget, &QListWidget::itemClicked, this, &T_MainWindow::handleUserClicked);
	connect(m_ui->allowedTaskTreeWidget, SIGNAL(refreshMW()), SLOT(refreshMain()));
	
	statusBar()->showMessage(tr("System is Ready"));
}

void T_MainWindow::refreshMain()
{
	statusBar()->showMessage(tr("System is Busy"));
	loadListOfUsers();
	m_ui->allowedTaskTreeWidget->initialize(m_db);
	QListWidgetItem* item = m_ui->usersListWidget->item(m_currentRow);
	m_ui->usersListWidget->setItemSelected(item, true);
	Q_EMIT m_ui->usersListWidget->itemClicked(item);
	statusBar()->showMessage(tr("System is Ready"));
}

void T_MainWindow::loadListOfUsers()
{
	m_ui->usersListWidget->clear();
	T_PrivDBTransaction dbTrans(m_db, true);
	const QVector<T_PrivUser> vectorPrivUser = dbTrans.deliverListOfUsers();

	for (auto element : vectorPrivUser)
	{
		QListWidgetItem *item = new QListWidgetItem(element.getUserName());
		QVariant var;
		const T_PrivTaskAlloc aPrivTaskAlloc = dbTrans.deliverTaskAllocation(element.getUserName());
		//Stores a copy of value. QMetaType is used to store the value
		var.setValue(aPrivTaskAlloc);
		//Sets the data for a given role to the given value
		item->setData(TASK_ALLOC_ROLE, var);
		m_ui->usersListWidget->addItem(item);
	}
}


//Method to handle a click on the list of allowed users.
//It shows user info and a complete list of task allowed for given user
void T_MainWindow::handleUserClicked(QListWidgetItem* item)
{
	if (m_ui->usersListWidget->currentRow() >= 0) 
		m_currentRow = m_ui->usersListWidget->currentRow();
	m_ui->userInfoTextEdit->clear();
	
	const T_PrivTaskAlloc aPrivTaskAlloc = item->data(TASK_ALLOC_ROLE).value<T_PrivTaskAlloc>();
	const T_PrivUser &rcPrivUser = aPrivTaskAlloc.getPrivUser();

	//Filling user info
	m_ui->userInfoTextEdit->insertPlainText(QString("User first name: %1 \n").arg(rcPrivUser.getFirstName()));
	m_ui->userInfoTextEdit->insertPlainText(QString("User last name: %1 \n").arg(rcPrivUser.getLastName()));
}


void T_MainWindow::close()
{
	QApplication::quit();
	T_Configuration::saveSettings();
}

void T_MainWindow::showAboutMessage()
{
	const QString version = QLatin1Literal("1.0");
	T_QtProgramAboutDlg dlg(version);
	RCT_DatabaseConnectionInfo rcConnectionInfo = m_db.getConnectionInfo();
	dlg.showAboutMessage(rcConnectionInfo, this);
}


