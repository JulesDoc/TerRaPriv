#pragma once

#include "T_TerRaProgram.hpp"
#include "T_PrivTask.hpp"
#include "T_PrivTaskAlloc.hpp"
#include "T_DoubleListDlg.hpp"
#include "T_Database.hpp"

class Ui_editTaskDlg;

class T_EditTaskDlg : public QDialog
{
	Q_OBJECT

public:
	T_EditTaskDlg(QWidget* parent = nullptr);
	virtual ~T_EditTaskDlg();
	void initialize(T_Database& db);
	T_Database &database() const { return *m_db; }
	void open(const T_PrivTaskAlloc& rPrivTaskAlloc);
    void accept() override;
	void reject() override;
	
signals:
	void refresh();

public slots:
	void loadTasksLists(QString arg);

private:
	void initializeGUI();
	void loadComboBox();
	
private:
	Ui_editTaskDlg* m_ui{ nullptr };
	T_Database *m_db = nullptr;
	T_PrivTaskAlloc m_aPrivTaskAlloc;
	QMultiMap<T_TerRaProgram, T_PrivTask> m_completeMapPrograms;
	QMap<QString, QStringList> m_mapAllowedProgramList;
	QString m_previousArg;
	bool m_changed;
};
