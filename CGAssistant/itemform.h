#ifndef ITEMFORM_H
#define ITEMFORM_H

#include <QWidget>
#include <QMenu>
#include <QHash>
#include <QStandardItemModel>
#include <memory>
#include "itemdropper.h"
#include "itemtweaker.h"

// 前向声明 - 新架构
namespace cga {
    namespace application {
        class ServiceFactory;
    }
}

namespace Ui {
class ItemForm;
}

class QTextStream;

class ItemForm : public QWidget
{
    Q_OBJECT

public:
    explicit ItemForm(CPlayerWorker *worker, QWidget *parent = 0);
    ~ItemForm();
    
    /**
     * @brief 使用新架构初始化
     */
    void InitializeWithServices(std::shared_ptr<cga::application::ServiceFactory> serviceFactory);

signals:
    void QueueDropItem(int itempos, int itemid);
public slots:
    void OnNotifyGetInfoFailed(bool bIsConnected, bool bIsInGame);
    void OnNotifyGetItemsInfo(QSharedPointer<CGA_ItemList_t> items);
    void OnCloseWindow();

private slots:
    void on_tableView_customContextMenuRequested(const QPoint &pos);
    void OnDropItemAction();
    void OnAddAutoDropAction();
    void OnAddAutoTweakAction();
    void AddItemDropper(const QString &str);
    void AddItemTweaker(const QString &str);
    void on_lineEdit_drop_returnPressed();
    void on_lineEdit_tweak_returnPressed();
    void OnItemDropperDeletePressed(int row);
    void OnItemTweakerDeletePressed(int row);

private:
    void GetRowColumnFromItemPos(int itempos, int &row, int &col);
    bool GetItemTips(int itemid, QString &str);
    
private:
    Ui::ItemForm *ui;
    
    // 旧架构 (向后兼容)
    CPlayerWorker *m_worker;
    
    // 新架构
    std::shared_ptr<cga::application::ServiceFactory> m_serviceFactory;
    
    QStandardItemModel *m_model_Item;
    QMenu *m_item_Menu;
    QHash<int, QString> m_IdMap;
    
public:
    CItemDropperModel *m_model_Drop;
    CItemTweakerModel *m_model_Tweaker;
    
public slots:
    bool ParseItemTweaker(const QJsonValue &val);
    bool ParseItemDropper(const QJsonValue &val);
    bool ParseItemIdMap(const QJsonValue &val);
    void SaveItemDropper(QJsonArray &arr);
    void SaveItemTweaker(QJsonArray &arr);
    void SaveItemIdMap(QJsonObject &obj);
    
private:
    // 新架构方法
    void UpdateItemsInfoNew();
};

#endif // ITEMFORM_H
