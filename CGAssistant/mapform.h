#ifndef MAPFORM_H
#define MAPFORM_H

#include <QWidget>
#include <memory>
#include "player.h"

// 前向声明 - 新架构
namespace cga {
    namespace application {
        class ServiceFactory;
    }
}

namespace Ui {
class MapForm;
}

class MapForm : public QWidget
{
    Q_OBJECT

public:
    explicit MapForm(QWidget *parent = nullptr);
    ~MapForm();
    virtual void keyPressEvent(QKeyEvent *event);
    
    /**
     * @brief 使用新架构初始化
     */
    void InitializeWithServices(std::shared_ptr<cga::application::ServiceFactory> serviceFactory);
    
public slots:
    void OnNotifyRefreshMapRegion(int xbase, int ybase, int xtop, int ytop, int index3);
    void OnNotifyGetMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units);
    void OnNotifyGetMapInfo(QString name, int index1, int index2, int index3, int x, int y, int worldstatus, int gamestatus);
    void OnReportNavigatorPath(QString json);
    void OnReportNavigatorFinish(int exitCode);
    void OnCloseWindow();
    void RunNavigatorScript(int x, int y, int enter, QString *result);
    void StopNavigatorScript();
    void UpdateMousePosition(int x, int y);

private slots:
    void on_pushButton_loadmap_clicked();

signals:
    void RequestDownloadMap(int xsize, int ysize);
    void runNavigatorScript(int x, int y, int enter, QString *result);
    void stopNavigatorScript();
    
private:
    Ui::MapForm *ui;
    
    // 旧架构 (向后兼容)
    QSharedPointer<CGA_MapCellData_t> m_collision;
    
    // 新架构
    std::shared_ptr<cga::application::ServiceFactory> m_serviceFactory;
    
    // 新架构方法
    void UpdateMapInfoNew();
};

#endif // MAPFORM_H
