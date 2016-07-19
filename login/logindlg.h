/*
 * Title:登陆界面
 * 载入图片的部分需要确定资源文件中有对应的文件
 *
 *
 */

#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QObject>
#include <QtWidgets>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>

class QLineEdit;
class QPushButton;
class QLabel;

class LoginDlg : public QDialog
{
    Q_OBJECT
public:
   // LoginDlg();
    LoginDlg(QWidget* = 0);

    virtual ~LoginDlg();

    bool getLoginStatus();                                                                      //获取登陆状态
    void verifyAccountInfo(QString username, QString password); //验证用户名密码
    void openDialog();                                                                            //打开登陆对话框

signals:
    void loginSuccess();        //登陆成功
    void addLog(const QString &l);      //
    void showBalloon(const int &type, const QString &t, const QString &m);  //气泡信息


public slots:
    virtual void accept();
    void beginVerify();
    void endVerify();

private:
    QLabel  *img;
    QLineEdit   *usrLineEdit;
    QLineEdit   *pwdLineEdit;
    QLabel  *username;
    QLabel  *passwd;
    QPushButton *connectBtn;
    QCheckBox   *remember;
    QIcon       *icon;
    bool        loginStatus;
    QFutureWatcher<void> *watcher;

    char accountInfoFile[255];
};

#endif // LOGINDLG_H
