#pragma once

/**
 * CGAssistant 应用层 - 服务工厂
 * 
 * 创建和管理所有服务实例，提供依赖注入功能
 */

#include "services.h"
#include "../infrastructure/repositories.h"
#include <memory>
#include <unordered_map>

// 前向声明
namespace CGA {
    class CGAInterface;
}

namespace cga { namespace application {

/**
 * @brief 服务工厂
 * 
 * 使用示例:
 *   auto factory = ServiceFactory::Create(cgaInterface);
 *   auto& playerService = factory->player();
 *   auto player = playerService.getPlayerInfo();
 */
class ServiceFactory {
public:
    /**
     * @brief 创建服务工厂
     * @param cga CGA 接口实例
     * @return 服务工厂实例
     */
    static std::shared_ptr<ServiceFactory> Create(CGA::CGAInterface* cga);
    
    /**
     * @brief 获取玩家服务
     */
    IPlayerService& player();
    
    /**
     * @brief 获取战斗服务
     */
    IBattleService& battle();
    
    /**
     * @brief 获取地图服务
     */
    IMapService& map();
    
    /**
     * @brief 获取 NPC 服务
     */
    INpcService& npc();
    
    /**
     * @brief 获取聊天服务
     */
    IChatService& chat();
    
    /**
     * @brief 获取游戏服务 (聚合所有服务)
     */
    class GameService : public domain::IGameService {
    public:
        GameService(
            IPlayerService& player,
            IBattleService& battle,
            IMapService& map,
            INpcService& npc,
            IChatService& chat,
            CGA::CGAInterface* cga
        );
        
        IPlayerService& player() override;
        IBattleService& battle() override;
        IMapService& map() override;
        INpcService& npc() override;
        IChatService& chat() override;
        bool isConnected() override;
        bool isInGame() override;
        
    private:
        IPlayerService& m_player;
        IBattleService& m_battle;
        IMapService& m_map;
        INpcService& m_npc;
        IChatService& m_chat;
        CGA::CGAInterface* m_cga;
    };
    
    /**
     * @brief 获取游戏服务
     */
    GameService& game();

private:
    explicit ServiceFactory(CGA::CGAInterface* cga);
    
    CGA::CGAInterface* m_cga;
    
    // 仓库
    std::unique_ptr<infrastructure::PlayerRepository> m_playerRepo;
    std::unique_ptr<infrastructure::BattleRepository> m_battleRepo;
    std::unique_ptr<infrastructure::MapRepository> m_mapRepo;
    std::unique_ptr<infrastructure::NpcRepository> m_npcRepo;
    std::unique_ptr<infrastructure::ChatRepository> m_chatRepo;
    
    // 服务
    std::unique_ptr<PlayerService> m_playerService;
    std::unique_ptr<BattleService> m_battleService;
    std::unique_ptr<MapService> m_mapService;
    std::unique_ptr<NpcService> m_npcService;
    std::unique_ptr<ChatService> m_chatService;
    
    // 游戏服务
    std::unique_ptr<GameService> m_gameService;
};

} } // namespace cga::application
