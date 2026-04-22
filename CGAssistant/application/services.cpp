#include "services.h"
#include "../infrastructure/repositories.h"
#include "../domain/entities.h"
#include "../CGALib/gameinterface.h"

namespace cga { namespace application {

// ============================================================================
// PlayerService 实现
// ============================================================================

PlayerService::PlayerService(CGA::CGAInterface* cga)
    : m_cga(cga)
{
}

std::shared_ptr<domain::Player> PlayerService::getPlayerInfo()
{
    auto player = std::make_shared<domain::Player>();
    
    if (!m_cga || !m_cga->IsConnected()) {
        return player;
    }
    
    CGA::cga_player_info_t info;
    if (m_cga->GetPlayerInfo(info)) {
        player->name = QString::fromStdString(info.name);
        player->job = QString::fromStdString(info.job);
        player->level = info.level;
        player->gold = info.gold;
        player->hp = info.hp;
        player->maxHp = info.maxhp;
        player->mp = info.mp;
        player->maxMp = info.maxmp;
        player->xp = info.xp;
        player->maxXp = info.maxxp;
        player->unitId = info.unitid;
        player->serverIndex = info.petid;  // 注意：这里实际应该是地图索引
    }
    
    return player;
}

std::vector<domain::Pet> PlayerService::getPets()
{
    std::vector<domain::Pet> pets;
    
    if (!m_cga) {
        return pets;
    }
    
    CGA::cga_pets_info_t petsInfo;
    if (m_cga->GetPetsInfo(petsInfo)) {
        for (const auto& petInfo : petsInfo) {
            domain::Pet pet;
            pet.id = petInfo.index;
            pet.name = QString::fromStdString(petInfo.name);
            pet.realName = QString::fromStdString(petInfo.realname);
            pet.level = petInfo.level;
            pet.hp = petInfo.hp;
            pet.maxHp = petInfo.maxhp;
            pet.mp = petInfo.mp;
            pet.maxMp = petInfo.maxmp;
            pet.loyalty = petInfo.loyality;
            pet.flags = petInfo.flags;
            pet.battleFlags = petInfo.battle_flags;
            pets.push_back(pet);
        }
    }
    
    return pets;
}

std::vector<domain::Item> PlayerService::getItems()
{
    std::vector<domain::Item> items;
    
    if (!m_cga) {
        return items;
    }
    
    CGA::cga_items_info_t itemsInfo;
    if (m_cga->GetItemsInfo(itemsInfo)) {
        for (const auto& itemInfo : itemsInfo) {
            domain::Item item;
            item.itemId = itemInfo.itemid;
            item.type = itemInfo.type;
            item.count = itemInfo.count;
            item.pos = itemInfo.pos;
            item.name = QString::fromStdString(itemInfo.name);
            item.attr = QString::fromStdString(itemInfo.attr);
            item.info = QString::fromStdString(itemInfo.info);
            item.assessed = itemInfo.assessed;
            items.push_back(item);
        }
    }
    
    return items;
}

std::vector<domain::Skill> PlayerService::getSkills()
{
    std::vector<domain::Skill> skills;
    
    if (!m_cga) {
        return skills;
    }
    
    CGA::cga_skills_info_t skillsInfo;
    if (m_cga->GetSkillsInfo(skillsInfo)) {
        for (const auto& skillInfo : skillsInfo) {
            domain::Skill skill;
            skill.id = skillInfo.index;
            skill.pos = skillInfo.pos;
            skill.level = skillInfo.lv;
            skill.maxLevel = skillInfo.maxlv;
            skill.name = QString::fromStdString(skillInfo.name);
            skills.push_back(skill);
        }
    }
    
    return skills;
}

bool PlayerService::useItem(int pos)
{
    return m_cga && m_cga->UseItem(pos);
}

bool PlayerService::dropItem(int pos)
{
    return m_cga && m_cga->DropItem(pos);
}

// ============================================================================
// BattleService 实现
// ============================================================================

BattleService::BattleService(CGA::CGAInterface* cga)
    : m_cga(cga)
{
}

std::shared_ptr<domain::BattleContext> BattleService::getBattleContext()
{
    auto context = std::make_shared<domain::BattleContext>();
    
    if (!m_cga) {
        return context;
    }
    
    CGA::cga_battle_context_t ctx;
    if (m_cga->GetBattleContext(ctx)) {
        context->round = ctx.round_count;
        context->playerPos = ctx.player_pos;
        context->playerStatus = ctx.player_status;
        context->petId = ctx.petid;
        context->isPlayerTurn = true;  // 需要根据实际情况判断
    }
    
    return context;
}

bool BattleService::normalAttack(int target)
{
    return m_cga && m_cga->BattleNormalAttack(target);
}

bool BattleService::skillAttack(int skillId, int level, int target)
{
    return m_cga && m_cga->BattleSkillAttack(skillId, level, target, false);
}

bool BattleService::guard()
{
    return m_cga && m_cga->BattleGuard();
}

bool BattleService::escape()
{
    return m_cga && m_cga->BattleEscape();
}

bool BattleService::changePet(int petId)
{
    return m_cga && m_cga->BattleChangePet(petId);
}

void BattleService::setAutoBattle(bool enabled)
{
    m_autoBattle = enabled;
    // 这里应该通知底层的 BattleWorker
}

// ============================================================================
// MapService 实现
// ============================================================================

MapService::MapService(CGA::CGAInterface* cga)
    : m_cga(cga)
{
}

std::pair<int, int> MapService::getPosition()
{
    if (!m_cga) {
        return {0, 0};
    }
    
    auto [x, y] = m_cga->GetMapXY();
    return {x, y};
}

QString MapService::getMapName()
{
    if (!m_cga) {
        return QString();
    }
    
    return QString::fromStdString(m_cga->GetMapName());
}

std::vector<domain::MapUnit> MapService::getMapUnits()
{
    std::vector<domain::MapUnit> units;
    
    if (!m_cga) {
        return units;
    }
    
    CGA::cga_map_units_t mapUnits;
    if (m_cga->GetMapUnits(mapUnits)) {
        for (const auto& unit : mapUnits) {
            domain::MapUnit domainUnit;
            domainUnit.type = unit.valid;
            domainUnit.unitId = unit.unit_id;
            domainUnit.modelId = unit.model_id;
            domainUnit.x = unit.xpos;
            domainUnit.y = unit.ypos;
            domainUnit.level = unit.level;
            domainUnit.flags = unit.flags;
            domainUnit.name = QString::fromStdString(unit.unit_name);
            units.push_back(domainUnit);
        }
    }
    
    return units;
}

bool MapService::walkTo(int x, int y)
{
    if (!m_cga) {
        return false;
    }
    
    m_cga->WalkTo(x, y);
    return true;
}

bool MapService::isPassable(int x, int y)
{
    return m_cga && m_cga->IsMapCellPassable(x, y);
}

// ============================================================================
// NpcService 实现
// ============================================================================

NpcService::NpcService(CGA::CGAInterface* cga)
    : m_cga(cga)
{
}

bool NpcService::clickDialog(int option, int index)
{
    return m_cga && m_cga->ClickNPCDialog(option, index);
}

bool NpcService::sellItems(const std::vector<std::pair<int, int>>& items)
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

bool NpcService::buyItems(const std::vector<std::pair<int, int>>& items)
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
// ChatService 实现
// ============================================================================

ChatService::ChatService(CGA::CGAInterface* cga)
    : m_cga(cga)
{
}

bool ChatService::sendMessage(const QString& message)
{
    return m_cga && m_cga->SayWords(message.toStdString(), 0, 3, 1);
}

void ChatService::setMessageCallback(std::function<void(const domain::ChatMessage&)> callback)
{
    m_messageCallback = callback;
    
    // 这里应该注册到 CGAInterface 的消息回调
    // m_cga->RegisterChatMsgNotify([...]);
}

} } // namespace cga::application
