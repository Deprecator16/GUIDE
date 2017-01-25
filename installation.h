#ifndef INVOICE_H
#define INVOICE_H

#include "element.h"

#include <QDialog>

namespace Ui {
class Installation;
}

/*
 *  Installation
 */

class Installation : public DialogElement
{
    Q_OBJECT

public:
    explicit Installation(QWidget *parent = 0);
    ~Installation();

protected slots:
    void checkTechnician();

protected:
    Ui::Installation *ui;
    bool eventFilter(QObject *object, QEvent *event);

    QString technicianInventory_m;
};

/*
 *  New Installation
 */


class NewInstallation : public Installation
{
    Q_OBJECT

public:
    explicit NewInstallation(QWidget *parent = 0);
    ~NewInstallation();

protected:
    void accept();
    void reject();
    void closeEvent(QCloseEvent *e);
    bool processItem(const QStringList queryList, const QString quantity, const QString uniqueIdentifier) const;


protected slots:
    void addItem(const QString itemId, const QString line, const QString quantity, const QString uniqueIdentifier);
    void addItem(const QString itemId, const QString line, const QString quantity);
    void addComment(const QString line, const QString comment);
    void openNewTransaction(QString itemId, QString comment, QString line, QString quantity, QString uniqueIdentifier, int type);
    void getAllocatedQuantity(const QString itemId, const QString manufacturer, const QString partNumber, const QString productName, const QString uniqueIdentifierEnabled);
    void maybeAddItem();
    void maybeAddComment();
    void maybeCreateNewTransaction(int type); // type : 0 - Item, 1 - Comment
    void clearItemFields();
    void clearCommentFields();
    void updateQuantityLabel();

private:
    QStringList allocatedIdentifierList_m;
    QString uniqueIdentifierEnabled_m;
    QString itemId_m;
    QString allocatedQuantity_m;
};

#endif // INVOICE_H
