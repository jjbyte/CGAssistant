#include "services.h"
#include "../infrastructure/repositories.h"
#include "../domain/entities.h"
#include "../CGALib/gameinterface.h"
#include "../CGALib/logger.h"
#include "optimization/performance.h"

namespace cga { namespace application {

// ============================================================================
// PlayerService 实现 (带缓存优化)
// ============================================================================

PlayerService::PlayerService(CGA::CGAInterface* cga)
    : m_cga(cga)
    , m_playerCache(std::chrono::seconds(2))  // 2 秒缓存
{
}

std::shared_ptr<domain::Player> PlayerService::getPlayerInfo()
{
    PERF_START("PlayerService::getPlayerInfo");
    
    // 尝试从缓存获取
    std::shared_ptr<domain::Player> cached;
    if (m_playerCache.get("player", cached)) {
        PERF_END("PlayerService::getPlayerInfo");
        return cached;
    }
    
    auto player = std::make_shared<domain::Player>();
    
    if (!m_cga || !m_cga->IsConnected()) {
        PERF_END("PlayerService::getPlayerInfo");
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
        player->serverIndex = info.petid;
        
        // 更新缓存
        m_playerCache.set("player", player);
    }
    
    PERF_END("PlayerService::getPlayerInfo");
    return player;
}

std::vector<domain::Pet> PlayerService::getPets()
{
    PERF_START("PlayerService::getPets");
    
    std::vector<domain::Pet> pets;
    
    if (!m_cga) {
        PERF_END("PlayerService::getPets");
        return pets;
    }
    
    CGA::cga_pets_info_t petsInfo;
    if (m_cga->GetPetsInfo(petsInfo)) {
        pets.reserve(petsInfo.size());  // 预分配内存
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
            pets.push_back(std::move(pet));  // 移动语义
        }
    }
    
    PERF_END("PlayerService::getPets");
    return pets;
}

std::vector<domain::Item> PlayerService::getItems()
{
    PERF_START("PlayerService::getItems");
    
    std::vector<domain::Item> items;
    
    if (!m_cga) {
        PERF_END("PlayerService::getItems");
        return items;
    }
    
    CGA::cga_items_info_t itemsInfo;
    if (m_cga->GetItemsInfo(itemsInfo)) {
        items.reserve(itemsInfo.size());
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
            items.push_back(std::move(item));
        }
    }
    
    PERF_END("PlayerService::getItems");
    return items;
}

std::vector<domain::Skill> PlayerService::getSkills()
{
    PERF_START("PlayerService::getSkills");
    
    std::vector<domain::Skill> skills;
    
    if (!m_cga) {
        PERF_END("PlayerService::getSkills");
        return skills;
    }
    
    CGA::cga_skills_info_t skillsInfo;
    if (m_cga->GetSkillsInfo(skillsInfo)) {
        skills.reserve(skillsInfo.size());
        for (const auto& skillInfo : skillsInfo) {
            domain::Skill skill;
            skill.id = skillInfo.index;
            skill.pos = skillInfo.pos;
            skill.level = skillInfo.lv;
            skill.maxLevel = skillInfo.maxlv;
            skill.name = QString::fromStdString(skillInfo.name);
            skills.push_back(std::move(skill));
        }
    }
    
    PERF_END("PlayerService::getSkills");
    return skills;
}

bool PlayerService::useItem(int pos)
{
    PERF_START("PlayerService::useItem");
    bool result = m_cga && m_cga->UseItem(pos);
    PERF_END("PlayerService::useItem");
    return result;
}

bool PlayerService::dropItem(int pos)
{
    PERF_START("PlayerService::dropItem");
    
    // 清除缓存
    m_playerCache.clear();
    
    bool result = m_cga && m_cga->DropItem(pos);
    PERF_END("PlayerService::dropItem");
    return result;
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
    PERF_START("BattleService::getBattleContext");
    
    auto context = std::make_shared<domain::BattleContext>();
    
    if (!m_cga) {
        PERF_END("BattleService::getBattleContext");
        return context;
    }
    
    CGA::cga_battle_context_t ctx;
    if (m_cga->GetBattleContext(ctx)) {
        context->round = ctx.round_count;
        context->playerPos = ctx.player_pos;
        context->playerStatus = ctx.player_status;
        context->petId = ctx.petid;
        context->isPlayerTurn = true;
    }
    
    PERF_END("BattleService::getBattleContext");
    return context;
}

bool BattleService::normalAttack(int target)
{
    PERF_START("BattleService::normalAttack");
    bool result = m_cga && m_cga->BattleNormalAttack(target);
    PERF_END("BattleService::normalAttack");
    return result;
}

