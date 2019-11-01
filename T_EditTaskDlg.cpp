#include <QMessageBox>

#include "T_EditTaskDlg.hpp"
#include "ui_editTaskDlg.h"
#include "T_PrivDBTransaction.hpp"

T_EditTaskDlg::T_EditTaskDlg(QWidget* parent)
	:QDialog(parent)
{
	initializeGUI();
}

T_EditTaskDlg::~T_EditTaskDlg()
{
	delete m_ui;
}

void T_EditTaskDlg::initializeGUI()
{
	m_ui = new Ui_editTaskDlg;
	m_ui->setupUi(this);
}

void T_EditTaskDlg::initialize(T_Database& db)
{
	m_db = &db;
	T_PrivDBTransaction dbTrans(database(), true);
	m_completeMapPrograms = dbTrans.deliverListOfTasks();
}

void T_EditTaskDlg::open(const T_PrivTaskAlloc& rPrivTaskAlloc)
{
	m_changed = false;
	m_aPrivTaskAlloc = rPrivTaskAlloc;
	m_ui->doubleListWidget->clearAvailableItems();
	m_previousArg.clear();
	m_mapAllowedProgramList.clear();
	loadComboBox();
	connect(m_ui->comboBox, SIGNAL(currentIndexChanged(QString)), SLOT(loadTasksLists(QString)));
	m_ui->comboBox->setCurrentIndex(1);
	QDialog::open();
}

void T_EditTaskDlg::loadComboBox()
{
	m_ui->comboBox->clear();
	QStringList programList;
	foreach(T_TerRaProgram program, m_completeMapPrograms.keys())
	{
		QString programName = program.asBaseFileName();
		programList.append(programName);
	}
	programList.removeDuplicates();
	m_ui->comboBox->insertItems(0, programList);
}

void T_EditTaskDlg::loadTasksLists(QString arg)
{

	if (arg == "TerRaPriv") {
		m_ui->doubleListWidget->setReadOnly(true);
	}else m_ui->doubleListWidget->setReadOnly(false);

	QStringList availableTasks;
	QStringList allowedTasks;
	T_TerRaProgram program(arg);

	if (!m_mapAllowedProgramList.isEmpty() && (m_mapAllowedProgramList.value(m_previousArg) != m_ui->doubleListWidget->selectedItems())) {
		m_changed = true;
		m_mapAllowedProgramList.insert(m_previousArg, m_ui->doubleListWidget->selectedItems());
	}
	if (m_mapAllowedProgramList.contains(arg))
	{
		foreach(T_PrivTask task, m_completeMapPrograms.values(program))
		{
			availableTasks.append(task.getPrivTaskName().getTaskName());
		}
		m_ui->doubleListWidget->setAvailableItems(availableTasks);
		m_ui->doubleListWidget->setSelectedItems(m_mapAllowedProgramList.value(arg));
	}
	else
	{
		foreach(T_PrivTask task, m_completeMapPrograms.values(program))
		{
			availableTasks.append(task.getPrivTaskName().getTaskName());
			if ((m_aPrivTaskAlloc.getAllowedTasks().contains(task)))
				allowedTasks.append(task.getPrivTaskName().getTaskName());
		}
		m_ui->doubleListWidget->setAvailableItems(availableTasks);
		m_ui->doubleListWidget->setSelectedItems(allowedTasks);
	}
	m_previousArg = arg;
	m_mapAllowedProgramList.insert(arg, m_ui->doubleListWidget->selectedItems());
}

void T_EditTaskDlg::accept()
{
	if (m_mapAllowedProgramList.value(m_previousArg) != m_ui->doubleListWidget->selectedItems()) {
		m_changed = true;
		m_mapAllowedProgramList.insert(m_previousArg, m_ui->doubleListWidget->selectedItems());
	}

	if (!m_changed) QDialog::accept();

	else {
		const T_DateUpdatedRecycleCondition aRecycleCondition(m_aPrivTaskAlloc.getPrivUser().getUpdateBlock().getDateUpdated());
		m_mapAllowedProgramList.insert(m_previousArg, m_ui->doubleListWidget->selectedItems());
		T_PrivDBTransaction dbTrans(database(), true);
		QStringList oldListAllowed;
		QStringList newListAllowed;
		QList<QStringList> lists = m_mapAllowedProgramList.values();

		for (auto list : lists)
		{
			newListAllowed.append(list);
		}
		for (auto task : m_aPrivTaskAlloc.getAllowedTasks())
		{
			oldListAllowed.append(task.getPrivTaskName().getTaskName());
		}

		QVector<QString> tasksToDB;
		for (auto task : newListAllowed)
		{
			if (!oldListAllowed.contains(task))
			{
				tasksToDB.push_back(task);
			}
		}

		QVector<QString> tasksFromDB;
		foreach(QString programName, m_mapAllowedProgramList.keys())
		{
			foreach(QStringList list, m_mapAllowedProgramList.values(programName))
			{
				for (auto task : m_aPrivTaskAlloc.getAllowedTasks())
				{
					if (task.getTerraProgram().asBaseFileName() == programName && !list.contains(task.getPrivTaskName().getTaskName()))
					{
						tasksFromDB.push_back(task.getPrivTaskName().getTaskName());
					}
				}
			}
		}

		try {
			dbTrans.updateTasksUser(tasksToDB, tasksFromDB, m_aPrivTaskAlloc.getPrivUser().getUserName(), aRecycleCondition);
		}
		catch (T_DBLostUpdateException e) {
			QMessageBox::warning(this, "Lost update", e.what());
			return;
		}

		Q_EMIT refresh();
		QDialog::accept();
	}
}

void T_EditTaskDlg::reject()
{
	if (m_mapAllowedProgramList.value(m_previousArg) != m_ui->doubleListWidget->selectedItems()) {
		m_changed = true;
		m_mapAllowedProgramList.insert(m_previousArg, m_ui->doubleListWidget->selectedItems());
	}
	if (!m_changed) QDialog::reject();

	else {
		const int msg = QMessageBox::warning(this, "Changes will not be applied", "Do you want to continue?", QMessageBox::Yes | QMessageBox::No);
		if (msg == QMessageBox::Yes)
		{
			disconnect(m_ui->comboBox, 0, 0, SLOT(loadTasksLists(QString)));
			QDialog::reject();
		}
	}
}




