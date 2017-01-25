#include "element.h"

Element::Element()
{

}

Element::~Element()
{

}

DialogElement::DialogElement(QWidget *parent) :
   QDialog(parent), Element()
{

}

DialogElement::~DialogElement()
{

}

bool Element::TREMessageBox(QString str) const
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Gestionnaire d'inventaire Ondenet");
    msgBox.setText("Erreur de requête");
    msgBox.setInformativeText(QString("Impossible d'ouvrir une transaction %1").arg(str));
    msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Retry);
    msgBox.setDefaultButton(QMessageBox::Retry);

    if (msgBox.exec() == QMessageBox::Retry)
        return true;

    return false;
}

bool Element::IVAMessageBox(QString str) const
{
    QMessageBox msgBox;
    msgBox.setText(QString("%1 invalide").arg(str));
    msgBox.setInformativeText("Entrez une valeur valide pour continuer.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    if (msgBox.exec() == QMessageBox::Ok)
        return true;

    return false;
}

bool Element::AEXMessageBox(QString str) const
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Gestionnaire d'inventaire Ondenet");
    msgBox.setText(QString("%1 existe déjà").arg(str));
    msgBox.setInformativeText("Voulez-vous ouvrir ce compte?");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::Yes);

    if (msgBox.exec() == QMessageBox::Yes)
        return true;

    return false;
}

bool Element::EELMessageBox(QString str) const
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Gestionnaire d'inventaire Ondenet");
    msgBox.setText(QString("%1 vide").arg(str));
    msgBox.setInformativeText("Entrez une valeur valide pour continuer.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    if (msgBox.exec() == QMessageBox::Ok)
        return true;

    return false;
}

bool Element::IEXMessageBox(QString str) const
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Gestionnaire d'inventaire Ondenet");
    msgBox.setText(QString("%1 inexistant").arg(str));
    msgBox.setInformativeText("Entrez une valeur valide pour continuer.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    if (msgBox.exec() == QMessageBox::Ok)
        return true;

    return false;
}

bool Element::QERMessageBox(QString str) const
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Gestionnaire d'inventaire Ondenet");
    msgBox.setText("Erreur de requête SQLite");
    msgBox.setInformativeText(str);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    if (msgBox.exec() == QMessageBox::Ok)
        return true;

    return false;
}

bool Element::ITCMessageBox(QString str) const
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Gestionnaire d'inventaire Ondenet");
    msgBox.setText(QString("Impossible de créer %1").arg(str));
    msgBox.setInformativeText("Vérifiez que vous disposez des permissions nécessaire pour effectuer l'opération.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    if (msgBox.exec() == QMessageBox::Ok)
        return true;

    return false;
}

bool Element::INTMessageBox() const
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Gestionnaire d'inventaire Ondenet");
    msgBox.setText("Erreur interne");
    msgBox.setInformativeText("Veuillez réessayer");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    if (msgBox.exec() == QMessageBox::Ok)
        return true;

    return false;
}

bool Element::ROEMessageBox() const
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Gestionnaire d'inventaire Ondenet");
    msgBox.setText("Erreur de rollback");
    msgBox.setInformativeText("Veuillez communiquer avec votre administrateur afin de vérifier l'intégrité de la base de données.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    if (msgBox.exec() == QMessageBox::Ok)
        return true;

    return false;
}

bool Element::NTRMessageBox() const
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Gestionnaire d'inventaire Ondenet");
    msgBox.setText("Impossible de complèter la transaction");
    msgBox.setInformativeText(QString("Il n'existe aucune transaction ouverte."));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    if (msgBox.exec() == QMessageBox::Ok)
        return true;

    return false;
}

bool Element::IQYMessageBox(QString item, QString technician) const
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Gestionnaire d'inventaire Ondenet");
    msgBox.setText("Quantité insuffisante");
    msgBox.setInformativeText(QString("Il n'y a aucun %1 dans l'inventaire de %2.")
                                      .arg(item).arg(technician));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    if (msgBox.exec() == QMessageBox::Ok)
        return true;

    return false;
}

bool Element::IQTMessageBox() const
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Gestionnaire d'inventaire Ondenet");
    msgBox.setText("Quantité invalide");
    msgBox.setInformativeText("Veuillez entrer une quantité valide.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    if (msgBox.exec() == QMessageBox::Ok)
        return true;

    return false;
}


WidgetElement::WidgetElement(QWidget *parent) :
    QWidget(parent), Element()
{

}

WidgetElement::~WidgetElement()
{

}
