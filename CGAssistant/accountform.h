#ifndef ACCOUNTFORM_H
#define ACCOUNTFORM_H

#include <QWidget>
#include <QProcess>
#include <QTime>
#include <windows.h>
#include <memory>

// 前向声明 - 新架构
namespace cga {
    namespace application {
        class ServiceFactory;
    }
}

namespace Ui {
class AccountForm;
}

class AccountForm : public QWidget
{
    Q_OBJECT

public:
    explicit AccountForm(QWidget *parent = nullptr);
    ~AccountForm();
    
    /**
     * @brief 使用新架构初始化
     */
    void InitializeWithServices(std::shared_ptr<cga::application::ServiceFactory> serviceFactory);

    bool QueryAccount(QString &label, QString &errorMsg);
    bool IsGltExpired();
private slots:
    void OnPOLCNReadyReadStdOut();
    void OnPOLCNFinish(int exitCode, QProcess::ExitStatus exitStatus);
    void OnPOLCNStarted();
    void OnAutoLogin();
    void on_pushButton_getgid_clicked();    
    void on_pushButton_logingame_clicked();
    void on_checkBox_createChara_stateChanged(int arg1);
    void on_horizontalSlider_loginDuration_valueChanged(int value);

public slots:
    void OnNotifyFillAutoLogin(int game, QString user, QString pwd, QString gid,
                               int bigserver, int server, int character,
                               bool autologin, bool skipupdate, bool autochangeserver,bool autokillgame,int loginduration,
                               bool create_chara, int create_chara_chara, int create_chara_eye, int create_chara_mou, int create_chara_color,
                               QString create_chara_points, QString create_chara_elements, QString create_chara_name);
    void OnHttpLoadAccount(QString query, QByteArray postdata, QJsonDocument* doc);
    void OnNotifyConnectionState(int state, QString msg);
signals:
    void NotifyAutoAttachProcess(quint32 pid, quint32 tid);
    void NotifyKillProcess();
    void NotifyLoginProgressStart();
    void NotifyLoginProgressEnd();

private:
    Ui::AccountForm *ui;
    QProcess *m_POLCN;
    
    // 新架构
    std::shared_ptr<cga::application::ServiceFactory> m_serviceFactory;
    
    QString m_GLA;
    QString m_GLT;
    QString m_GID;
    int m_ServerIndex;
    int m_BigServerIndex;
    int m_GameType;
    bool m_bAutoLogin;
    bool m_bSkipUpdate;
    bool m_bAutoChangeServer;
    bool m_bAutoKillGame;
    int m_LoginDuration;
    bool m_bCreateChara;
    int m_CreateChara_Chara;
    int m_CreateChara_Eye;
    int m_CreateChara_Mouth;
    int m_CreateChara_Color;
    QString m_CreateChara_Points;
    QString m_CreateChara_Elements;
    QString m_CreateChara_Name;
    QTime m_LastLoginAttempt;
};

#endif // ACCOUNTFORM_H
