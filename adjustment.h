#ifndef ADJUSTMENT_H
#define ADJUSTMENT_H

#include "element.h"

#include <QDialog>

namespace Ui {
class Adjustment;
}

/*
 *  Adjustement
 */

class Adjustment : public DialogElement
{
    Q_OBJECT

public:
    explicit Adjustment(QWidget *parent = 0);
    ~Adjustment();

protected slots:
    void checkSite();

protected:
    Ui::Adjustment *ui;
    bool eventFilter(QObject *object, QEvent *event);

    QString siteInventory_m;
};

/*
 *  New Adjustement
 */

class NewAdjustement : public Adjustment
{
    Q_OBJECT

public:
    explicit NewAdjustement(QWidget *parent = 0);
    ~NewAdjustement();

protected:
    void accept();
    void reject();
    void closeEvent(QCloseEvent *e);
    bool processItem(const QStringList queryList, const QString quantity, const QString uniqueIdentifier) const;


protected slots:
    void addItem(const QString itemId, const QString line, const QString quantity, const QString uniqueIdentifier) const;
    void addItem(const QString itemId, const QString line, const QString quantity) const;
    void openNewTransaction(const QString itemId, const QString line, const QString quantity, const QString uniqueIdentifier);
    void getStockQuantity(const QString itemId, const QString manufacturer, const QString partNumber, const QString productName, const QString uniqueIdentifierEnabled);
    void maybeAddItem();
    void maybeCreateNewTransaction();
    void clearItemFields();
    void updateQuantityLabel();

signals:
    void itemAdded();

private:
    QString uniqueIdentifierEnabled_m;
    QString itemId_m;
};

#endif // ADJUSTMENT_H
