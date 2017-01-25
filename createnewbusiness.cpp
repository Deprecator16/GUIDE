#include "createnewbusiness.h"
#include "ui_createnewbusiness.h"

#include "mainwindow.h"

#include <QtXml/QDomDocument>
#include <QtSql>
#include <QFileDialog>
#include <QSettings>
#include <QTextStream>
#include <QMessageBox>

CreateNewBusiness::CreateNewBusiness(QWidget *parent) :
    DialogElement(parent),
    ui(new Ui::CreateNewBusiness)
{
    ui->setupUi(this);

    this->setWindowTitle("GUIDE");

    connect(ui->cancel_PushButton, SIGNAL(pressed()),
            this, SLOT(reject()));
    connect(ui->create_PushButton, SIGNAL(pressed()),
            this, SLOT(accept()));
    connect(ui->directory_PushButton, SIGNAL(pressed()),
            this, SLOT(getBusinessDirectory()));
}

CreateNewBusiness::~CreateNewBusiness()
{
    delete ui;
}

void CreateNewBusiness::accept()
{
    QString directory = ui->directory_LineEdit->text();
    QString filename;
    QString businessName = ui->businessName_LineEdit->text().toLower();

    for(int i = 0; i < businessName.length(); i++)
    {
        if (businessName.at(i).isLetter() == true)
            filename.append(businessName.at(i));
    }

    QString pathToDatabase = directory + QString("/%1.guide").arg(filename);

    bool ret = createDatabase(pathToDatabase);

    if (ret == true)
    {
        QSettings settings ("GUIDE", "Samuel Drouin");
        settings.setValue("Business/DefaultDatabase", pathToDatabase);
        emit doReadSettings();

        QSqlDatabase::removeDatabase("QSQLITE");

        QDialog::accept();
    }
}

