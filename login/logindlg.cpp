#include "logindlg.h"
#include <QDebug>

#if defined(Q_OS_WIN)

#else
#include <unistd.h>
#endif


LoginDlg::LoginDlg(QWidget* parent) :QDialog(parent)
{
    img = new QLabel();

    usrLineEdit = new QLineEdit(this);
    usrLineEdit->setGeometry(QRect(20, 95, 260, 30));//输入用户名
    pwdLineEdit = new QLineEdit(this);
    pwdLineEdit->setGeometry(QRect(20, 153, 260, 30));
    pwdLineEdit->setEchoMode(QLineEdit::Password);  //输入密码

    connectBtn = new QPushButton(this);
    connectBtn->setGeometry(QRect(20, 210, 100, 40));//登陆按钮

    remember = new QCheckBox(tr("REMEMBER ME"), this);
    remember->setChecked(true);
    remember->setGeometry(QRect(145, 212, 110, 30));    //记住密码

    QLabel *u_label = new QLabel(this);
    u_label->setGeometry(QRect(252, 103, 15, 15));
    QLabel *p_label = new QLabel(this);
    p_label->setGeometry(QRect(252, 161, 14, 14));


    connect(connectBtn, SIGNAL(clicked()), this, SLOT(accept()));

    (void) memset(accountInfoFile, '\0', sizeof (accountInfoFile));

    //读取帐户信息
#if defined(Q_OS_WIN)
    sprintf(accountInfoFile,"%s\\safejumper\\accont.xml",getenv("LOCALAPPDATA"));
#else
    sprintf(accountInfoFile,"%s/.safejumper/accont.xml",getenv("HOME"));
#endif
    qDebug() << "Path:" << accountInfoFile;
    // 从文件中读取帐户信息
    //FILE *accuntfile = NULL;
    // if remember
    usrLineEdit->setText("root");
    pwdLineEdit->setText("admin");

    icon = new QIcon(":/images/images/vc.ico");
    setWindowIcon(*icon);
    setWindowTitle(tr("Login Me"));
    resize(300,260);

    this->setStyleSheet("QDialog{border-image: url(:/images/images/login-bg.png);}"
                        "QPushButton{border-image: url(:/images/images/login-btn.jpg);}"
                        "QLineEdit{border-width:2px;}"
                        "QCheckBox::indicator{width: 15px; height: 15px;}"
                        "QCheckBox::indicator:unchecked{image: url(:/images/images/login-checkbox-unselected.jpg);}"
                        "QCheckBox::indicator:checked {image: url(:/images/images/login-checkbox-selected.jpg);}"
                        );


    u_label->setStyleSheet("border-image: url(:/images/images/login-u.jpg);");
    p_label->setStyleSheet("border-image: url(:/images/images/login-p.jpg);");

    loginStatus = false;
    printf("Finish Login dialog init\n");
}


LoginDlg::~LoginDlg()
{
    delete usrLineEdit;
    delete pwdLineEdit;
    delete username;
    delete passwd;
    delete connectBtn;
    delete remember;
    delete icon;
    delete watcher;
}

bool LoginDlg::getLoginStatus()
{
    return loginStatus;
}

void LoginDlg::openDialog()
{
    show();
    usrLineEdit->setFocus();

    if (loginStatus) {
        emit loginSuccess();
    }
}

//下载服务器上的配置信息文件
void downloadServerFile(QString fileString, QString linkString)
{
    QByteArray f = fileString.toLatin1();
    QByteArray l = linkString.toLatin1();

    char file[128] = {0};
    char link[256] = {0};
    strcpy(file, f.data());
    strcpy(link, l.data());

//      这可能会是一个阻塞的操作
//    (void) getServerListFile(file, link);
    printf("Test for download ,sleep for 10s\n");
#if defined(Q_OS_WIN)
    Sleep(1000*10);
#else
    sleep(10);
#endif

}



