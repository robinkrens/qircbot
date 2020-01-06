#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QTime>
#include "ircbot.h"
#include "irclog.h"

bool IrcLog::debugging{false};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qInstallMessageHandler(IrcLog::messageHander); /* Global message hander (footprint) of QT */
    a.setApplicationName("qircbot");
    a.setApplicationVersion("0.1");

    /* Commandline parser *
     * Terminals shows more info when invoked with --help */
    QCommandLineParser parser;
    //parser.setApplicationDescription("qircbot helper");
       parser.addHelpOption();
       parser.addVersionOption();
       parser.addOptions({
           {"verbose",
            "Enable verbose mode. Print all IRC communication to stdout and logfile "},

           {{"u", "user"},
            "use username <user>. Default is 'wogglebot'. Note, pick a unique name!",
            "user", "wogglebot"},

          {{"s", "server"},
            "use server <server>. Default is 'irc.freenode.net'",
            "server", "irc.freenode.net"},

          {{"p", "port"},
            "use port <port>. Default is 8001",
            "port", "8001"},

          {{"c", "channel"},
           "join channel <channel>. Default is #woggleweb",
           "channel", "#woggleweb"},
       });

    parser.process(a);

    if (parser.isSet("verbose")) {
        IrcLog::verbose(true);
    }

    /* Create (debug) log file */
    QFile file(LOGFILE);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream textStream(&file);
    textStream << "=== STARTING LOG FOR "
               << a.applicationName().toUpper() << " " << a.applicationVersion()
               << " ===" << endl;
    file.close();

    /* Create IRC bot with command line (or default) values */
    IrcBot bot(parser.value("server"),
               parser.value("channel"),
               parser.value("user"),
               parser.value("port").toInt());
    bot.startup();

    return a.exec();

}



