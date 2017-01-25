


#ifndef INVENTORYTRANSFER_H
#define INVENTORYTRANSFER_H

#include "element.h"

namespace Ui {
class InventoryTransfer;
}

/*
 *  Inventory Transfer
 */

class InventoryTransfer : public DialogElement
{
    Q_OBJECT

public:
    explicit InventoryTransfer(QWidget *parent = 0);
    ~InventoryTransfer();

protected slots:
    void checkFromAccount();
    void checkToAccount();

protected:
    Ui::InventoryTransfer *ui;
    bool eventFilter(QObject *object, QEvent *event);
    void getAccountInformation(const int sender, const QString account);
        /*
         * Sender :
         * 0 - From
         * 1 - To
         */

    QString fromInventory_m;
    QString toInventory_m;
    int fromType;
        /* 0 - Tech
         * 1 - Site
         */
    int toType;
};

/*
 *  New Inventory Transfer
 */

class NewInventoryTransfer : public InventoryTransfer
{
    Q_OBJECT

public:
    explicit NewInventoryTransfer(QWidget *parent = 0);
    ~NewInventoryTransfer();

protected:
    void accept();
    void reject();
    void closeEvent(QCloseEvent *e);
    bool processItem(const QStringList queryList, const QString quantity, const QString uniqueIdentifier) const;

protected slots:
    void addItem(const QString itemId, const QString line, const QString quantity, const QString uniqueIdentifier) const;
    void addItem(const QString itemId, const QString line, const QString quantity) const;
    void openNewTransaction(const QString itemId, const QString line, const QString quantity, const QString uniqueIdentifier);
    void getAllocatedQuantity(const QString itemId, const QString manufacturer, const QString partNumber, const QString productName, const QString uniqueIdentifierEnabled);
    void maybeAddItem();
    void maybeCreateNewTransaction();
    void clearItemFields();
    void updateQuantityLabel();


signals:
    void itemAdded();

private:
    QString uniqueIdentifierEnabled_m;
    QString itemId_m;
    QStringList allocatedIdentifierList_m;
    QString allocatedQuantity_m;
};

#endif // INVENTORYTRANSFER_H
