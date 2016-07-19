#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindlg.h"
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    LoginDlg *dlg = new LoginDlg();
    dlg->openDialog();
}

MainWindow::~MainWindow()
{
    delete ui;
}
