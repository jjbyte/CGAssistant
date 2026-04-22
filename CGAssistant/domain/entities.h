#pragma once

/**
 * CGAssistant 领域层 - 核心业务实体
 * 
 * 分层架构：
 * - Domain (领域层): 核心业务实体和规则
 * - Application (应用层): 业务逻辑和服务
 * - Infrastructure (基础设施层): RPC、文件、日志等
 * - Presentation (展示层): UI
 */

#include <QString>
#include <QList>
#include <memory>

namespace cga { namespace domain {

/**
 * @brief 玩家实体
 */
struct Player {
    QString name;           // 角色名
    QString job;            // 职业
    int level = 0;          // 等级
    int gold = 0;           // 金币
    int hp = 0;             // 当前 HP
    int maxHp = 0;          // 最大 HP
    int mp = 0;             // 当前 MP
    int maxMp = 0;          // 最大 MP
    int xp = 0;             // 当前经验
    int maxXp = 0;          // 最大经验
    int unitId = 0;         // 单位 ID
    int serverIndex = 0;    // 服务器索引
    
    /**
     * @brief HP 百分比
     */
    int hpPercent() const {
        return maxHp > 0 ? (hp * 100 / maxHp) : 0;
    }
    
    /**
     * @brief MP 百分比
     */
    int mpPercent() const {
        return maxMp > 0 ? (mp * 100 / maxMp) : 0;
    }
    
    /**
     * @brief 是否需要治疗
     */
    bool needsHealing(int threshold = 30) const {
        return hpPercent() < threshold;
    }
    
    /**
     * @brief 是否需要补魔
     */
    bool needsMana(int threshold = 30) const {
        return mpPercent() < threshold;
    }
};

/**
 * @brief 宠物实体
 */
struct Pet {
    int id = -1;            // 宠物 ID
    QString name;           // 昵称
    QString realName;       // 真名
    int level = 0;          // 等级
    int hp = 0;
    int maxHp = 0;
    int mp = 0;
    int maxMp = 0;
    int loyalty = 0;        // 忠诚度
    int flags = 0;          // 状态标志
    int battleFlags = 0;    // 战斗标志
    bool isDefault = false; // 是否默认出战
    
    int hpPercent() const {
        return maxHp > 0 ? (hp * 100 / maxHp) : 0;
    }
};

/**
 * @brief 物品实体
 */
struct Item {
    int itemId = -1;        // 物品 ID
    int type = -1;          // 物品类型
    int count = -1;         // 数量
    int pos = -1;           // 位置
    QString name;           // 名称
    QString attr;           // 属性
    QString info;           // 信息
    bool assessed = false;  // 已鉴定
    
    bool isValid() const {
        return pos >= 0 && itemId > 0;
    }
};

/**
 * @brief 技能实体
 */
struct Skill {
    int id = 0;             // 技能 ID
    int pos = 0;            // 位置
    int level = 0;          // 等级
    int maxLevel = 0;       // 最大等级
    QString name;           // 名称
    
    bool isValid() const {
        return id > 0 && level > 0;
    }
};

/**
 * @brief 地图单元
 */
struct MapUnit {
    int type = 0;           // 类型
    int unitId = 0;         // 单位 ID
    int modelId = 0;        // 模型 ID
    int x = 0;              // X 坐标
    int y = 0;              // Y 坐标
    int level = 0;          // 等级
    int flags = 0;          // 标志
    QString name;           // 名称
    
    bool isPlayer() const {
        return (flags & 256) != 0;
    }
    
    bool isPet() const {
        return (flags & 4096) != 0;
    }
};

/**
 * @brief 战斗上下文
 */
struct BattleContext {
    int round = 0;                  // 回合数
    int playerPos = -1;             // 玩家位置
    int playerStatus = 0;           // 玩家状态
    int petId = -1;                 // 宠物 ID
    int petPos = -1;                // 宠物位置
    int enemyCount = 0;             // 敌人数量
    int teammateCount = 0;          // 队友数量
    int bgmIndex = -1;              // BGM 索引
    bool isPlayerTurn = false;      // 是否玩家回合
    bool isDoubleAction = false;    // 是否双重行动
    
    bool isInBattle() const {
        return playerStatus == 1 || playerStatus == 4;
    }
};

/**
 * @brief NPC 对话
 */
struct NpcDialog {
    int type = 0;           // 对话类型
    int options = 0;        // 选项数量
    int dialogId = 0;       // 对话 ID
    int npcId = 0;          // NPC ID
    QString message;        // 对话内容
};

/**
 * @brief 聊天消息
 */
struct ChatMessage {
    int unitId = -1;        // 发送者 ID
    QString message;        // 消息内容
    int color = 0;          // 颜色
    bool isSystem = false;  // 是否系统消息
    bool isSelf = false;    // 是否自己发送
};

} } // namespace cga::domain
