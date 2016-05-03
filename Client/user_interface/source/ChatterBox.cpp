
#include "ChatterBox.h"
#include "Authorization.h"
#include <QMap>

ChatterBox::ChatterBox(PortController* _port, QWidget* parent) : 
                                                        QMainWindow(parent),
                                                        port(_port) 
{
    setWindowTitle("Kaspiyskiy");
    Authorization* pAuthorization = new Authorization( _port);
    if (pAuthorization->exec() == QDialog::Accepted)
    {
        _login_ = pAuthorization->login();
        std::cerr << login().toStdString() << std::endl;
        createFormIterior();
    }
    delete pAuthorization;
}

void ChatterBox::createFormIterior()
{
    QFrame *mainWidget = new QFrame(this);
    QGridLayout *grid = new QGridLayout(this);

    // MessageBlock
    pmsgBlock = new MessageBlock(_login_, port, this);
    grid->addWidget(pmsgBlock, 1, 0, 17, 16);

    // FriendList
    FriendList* pfriendList = new FriendList(port, pmsgBlock, this);
    grid->addWidget(pfriendList, 0, 17, 18, 3);

    // Active chats
    pbarActiveChats = new QTabBar;
    pbarActiveChats->addTab("asdasd");
    grid->addWidget(pbarActiveChats, 0, 0, 1, 16);

    mainWidget->setLayout(grid);
    setCentralWidget(mainWidget);
}

ChatterBox::~ChatterBox()
{
}


/****************MESSAGE BLOCK************************/
MessageBlock::MessageBlock(QString& _login, PortController* _port, QWidget* parent) 
                                    : InterfaceItem(_port),
                                      QWidget(parent),
                                      login(_login)

{
    port->attach(this);
    QGridLayout *grid = new QGridLayout(this);

    // StackedWidget
    pswFriendField = new QStackedWidget(this);
    grid->addWidget(pswFriendField);

    setLayout(grid);
}

void MessageBlock::updateState(QString name)
{
    MessageBox* pfriendWidget = new MessageBox(this);
    connect(pfriendWidget, SIGNAL(messageIsCompleted(QString)), 
            this, SLOT(sendMessage(QString)));

    int idx = pswFriendField->addWidget(pfriendWidget);
    std::cerr << "idx == " << idx << std::endl;
    stackIdxMap.insert(name, idx);

    curFriend = name;
    showCurrent();
}

void MessageBlock::showCurrent()
{
    pswFriendField->setCurrentIndex(stackIdxMap.value(curFriend));   
}

void MessageBlock::update()
{
    std::cerr << "UPDATE MESSAGE BLOCK\n";
    Stanza stanza = port->getStanza();
    QString from = QString::fromStdString(stanza.getFrom().getNode());
    QString message_body = QString::fromStdString(stanza.getMSG());
    if (stanza.getStanzaType() == Stanza::MESSAGE)
    {
        QWidget* wdgt = pswFriendField->widget(stackIdxMap.value(from));
        if (MessageBox* msgbox = dynamic_cast<MessageBox*>(wdgt))
            msgbox->addMessage(from + ": " + message_body);
    }
}

void MessageBlock::sendMessage(QString body) const
{
    JID from, to;
    
    from.setNode(login.toStdString());
    to.setNode(curFriend.toStdString());

    Stanza message(from, to);
    message.setStanzaType(Stanza::MESSAGE);
    message.setMSG(Stanza::PRIVATE, body.toStdString());

    port->send(message);
}

MessageBox::MessageBox(QWidget* parent) : QWidget(parent)                                                                                                   
{
    QGridLayout *grid = new QGridLayout(this);

    // Message Edit
    ptxtEdit = new QTextEdit(this);
    grid->addWidget(ptxtEdit, 16, 0, 2, 14);

    // Message List
    ptxtChat = new QTextEdit(this);
    ptxtChat->setReadOnly(true);
    grid->addWidget(ptxtChat, 0, 0, 16, 16);

    // Button "Say"
    pbtnSay = new QPushButton("&Say");
    grid->addWidget(pbtnSay, 16, 14, 2, 2);
    connect(pbtnSay, SIGNAL(clicked()), this, SLOT(sayButtonClicked()));

    setLayout(grid);
}

void MessageBox::addMessage(const QString body)
{
    std::cerr << "MESSAGE BOX\n";
    ptxtChat->append(body);
}

void MessageBox::sayButtonClicked()
{
    // parentWidget() returns QWidget*

    if (!ptxtEdit->toPlainText().isEmpty())
    {
        emit messageIsCompleted(ptxtEdit->toPlainText());
        addMessage("ME: " + ptxtEdit->toPlainText());
        ptxtEdit->clear();

    }
}

MessageBox::~MessageBox()
{
}

/****************FRIEND LIST*****************************/
FriendList::FriendList(PortController* _port, MessageBlock* block, QWidget* parent)
                                        : QWidget(parent),
                                          InterfaceItem(_port),
                                          connectedBlock(block)
{
    port->attach(this);

    QGridLayout *grid = new QGridLayout(this); 

    // Friend List
    plstFriendList = new QListWidget(this);
    grid->addWidget(plstFriendList, 1, 0, 17, 3);
    connect(plstFriendList, SIGNAL(clicked(QModelIndex)), this, SLOT(friendListClicked(QModelIndex)));

    // Add friends section
    pbtnAdd = new QPushButton("&add");
    pbtnAdd->setEnabled(false);
    grid->addWidget(pbtnAdd, 0, 2, 1, 1);
    plnAdd = new QLineEdit;
    grid->addWidget(plnAdd, 0, 0, 1, 2);
    connect(pbtnAdd, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
    connect(plnAdd, SIGNAL(textChanged(QString)), this, SLOT(setAddEnabled()));

    setLayout(grid);
}


void FriendList::setAddEnabled()
{
    pbtnAdd->setEnabled(!plnAdd->text().isEmpty());
}

void FriendList::addButtonClicked()
{
    // update Message Block
    connectedBlock->updateState(plnAdd->text());

    plstFriendList->addItem(plnAdd->text());
    plnAdd->clear();
}

void FriendList::friendListClicked(const QModelIndex &index)
{
    if ( !index.isValid())
        return;

    if( QListWidget* listWgt = dynamic_cast< QListWidget* >( sender()))
    {
        if( QListWidgetItem* item = listWgt->item( index.row()))
        {
            connectedBlock->curFriend = item->text();
            connectedBlock->showCurrent();
        }
    }
}

void FriendList::update()
{
}