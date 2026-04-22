#include "worker_adapters.h"
#include "../CGALib/gameinterface.h"
#include "../CGALib/logger.h"
#include <QTimer>

namespace cga {

// ============================================================================
// PlayerWorkerAdapter 实现
// ============================================================================

PlayerWorkerAdapter::PlayerWorkerAdapter(CGA::CGAInterface* cga, QObject* parent)
    : QObject(parent)
    , m_cga(cga)
    , m_connected(false)
{
    m_serviceFactory = application::ServiceFactory::Create(cga);
    
    // 设置定时器，模拟旧 Worker 的轮询行为
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &PlayerWorkerAdapter::OnQueueGetPlayerInfo);
    timer->start(1000);  // 每秒轮询
}

void PlayerWorkerAdapter::OnQueueGetPlayerInfo()
{
    if (!m_cga || !m_cga->IsConnected()) {
        if (m_connected) {
            m_connected = false;
            emit NotifyGetInfoFailed(false, false);
        }
        return;
    }
    
    m_connected = true;
    
    // 使用新的服务获取数据
    auto& playerService = m_serviceFactory->player();
    
    // 获取玩家信息
    auto player = playerService.getPlayerInfo();
    if (player) {
        // 转换为旧的数据格式
        auto playerInfo = QSharedPointer<CGA_PlayerInfo_t>::create();
        playerInfo->name = player->name;
        playerInfo->job = player->job;
        playerInfo->level = player->level;
        playerInfo->gold = player->gold;
        playerInfo->hp = player->hp;
        playerInfo->maxhp = player->maxHp;
        playerInfo->mp = player->mp;
        playerInfo->maxmp = player->maxMp;
        playerInfo->xp = player->xp;
        playerInfo->maxxp = player->maxXp;
        playerInfo->unitid = player->unitId;
        playerInfo->serverindex = player->serverIndex;
        
        emit NotifyGetPlayerInfo(playerInfo);
    }
    
    // 获取宠物信息
    auto pets = playerService.getPets();
    if (!pets.empty()) {
        auto petList = QSharedPointer<CGA_PetList_t>::create();
        for (const auto& pet : pets) {
            CGA_PetInfo_t petInfo;
            petInfo.id = pet.id;
            petInfo.name = pet.name;
            petInfo.realname = pet.realName;
            petInfo.level = pet.level;
            petInfo.hp = pet.hp;
            petInfo.maxhp = pet.maxHp;
            petInfo.mp = pet.mp;
            petInfo.maxmp = pet.maxMp;
            petInfo.flags = pet.flags;
            petInfo.battle_flags = pet.battleFlags;
            petInfo.loyality = pet.loyalty;
            petList->append(petInfo);
        }
        emit NotifyGetPetsInfo(petList);
    }
    
    // 获取物品信息
    auto items = playerService.getItems();
    if (!items.empty()) {
        auto itemList = QSharedPointer<CGA_ItemList_t>::create();
        for (const auto& item : items) {
            CGA_ItemInfo_t itemInfo;
            itemInfo.itemid = item.itemId;
            itemInfo.type = item.type;
            itemInfo.count = item.count;
            itemInfo.pos = item.pos;
            itemInfo.name = item.name;
            itemInfo.attr = item.attr;
            itemInfo.info = item.info;
            itemInfo.assessed = item.assessed;
            itemList->append(itemInfo);
        }
        emit NotifyGetItemsInfo(itemList);
    }
    
    // 获取技能信息
    auto skills = playerService.getSkills();
    if (!skills.empty()) {
        auto skillList = QSharedPointer<CGA_SkillList_t>::create();
        for (const auto& skill : skills) {
            CGA_SkillInfo_t skillInfo;
            skillInfo.id = skill.id;
            skillInfo.pos = skill.pos;
            skillInfo.lv = skill.level;
            skillInfo.maxlv = skill.maxLevel;
            skillInfo.name = skill.name;
            skillList->append(skillInfo);
        }
        emit NotifyGetSkillsInfo(skillList);
    }
}

void PlayerWorkerAdapter::OnQueueGetItemInfo()
{
    // 复用 OnQueueGetPlayerInfo 的物品获取逻辑
    OnQueueGetPlayerInfo();
}

void PlayerWorkerAdapter::OnNotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd)
{
    LOG_INFO("Worker 适配器已连接 - PID: {} 端口：{}", ProcessId, port);
    m_connected = true;
}

// ============================================================================
// BattleWorkerAdapter 实现
// ============================================================================

BattleWorkerAdapter::BattleWorkerAdapter(CGA::CGAInterface* cga, QObject* parent)
    : QObject(parent)
    , m_cga(cga)
    , m_autoBattle(false)
{
    m_serviceFactory = application::ServiceFactory::Create(cga);
}

void BattleWorkerAdapter::OnSetAutoBattle(int state)
{
    m_autoBattle = (state != 0);
    
    if (m_serviceFactory) {
        m_serviceFactory->battle().setAutoBattle(m_autoBattle);
    }
    
    LOG_INFO("自动战斗已{}", m_autoBattle ? "启用" : "禁用");
}

void BattleWorkerAdapter::OnNotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd)
{
    LOG_INFO("战斗 Worker 适配器已连接 - PID: {}", ProcessId);
}

// ============================================================================
// ProcessWorkerAdapter 实现
// ============================================================================

ProcessWorkerAdapter::ProcessWorkerAdapter(CGA::CGAInterface* cga, QObject* parent)
    : QObject(parent)
    , m_cga(cga)
    , m_freezeTime(60)
{
}

void ProcessWorkerAdapter::OnQueueQueryProcess()
{
    // 这里应该实现进程查询逻辑
    // 暂时保持简单实现
}

void ProcessWorkerAdapter::OnNotifyFillStaticSettings(int freezetime, int chatmaxlines)
{
    m_freezeTime = freezetime;
    LOG_DEBUG("进程 Worker 设置更新 - 冻结时间：{}s", freezetime);
}

} // namespace cga
