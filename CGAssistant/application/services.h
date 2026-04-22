#pragma once

/**
 * CGAssistant 应用层 - 玩家服务实现
 * 
 * 实现领域服务接口，协调领域对象和基础设施
 */

#include "../domain/services.h"
#include "../domain/entities.h"
#include <memory>
#include <vector>

// 前向声明
namespace CGA {
    class CGAInterface;
}

namespace cga { namespace application {

/**
 * @brief 玩家服务实现
 */
class PlayerService : public domain::IPlayerService {
public:
    explicit PlayerService(CGA::CGAInterface* cga);
    
    // IPlayerService 实现
    std::shared_ptr<domain::Player> getPlayerInfo() override;
    std::vector<domain::Pet> getPets() override;
    std::vector<domain::Item> getItems() override;
    std::vector<domain::Skill> getSkills() override;
    bool useItem(int pos) override;
    bool dropItem(int pos) override;

private:
    CGA::CGAInterface* m_cga;
};

/**
 * @brief 战斗服务实现
 */
class BattleService : public domain::IBattleService {
public:
    explicit BattleService(CGA::CGAInterface* cga);
    
    // IBattleService 实现
    std::shared_ptr<domain::BattleContext> getBattleContext() override;
    bool normalAttack(int target) override;
    bool skillAttack(int skillId, int level, int target) override;
    bool guard() override;
    bool escape() override;
    bool changePet(int petId) override;
    void setAutoBattle(bool enabled) override;

private:
    CGA::CGAInterface* m_cga;
    bool m_autoBattle = false;
};

/**
 * @brief 地图服务实现
 */
class MapService : public domain::IMapService {
public:
    explicit MapService(CGA::CGAInterface* cga);
    
    // IMapService 实现
    std::pair<int, int> getPosition() override;
    QString getMapName() override;
    std::vector<domain::MapUnit> getMapUnits() override;
    bool walkTo(int x, int y) override;
    bool isPassable(int x, int y) override;

private:
    CGA::CGAInterface* m_cga;
};

/**
 * @brief NPC 服务实现
 */
class NpcService : public domain::INpcService {
public:
    explicit NpcService(CGA::CGAInterface* cga);
    
    // INpcService 实现
    bool clickDialog(int option, int index) override;
    bool sellItems(const std::vector<std::pair<int, int>>& items) override;
    bool buyItems(const std::vector<std::pair<int, int>>& items) override;

private:
    CGA::CGAInterface* m_cga;
};

/**
 * @brief 聊天服务实现
 */
class ChatService : public domain::IChatService {
public:
    explicit ChatService(CGA::CGAInterface* cga);
    
    // IChatService 实现
    bool sendMessage(const QString& message) override;
    void setMessageCallback(std::function<void(const domain::ChatMessage&)> callback) override;

private:
    CGA::CGAInterface* m_cga;
    std::function<void(const domain::ChatMessage&)> m_messageCallback;
};

} } // namespace cga::application
