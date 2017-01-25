#include "refine.h"
#include "mainwindow.h"
#include "item.h"
#include "manufacturer.h"
#include "supplier.h"
#include "technician.h"

/*
 *  REFINE
 */

Refine::Refine(QWidget *parent) :
    QWidget(parent)
{

}

Refine::~Refine()
{

}

/*
 *  STOCK REFINE
 */

StockRefine::StockRefine(QWidget *parent) :
    Refine(parent),
    ui(new Ui::StockRefine)
{
    ui->setupUi(this);

    connect(ui->refine_PushButton, SIGNAL(clicked()),
            this, SLOT(makeRefineElement()));
    connect(ui->new_PushButton, SIGNAL(clicked()),
            this, SLOT(newElement()));
}

StockRefine::~StockRefine()
{
    delete ui;
}

void StockRefine::makeRefineElement()
{
    emit doRefineElement(ui->underMinimum_CheckBox->isChecked(), ui->overMaximum_CheckBox->isChecked(),
                         ui->manufacturer_LineEdit->text(), ui->item_LineEdit->text(), ui->localisation_LineEdit->text());
    ui->underMinimum_CheckBox->setChecked(false);
    ui->overMaximum_CheckBox->setChecked(false);
    ui->manufacturer_LineEdit->clear();
    ui->item_LineEdit->clear();
    ui->localisation_LineEdit->clear();
}

void StockRefine::newElement()
{
    NewItem *newItem = new NewItem(this);
    newItem->show();
}

/*
 *  ACCOUNT REFINE
 */

AccountRefine::AccountRefine(QWidget *parent)
    : Refine(parent)
{

}

AccountRefine::~AccountRefine()
{

}

/*
 *  MANUFACTURER REFINE
 */

ManufacturerRefine::ManufacturerRefine(QWidget *parent)
    : AccountRefine(parent),
      ui(new Ui::ManufacturerRefine)
{
    ui->setupUi(this);
    connect(ui->new_PushButton, SIGNAL(clicked()),
            this, SLOT(newElement()));
    connect(ui->refine_PushButton, SIGNAL(clicked()),
            this, SLOT(makeRefineElement()));

    // Navigation
    setTabOrder(ui->name_LineEdit, ui->code_LineEdit);
    connect(ui->name_LineEdit, SIGNAL(returnPressed()),
            ui->code_LineEdit, SLOT(setFocus()));
    connect(ui->name_LineEdit, SIGNAL(returnPressed()),
            ui->code_LineEdit, SLOT(selectAll()));
    setTabOrder(ui->code_LineEdit, ui->name_LineEdit);
    connect(ui->code_LineEdit, SIGNAL(returnPressed()),
            ui->name_LineEdit, SLOT(setFocus()));
    connect(ui->code_LineEdit, SIGNAL(returnPressed()),
            ui->name_LineEdit, SLOT(selectAll()));
}

ManufacturerRefine::~ManufacturerRefine()
{

}

void ManufacturerRefine::makeRefineElement()
{
    emit doRefineElement(ui->name_LineEdit->text(), ui->code_LineEdit->text());
    ui->name_LineEdit->clear();
    ui->code_LineEdit->clear();
}

void ManufacturerRefine::newElement()
{
    NewManufacturer *newManufacturer = new NewManufacturer(this);
    newManufacturer->show();
}

/*
 *  SUPPLIER REFINE
 */

SupplierRefine::SupplierRefine(QWidget *parent)
    : AccountRefine(parent), ui(new Ui::SupplierRefine)
{
    ui->setupUi(this);
    connect(ui->new_PushButton, SIGNAL(clicked()),
            this, SLOT(newElement()));
    connect(ui->refine_PushButton, SIGNAL(clicked()),
            this, SLOT(makeRefineElement()));
}

SupplierRefine::~SupplierRefine()
{

}

void SupplierRefine::makeRefineElement()
{
    emit doRefineElement(ui->name_LineEdit->text(), ui->number_LineEdit->text());
    ui->name_LineEdit->clear();
    ui->number_LineEdit->clear();
}

void SupplierRefine::newElement()
{
    NewSupplier *newSupplier = new NewSupplier(this);
    newSupplier->show();
}

/*
 *  TECHNICIAN REFINE
 */

TechnicianRefine::TechnicianRefine(QWidget *parent)
    : AccountRefine(parent), ui(new Ui::TechnicianRefine)
{
    ui->setupUi(this);
    connect(ui->new_PushButton, SIGNAL(clicked()),
            this, SLOT(newElement()));
    connect(ui->refine_PushButton, SIGNAL(clicked()),
            this, SLOT(makeRefineElement()));
}

TechnicianRefine::~TechnicianRefine()
{

}

void TechnicianRefine::makeRefineElement()
{
    emit doRefineElement(ui->name_LineEdit->text(), ui->number_lineEdit->text());
    ui->name_LineEdit->clear();
    ui->number_lineEdit->clear();
}

void TechnicianRefine::newElement() {
    NewTechnician *newTechnician = new NewTechnician(this);
    newTechnician->show();
}
