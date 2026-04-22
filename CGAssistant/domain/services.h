#pragma once

/**
 * CGAssistant 领域层 - 服务接口定义
 * 
 * 定义领域服务接口，由应用层实现
 */

#include "entities.h"
#include <functional>
#include <memory>
#include <vector>

namespace cga { namespace domain {

/**
 * @brief 玩家服务接口
 */
class IPlayerService {
public:
    virtual ~IPlayerService() = default;
    
    /**
     * @brief 获取玩家信息
     */
    virtual std::shared_ptr<Player> getPlayerInfo() = 0;
    
    /**
     * @brief 获取宠物列表
     */
    virtual std::vector<Pet> getPets() = 0;
    
    /**
     * @brief 获取物品列表
     */
    virtual std::vector<Item> getItems() = 0;
    
    /**
     * @brief 获取技能列表
     */
    virtual std::vector<Skill> getSkills() = 0;
    
    /**
     * @brief 使用物品
     */
    virtual bool useItem(int pos) = 0;
    
    /**
     * @brief 丢弃物品
     */
    virtual bool dropItem(int pos) = 0;
};

/**
 * @brief 战斗服务接口
 */
class IBattleService {
public:
    virtual ~IBattleService() = default;
    
    /**
     * @brief 获取战斗上下文
     */
    virtual std::shared_ptr<BattleContext> getBattleContext() = 0;
    
    /**
     * @brief 普通攻击
     */
    virtual bool normalAttack(int target) = 0;
    
    /**
     * @brief 技能攻击
     */
    virtual bool skillAttack(int skillId, int level, int target) = 0;
    
    /**
     * @brief 防御
     */
    virtual bool guard() = 0;
    
    /**
     * @brief 逃跑
     */
    virtual bool escape() = 0;
    
    /**
     * @brief 换宠
     */
    virtual bool changePet(int petId) = 0;
    
    /**
     * @brief 设置自动战斗
     */
    virtual void setAutoBattle(bool enabled) = 0;
};

/**
 * @brief 地图服务接口
 */
class IMapService {
public:
    virtual ~IMapService() = default;
    
    /**
     * @brief 获取当前位置
     */
    virtual std::pair<int, int> getPosition() = 0;
    
    /**
     * @brief 获取地图名称
     */
    virtual QString getMapName() = 0;
    
    /**
     * @brief 获取地图单位
     */
    virtual std::vector<MapUnit> getMapUnits() = 0;
    
    /**
     * @brief 移动到坐标
     */
    virtual bool walkTo(int x, int y) = 0;
    
    /**
     * @brief 检查坐标是否可通过
     */
    virtual bool isPassable(int x, int y) = 0;
};

/**
 * @brief NPC 服务接口
 */
class INpcService {
public:
    virtual ~INpcService() = default;
    
    /**
     * @brief 点击 NPC 对话选项
     */
    virtual bool clickDialog(int option, int index) = 0;
    
    /**
     * @brief 出售物品
     */
    virtual bool sellItems(const std::vector<std::pair<int, int>>& items) = 0;
    
    /**
     * @brief 购买物品
     */
    virtual bool buyItems(const std::vector<std::pair<int, int>>& items) = 0;
};

/**
 * @brief 聊天服务接口
 */
class IChatService {
public:
    virtual ~IChatService() = default;
    
    /**
     * @brief 发送消息
     */
    virtual bool sendMessage(const QString& message) = 0;
    
    /**
     * @brief 设置消息回调
     */
    virtual void setMessageCallback(std::function<void(const ChatMessage&)> callback) = 0;
};

/**
 * @brief 游戏服务接口 (聚合所有服务)
 */
class IGameService {
public:
    virtual ~IGameService() = default;
    
    virtual IPlayerService& player() = 0;
    virtual IBattleService& battle() = 0;
    virtual IMapService& map() = 0;
    virtual INpcService& npc() = 0;
    virtual IChatService& chat() = 0;
    
    /**
     * @brief 是否连接游戏
     */
    virtual bool isConnected() = 0;
    
    /**
     * @brief 是否在游戏中
     */
    virtual bool isInGame() = 0;
};

} } // namespace cga::domain
