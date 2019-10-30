
#include <QList>
#include <QCompleter>
#include <QListWidget>
#include <QPushButton>
#include <T_DoubleListDlg.hpp>

#include "T_TaskTreeWidget.hpp"
#include "ui_taskTreeWidget.h"
#include "T_PrivDBTransaction.hpp"


T_TaskTreeWidget::T_TaskTreeWidget(QWidget *parent)
	:QWidget(parent)
{
	initializeGUI();
}

T_TaskTreeWidget::~T_TaskTreeWidget()
{
	delete m_ui;
}

void T_TaskTreeWidget::initializeGUI()
{
	m_ui = new Ui_taskTreeWidget;
	m_ui->setupUi(this);

	connect(m_ui->program_comboBox, SIGNAL(currentIndexChanged(QString)), SLOT(handleKeyProgramFilterChanged(QString)));
	connect(m_ui->task_filter_lineEdit, SIGNAL(textChanged(QString)), SLOT(handleKeyTaskFilterChanged(QString)));
	connect(m_ui->treeWidget, &QTreeWidget::itemClicked, this, &T_TaskTreeWidget::handleTaskProgramClicked);
	connect(m_ui->pushButton, &QPushButton::clicked, this, &T_TaskTreeWidget::clear);
}

void T_TaskTreeWidget::initialize(T_Database& db)
{
	m_db = &db;
	T_PrivDBTransaction dbTrans(database(), true);
	m_mapPrograms = dbTrans.deliverListOfTasks();

	m_ui->taskDescPlainTextEdit->clear();
	loadDataTasksTab();
	loadComboBox();
}

//Load data of tasks and programs in the Taks tab
void T_TaskTreeWidget::loadDataTasksTab()
{
	m_ui->treeWidget->clear();
	m_ui->treeWidget->setColumnCount(1);
	QSet<QString> set;
	foreach(T_TerRaProgram program, m_mapPrograms.keys())
	{
		QString programName = program.asDBString();
		if (set.contains(programName)) continue;
		set.insert(programName);

		QTreeWidgetItem *parentItem = new QTreeWidgetItem(m_ui->treeWidget);
		parentItem->setText(0, programName);
		foreach(T_PrivTask task, m_mapPrograms.values(program))
		{
			QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
			childItem->setText(0, task.getPrivTaskName().getTaskName());
		}
	}
	m_ui->treeWidget->sortItems(0, Qt::SortOrder(0));
	m_ui->treeWidget->expandAll();
}

void T_TaskTreeWidget::loadComboBox()
{
	QStringList list;
	foreach(T_TerRaProgram program, m_mapPrograms.keys())
	{
		QString programName = program.asDBString();
		list.append(programName);
	}
	list.removeDuplicates();
	m_ui->program_comboBox->clear();
	m_ui->program_comboBox->addItem("(All)");
	m_ui->program_comboBox->setInsertPolicy(QComboBox::InsertAtBottom);
	m_ui->program_comboBox->insertItems(1, list);
}

//Method to handle when the user clicks on a task or program.
void T_TaskTreeWidget::handleTaskProgramClicked(QTreeWidgetItem* item, int column)
{
	m_ui->taskDescPlainTextEdit->clear();
	if (item->childCount() == 0) { //It is a task clicked
		QString taskName = item->text(column);
		foreach(T_PrivTask task, m_mapPrograms.values())
		{
			if (task.getPrivTaskName().getTaskName() == taskName)
			{
				m_ui->taskDescPlainTextEdit->setPlainText(task.getTaskDescription());
				break;
			}
		}

	}
}

void T_TaskTreeWidget::showTaskInfo(QListWidgetItem* item)
{
	foreach(T_PrivTask task, m_mapPrograms.values())
	{
		if (task.getPrivTaskName().getTaskName() == item->text())
		{
			m_ui->taskDescPlainTextEdit->setPlainText(task.getTaskDescription());
			break;
		}
	}
}

void T_TaskTreeWidget::clear()
{
	m_ui->taskDescPlainTextEdit->clear();
	m_ui->task_filter_lineEdit->clear();
}

//Filter by program
void T_TaskTreeWidget::handleKeyProgramFilterChanged(QString arg1)
{
	if (arg1 == "(All)")
	{
		loadDataTasksTab();
		return;
	}

	m_ui->treeWidget->collapseAll();
	QRegExp regExp(arg1, Qt::CaseInsensitive);
	QStringList list;
	foreach(T_TerRaProgram program, m_mapPrograms.keys())
	{
		QString programName = program.asDBString();
		list.append(programName);
		list.removeDuplicates();
	}

	QCompleter* completer = new QCompleter(list, this);
	m_ui->program_comboBox->setCompleter(completer);

	m_ui->treeWidget->clear();
	m_ui->treeWidget->setColumnCount(1);
	QSet<QString> set;
	foreach(T_TerRaProgram program, m_mapPrograms.keys())
	{
		QString programName = program.asDBString();
		if (!programName.contains(regExp)) continue;
		if (set.contains(programName)) continue;
		set.insert(programName);

		QTreeWidgetItem *parentItem = new QTreeWidgetItem(m_ui->treeWidget);
		parentItem->setText(0, programName);
		foreach(T_PrivTask task, m_mapPrograms.values(program))
		{
			QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
			childItem->setText(0, task.getPrivTaskName().getTaskName());
		}
	}

}

//Filter by task
void T_TaskTreeWidget::handleKeyTaskFilterChanged(QString arg1)
{
	if (arg1 == "") {
		m_ui->treeWidget->collapseAll();
		loadDataTasksTab();
		int index = m_ui->program_comboBox->findText("(All)");
		m_ui->program_comboBox->setCurrentIndex(index);
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
	foreach(T_TerRaProgram program, m_mapPrograms.keys())
	{
		QString programName = program.asDBString();
		if (set.contains(programName)) continue;
		set.insert(programName);

		QTreeWidgetItem *parentItem = new QTreeWidgetItem(m_ui->treeWidget);
		parentItem->setText(0, programName);
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