bool BattleService::skillAttack(int skillId, int level, int target)
{
    PERF_START("BattleService::skillAttack");
    bool result = m_cga && m_cga->BattleSkillAttack(skillId, level, target, false);
    PERF_END("BattleService::skillAttack");
    return result;
}

bool BattleService::guard()
{
    PERF_START("BattleService::guard");
    bool result = m_cga && m_cga->BattleGuard();
    PERF_END("BattleService::guard");
    return result;
}

bool BattleService::escape()
{
    PERF_START("BattleService::escape");
    bool result = m_cga && m_cga->BattleEscape();
    PERF_END("BattleService::escape");
    return result;
}

bool BattleService::changePet(int petId)
{
    PERF_START("BattleService::changePet");
    bool result = m_cga && m_cga->BattleChangePet(petId);
    PERF_END("BattleService::changePet");
    return result;
}

void BattleService::setAutoBattle(bool enabled)
{
    m_autoBattle = enabled;
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
    PERF_START("MapService::getPosition");
    
    if (!m_cga) {
        PERF_END("MapService::getPosition");
        return {0, 0};
    }
    
    auto [x, y] = m_cga->GetMapXY();
    PERF_END("MapService::getPosition");
    return {x, y};
}

QString MapService::getMapName()
{
    PERF_START("MapService::getMapName");
    
    if (!m_cga) {
        PERF_END("MapService::getMapName");
        return QString();
    }
    
    QString name = QString::fromStdString(m_cga->GetMapName());
    PERF_END("MapService::getMapName");
    return name;
}

std::vector<domain::MapUnit> MapService::getMapUnits()
{
    PERF_START("MapService::getMapUnits");
    
    std::vector<domain::MapUnit> units;
    
    if (!m_cga) {
        PERF_END("MapService::getMapUnits");
        return units;
    }
    
    CGA::cga_map_units_t mapUnits;
    if (m_cga->GetMapUnits(mapUnits)) {
        units.reserve(mapUnits.size());
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
            units.push_back(std::move(domainUnit));
        }
    }
    
    PERF_END("MapService::getMapUnits");
    return units;
}

bool MapService::walkTo(int x, int y)
{
    PERF_START("MapService::walkTo");
    
    if (!m_cga) {
        PERF_END("MapService::walkTo");
        return false;
    }
    
    m_cga->WalkTo(x, y);
    PERF_END("MapService::walkTo");
    return true;
}

bool MapService::isPassable(int x, int y)
{
    PERF_START("MapService::isPassable");
    bool result = m_cga && m_cga->IsMapCellPassable(x, y);
    PERF_END("MapService::isPassable");
    return result;
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
    PERF_START("NpcService::clickDialog");
    bool result = m_cga && m_cga->ClickNPCDialog(option, index);
    PERF_END("NpcService::clickDialog");
    return result;
}

bool NpcService::sellItems(const std::vector<std::pair<int, int>>& items)
{
    PERF_START("NpcService::sellItems");
    
    if (!m_cga) {
        PERF_END("NpcService::sellItems");
        return false;
    }
    
    CGA::cga_sell_items_t sellItems;
    sellItems.reserve(items.size());
    for (const auto& [pos, count] : items) {
        CGA::cga_sell_item_t item;
        item.itempos = pos;
        item.count = count;
        sellItems.push_back(item);
    }
    
    bool result = m_cga->SellNPCStore(sellItems);
    PERF_END("NpcService::sellItems");
    return result;
}

bool NpcService::buyItems(const std::vector<std::pair<int, int>>& items)
{
    PERF_START("NpcService::buyItems");
    
    if (!m_cga) {
        PERF_END("NpcService::buyItems");
        return false;
    }
    
    CGA::cga_buy_items_t buyItems;
    buyItems.reserve(items.size());
    for (const auto& [index, count] : items) {
        CGA::cga_buy_item_t item;
        item.index = index;
        item.count = count;
        buyItems.push_back(item);
    }
    
    bool result = m_cga->BuyNPCStore(buyItems);
    PERF_END("NpcService::buyItems");
    return result;
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
    PERF_START("ChatService::sendMessage");
    bool result = m_cga && m_cga->SayWords(message.toStdString(), 0, 3, 1);
    PERF_END("ChatService::sendMessage");
    return result;
}

void ChatService::setMessageCallback(std::function<void(const domain::ChatMessage&)> callback)
{
    m_messageCallback = callback;
}

} } // namespace cga::application
