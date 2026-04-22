#pragma once

/**
 * CGAssistant 基础设施层 - RPC 仓库
 * 
 * 封装 RPC 调用，提供统一的数据访问接口
 */

#include <memory>
#include <vector>
#include <tuple>

// 前向声明
namespace CGA {
    class CGAInterface;
    struct cga_player_info_t;
    struct cga_pet_info_t;
    struct cga_item_info_t;
    struct cga_skill_info_t;
    struct cga_battle_unit_t;
    struct cga_npc_dialog_t;
    struct cga_chat_msg_t;
}

namespace cga { namespace infrastructure {

/**
 * @brief 玩家数据仓库
 */
class PlayerRepository {
public:
    explicit PlayerRepository(CGA::CGAInterface* cga);
    
    /**
     * @brief 获取玩家信息
     */
    std::shared_ptr<CGA::cga_player_info_t> getInfo();
    
    /**
     * @brief 获取宠物列表
     */
    std::vector<CGA::cga_pet_info_t> getPets();
    
    /**
     * @brief 获取物品列表
     */
    std::vector<CGA::cga_item_info_t> getItems();
    
    /**
     * @brief 获取技能列表
     */
    std::vector<CGA::cga_skill_info_t> getSkills();
    
    /**
     * @brief 使用物品
     */
    bool useItem(int pos);
    
    /**
     * @brief 丢弃物品
     */
    bool dropItem(int pos);

private:
    CGA::CGAInterface* m_cga;
};

/**
 * @brief 战斗数据仓库
 */
class BattleRepository {
public:
    explicit BattleRepository(CGA::CGAInterface* cga);
    
    /**
     * @brief 获取战斗单位
     */
    std::vector<CGA::cga_battle_unit_t> getUnits();
    
    /**
     * @brief 普通攻击
     */
    bool normalAttack(int target);
    
    /**
     * @brief 技能攻击
     */
    bool skillAttack(int skillId, int level, int target);
    
    /**
     * @brief 防御
     */
    bool guard();
    
    /**
     * @brief 逃跑
     */
    bool escape();
    
    /**
     * @brief 换宠
     */
    bool changePet(int petId);

private:
    CGA::CGAInterface* m_cga;
};

/**
 * @brief 地图数据仓库
 */
class MapRepository {
public:
    explicit MapRepository(CGA::CGAInterface* cga);
    
    /**
     * @brief 获取位置
     */
    std::pair<int, int> getPosition();
    
    /**
     * @brief 获取地图名称
     */
    QString getMapName();
    
    /**
     * @brief 获取地图单位
     */
    std::vector<CGA::cga_battle_unit_t> getUnits();
    
    /**
     * @brief 移动
     */
    bool walkTo(int x, int y);
    
    /**
     * @brief 检查可通过
     */
    bool isPassable(int x, int y);

private:
    CGA::CGAInterface* m_cga;
};

/**
 * @brief NPC 数据仓库
 */
class NpcRepository {
public:
    explicit NpcRepository(CGA::CGAInterface* cga);
    
    /**
     * @brief 点击对话
     */
    bool clickDialog(int option, int index);
    
    /**
     * @brief 出售物品
     */
    bool sellItems(const std::vector<std::pair<int, int>>& items);
    
    /**
     * @brief 购买物品
     */
    bool buyItems(const std::vector<std::pair<int, int>>& items);

private:
    CGA::CGAInterface* m_cga;
};

/**
 * @brief 聊天数据仓库
 */
class ChatRepository {
public:
    explicit ChatRepository(CGA::CGAInterface* cga);
    
    /**
     * @brief 发送消息
     */
    bool sendMessage(const QString& message);
    
    /**
     * @brief 设置消息回调
     */
    using MessageCallback = std::function<void(const CGA::cga_chat_msg_t&)>;
    void setMessageCallback(MessageCallback callback);

private:
    CGA::CGAInterface* m_cga;
    MessageCallback m_callback;
};

} } // namespace cga::infrastructure