bool CreateNewBusiness::createDatabase(QString pathToDatabase)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(pathToDatabase);

    QStringList queryString;

    queryString.append(QString("CREATE TABLE item"
                               "("
                                   "item_id                      INTEGER PRIMARY KEY     AUTOINCREMENT, "
                                   "manufacturer_code            TEXT       NOT NULL     REFERENCES manufacturer (manufacturer_code),"
                                   "part_number                  TEXT       NOT NULL,"
                                   "product_name                 TEXT       NOT NULL,"
                                   "localisation                 TEXT,  "
                                   "minimum                      INT        DEFAULT 0,"
                                   "maximum                      INT        DEFAULT 0,"
                                   "unique_identifier_enabled    INT        DEFAULT 0     NOT NULL,"
                                   "UNIQUE (manufacturer_code, part_number)"
                               ")")); // Identifier enabled : 0 - No, 1 - Yes

    queryString.append(QString("CREATE TABLE history"
                               "("
                                   "item_id              INT        REFERENCES item (item_id),"
                                   "quantity             TEXT       NOT NULL     DEFAULT 0,"
                                   "unique_identifier    TEXT,"
                                   "date                 TEXT       NOT NULL,"
                                   "event_type           TEXT       NOT NULL,"
                                   "reference            TEXT,"
                                   "note                 TEXT"
                               ")"));

    queryString.append(QString("CREATE TABLE manufacturer"
                               "("
                                   "manufacturer_code    TEXT      PRIMARY KEY      NOT NULL,"
                                   "manufacturer_name    TEXT      NOT NULL"
                               ") WITHOUT ROWID"));

    queryString.append(QString("CREATE TABLE technician"
                               "("
                                   "technician_id          INT        PRIMARY KEY    NOT NULL,"
                                   "technician_name        TEXT       NOT NULL,"
                                   "phone_number           TEXT,"
                                   "email                  TEXT,"
                                   "note                   TEXT"
                               ")"));

    queryString.append(QString("CREATE TABLE supplier"
                               "("
                                   "supplier_id            INT        PRIMARY KEY   NOT NULL,"
                                   "supplier_name          TEXT       NOT NULL,"
                                   "phone_number           TEXT,"
                                   "email                  TEXT,"
                                   "note                   TEXT"
                               ")"));

    queryString.append(QString("CREATE TABLE installation"
                               "("
                                   "installation_id      INTEGER PRIMARY KEY     AUTOINCREMENT   NOT NULL,"
                                   "technician_id        TEXT                    NOT NULL        REFERENCES technician(technician_id),"
                                   "customer             TEXT,"
                                   "tower                TEXT,"
                                   "date                 TEXT                    NOT NULL"
                               ")"));

    queryString.append(QString("CREATE TABLE installed"
                               "("
                                   "item_id              INT         REFERENCES item (item_id),"
                                   "comment              TEXT,"
                                   "installation_id      INT         NOT NULL      REFERENCES installation (installation_id),"
                                   "line                 INT         NOT NULL,"
                                   "quantity             INT,"
                                   "unique_identifier    TEXT,"
                                   "PRIMARY KEY (installation_id, line)"
                               ")"));

    queryString.append(QString("CREATE TABLE inventory_transfer"
                               "("
                                   "transfer_id                INTEGER PRIMARY KEY     AUTOINCREMENT      NOT NULL,"
                                   "from_technician_id         TEXT                    REFERENCES technician(technician_id),"
                                   "from_site_id               TEXT                    REFERENCES site(site_code),"
                                   "to_technician_id           TEXT                    REFERENCES technician(technician_id),"
                                   "to_site_id                 TEXT                    REFERENCES site(site_code),"
                                   "date                       TEXT                    NOT NULL,"
                                   "reference                  TEXT"
                               ")"));

    queryString.append(QString("CREATE TABLE transfered"
                               "("
                                   "item_id              INT         REFERENCES item (item_id),"
                                   "transfer_id          INT         NOT NULL      REFERENCES inventory_transfer (transfer_id),"
                                   "line                 INT         NOT NULL,"
                                   "quantity             INT         NOT NULL      DEFAULT 0,"
                                   "unique_identifier    TEXT,"
                                   "PRIMARY KEY (transfer_id, line)"
                               ")"));

    queryString.append(QString("CREATE TABLE purchase"
                               "("
                                   "purchase_id            INTEGER PRIMARY KEY     AUTOINCREMENT     NOT NULL,"
                                   "supplier_id            TEXT                    NOT NULL,"
                                   "reference              TEXT,"
                                   "purchase_state         TEXT                    NOT NULL,"
                                   "date                   TEXT                    NOT NULL,"
                                   "comment                TEXT"
                               ")")); // State : 0 - Created, 1 - Received

    queryString.append(QString("CREATE TABLE purchased"
                               "("
                                   "item_id              INT         REFERENCES item (item_id),"
                                   "comment              TEXT,"
                                   "purchase_id          INT         NOT NULL      REFERENCES purchase (purchase_id),"
                                   "line                 INT         NOT NULL,"
                                   "quantity             INT         NOT NULL      DEFAULT 0,"
                                   "unique_identifier    TEXT,"
                                   "received_quantity    INT,"
                                   "PRIMARY KEY (purchase_id, line)"
                               ")")); // Received quantity : All - Completly received

    queryString.append(QString("CREATE TABLE adjustment"
                               "("
                                   "adjustment_id              INTEGER PRIMARY KEY     AUTOINCREMENT      NOT NULL,"
                                   "site_code                  TEXT                    NOT NULL           REFERENCES site(site_code),"
                                   "date                       TEXT                    NOT NULL,"
                                   "reference                  TEXT"
                               ")"));

    queryString.append(QString("CREATE TABLE adjusted"
                               "("
                                   "item_id              INT         REFERENCES item (item_id),"
                                   "adjustment_id        INT         NOT NULL      REFERENCES adjustment (adjustment_id),"
                                   "line                 INT         NOT NULL,"
                                   "quantity             INT         NOT NULL      DEFAULT 0,"
                                   "unique_identifier    TEXT,"
                                   "PRIMARY KEY (adjustment_id, line)"
                               ")"));

    queryString.append(QString("CREATE TABLE site"
                               "("
                                   "site_code    TEXT      NOT NULL       PRIMARY KEY,"
                                   "name         TEXT      NOT NULL"
                               ")"));

    queryString.append(QString("CREATE TABLE account"
                               "("
                                   "initials        TEXT        NOT NULL        PRIMARY KEY,"
                                   "name            TEXT"
                               ")"));

    queryString.append(QString("CREATE TABLE state"
                               "("
                                   "item_id              INT        REFERENCES item (item_id),"
                                   "quantity             TEXT       NOT NULL     DEFAULT 0,"
                                   "unique_identifier    TEXT,"
                                   "date                 TEXT       NOT NULL,"
                                   "event_type           TEXT       NOT NULL,"
                                   "is_stock             INT, " // 0 - TRUE
                                   "reference            TEXT, "
                                   "note                 TEXT,"
                                   "PRIMARY KEY (item_id, event_type, unique_identifier)"
                               ")"));

    queryString.append(QString("CREATE TRIGGER update_history "
                              "AFTER UPDATE OF date "
                                           "ON state "
                               "BEGIN "
                                   "INSERT INTO history "
                                                        "( "
                                                            "item_id, "
                                                            "unique_identifier, "
                                                            "quantity, date, "
                                                            "event_type, "
                                                            "reference, "
                                                            "note"
                                                        ") "
                                                        "VALUES "
                                                        "( "
                                                            "OLD.item_id, "
                                                            "OLD.unique_identifier, "
                                                            "(NEW.quantity - OLD.quantity), "
                                                            "NEW.date, "
                                                            "NEW.event_type, "
                                                            "NEW.reference, "
                                                            "NEW.note"
                                                         "); "
                               "END; "));

    queryString.append(QString("CREATE TRIGGER new_history "
                                 "AFTER INSERT "
                                           "ON state "
                                   "BEGIN "
                                       "INSERT INTO history "
                                                            "( "
                                                            "item_id,"
                                                            "unique_identifier, "
                                                            "quantity, "
                                                            "date, "
                                                            "event_type, "
                                                            "reference, "
                                                            "note "
                                                            ") "
                                                            "VALUES "
                                                            "( "
                                                            "NEW.item_id, "
                                                            "NEW.unique_identifier, "
                                                            "NEW.quantity, "
                                                            "NEW.date, "
                                                            "NEW.event_type, "
                                                            "NEW.reference, "
                                                            "NEW.note"
                                                            "); "
                               "END; "));
    db.open();
    if (db.lastError().isValid())
    {
        QERMessageBox(db.lastError().text());
        return false;
    }
    db.transaction();
    if (db.lastError().isValid())
    {
        QERMessageBox(db.lastError().text());
        return false;
    }
    for (int i = 0; i < queryString.count(); i++)
    {
        QSqlQuery query;
        query.exec(queryString.at(i));
        if (query.lastError().isValid())
        {
            QERMessageBox(query.lastError().text());
            db.rollback();
            return false;
        }
    }
    db.commit();
    if (db.lastError().isValid())
    {
        QERMessageBox(db.lastError().text());
        db.rollback();
        return false;
    }
    db.close();
    if (db.lastError().isValid())
    {
        QERMessageBox(db.lastError().text());
    }

    return true;
}

void CreateNewBusiness::getBusinessDirectory()
{
    QString directory = QFileDialog::getExistingDirectory(this);
    if(!directory.isEmpty())
        ui->directory_LineEdit->setText(directory);
}
