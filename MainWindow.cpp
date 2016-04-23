#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include "FileParser.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	QStringList headers;
	headers << tr("Tree") << tr("Value") << tr("Index");

	quint32 indexColumn = 2;

	m_treeModel = new TreeModel(headers, this);
	m_proxyModel = new QSortFilterProxyModel(this);

	m_proxyModel->setSourceModel(m_treeModel);
	m_proxyModel->sort(indexColumn, Qt::AscendingOrder);

	ui->treeView->setModel(m_proxyModel);
	ui->treeView->hideColumn(indexColumn);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_action_Load_model_from_file_triggered()
{
	QFileDialog dialog(this);
	dialog.setAcceptMode(QFileDialog::AcceptOpen);

	if (dialog.exec()) {

		QStringList fileNames = dialog.selectedFiles();

		FileParser parser(this);
		parser.setFilename(fileNames.at(0));

		FileParser::ParserErrors error = parser.parse(m_treeModel);

		if (error != FileParser::Successful) {
			QMessageBox::critical(this,
								  tr("Parse error"),
								  parser.errorString(error),
								  QMessageBox::Ok,
								  QMessageBox::NoButton);

		}
	}
}

void MainWindow::on_action_Exit_triggered()
{
	qApp->quit();
}
