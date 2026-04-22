#ifndef CHATFORM_H
#define CHATFORM_H

#include <QWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include "player.h"

// 前向声明 - 新架构
namespace cga {
    namespace application {
        class ServiceFactory;
    }
}

namespace Ui {
class ChatForm;
}

class ChatForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChatForm(QWidget *parent = nullptr);
    ~ChatForm();
    
    /**
     * @brief 使用新架构初始化
     */
    void InitializeWithServices(std::shared_ptr<cga::application::ServiceFactory> serviceFactory);

public slots:
    void OnNotifyFillChatSettings(int blockchatmsgs);
    void OnNotifyFillStaticSettings(int freezetime, int chatmaxlines);
    void OnNotifyGetPlayerInfo(QSharedPointer<CGA_PlayerInfo_t> player);
    void OnNotifyChatMsg(int unitid, QString msg, int size, int color);
    bool ParseChatSettings(const QJsonValue &val);
    void SaveChatSettings(QJsonObject &obj);

private slots:
    void on_lineEdit_returnPressed();
    void on_checkBox_BlockAllChatMsgs_stateChanged(int state);
    void OnTimer();
    
private:
    Ui::ChatForm *ui;
    
    QSharedPointer<CGA_PlayerInfo_t> m_player;
    int m_ChatMaxLines;
    
    // 新架构
    std::shared_ptr<cga::application::ServiceFactory> m_serviceFactory;
    
    // 新架构方法
    void SendMessageNew(const QString& message);
};

#endif // CHATFORM_H
