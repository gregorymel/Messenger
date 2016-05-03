#ifndef CHATTERBOX_H
#define CHATTERBOX_H

#include <QMainWindow>
#include <QWidget>
#include <QTextEdit>
#include <QMainWindow>
#include <QGridLayout>
#include <QFrame>
#include <QPushButton>
#include <QListWidget>
#include <QTabBar>
#include <QLabel>
#include <QStackedWidget>
#include "PortController.h"

class FriendList;
class MessageBlock;

class ChatterBox : public QMainWindow
{
    Q_OBJECT
public:
    ChatterBox(PortController* _port, QWidget* parent = 0);
    ~ChatterBox();
    QString login() const { return _login_; }
private:
    PortController* port;

    // MessageBlock
    MessageBlock *pmsgBlock;

    // FriendList
    FriendList *friendList;
    
    // Active chats
    QTabBar *pbarActiveChats;

    QString _login_;

    void createFormIterior();
};

class MessageBlock : public QWidget, public InterfaceItem
{
    Q_OBJECT
public:
    MessageBlock(QString& _login, PortController* _port, QWidget *parent = 0);
    ~MessageBlock(){};
    void update();
    void updateState(QString name);
    void showCurrent();

    QString curFriend; // this atribute is neccessary for FriendList 
private:
    QString& login;
     // StackWidget
    QStackedWidget *pswFriendField;
    QMap<QString, int> stackIdxMap;
public slots:
    void sendMessage(QString body) const;
};


class MessageBox : public QWidget
{
    Q_OBJECT
public:
    MessageBox(QWidget *parent = 0);
    ~MessageBox();
    void addMessage(const QString body);
private:
    // Chat box
    QTextEdit *ptxtEdit;
    // Button Say
    QPushButton *pbtnSay;
    // Chat block
    QTextEdit *ptxtChat;
public slots:
    void sayButtonClicked();
signals:
    void messageIsCompleted(QString body);
};


class FriendList : public QWidget, public InterfaceItem
{
    Q_OBJECT
public:
    FriendList(PortController* _port, MessageBlock* block, QWidget *parent = 0);
    ~FriendList(){};
    void update();
private:
    MessageBlock* connectedBlock;
    // Friend List
    QListWidget *plstFriendList;
    // Add friens section
    QPushButton *pbtnAdd;
    QLineEdit *plnAdd;
public slots:
    void setAddEnabled();
    void addButtonClicked();
    void friendListClicked(const QModelIndex& index);
};



#endif // CHATTERBOX_H