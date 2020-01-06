#include "irclog.h"

/* Message debugger for IRC Bot
 * Outputs various information of various levels */
void IrcLog::messageHander(QtMsgType type, const QMessageLogContext& context, const QString& message) {
    QString levelText;
    //bool debug = true; /* TODO: parse command line */
    switch (type) {
        case QtDebugMsg:
            levelText = "Debug";
            if (!debugging)
                return;
            break;
        case QtInfoMsg:
            levelText = "Info";
            break;
        case QtWarningMsg:
            levelText = "Warning";
            break;
        case QtCriticalMsg:
            levelText = "Critical";
            break;
        case QtFatalMsg:
            levelText = "Fatal";
        break;
    }

    QString text = QString("[%1 - %2] [%3] %4")
            .arg(QDate::currentDate().toString())
            .arg(QTime::currentTime().toString())
            .arg(levelText)
            .arg(message);
    QFile file(LOGFILE);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream textStream(&file);
    textStream << text << endl;

    QTextStream term(stdout);
    term << text << endl;
}

void IrcLog::verbose(bool s) {
    debugging = s;
}
