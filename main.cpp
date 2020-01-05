#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QTime>
#include "ircbot.h"

void messageHander(QtMsgType type, const QMessageLogContext& context, const QString& message) {
    QString levelText;
    bool debug = false; /* TODO: parse command line */
    switch (type) {
        case QtDebugMsg:
            levelText = "Debug";
            if (!debug)
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
    QFile file("qircbot.log");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream textStream(&file);
    textStream << text << endl;

    QTextStream term(stdout);
    term << text << endl;

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qInstallMessageHandler(messageHander);

    IrcBot bot(nullptr,"195.154.200.232", "#woggleweb", "wogglebot", 6667);

    a.setApplicationName("qircbot");
    a.setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("qirbot helper");
       parser.addHelpOption();
       parser.addVersionOption();

       parser.addOptions({
           {"debug",
            "Enable the debug mode."},

           {{"u", "user"},
            "use username <user>. Default is 'testbot'",
            "user",
            "testbot"},
       });

    parser.process(a);

    QFile file("qircbot.log");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream textStream(&file);
    textStream << "=== STARTING LOG FOR "
               << a.applicationName().toUpper() << " " << a.applicationVersion()
               << " ===" << endl;
    file.close();

    bot.startup();

    return a.exec();

}



