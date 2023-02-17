#include "BotMain.h"


BotMain::BotMain(QObject *parent) : QObject(parent)
  , botThreadManager(nullptr)
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
                                                << commandSearch << commandCountVotes << commandSetPetitionUrl));
//    petitionManager.reset(new PetitionManager());
//    petitionManager->init();
    setSettings();
}

void BotMain::slotOnAnyMessageWasWrite(const TgBot::Message::Ptr message)
{
    qDebug() << "print_function:" << __FUNCTION__ << "line:" << __LINE__ << "message->text:" << message->text.c_str() << Qt::endl;
    if (StringTools::startsWith(message->text, commandStart.toStdString())
            || StringTools::startsWith(message->text, commandHelp.toStdString())
            || StringTools::startsWith(message->text, commandSearch.toStdString())
            || StringTools::startsWith(message->text, commandCountVotes.toStdString())
            || StringTools::startsWith(message->text, commandSetPetitionUrl.toStdString())
            ) {
        return;
    }
    else if (StringTools::startsWith(message->text, "https://petition.president.gov.ua/petition/")) {
        static const std::string postfix = "default_petition_url";
        if (StringTools::endsWith(message->text, postfix)) {
            DbManager::instance().insertOrReplaceDefaultPetitionUrl(message->text.substr(0, message->text.length() - postfix.length()));
            botThreadManager->bot().getApi().sendMessage(message->chat->id, "Ви успішно змінили дефолтну петицію.");
            return;
        }
        DbManager::instance().insertOrUpdatePetitionUrlForChatId(message->text, message->chat->id);
        mapPetitionUrlForChatId.remove(message->chat->id);
        botThreadManager->bot().getApi().sendMessage(message->chat->id, "Ви успішно змінили петицію.");
        return;
    }
    getPetitionManager(message->chat->id).toStrongRef()->executeRequestToBot(RequestToBot(message->chat->id, message->text, RequestToBot::RequestType::RequestToSearch));
}

void BotMain::slotOnCallbackQueryWasWrite(const TgBot::CallbackQuery::Ptr callbackQuery)
{
    qDebug() << "print_function:" << __FUNCTION__ << "line:" << __LINE__ << "callbackQuery->data:" << callbackQuery->data.c_str() << Qt::endl;
    botThreadManager->bot().getApi().sendMessage(callbackQuery->id, "Your message is: " + callbackQuery->data);
}

void BotMain::slotResponseFromBotReady(QSharedPointer<ResponseFromBot> rs)
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
        getPetitionManager(message->chat->id).toStrongRef()->executeRequestToBot(RequestToBot(message->chat->id, message->text, RequestToBot::RequestType::GetDescription));
    }
    else if (commandName == commandSearch) {
        botThreadManager->bot().getApi().sendMessage(message->chat->id, "Якщо ви підписали петицію, введіть свою фамілію чи ім'я:");
    }
    else if (commandName == commandCountVotes) {
        getPetitionManager(message->chat->id).toStrongRef()->executeRequestToBot(RequestToBot(message->chat->id, message->text, RequestToBot::RequestType::CheckCountVotes));
    }
    else if (commandName == commandSetPetitionUrl) {
        botThreadManager->bot().getApi().sendMessage(message->chat->id, "Для змінення петиції введіть адресу петиції. Посилання має бути схожим на адресу в розділі /help");
    }
}

void BotMain::setSettings()
{
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
    }
}

QWeakPointer<PetitionManager> BotMain::getPetitionManager(const int64_t chat_id)
{
    QString petitionUrl = mapPetitionUrlForChatId.value(chat_id);
    if (petitionUrl.isEmpty()){
        petitionUrl = DbManager::instance().getPetitionUrlForChatId(chat_id);
        mapPetitionUrlForChatId.insert(chat_id, petitionUrl);
    }
    if (mapPetitionManagers.contains(petitionUrl)){
        return mapPetitionManagers.value(petitionUrl);
    }
    QSharedPointer<PetitionManager> petitionManager = QSharedPointer<PetitionManager>(new PetitionManager());
    petitionManager->init(petitionUrl);
    connect(petitionManager.get(), &PetitionManager::signalResponseFromBotReady, this, &BotMain::slotResponseFromBotReady);
    mapPetitionManagers.insert(petitionUrl, petitionManager);
    return petitionManager;
}
