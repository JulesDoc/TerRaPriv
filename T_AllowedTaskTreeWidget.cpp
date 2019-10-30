
#include <QList>
#include <QCompleter>
#include <QListWidget>
#include <QPushButton>
#include <QMessageBox>

#include "T_AllowedTaskTreeWidget.hpp"
#include "ui_allowedTaskTreeWidget.h"
#include "T_PrivDBTransaction.hpp"


T_AllowedTaskTreeWidget::T_AllowedTaskTreeWidget(QWidget *parent)
	:QWidget(parent)
{
	initializeGUI();
}

T_AllowedTaskTreeWidget::~T_AllowedTaskTreeWidget()
{
	delete m_ui;
}

void T_AllowedTaskTreeWidget::initializeGUI()
{
	m_ui = new Ui_allowedTaskTreeWidget;
	m_ui->setupUi(this);
	m_editTaskDlg = new T_EditTaskDlg(this);

	connect(m_ui->task_filter_lineEdit, SIGNAL(textChanged(QString)), SLOT(handleKeyTaskFilterChanged(QString)));
	connect(m_ui->editTasksPushButton, SIGNAL(clicked()), this, SIGNAL(signalEditTasks()));
	connect(m_ui->treeWidget, &QTreeWidget::itemClicked, this, &T_AllowedTaskTreeWidget::handleTaskClicked);
	connect(m_ui->editTasksPushButton, &QPushButton::clicked, this, &T_AllowedTaskTreeWidget::handleEditTasksClicked);
	connect(m_editTaskDlg, SIGNAL(refresh()), this, SIGNAL(refreshMW()));
	connect(m_ui->expandCollapseButton, &QPushButton::clicked, this, &T_AllowedTaskTreeWidget::expandCollapseTree);
	connect(m_ui->pushButton, &QPushButton::clicked, this, &T_AllowedTaskTreeWidget::clear);
}

void T_AllowedTaskTreeWidget::initialize(T_Database& db)
{
	m_db = &db;
	m_editTaskDlg->initialize(database());
	m_superUser = isSuperUser();
	m_ui->taskDescPlainTextEdit->clear();
	m_ui->task_filter_lineEdit->clear();

	m_ui->program_comboBox->setEnabled(false);
	m_ui->task_filter_lineEdit->setEnabled(false);
	m_ui->editTasksPushButton->setEnabled(false);
	m_ui->expandCollapseButton->setEnabled(false);
}

void T_AllowedTaskTreeWidget::handleRefreshSignal()
{
	Q_EMIT refreshMW();
}

void T_AllowedTaskTreeWidget::handleUserClicked(QListWidgetItem* item)
{
	m_ui->program_comboBox->setEnabled(true);
	m_ui->task_filter_lineEdit->setEnabled(true);
	if (m_superUser) {
		m_ui->editTasksPushButton->setEnabled(true);
	}
	m_ui->expandCollapseButton->setEnabled(true);
	m_ui->taskDescPlainTextEdit->clear();

	m_aPrivTaskAlloc = item->data(TASK_ALLOC_ROLE).value<T_PrivTaskAlloc>();
	m_currentUserItem = item;
	loadAllowedTasks();
	loadComboBox();
	connect(m_ui->program_comboBox, SIGNAL(currentIndexChanged(QString)), SLOT(handleKeyProgramFilterChanged(QString)));
	m_ui->treeWidget->expandAll();
}

void T_AllowedTaskTreeWidget::loadAllowedTasks()
{
	disconnect(m_ui->program_comboBox, 0, 0, 0);
	m_ui->task_filter_lineEdit->clear();
	m_mapPrograms.clear();
	m_taskDescMap.clear();
	m_parentsMap.clear();
	m_ui->treeWidget->clear();
	m_ui->treeWidget->setColumnCount(1);

	for (auto element : m_aPrivTaskAlloc.getAllowedTasks())
	{
		QString parentName = element.getTerraProgram().asBaseFileName();
		if (m_parentsMap.contains(parentName))
		{
			QTreeWidgetItem *childItem = new QTreeWidgetItem(m_parentsMap.value(parentName));
			childItem->setText(0, element.getPrivTaskName().getTaskName());
			m_taskDescMap.insert(element.getPrivTaskName().getTaskName(), element.getTaskDescription());
			m_mapPrograms.insert(parentName, element);
			continue;
		}
		QTreeWidgetItem *parentItem = new QTreeWidgetItem(m_ui->treeWidget);
		parentItem->setText(0, parentName);
		QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
		childItem->setText(0, element.getPrivTaskName().getTaskName());
		m_taskDescMap.insert(element.getPrivTaskName().getTaskName(), element.getTaskDescription());
		m_mapPrograms.insert(parentName, element);
		m_parentsMap.insert(parentName, parentItem);
	}
	m_ui->treeWidget->sortItems(0, Qt::SortOrder(0));
}

