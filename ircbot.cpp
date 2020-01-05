#include "ircbot.h"

IrcBot::IrcBot(QObject *parent,
               QString s, QString c, QString u, int p)
    : QObject(parent),
      socket(new QTcpSocket(this)),
      server(s),
      channel(c),
      user(u),
      port(p)
{

    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
                  this, &IrcBot::displayError);
    connect(socket, &QTcpSocket::readyRead, this, &IrcBot::getLine);
}


void IrcBot::startup() {

    //qInfo().noquote();

    socket->connectToHost(server, port);
    if (socket->waitForConnected(1000))
          qInfo() << "Connected to:" << server << ':' << port;

    else {
        qCritical() << "Can't connect to server" << server;
        return;
    }

    QByteArray nick = "NICK ";
    nick.append(user).append("\r\n");

    QByteArray user = "USER ";
    user.append(user).append(" 0 * :").append(user).append("\r\n");

    QByteArray jchannel = "JOIN ";
    jchannel.append(channel).append("\r\n");

    socket->write(nick);
    socket->write(user);
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
   See: https://tools.ietf.org/html/rfc1459 for BNF
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

    handleCmd(cmd, args);

}

void IrcBot::handleCmd(QByteArray& cmd, QByteArray& args) {

    QByteArray msg;

    /* Reply PING with PONG command and send some text to the channel */
    if (cmd == "PING") {
        qInfo() << "Received PING, sending PONG to" << channel;
        msg = "PONG\r\n";
        socket->write(msg);

        qInfo() << "Sending PRIVMSG" << channel;
        msg = "PRIVMSG ";
        msg.append(channel).append(" :I'm still here").append("\r\n");
        socket->write(msg);
    }
    else if (cmd == "JOIN") { /* Succesfully joined a channel */
        qInfo() << "Joined channel" << args;
    }

    /* Implement more commands */

}
