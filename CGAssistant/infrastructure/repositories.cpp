#include "repositories.h"
#include "../CGALib/gameinterface.h"
#include <QDateTime>

namespace cga { namespace infrastructure {

// ============================================================================
// PlayerRepository 实现
// ============================================================================

PlayerRepository::PlayerRepository(CGA::CGAInterface* cga)
    : m_cga(cga)
{
}

std::shared_ptr<CGA::cga_player_info_t> PlayerRepository::getInfo()
{
    auto info = std::make_shared<CGA::cga_player_info_t>();
    
    if (!m_cga || !m_cga->IsConnected()) {
        return info;
    }
    
    if (!m_cga->GetPlayerInfo(*info)) {
        return nullptr;
    }
    
    return info;
}

std::vector<CGA::cga_pet_info_t> PlayerRepository::getPets()
{
    std::vector<CGA::cga_pet_info_t> pets;
    
    if (!m_cga) {
        return pets;
    }
    
    CGA::cga_pets_info_t petsInfo;
    if (m_cga->GetPetsInfo(petsInfo)) {
        pets.assign(petsInfo.begin(), petsInfo.end());
    }
    
    return pets;
}

std::vector<CGA::cga_item_info_t> PlayerRepository::getItems()
{
    std::vector<CGA::cga_item_info_t> items;
    
    if (!m_cga) {
        return items;
    }
    
    CGA::cga_items_info_t itemsInfo;
    if (m_cga->GetItemsInfo(itemsInfo)) {
        items.assign(itemsInfo.begin(), itemsInfo.end());
    }
    
    return items;
}

std::vector<CGA::cga_skill_info_t> PlayerRepository::getSkills()
{
    std::vector<CGA::cga_skill_info_t> skills;
    
    if (!m_cga) {
        return skills;
    }
    
    CGA::cga_skills_info_t skillsInfo;
    if (m_cga->GetSkillsInfo(skillsInfo)) {
        skills.assign(skillsInfo.begin(), skillsInfo.end());
    }
    
    return skills;
}

bool PlayerRepository::useItem(int pos)
{
    return m_cga && m_cga->UseItem(pos);
}

bool PlayerRepository::dropItem(int pos)
{
    return m_cga && m_cga->DropItem(pos);
}

// ============================================================================
// BattleRepository 实现
// ============================================================================

BattleRepository::BattleRepository(CGA::CGAInterface* cga)
    : m_cga(cga)
{
}

std::vector<CGA::cga_battle_unit_t> BattleRepository::getUnits()
{
    std::vector<CGA::cga_battle_unit_t> units;
    
    if (!m_cga) {
        return units;
    }
    
    CGA::cga_battle_units_t battleUnits;
    if (m_cga->GetBattleUnits(battleUnits)) {
        units.assign(battleUnits.begin(), battleUnits.end());
    }
    
    return units;
}

bool BattleRepository::normalAttack(int target)
{
    return m_cga && m_cga->BattleNormalAttack(target);
}

bool BattleRepository::skillAttack(int skillId, int level, int target)
{
    return m_cga && m_cga->BattleSkillAttack(skillId, level, target, false);
}

bool BattleRepository::guard()
{
    return m_cga && m_cga->BattleGuard();
}

bool BattleRepository::escape()
{
    return m_cga && m_cga->BattleEscape();
}

bool BattleRepository::changePet(int petId)
{
    return m_cga && m_cga->BattleChangePet(petId);
}

// ============================================================================
// MapRepository 实现
// ============================================================================

MapRepository::MapRepository(CGA::CGAInterface* cga)
    : m_cga(cga)
{
}

std::pair<int, int> MapRepository::getPosition()
{
    if (!m_cga) {
        return {0, 0};
    }
    
    auto [x, y] = m_cga->GetMapXY();
    return {x, y};
}

QString MapRepository::getMapName()
{
    if (!m_cga) {
        return QString();
    }
    
    return QString::fromStdString(m_cga->GetMapName());
}

std::vector<CGA::cga_battle_unit_t> MapRepository::getUnits()
{
    std::vector<CGA::cga_battle_unit_t> units;
    
    if (!m_cga) {
        return units;
    }
    
    CGA::cga_map_units_t mapUnits;
    if (m_cga->GetMapUnits(mapUnits)) {
        for (const auto& unit : mapUnits) {
            CGA::cga_battle_unit_t battleUnit;
            battleUnit.name = unit.unit_name;
            battleUnit.modelid = unit.model_id;
            battleUnit.level = unit.level;
            battleUnit.pos = unit.xpos;  // 注意坐标转换
            units.push_back(battleUnit);
        }
    }
    
    return units;
}

bool MapRepository::walkTo(int x, int y)
{
    if (!m_cga) {
        return false;
    }
    
    m_cga->WalkTo(x, y);
    return true;
}

bool MapRepository::isPassable(int x, int y)
{
    return m_cga && m_cga->IsMapCellPassable(x, y);
}

// ============================================================================
// NpcRepository 实现
// ============================================================================

NpcRepository::NpcRepository(CGA::CGAInterface* cga)
    : m_cga(cga)
{
}

bool NpcRepository::clickDialog(int option, int index)
{
    return m_cga && m_cga->ClickNPCDialog(option, index);
}

bool NpcRepository::sellItems(const std::vector<std::pair<int, int>>& items)
{
    if (!m_cga) {
        return false;
    }
    
    CGA::cga_sell_items_t sellItems;
    for (const auto& [pos, count] : items) {
        CGA::cga_sell_item_t item;
        item.itempos = pos;
        item.count = count;
        sellItems.push_back(item);
    }
    
    return m_cga->SellNPCStore(sellItems);
}

bool NpcRepository::buyItems(const std::vector<std::pair<int, int>>& items)
{
    if (!m_cga) {
        return false;
    }
    
    CGA::cga_buy_items_t buyItems;
    for (const auto& [index, count] : items) {
        CGA::cga_buy_item_t item;
        item.index = index;
        item.count = count;
        buyItems.push_back(item);
    }
    
    return m_cga->BuyNPCStore(buyItems);
}

// ============================================================================
// ChatRepository 实现
// ============================================================================

ChatRepository::ChatRepository(CGA::CGAInterface* cga)
    : m_cga(cga)
{
}

bool ChatRepository::sendMessage(const QString& message)
{
    return m_cga && m_cga->SayWords(message.toStdString(), 0, 3, 1);
}

void ChatRepository::setMessageCallback(MessageCallback callback)
{
    m_callback = callback;
    
    // 注册到 CGAInterface 的消息回调
    // 这需要在 CGAInterface 中添加相应的注册方法
    // m_cga->RegisterChatMsgNotify([this](CGA::cga_chat_msg_t msg) {
    //     if (m_callback) {
    //         m_callback(msg);
    //     }
    // });
}

} } // namespace cga::infrastructure
