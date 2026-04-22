#include "mapform.h"
#include "application/service_factory.h"
#include "domain/entities.h"
#include "../CGALib/logger.h"
#include "ui_mapform.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

extern CGA::CGAInterface *g_CGAInterface;

MapForm::MapForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapForm),
    m_serviceFactory(nullptr)
{
    ui->setupUi(this);

    connect(ui->widget_paintmap, &MyPaintMap::updateMousePosition, this, &MapForm::UpdateMousePosition);
    connect(ui->widget_paintmap, &MyPaintMap::runNavigatorScript, this, &MapForm::RunNavigatorScript);
    connect(ui->widget_paintmap, &MyPaintMap::stopNavigatorScript, this, &MapForm::StopNavigatorScript);
}

void MapForm::InitializeWithServices(std::shared_ptr<cga::application::ServiceFactory> serviceFactory)
{
    m_serviceFactory = serviceFactory;
    LOG_INFO("MapForm 已使用新架构初始化");
}

MapForm::~MapForm()
{
    delete ui;
}

void MapForm::keyPressEvent(QKeyEvent *event)
{
    if(!m_collision)
        return;

    ui->widget_paintmap->keyPressEvent(event);
}

void MapForm::UpdateMousePosition(int x, int y)
{
    ui->label_xy_2->setText(tr("mouse click (%1, %2)").arg(x).arg(y));
}

void MapForm::RunNavigatorScript(int x, int y, int enter, QString *result)
{
    runNavigatorScript(x, y, enter, result);
}

void MapForm::StopNavigatorScript()
{
    stopNavigatorScript();
}

void MapForm::OnReportNavigatorFinish(int exitCode)
{
    if(exitCode != 0)
        ui->widget_paintmap->ShowNavError(QObject::tr("Navigation Failed"));
    ui->widget_paintmap->LoadNavPath(nullptr);
}

void MapForm::OnReportNavigatorPath(QString json)
{
    QJsonParseError err;
    auto doc = QJsonDocument::fromJson(json.toLocal8Bit(), &err);

    if(err.error != QJsonParseError::NoError)
        return;

    if(!doc.isArray())
        return;

    auto arr = doc.array();

    QSharedPointer<QVector<uint32_t>> navpath(new QVector<uint32_t>);
    for(auto itor = arr.begin(); itor != arr.end(); ++itor)
    {
        if(itor->isArray())
        {
            auto sub = itor->toArray();
            if(sub.count() == 2)
            {
                int x = sub.at(0).toInt();
                int y = sub.at(1).toInt();
                navpath->push_back((x & 0xFFFF) | ((y & 0xFFFF) << 16));
            }
        }
    }


    if(navpath->empty())
        ui->widget_paintmap->ShowNavError(QObject::tr("Navigation Failed"));

    ui->widget_paintmap->LoadNavPath(navpath);
}

void MapForm::OnNotifyGetMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units)
{
    ui->widget_paintmap->m_bShowCrosshair = ui->checkBox_showcrosshair->isChecked();

    if(coll && coll->xsize && coll->ysize)
    {
        LOG_DEBUG("地图碰撞数据更新 - 尺寸：{}x{} 索引：{}", coll->xsize, coll->ysize, coll->mapindex);
        m_collision = coll;
        ui->widget_paintmap->LoadMapCellInfo(coll, obj, units);
    }
}

void MapForm::OnNotifyGetMapInfo(QString name, int index1, int index2, int index3, int x, int y, int worldstatus, int gamestatus)
{
    // 如果使用了新架构，跳过旧架构的处理
    if (m_serviceFactory) {
        UpdateMapInfoNew();
        return;
    }
    
    // 旧架构处理逻辑
    ui->widget_paintmap->m_bShowCrosshair = ui->checkBox_showcrosshair->isChecked();

    LOG_TRACE("地图信息更新 - 名称：{} 坐标：({}, {}) 索引：{} 世界状态：{}", 
             name.toStdString().c_str(), x, y, index3, worldstatus);
    
    ui->widget_paintmap->LoadMapInfo(name, x, y, index3, worldstatus, gamestatus);
    ui->label_mapname->setText(name);
    ui->label_xy->setText(tr("player at (%1, %2)").arg(x).arg(y));
    ui->label_index->setText(tr("map index : %1").arg(index3));
}

void MapForm::OnNotifyRefreshMapRegion(int xbase, int ybase, int xtop, int ytop, int index3)
{
    if(m_collision && index3 == m_collision->mapindex) {
        LOG_TRACE("刷新地图区域 - 起点：({}, {}) 终点：({}, {})", xbase, ybase, xtop, ytop);
        ui->widget_paintmap->RepaintCollisionPixels(xbase, ybase, xtop, ytop);
    }
}

void MapForm::OnCloseWindow()
{

}

void MapForm::on_pushButton_loadmap_clicked()
{
    if (m_serviceFactory) {
        // 新架构
        auto& map = m_serviceFactory->map();
        auto [x, y] = map.getPosition();
        RequestDownloadMap(51, 51);  // 默认下载 51x51
    } else if(m_collision) {
        // 旧架构
        RequestDownloadMap(m_collision->xsize, m_collision->ysize);
    }
}

// ============================================================================
// 新架构方法实现
// ============================================================================

void MapForm::UpdateMapInfoNew()
{
    if (!m_serviceFactory) {
        return;
    }
    
    auto& map = m_serviceFactory->map();
    
    // 获取地图信息
    auto mapName = map.getMapName();
    auto [x, y] = map.getPosition();
    
    // 更新 UI
    ui->label_mapname->setText(mapName);
    ui->label_xy->setText(tr("player at (%1, %2)").arg(x).arg(y));
    
    // 获取地图单位
    auto units = map.getMapUnits();
    // 可以在这里更新地图显示
    
    LOG_DEBUG("地图信息更新 (新架构): {} ({}, {})", 
             mapName.toStdString(), x, y);
}
