#pragma once

#include <QWidget>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QStringList>
#include <set>


#include "T_PrivTask.hpp"
#include "T_PrivUser.hpp"
#include "T_TerRaProgram.hpp"
#include "T_Database.hpp"

class Ui_taskTreeWidget;

class T_TaskTreeWidget : public QWidget
{
	Q_OBJECT

public:
	T_TaskTreeWidget(QWidget *parent = nullptr);
	virtual ~T_TaskTreeWidget();

	void initialize(T_Database& db);
	T_Database &database() const { return *m_db; }
	void loadDataTasksTab();

private:
	void initializeGUI();
	void loadListOfUsers();
	void loadComboBox();
	
public slots:
	void showTaskInfo(QListWidgetItem* item);
	void clear();
	
private slots:
	void handleTaskProgramClicked(QTreeWidgetItem* item, int col);
	void handleKeyProgramFilterChanged(QString arg);
	void handleKeyTaskFilterChanged(QString arg);

private:

	Ui_taskTreeWidget *m_ui{ nullptr };
	T_Database *m_db{ nullptr };

	QMultiMap<T_TerRaProgram, T_PrivTask> m_mapPrograms{};

	enum {
		TASK_ALLOC_ROLE = Qt::UserRole,
	};
};