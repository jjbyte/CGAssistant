#include "service_factory.h"
#include "../CGALib/gameinterface.h"

namespace cga { namespace application {

// ============================================================================
// ServiceFactory 实现
// ============================================================================

ServiceFactory::ServiceFactory(CGA::CGAInterface* cga)
    : m_cga(cga)
{
    // 创建仓库
    m_playerRepo = std::make_unique<infrastructure::PlayerRepository>(cga);
    m_battleRepo = std::make_unique<infrastructure::BattleRepository>(cga);
    m_mapRepo = std::make_unique<infrastructure::MapRepository>(cga);
    m_npcRepo = std::make_unique<infrastructure::NpcRepository>(cga);
    m_chatRepo = std::make_unique<infrastructure::ChatRepository>(cga);
    
    // 创建服务 (注入仓库)
    m_playerService = std::make_unique<PlayerService>(cga);
    m_battleService = std::make_unique<BattleService>(cga);
    m_mapService = std::make_unique<MapService>(cga);
    m_npcService = std::make_unique<NpcService>(cga);
    m_chatService = std::make_unique<ChatService>(cga);
}

std::shared_ptr<ServiceFactory> ServiceFactory::Create(CGA::CGAInterface* cga)
{
    return std::make_shared<ServiceFactory>(cga);
}

IPlayerService& ServiceFactory::player()
{
    return *m_playerService;
}

IBattleService& ServiceFactory::battle()
{
    return *m_battleService;
}

IMapService& ServiceFactory::map()
{
    return *m_mapService;
}

INpcService& ServiceFactory::npc()
{
    return *m_npcService;
}

IChatService& ServiceFactory::chat()
{
    return *m_chatService;
}

// ============================================================================
// GameService 实现
// ============================================================================

ServiceFactory::GameService::GameService(
    IPlayerService& player,
    IBattleService& battle,
    IMapService& map,
    INpcService& npc,
    IChatService& chat,
    CGA::CGAInterface* cga
)
    : m_player(player)
    , m_battle(battle)
    , m_map(map)
    , m_npc(npc)
    , m_chat(chat)
    , m_cga(cga)
{
}

IPlayerService& ServiceFactory::GameService::player()
{
    return m_player;
}

IBattleService& ServiceFactory::GameService::battle()
{
    return m_battle;
}

IMapService& ServiceFactory::GameService::map()
{
    return m_map;
}

INpcService& ServiceFactory::GameService::npc()
{
    return m_npc;
}

IChatService& ServiceFactory::GameService::chat()
{
    return m_chat;
}

bool ServiceFactory::GameService::isConnected()
{
    return m_cga && m_cga->IsConnected();
}

bool ServiceFactory::GameService::isInGame()
{
    if (!m_cga) {
        return false;
    }
    
    int ingame = 0;
    return m_cga->IsInGame(ingame) && ingame;
}

ServiceFactory::GameService& ServiceFactory::game()
{
    if (!m_gameService) {
        m_gameService = std::make_unique<GameService>(
            player(),
            battle(),
            map(),
            npc(),
            chat(),
            m_cga
        );
    }
    
    return *m_gameService;
}

} } // namespace cga::application
