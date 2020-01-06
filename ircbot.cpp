#include "ircbot.h"
#define TIMEOUT 2500

IrcBot::IrcBot(/* QObject *parent , */
               QString s, QString c, QString u, int p)
    /*: QObject(parent), */
    :  server(s),
      channel(c),
      user(u),
      port(p),
      socket(new QTcpSocket(this))
{

    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
                  this, &IrcBot::displayError);
    connect(socket, &QTcpSocket::readyRead, this, &IrcBot::getLine);
}

IrcBot::~IrcBot() {
    delete socket;
}

void IrcBot::startup() {

    socket->connectToHost(server, port);
    if (socket->waitForConnected(TIMEOUT))
          qInfo() << "Connected to:" << server << ':' << port;

    else {
        qCritical() << "Can't connect to server" << server;
        return;
    }

    QByteArray nick = "NICK ";
    nick.append(user).append("\r\n");

    QByteArray usercmd = "USER ";
    usercmd.append(user).append(" 0 * :").append(user).append("\r\n");
    //qDebug() << usercmd;

    QByteArray jchannel = "JOIN ";
    jchannel.append(channel).append("\r\n");

    socket->write(nick);
    socket->write(usercmd);
    socket->write(jchannel);

    QByteArray firstMsg = "PRIVMSG ";
    firstMsg.append(channel).append(" :Hello folks!").append("\r\n");

    qInfo() << firstMsg;
    socket->write(firstMsg);

}

void IrcBot::displayError(QAbstractSocket::SocketError socketError) {
   qWarning() << "Can't read socket:" << socketError;
}

void IrcBot::getLine() {

    /* If concurrent call; use a mutex */
    QByteArray buf;

    while (socket->canReadLine()) {
            buf = socket->readLine(512); /* IRC specification: max size is 512 */
            handleLine(buf);
    }

}

/* Quick and dirty IRC parser
 *
 * An IRC message is structured like this:
 * [':' <prefix> <SPACE> ] <command> <params> <crlf>
 * handleLine splits an IRC message in a prefix, command and
 * argument part and passes it on to handleCmd
 *
 * This function is dirty in the sense that it doesn't
 * completely tokenizes and parses an IRC message, but just
 * does some basic splitting and calls handleCmd
 *
   See: https://tools.ietf.org/html/rfc1459
*/
void IrcBot::handleLine(QByteArray& buf)
{
    QByteArray prefix, cmd, args;
    int pstart, pend; /* Prefix start and end */
    int cstart, cend; /* Command start and end */

    /* Get prefix */
    pstart = buf.indexOf(':');
    pend = buf.indexOf(' ', pstart+1);

    if (pstart == -1 || pend == -1) {
        qInfo() << "No IRC prefix in message";
    }
    else {
        prefix = buf.mid(pstart, pend - pstart);
        qDebug() << "PREFIX:" << prefix;
    }

    /* Get command, return if empty */
    cstart = buf.indexOf(' ');
    if (cstart == -1) {
        qWarning() << "Can't read IRC packet";
        return;
    }

    int tmp = buf.indexOf(':');
    if (tmp < cstart) {
        cend = buf.indexOf(' ', cstart+1);
        cmd = buf.mid(cstart+1, cend - cstart-1);
        qDebug() << "CMD:" << cmd;
    }
    else {
        cend = cstart;
        cstart = 0;
        cmd = buf.mid(cstart, cend);
        qDebug() << "CMD:" << cmd;
    }

    /* Get the rest, i.e. arguments after command */
    args = buf.mid(cend+1, buf.size() - 1);
    qDebug() << "ARGS" << args;

    handleCmd(prefix, cmd, args);

}

/*
 * Handles the most common IRC commands
 * and replies and/or logs info
 */
void IrcBot::handleCmd(QByteArray& pre, QByteArray& cmd, QByteArray& args) {

    QByteArray msg;

    if (cmd == "PING") {
        qInfo() << "Received PING, sending PONG to" << channel;
        msg = "PONG\r\n";
        socket->write(msg);

        qInfo() << "Sending PRIVMSG" << channel;
        msg = "PRIVMSG ";
        msg.append(channel).append(" :I'm still here").append("\r\n");
        socket->write(msg);
    }
    else if (cmd == "JOIN" || cmd == "QUIT" || cmd == "PART") { /* Channel leave, join or quit */
        /* Get username from prefix */
        int us = pre.indexOf(':');
        int ue = pre.indexOf("!", us);
        QString tmpuser = pre.mid(us+1, ue-1);
        if (cmd == "JOIN") {
            qInfo() << tmpuser << "joined channel" << args;
            if (tmpuser != user) {
                msg = "PRIVMSG ";
                msg.append(channel).append(" :Hello ").append(tmpuser).append("\r\n");
                socket->write(msg);
            }
        }
        else if (cmd == "PART")
            qInfo() << pre.mid(us+1, ue-1) << "left channel" << args;
        else if (cmd == "QUIT")
            qInfo() << pre.mid(us+1, ue-1) << "quit" << args;
    }
    else if (cmd == "PRIVMSG") {

        int se = args.indexOf(' ');

        int us = pre.indexOf(':');
        int ue = pre.indexOf("!", us);
        qInfo() << args.mid(0, se)
                << pre.mid(us+1, ue-1)
                << "says" << args.mid(se+1, args.size());
    }

}

