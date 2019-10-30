#pragma once

#include <QWidget>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QStringList>
#include <set>

#include "T_PrivTask.hpp"
#include "T_EditTaskDlg.hpp"
#include "T_PrivUser.hpp"
#include "T_TerRaProgram.hpp"
#include "T_Database.hpp"
#include "T_PrivTaskAlloc.hpp"

class Ui_allowedTaskTreeWidget;

class T_AllowedTaskTreeWidget : public QWidget
{
	Q_OBJECT

public:
	T_AllowedTaskTreeWidget(QWidget *parent = nullptr);
	virtual ~T_AllowedTaskTreeWidget();

	void initialize(T_Database& db);
	T_Database &database() const { return *m_db; }

signals:
	void signalEditTasks();
	void refreshMW();
	
private:
	void initializeGUI();
	void loadComboBox();
	void loadAllowedTasks();
	bool isSuperUser();
	
public slots:
	void handleUserClicked(QListWidgetItem* item);
	void clear();
	
private slots:
	void handleTaskClicked(QTreeWidgetItem* item, int col);
	void handleKeyProgramFilterChanged(QString arg);
	void handleKeyTaskFilterChanged(QString arg);
	void handleEditTasksClicked();
	void handleRefreshSignal();
	void expandCollapseTree();
	
private:

	Ui_allowedTaskTreeWidget *m_ui{ nullptr };
	T_Database *m_db = 0;

	QMap<QString, QString> m_taskDescMap;
	QMap<QString, QTreeWidgetItem*> m_parentsMap;
	QMultiMap<QString, T_PrivTask> m_mapPrograms{};

	T_PrivTaskAlloc m_aPrivTaskAlloc;
	QListWidgetItem* m_currentUserItem;

	QVector<T_PrivUser> m_usersPrivSA;
	bool m_superUser{ false };

	T_EditTaskDlg *m_editTaskDlg{ nullptr };
	
	enum {
		TASK_ALLOC_ROLE = Qt::UserRole,
	};
};