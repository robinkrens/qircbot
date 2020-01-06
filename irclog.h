#ifndef IRCLOG_H
#define IRCLOG_H
#include <QDebug>
#include <QFile>
#include <QTime>
#define LOGFILE "qircbot.log"

class IrcLog
{
private:
    static bool debugging;
public:
    static void verbose(bool s);
    static void messageHander(QtMsgType type, const QMessageLogContext& context, const QString& message);

};

#endif // IRCLOG_H
