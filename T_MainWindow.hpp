#pragma once

#include <QMainWindow>
#include <QWidget>

#include "T_DateUpdatedRecycleCondition.hpp"
#include "T_TaskTreeWidget.hpp"
#include "T_Database.hpp"

class Ui_MainWindow;

class T_MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	T_MainWindow(T_Database& rDatabase, QWidget *parent = nullptr);
	virtual ~T_MainWindow();

private:
	void initializeGUI();
	void loadListOfUsers();
	

public slots:
	void refreshMain();
	
private slots:
	void handleUserClicked(QListWidgetItem* item);
	void showAboutMessage();
	void close();

private:
	Ui_MainWindow *m_ui;
	T_Database& m_db;
	int m_currentRow;
	enum {
		TASK_ALLOC_ROLE = Qt::UserRole,
	};
};