void LoginDlg::verifyAccountInfo(QString username, QString password)
{
    QString serverListFile;
    QString link;
    //认证结果保存为xml
#if defined(Q_OS_WIN)
    QString homeDir = QString(QLatin1String(getenv("LOCALAPPDATA")));
    serverListFile = homeDir + "\\safejumper\\server.xml";
#else
    QString homeDir = QString(QLatin1String(getenv("HOME")));
    serverListFile = homeDir + "/.safejumper/server.xml";
#endif

    link = "https://proxy.sh/access.php?u=" + username + "&p=" + password + "&hub=ture";

    watcher = new QFutureWatcher<void>();
    qDebug()<<serverListFile<<endl;
    qDebug()<<link<<endl;
    connect(watcher, SIGNAL(started()), this, SLOT(beginVerify()));
    connect(watcher, SIGNAL(finished()), this, SLOT(endVerify()));
    QFuture<void> future =
        QtConcurrent::run(downloadServerFile, serverListFile, link);
    watcher->setFuture(future);
}

void LoginDlg::accept()
{
    printf("Function: accept\n");
    verifyAccountInfo(usrLineEdit->text().trimmed(), pwdLineEdit->text());
}

void LoginDlg::beginVerify()
{
    printf("begin to verify the account\n");
    qDebug()<<"begin to verify"<<endl;
    usrLineEdit->setEnabled(0);
    pwdLineEdit->setEnabled(0);
    connectBtn->setEnabled(0);
    remember->setEnabled(0);
}

char * qstringToChar(const QString &s)
{
    QByteArray t = s.toLatin1();
    return (char *)(t.data());
}


void LoginDlg::endVerify()
{
    char serverListFile[255];
    char homeDir[64];
    char username[64] = {0};
    char passwd[64] = {0};
    (void) memset(serverListFile, '\0', sizeof(serverListFile));
    (void) memset(homeDir, '\0', sizeof(homeDir));

#if defined(Q_OS_WIN)
    (void) strcpy(homeDir, getenv("LOCALAPPDATA"));
    (void) sprintf(serverListFile, "%s\\safejumper\\server.xml", homeDir);
#else
    (void) strcpy(homeDir, getenv("HOME"));
    (void) sprintf(serverListFile, "%s/.safejumper/server.xml", homeDir);
#endif
    //检查返回认证结果
    //删除之前的用户信息，如果记住用户，则重新将新用户的信息保存
    if(0){
//    if (verifyAccount(serverListFile) == true) {
//        remove(accountInfoFile);
//        if (remember->isChecked()) {
//            if ((usrLineEdit->text().trimmed().length() != 0) && (pwdLineEdit->text().trimmed().length() != 0)) {
//                strcpy(username, qstringToChar(usrLineEdit->text().trimmed()));
//                strcpy(passwd, qstringToChar(pwdLineEdit->text().trimmed()));
//                saveAccountInfo(accountInfoFile, username, passwd, 1);
//            }
//        }

        // XXX for easy handle
        char passwdFile[255] = {0};
#if defined(Q_OS_WIN)
        sprintf(passwdFile, "%s\\safejumper\\passwd.txt", homeDir);
#else
        sprintf(passwdFile, "%s/.safejumper/passwd.txt", homeDir);
#endif
        //保存用户信息
        FILE *f = fopen(passwdFile, "wb");
        if (f != NULL) {
            fputs(qstringToChar(usrLineEdit->text().trimmed()), f);
            fputs("\n", f);
            fputs(qstringToChar(pwdLineEdit->text().trimmed()), f);
            fclose(f);
        }

        loginStatus = true;
        QDialog::accept();
        emit addLog("Success to login the vpn client\n");
        emit loginSuccess();
    } else {
        loginStatus = false;
        emit addLog("Fail to login the vpn client, username or password is not correct\n");
        emit showBalloon(2, "Proxy.sh vpn client", "Password verification Failed.");
        QMessageBox::warning( this, "Warning!" , "<font size = 5 color = black> Username or Password is not correct!</font> " );
        usrLineEdit->setFocus();
    }
    printf("end to verify the account\n");
    usrLineEdit->setEnabled(1);
    pwdLineEdit->setEnabled(1);
    connectBtn->setEnabled(1);
    remember->setEnabled(1);
}






