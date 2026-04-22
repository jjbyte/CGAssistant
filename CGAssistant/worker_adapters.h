#pragma once

/**
 * CGAssistant Worker 适配器
 * 
 * 在迁移期间提供向后兼容性
 * 让现有 UI 代码可以继续使用 Worker 接口
 * 但底层已经使用新的服务架构
 */

#include <QObject>
#include <QSharedPointer>
#include <memory>
#include "../domain/entities.h"
#include "../domain/services.h"
#include "../application/service_factory.h"

// 前向声明
namespace CGA {
    class CGAInterface;
}

namespace cga {

/**
 * @brief 玩家 Worker 适配器
 * 
 * 适配旧的 CPlayerWorker 接口到新架构
 */
class PlayerWorkerAdapter : public QObject {
    Q_OBJECT
    
public:
    explicit PlayerWorkerAdapter(CGA::CGAInterface* cga, QObject* parent = nullptr);
    
signals:
    // 保持与旧 CPlayerWorker 兼容的信号
    void NotifyGetPlayerInfo(QSharedPointer<struct CGA_PlayerInfo_t> player);
    void NotifyGetPetsInfo(QSharedPointer<struct CGA_PetList_t> pets);
    void NotifyGetItemsInfo(QSharedPointer<struct CGA_ItemList_t> items);
    void NotifyGetSkillsInfo(QSharedPointer<struct CGA_SkillList_t> skills);
    void NotifyGetInfoFailed(bool bIsConnected, bool bIsInGame);
    
public slots:
    // 保持与旧 CPlayerWorker 兼容的槽
    void OnQueueGetPlayerInfo();
    void OnQueueGetItemInfo();
    void OnNotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd);
    
private:
    CGA::CGAInterface* m_cga;
    std::shared_ptr<application::ServiceFactory> m_serviceFactory;
    bool m_connected;
};

/**
 * @brief 战斗 Worker 适配器
 */
class BattleWorkerAdapter : public QObject {
    Q_OBJECT
    
public:
    explicit BattleWorkerAdapter(CGA::CGAInterface* cga, QObject* parent = nullptr);
    
signals:
    void NotifyBattleAction(int flags);
    
public slots:
    void OnSetAutoBattle(int state);
    void OnNotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd);
    
private:
    CGA::CGAInterface* m_cga;
    std::shared_ptr<application::ServiceFactory> m_serviceFactory;
    bool m_autoBattle;
};

/**
 * @brief 进程 Worker 适配器
 */
class ProcessWorkerAdapter : public QObject {
    Q_OBJECT
    
public:
    explicit ProcessWorkerAdapter(CGA::CGAInterface* cga, QObject* parent = nullptr);
    
signals:
    void NotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd);
    void NotifyQueryProcess(QList<QSharedPointer<struct CProcessItem>> list);
    
public slots:
    void OnQueueQueryProcess();
    void OnNotifyFillStaticSettings(int freezetime, int chatmaxlines);
    
private:
    CGA::CGAInterface* m_cga;
    int m_freezeTime;
};

} // namespace cga
