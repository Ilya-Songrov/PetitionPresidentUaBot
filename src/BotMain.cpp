#include "BotMain.h"


BotMain::BotMain(QObject *parent) : QObject(parent)
  , botThreadManager(nullptr)
  , petitionManager(nullptr)
{

}

void BotMain::startBot()
{
    //    signal(SIGINT, [](int s) {
    //        printf("SIGINT got\n");
    //        exit(0);
    //    });

    initDb();
    botThreadManager.reset(new BotThreadManager(GlobalConfigInstance::instance().getTokenFromFile(),
                                                QStringList() << commandStart << commandHelp
                                                << commandSearch << commandCountVotes));
    petitionManager.reset(new PetitionManager());
    petitionManager->init();
    setSettings();
}

void BotMain::slotOnAnyMessageWasWrite(const TgBot::Message::Ptr message)
{
    qDebug() << "print_function:" << __FUNCTION__ << "line:" << __LINE__ << "message->text:" << message->text.c_str() << Qt::endl;
    if (StringTools::startsWith(message->text, commandStart.toStdString())
            || StringTools::startsWith(message->text, commandHelp.toStdString())
            || StringTools::startsWith(message->text, commandSearch.toStdString())
            ) {
        return;
    }
    petitionManager->executeRequestToBot(RequestToBot(message->chat->id, message->text, RequestToBot::RequesttToSearch));
}

void BotMain::slotOnCallbackQueryWasWrite(const TgBot::CallbackQuery::Ptr callbackQuery)
{
    qDebug() << "print_function:" << __FUNCTION__ << "line:" << __LINE__ << "callbackQuery->data:" << callbackQuery->data.c_str() << Qt::endl;
    botThreadManager->bot().getApi().sendMessage(callbackQuery->id, "Your message is: " + callbackQuery->data);
}

void BotMain::slotListResultReady(QSharedPointer<ResponseFromBot> rs)
{
    try {
        botThreadManager->bot().getApi().sendMessage(rs->chat_id, rs->response_text);
    } catch (std::exception& e) {
        std::cout << "print_function: " << __FUNCTION__ << " line: " << __LINE__ << " e.what(): " << e.what() << std::endl;
    }
}

void BotMain::slotOnCommand(const TgBot::Message::Ptr message, QString commandName)
{
    qDebug() << "print_function:" << __FUNCTION__ << "line:" << __LINE__ << "message->text:" << message->text.c_str()
             << "commandName:" << commandName << Qt::endl;
    if (commandName == commandStart || commandName == commandHelp) {
        botThreadManager->bot().getApi().sendMessage(message->chat->id, "❗️ Наша ціль - зібрати більше 1000000 підписів.\n"
                                                                        "Це петиція про денонсування закону про ратифікацію Стабмульської конвенції.\n"
                                                                        "Не полінінуйтесь прочитати і підписати.",
                                                     false, 0, std::make_shared<TgBot::GenericReply>(), "Markdown", false);
        botThreadManager->bot().getApi().sendMessage(message->chat->id, "https://petition.president.gov.ua/petition/146508");
        botThreadManager->bot().getApi().sendMessage(message->chat->id, "Якщо ви підписали петицію, введіть свою фамілію чи ім'я:");
    }
    else if (commandName == commandSearch) {
        botThreadManager->bot().getApi().sendMessage(message->chat->id, "Якщо ви підписали петицію, введіть свою фамілію чи ім'я:");
    }
    else if (commandName == commandCountVotes) {
        petitionManager->executeRequestToBot(RequestToBot(message->chat->id, message->text, RequestToBot::CheckCountVotes));
    }
}

void BotMain::setSettings()
{
    connect(petitionManager.get(), &PetitionManager::signalListResultReady, this, &BotMain::slotListResultReady);

    connect(botThreadManager.get(), &BotThreadManager::signalOnCommand, this, std::bind(&BotMain::slotOnCommand, this, std::placeholders::_1, std::placeholders::_2));
    connect(botThreadManager.get(), &BotThreadManager::signalOnAnyMessage, this, std::bind(&BotMain::slotOnAnyMessageWasWrite, this, std::placeholders::_1));
    connect(botThreadManager.get(), &BotThreadManager::signalOnCallbackQuery, this, std::bind(&BotMain::slotOnCallbackQueryWasWrite, this, std::placeholders::_1));
}

void BotMain::initDb()
{
    const auto dbFilePath = GlobalConfigInstance::instance().getGlobalConfig().dbFilePath;
    const bool isOpened = DbManager::instance().init(dbFilePath);
    if (isOpened && DbManager::instance().sizeDb() == 0) {
        DbManager::instance().createTables();
        petitionManager.reset(new PetitionManager());
        petitionManager->fillDatabase();
    }
}
