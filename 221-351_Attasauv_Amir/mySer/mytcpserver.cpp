#include "mytcpserver.h"
#include <QDebug>
#include <QCoreApplication>

MyTcpServer::~MyTcpServer()
{
    mTcpServer->close();
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent){
    mTcpServer = new QTcpServer(this);
    connect(mTcpServer, &QTcpServer::newConnection, this, &MyTcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "server is not started";
    } else {
        qDebug() << "server is started";
    }
}

void MyTcpServer::slotNewConnection(){
    QTcpSocket* socket = mTcpServer->nextPendingConnection();
    mTcpSockets.append(socket); // добавление сокета в список
    socket->write("Hello, World!!! I am echo server!\r\n");
    connect(socket, &QTcpSocket::readyRead, this, &MyTcpServer::slotServerRead);
    connect(socket, &QTcpSocket::disconnected, this, &MyTcpServer::slotClientDisconnected);
}

void MyTcpServer::slotServerRead(){
    QTcpSocket* senderSocket = qobject_cast<QTcpSocket*>(sender());
    QByteArray array = senderSocket->readAll();
    QString message = QString(array);
    QString newMessage;
    for (int i = 0; i < message.length(); i++) {
        newMessage.append(message[i]);
        if ((i + 1) % 3 == 0) {
            newMessage.append(" ");
            newMessage.append(message[i]);
        }
    }
    for (int j = 0; j < mTcpSockets.length(); j++) {
        if (mTcpSockets[j] != senderSocket) {
            mTcpSockets[j]->write(newMessage.toUtf8());
        }
    }
}

void MyTcpServer::slotClientDisconnected(){
    QTcpSocket* senderSocket = qobject_cast<QTcpSocket*>(sender());
    mTcpSockets.removeOne(senderSocket); // удаление сокета из списка
}
