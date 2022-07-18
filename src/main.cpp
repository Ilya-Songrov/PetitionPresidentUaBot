#include <QCoreApplication>
#include <QCommandLineParser>

#include "BotMain.h"

int main(int argc, char *argv[])
{
    try {
        QCoreApplication app(argc, argv);
        QCoreApplication::setApplicationVersion("1.0.0");

#ifdef QT_DEBUG
        const QString globalConfigFile = "../../PetitionPresidentUaBot/Data/GlobalConfig.json";
#else
        const QString globalConfigFile = "GlobalConfig.json";
#endif
        GlobalConfigInstance::instance().loadConfig(globalConfigFile);
        const auto& globalConfig = GlobalConfigInstance::instance().getGlobalConfig();
        LogManager::init(globalConfig.log.quantityFiles,
                         globalConfig.log.maxFileSizeMB,
                         globalConfig.log.loggingType,
                         globalConfig.log.dirToSave,
                         globalConfig.log.loggingCategories
                         );

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
