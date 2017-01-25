#ifndef REFINE_H
#define REFINE_H

#include "ui_stockrefine.h"
#include "ui_manufacturerrefine.h"
#include "ui_supplierrefine.h"
#include "ui_technicianrefine.h"

#include <QWidget>
#include <QtGui>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

namespace Ui {
class StockRefine;
class ManufacturerRefine;
class SupplierRefine;
class TechnicianRefine;
}

/*
 *  REFINE
 */

class Refine : public QWidget
{
    Q_OBJECT

public:
    explicit Refine(QWidget *parent = 0);
    ~Refine();

signals:
    void doRefineElement();

protected slots:
    virtual void newElement() = 0;
    virtual void makeRefineElement() = 0;
};

/*
 *  STOCK REFINE
 */

class StockRefine : public Refine
{
    Q_OBJECT

public:
    explicit StockRefine(QWidget *parent = 0);
    ~StockRefine();

signals:
    void doRefineElement(bool underMin, bool overMax, QString manufacturer, QString name, QString localisation);

protected slots:
    void newElement();
    void makeRefineElement();

private:
    Ui::StockRefine *ui;
};

/*
 *  ACCOUNT REFINE
 */

class AccountRefine : public Refine
{
    Q_OBJECT

public:
    AccountRefine(QWidget *parent = 0);
    ~AccountRefine();

signals:
    void doRefineElement(QString name, QString id);


protected slots:
    virtual void newElement() = 0;
    virtual void makeRefineElement() = 0;
};

/*
 *  MANUFACTURER REFINE
 */

class ManufacturerRefine : public AccountRefine
{
    Q_OBJECT

public:
    explicit ManufacturerRefine(QWidget *parent = 0);
    ~ManufacturerRefine();

protected slots:
    void newElement();
    void makeRefineElement();

private:
    Ui::ManufacturerRefine *ui;
};

/*
 *  SUPPLIER REFINE
 */

class SupplierRefine : public AccountRefine
{
    Q_OBJECT

public:
    explicit SupplierRefine(QWidget *parent = 0);
    ~SupplierRefine();

protected slots:
    void newElement();
    void makeRefineElement();

private:
    Ui::SupplierRefine *ui;
};

/*
 *  TECHNICIAN REFINE
 */

class TechnicianRefine : public AccountRefine
{
    Q_OBJECT

public:
    explicit TechnicianRefine(QWidget *parent = 0);
    ~TechnicianRefine();

protected slots:
    void newElement();
    void makeRefineElement();

private:
    Ui::TechnicianRefine *ui;
};

#endif // REFINE_H
