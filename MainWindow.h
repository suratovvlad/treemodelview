#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include "TreeModel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_action_Load_model_from_file_triggered();

	void on_action_Exit_triggered();

private:
	Ui::MainWindow *ui;
	TreeModel* m_treeModel;
	QSortFilterProxyModel* m_proxyModel;
};

#endif // MAINWINDOW_H
