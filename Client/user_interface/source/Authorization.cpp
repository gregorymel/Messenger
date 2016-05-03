#include <sstream>
#include "Authorization.h"
#include "ChatterBox.h"
#include "Stanza.hpp"
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <iostream>
#include <QMessageBox>

Authorization::Authorization(PortController* _port, QWidget* parent/*= 0*/)
     : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
       InterfaceItem( _port)
{
    port->attach(this);

    // LineEdit
    m_ptxtLogin = new QLineEdit;
    m_ptxtPassword  = new QLineEdit;
    m_ptxtPassword->setEchoMode(QLineEdit::Password);
    connect( m_ptxtLogin, SIGNAL(textChanged(QString)), this, SLOT(setOkEnabled()));
    connect( m_ptxtPassword, SIGNAL(textChanged(QString)), this, SLOT(setOkEnabled()));

    //Label
    QLabel* plblLogin = new QLabel("&Login");
    QLabel* plblPassword = new QLabel("&Password");
    plblLogin->setBuddy(m_ptxtLogin);
    plblPassword->setBuddy(m_ptxtPassword);

    // Buttons
    pcmdOk = new QPushButton("&Ok");
    pcmdOk->setEnabled( false);
    pcmdCancel = new QPushButton("&Cancel");
    connect(pcmdOk, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
    connect(pcmdCancel, SIGNAL(clicked()), SLOT(reject()));

    // CheckBoxes
    m_pcbSIGNIN = new QCheckBox("SignIn");
    m_pcbSIGNUP = new QCheckBox("SignUp");
    connect(m_pcbSIGNIN, SIGNAL(clicked()), this, SLOT(setBoxInEnabled()));
    connect(m_pcbSIGNUP, SIGNAL(clicked()), this, SLOT(setBoxUpEnabled()));
    connect(m_pcbSIGNIN, SIGNAL(clicked()), this, SLOT(setOkEnabled()));
    connect(m_pcbSIGNUP, SIGNAL(clicked()), this, SLOT(setOkEnabled()));


    //Layout setup
    QGridLayout* ptopLayout = new QGridLayout;
    ptopLayout->addWidget(m_pcbSIGNIN, 0, 0);
    ptopLayout->addWidget(m_pcbSIGNUP, 0, 1);
    ptopLayout->addWidget(plblLogin, 1, 0);
    ptopLayout->addWidget(plblPassword, 2, 0);
    ptopLayout->addWidget(m_ptxtLogin, 1, 1);
    ptopLayout->addWidget(m_ptxtPassword, 2, 1);
    ptopLayout->addWidget(pcmdOk, 3, 0);
    ptopLayout->addWidget(pcmdCancel, 3, 1);
    setLayout(ptopLayout);
}

QString Authorization::login() const
{
    return m_ptxtLogin->text();
}

QString Authorization::password() const
{
    return m_ptxtPassword->text ();
}

void Authorization::setBoxInEnabled()
{
    if ( m_pcbSIGNIN->isChecked())
        m_pcbSIGNUP->setChecked( false);
    else
        m_pcbSIGNUP->setEnabled( true);
}

void Authorization::setBoxUpEnabled()
{
    if ( m_pcbSIGNUP->isChecked())
        m_pcbSIGNIN->setChecked( false);
    else
        m_pcbSIGNIN->setChecked( true);
}

void Authorization::setOkEnabled()
{
    if (!m_ptxtLogin->text().isEmpty() && !m_ptxtPassword->text().isEmpty()
        && (m_pcbSIGNIN->isChecked() || m_pcbSIGNUP->isChecked()))
        pcmdOk->setEnabled( true);
    else
        pcmdOk->setEnabled( false);
}

void Authorization::update()
{
    if (port->getStanza().getStanzaType() == Stanza::IQ)
    {
        if (port->getStanza().getSubType() == Stanza::ERROR)
            authSuccess = false;
        
        if (port->getStanza().getSubType() == Stanza::AVAILABLE)
            authSuccess = true;
    }

    std::cerr << "here" << std::this_thread::get_id() << std::endl;
    emit wasUpdated();
}

void Authorization::okButtonClicked()
{
    Stanza sign_msg;
    sign_msg.setStanzaType(Stanza::IQ);
    std::stringstream ss; 
    ss << login().toStdString() << '\n' << password().toStdString();  

    if ( m_pcbSIGNIN->isChecked())
    {
        sign_msg.setMSG(Stanza::SIGNIN, ss.str());
    } else
    {
        sign_msg.setMSG(Stanza::SIGNUP, ss.str());
    }

    port->send(sign_msg);

    WaitMessage* pwaitMessage = new WaitMessage;
    connect(this, SIGNAL(wasUpdated()), pwaitMessage, SLOT(accept()));
    pwaitMessage->exec();
    delete pwaitMessage;
    m_ptxtPassword->clear();

    if ( authSuccess == true)
    {
        if ( m_pcbSIGNUP->isChecked())
        {
            QMessageBox::information(0,
                                     "Information",
                                     "Your login: "
                                     + login()
                                     + "\nTry to signup!"
                                    );
        }
        else
            accept();
    }

    std::cerr << "end" << std::endl;
}

Authorization::~Authorization()
{
    port->detach(this);    
}
