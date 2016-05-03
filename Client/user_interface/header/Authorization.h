//  Authorization.h
#ifndef _Authorization_h_
#define _Authorization_h_

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include "port.hpp"
#include "PortController.h"

class Authorization : public QDialog, public InterfaceItem  
{
    Q_OBJECT
private:

    bool authSuccess = false;

    QCheckBox* m_pcbSIGNIN;
    QCheckBox* m_pcbSIGNUP;

    QLineEdit* m_ptxtLogin;
    QLineEdit* m_ptxtPassword;

    QPushButton* pcmdOk;
    QPushButton* pcmdCancel;

public:
    Authorization(PortController* _port, QWidget* parent = 0);
    ~Authorization();
    void update();
    QString login() const;
    QString password() const;  
public slots:
    void setBoxInEnabled();
    void setBoxUpEnabled();
    void setOkEnabled();
    void okButtonClicked();
    //void onUpdate();
signals:
    void wasUpdated();
};

class WaitMessage : public QDialog
{
public:
    WaitMessage(QWidget* parent = 0)
    {
        QLabel* plblWait = new QLabel("Wait, please!", this);
    }
};

#endif  //_Authorization_h_