#ifndef IRCBOT_H
#define IRCBOT_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QTextStream>
#include <QDebug>

class IrcBot : public QObject
{
    Q_OBJECT
public:
    IrcBot(QObject *parent = nullptr,
                    QString s = 0, QString c = 0, QString u = 0, int p = 0);
    void startup();
    void getLine();
    void displayError(QAbstractSocket::SocketError socketError);

signals:

public slots:

private:
    QString server, channel, user;
    int port;
    QTcpSocket * socket;
    //QDataStream in;
    void handleLine(QByteArray&);
    void handleCmd(QByteArray& cmd, QByteArray& args);
};

#endif // IRCBOT_H
