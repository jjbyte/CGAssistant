#include "chatform.h"
#include "application/service_factory.h"
#include "domain/entities.h"
#include "../CGALib/logger.h"
#include "ui_chatform.h"

#include <QTimer>
#include <QDateTime>

extern CGA::CGAInterface *g_CGAInterface;

ChatForm::ChatForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatForm),
    m_serviceFactory(nullptr)
{
    ui->setupUi(this);

    m_ChatMaxLines = 100;
    ui->textEdit_chat->document()->setMaximumBlockCount(m_ChatMaxLines);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(OnTimer()));
    timer->start(1000);
}

void ChatForm::InitializeWithServices(std::shared_ptr<cga::application::ServiceFactory> serviceFactory)
{
    m_serviceFactory = serviceFactory;
    LOG_INFO("ChatForm 已使用新架构初始化");
}

ChatForm::~ChatForm()
{
    delete ui;
}

void ChatForm::OnTimer()
{
    if(g_CGAInterface->IsConnected())
    {
        switch(ui->checkBox_BlockChatMsgs->checkState())
        {
        case Qt::CheckState::Checked:
            g_CGAInterface->SetBlockChatMsgs(2);
            break;
        case Qt::CheckState::PartiallyChecked:
            g_CGAInterface->SetBlockChatMsgs(1);
            break;
        case Qt::CheckState::Unchecked:
            g_CGAInterface->SetBlockChatMsgs(0);
            break;
        }
    }
}

void ChatForm::on_lineEdit_returnPressed()
{
    auto message = ui->lineEdit->text();
    
    // 支持新旧两种架构
    if (m_serviceFactory) {
        SendMessageNew(message);
    } else {
        // 旧架构
        auto saystring = message.toStdString();
        int ingame = 0;
        if(g_CGAInterface->IsInGame(ingame) && ingame){
            LOG_INFO("发送聊天消息：{}", saystring);
            g_CGAInterface->SayWords(saystring, 0, 3, 1);
        }
    }

    ui->lineEdit->setText("");
}

void ChatForm::OnNotifyGetPlayerInfo(QSharedPointer<CGA_PlayerInfo_t> player)
{
    if (m_serviceFactory) {
        // 新架构 - 从服务获取
        auto& playerService = m_serviceFactory->player();
        auto playerInfo = playerService.getPlayerInfo();
        if (playerInfo) {
            LOG_TRACE("聊天模块玩家信息更新 (新架构) - 名称：{}", playerInfo->name.toStdString());
        }
    } else {
        // 旧架构
        m_player = player;
        LOG_TRACE("聊天模块玩家信息更新 - 名称：{} 单位 ID:{}", 
                 player->name.toStdString(), player->unitid);
    }
}

void ChatForm::OnNotifyFillChatSettings(int blockchatmsgs)
{
    if(blockchatmsgs == 2)
    {
        ui->checkBox_BlockChatMsgs->setCheckState(Qt::CheckState::Checked);
    }
    else if(blockchatmsgs == 1)
    {
        ui->checkBox_BlockChatMsgs->setCheckState(Qt::CheckState::PartiallyChecked);
    }
    else if(blockchatmsgs == 0)
    {
        ui->checkBox_BlockChatMsgs->setCheckState(Qt::CheckState::Unchecked);
    }
}

void ChatForm::OnNotifyFillStaticSettings(int freezetime, int chatmaxlines)
{
    m_ChatMaxLines = chatmaxlines;
    ui->textEdit_chat->document()->setMaximumBlockCount(m_ChatMaxLines);
}

void ChatForm::OnNotifyChatMsg(int unitid, QString msg, int size, int color)
{
    if(m_player && !m_player->name.isEmpty())
    {
        int type = -1;
        QString name;
        if(unitid > 0){
            if(m_player->unitid == unitid)
            {
                type = 1;  // 自己
                name = m_player->name;
            }
            else
            {
                CGA::cga_map_units_t units;
                if(g_CGAInterface->GetMapUnits(units))
                {
                    for(const auto &u :units){
                        if(u.valid == 2 && ((u.flags & 256) || (u.flags & 4096)) && u.unit_id == unitid){
                            type = (u.flags & 256) ? 2 : 3;  // 2=玩家 3=宠物
                            name = QString::fromStdString(u.unit_name);
                            break;
                        }
                    }
                }
            }
        } else {
            type = 0;  // 系统消息
        }

        if(type == -1)
            return;
        
        LOG_DEBUG("收到聊天消息 - 类型：{} 发送者：{} 内容：{}", 
                 type, name.toStdString(), msg.toStdString());

        if(type >= 1 && type <= 3)
        {
            auto header = QString("%1: ").arg(name);
            if(msg.indexOf(header) == 0)
            {
                msg = msg.mid(header.length());
            }
            else {
                auto header2 = QString("[GP]%1: ").arg(name);
                if(msg.indexOf(header2) == 0)
                {
                    msg = msg.mid(header2.length());
                }
            }
        }

        QString line;

        msg = msg.replace("\\c", ",");
        line.append(QString("[%1] ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));

        if(type == 0)
        {
            line.append(QString("<font color=\"#FF0000\">%1</font><br>").arg(msg));
        }
        else if(type == 1)
        {
            line.append(QString("<font color=\"#0000FF\">%1: %2</font><br>").arg(name).arg(msg));
        }
        else
        {
            line.append(QString("%1: %2<br>").arg(name).arg(msg));
        }

        QTextCursor txtcur = ui->textEdit_chat->textCursor();

        /*if(m_ChatMaxLines > 0){
            if (ui->textEdit_chat->document()->lineCount() > m_ChatMaxLines)
            {
                txtcur.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
                txtcur.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
                txtcur.removeSelectedText();
            }
        }*/

        txtcur.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        txtcur.insertHtml(line);
        txtcur.insertBlock();
    }
}

bool ChatForm::ParseChatSettings(const QJsonValue &val)
{
    if(!val.isObject())
        return false;

    auto obj = val.toObject();

    if(obj.contains("blockchatmsgs"))
    {
        int val = obj.take("blockchatmsgs").toInt();
        if(val == 2)
        {
            ui->checkBox_BlockChatMsgs->setCheckState(Qt::CheckState::Checked);
        }
        else if(val == 1)
        {
            ui->checkBox_BlockChatMsgs->setCheckState(Qt::CheckState::PartiallyChecked);
        }
        else if(val == 0)
        {
            ui->checkBox_BlockChatMsgs->setCheckState(Qt::CheckState::Unchecked);
        }
    }

    return true;
}

void ChatForm::SaveChatSettings(QJsonObject &obj)
{
    obj.insert("blockchatmsgs", (int)ui->checkBox_BlockChatMsgs->checkState());
}

void ChatForm::on_checkBox_BlockAllChatMsgs_stateChanged(int state)
{
    if(g_CGAInterface->IsConnected())
    {
        switch(state)
        {
        case Qt::CheckState::Checked:
            g_CGAInterface->SetBlockChatMsgs(2);
            break;
        case Qt::CheckState::PartiallyChecked:
            g_CGAInterface->SetBlockChatMsgs(1);
            break;
        case Qt::CheckState::Unchecked:
            g_CGAInterface->SetBlockChatMsgs(0);
            break;
        }
    }
}

// ============================================================================
// 新架构方法实现
// ============================================================================

void ChatForm::SendMessageNew(const QString& message)
{
    if (!m_serviceFactory) {
        return;
    }
    
    auto& chat = m_serviceFactory->chat();
    chat.sendMessage(message);
    
    LOG_INFO("发送聊天消息 (新架构): {}", message.toStdString());
}

