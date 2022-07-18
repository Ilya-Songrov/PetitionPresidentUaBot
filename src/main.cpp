#include <QCoreApplication>
#include <QCommandLineParser>

#include "BotMain.h"

int main(int argc, char *argv[])
{
    try {
        QCoreApplication app(argc, argv);

        QCommandLineParser parser;
        parser.setApplicationDescription("App description");
        parser.addHelpOption();
        parser.addVersionOption();
        parser.process(app);

        BotMain botMain;
        botMain.startBot();

        return app.exec();
    }
    catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught (...)" << std::endl;
    }
    return EXIT_FAILURE;
}