void T_AllowedTaskTreeWidget::loadComboBox()
{
	QStringList list;
	foreach(QString programName, m_parentsMap.keys())
	{
		list.append(programName);
	}
	list.removeDuplicates();
	m_ui->program_comboBox->clear();
	m_ui->program_comboBox->addItem("(All)");
	m_ui->program_comboBox->setInsertPolicy(QComboBox::InsertAtBottom);
	m_ui->program_comboBox->insertItems(1, list);
}

//Method to handle when the user clicks on a task.
void T_AllowedTaskTreeWidget::handleTaskClicked(QTreeWidgetItem* item, int col)
{
	m_ui->taskDescPlainTextEdit->clear();
	QString taskName = item->text(col);
	m_ui->taskDescPlainTextEdit->setPlainText(QString(m_taskDescMap.value(taskName)));
}

void T_AllowedTaskTreeWidget::handleEditTasksClicked()
{
	m_ui->task_filter_lineEdit->clear();
	m_editTaskDlg->open(m_aPrivTaskAlloc);
}

void T_AllowedTaskTreeWidget::expandCollapseTree()
{
	m_ui->taskDescPlainTextEdit->clear();
	for (int i = 0; i < m_ui->treeWidget->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *item = m_ui->treeWidget->topLevelItem(i);
		if (m_ui->treeWidget->isItemExpanded(item))
		{
			m_ui->treeWidget->collapseAll();
			return;
		}
	}
	m_ui->treeWidget->expandAll();
}

void T_AllowedTaskTreeWidget::clear()
{
	m_ui->taskDescPlainTextEdit->clear();
	m_ui->task_filter_lineEdit->clear();
	m_ui->treeWidget->expandAll();
}

bool T_AllowedTaskTreeWidget::isSuperUser()
{
	QString userSystemName = T_Database::getSystemUserName();
	T_PrivDBTransaction dbTrans(database(), true);
	QVector<T_PrivUser> usersPrivSA = dbTrans.deliverListOfUserInTask("SuperUser");
	if (usersPrivSA.contains(T_PrivUser(userSystemName))) {
		return true;
	}
	return false;
}

//Filter by program
void T_AllowedTaskTreeWidget::handleKeyProgramFilterChanged(QString arg1)
{
	if (arg1 == "(All)")
	{
		loadAllowedTasks();
		loadComboBox();
		connect(m_ui->program_comboBox, SIGNAL(currentIndexChanged(QString)), SLOT(handleKeyProgramFilterChanged(QString)));
		m_ui->treeWidget->expandAll();
		return;
	}

	m_ui->treeWidget->collapseAll();
	QRegExp regExp(arg1, Qt::CaseInsensitive);
	QStringList list;
	foreach(QString program, m_mapPrograms.keys())
	{
		list.append(program);
		list.removeDuplicates();
	}

	QCompleter* completer = new QCompleter(list, this);
	m_ui->program_comboBox->setCompleter(completer);

	m_ui->treeWidget->clear();
	m_ui->treeWidget->setColumnCount(1);
	QSet<QString> set;
	foreach(QString program, m_mapPrograms.keys())
	{
		if (!program.contains(regExp)) continue;
		if (set.contains(program)) continue;
		set.insert(program);

		QTreeWidgetItem *parentItem = new QTreeWidgetItem(m_ui->treeWidget);
		parentItem->setText(0, program);
		foreach(T_PrivTask task, m_mapPrograms.values(program))
		{
			QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
			childItem->setText(0, task.getPrivTaskName().getTaskName());
		}
	}
}

//Filter by task
void T_AllowedTaskTreeWidget::handleKeyTaskFilterChanged(QString arg1)
{
	if (arg1 == "") {
		m_ui->treeWidget->collapseAll();
		loadAllowedTasks();
		int index = m_ui->program_comboBox->findText("(All)");
		m_ui->program_comboBox->setCurrentIndex(index);
		connect(m_ui->program_comboBox, SIGNAL(currentIndexChanged(QString)), SLOT(handleKeyProgramFilterChanged(QString)));
		return;
	}

	QRegExp regExp(arg1, Qt::CaseInsensitive);
	QStringList list;
	foreach(T_PrivTask task, m_mapPrograms.values())
	{
		QString taskName = task.getPrivTaskName().getTaskName();
		list.append(taskName);
		list.removeDuplicates();
	}

	QCompleter *completer = new QCompleter(list, this);
	m_ui->task_filter_lineEdit->setCompleter(completer);

	m_ui->treeWidget->clear();
	m_ui->treeWidget->setColumnCount(1);
	QSet<QString> set;
	foreach(QString program, m_mapPrograms.keys())
	{
		if (set.contains(program)) continue;
		set.insert(program);

		QTreeWidgetItem *parentItem = new QTreeWidgetItem(m_ui->treeWidget);
		parentItem->setText(0, program);
		foreach(T_PrivTask task, m_mapPrograms.values(program))
		{
			QString taskName = task.getPrivTaskName().getTaskName();
			if (!taskName.contains(regExp)) continue;
			QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
			childItem->setText(0, taskName);
			m_ui->treeWidget->expandItem(parentItem);
		}

	}

}
//END FILTERS