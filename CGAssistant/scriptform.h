#ifndef SCRIPTFORM_H
#define SCRIPTFORM_H

#include <QWidget>
#include <QProcess>
#include <QTime>
#include <memory>

// 前向声明 - 新架构
namespace cga {
    namespace application {
        class ServiceFactory;
    }
}

namespace Ui {
class ScriptForm;
}
class QTextEdit;
class QPlainTextEdit;

class ScriptForm : public QWidget
{
    Q_OBJECT
public:
    explicit ScriptForm(QWidget *parent = 0);
    ~ScriptForm();
    
    /**
     * @brief 使用新架构初始化
     */
    void InitializeWithServices(std::shared_ptr<cga::application::ServiceFactory> serviceFactory);
    
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    void UpdateGameTextUI(bool show);
signals:
    void ReportNavigatorPath(QString json);
    void ReportNavigatorFinish(int exitCode);

public slots:
    void OnNotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd);
    void on_pushButton_load_clicked();
    void on_pushButton_run_clicked();
    void on_pushButton_debug_clicked();
    void on_pushButton_term_clicked();
    void OnNotifyFillLoadScript(QString path, int autorestart, bool freezestop, bool injuryprot, bool soulprot, int consolemaxlines, int scriptfreezeduration);
    void RunNavigatorScript(int x, int y, int enter, QString *result);
    void StopNavigatorScript();
    void OnHttpLoadScript(QString query, QByteArray postdata, QJsonDocument* doc);
private slots:
    void OnNodeStarted();
    void OnNodeStartError(QProcess::ProcessError error);
    void OnNodeReadyRead();
    void OnNodeFinish(int exitCode, QProcess::ExitStatus exitStatus);
    void OnCloseWindow();
    void OnTimer();
    void on_pushButton_suspend_clicked();
    void OnSetFreezeDuration(int value);

private:
    Ui::ScriptForm *ui;
    QPlainTextEdit *m_output;
    
    // 新架构
    std::shared_ptr<cga::application::ServiceFactory> m_serviceFactory;
    
    bool m_bDebugging;
    bool m_bListening;
    bool m_bNavigating;
    bool m_bPathBegin;
    bool m_bSuspending;
    int m_port;
    int m_ConsoleMaxLines;
    QProcess *m_node;
    QTime m_LastMapChange;
    QString m_scriptPath;
};

#endif // SCRIPTFORM_H
