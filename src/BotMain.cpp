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

    botThreadManager.reset(new BotThreadManager(GlobalConfigInstance::instance().getTokenFromFile()));
    petitionManager.reset(new PetitionManager());
    petitionManager->init();
    setSettings();
}

void BotMain::slotOnAnyMessageWasWrite(const TgBot::Message::Ptr message)
{
    qDebug() << "print_function:" << __FUNCTION__ << "message->chat->id:" << message->chat->id << Qt::endl;
    qDebug() << "print_function:" << __FUNCTION__ << "message->text:" << message->text.c_str() << Qt::endl;
    if (StringTools::startsWith(message->text, "/start")) {
        return;
    }
    RequestToBot rq;
    rq.chat_id      = message->chat->id;
    rq.request_text = message->text.c_str();
    petitionManager->checkRequestToBot(rq);
}

void BotMain::slotOnCallbackQueryWasWrite(const TgBot::CallbackQuery::Ptr callbackQuery)
{
    qDebug() << "print_function:" << __FUNCTION__ << "callbackQuery->message->chat->id:" << callbackQuery->message->chat->id << Qt::endl;
    qDebug() << "print_function:" << __FUNCTION__ << "callbackQuery->data:" << callbackQuery->data.c_str() << Qt::endl;
    botThreadManager->bot().getApi().sendMessage(callbackQuery->id, "Your message is: " + callbackQuery->data);
}

void BotMain::slotListResultReady(QSharedPointer<ResponseFromBot> rs)
{
    qDebug() << "print_function:" << __FUNCTION__ << "rs->chat_id:" << rs->chat_id << Qt::endl;
    qDebug() << "print_function:" << __FUNCTION__ << "rs->response_text:" << rs->response_text.c_str() << Qt::endl;
//    const auto maxLen = 4096;
//    if (rs->response_text.size() > maxLen) {
//        botThreadManager->bot().getApi().sendMessage(rs->chat_id, "Знайдені збіги:\n");
//        for (size_t var = 0; var < rs->response_text.size(); var += maxLen) {
//            std::cout << "print_function: " << __FUNCTION__ << " line: " << __LINE__ << " size: " << rs->response_text.substr(var, maxLen).size() << std::endl;
//            QThread::msleep(200);
//        }
//        return;
//    }
//    botThreadManager->bot().getApi().sendMessage(rs->chat_id, "Знайдені збіги:\n" + rs->response_text);
    try {
        botThreadManager->bot().getApi().sendMessage(rs->chat_id, rs->response_text);
    } catch (std::exception& e) {
        std::cout << "print_function: " << __FUNCTION__ << " line: " << __LINE__ << " e.what(): " << e.what() << std::endl;
    }
}

void BotMain::slotOnCommand(const TgBot::Message::Ptr message)
{
    qDebug() << "print_function:" << __FUNCTION__ << "message->chat->id:" << message->chat->id << Qt::endl;
    qDebug() << "print_function:" << __FUNCTION__ << "message->text:" << message->text.c_str() << Qt::endl;
    botThreadManager->bot().getApi().sendMessage(message->chat->id, "Напишіть фамілію:");
}

void BotMain::setSettings()
{
    connect(petitionManager.get(), &PetitionManager::signalListResultReady, this, &BotMain::slotListResultReady);

    connect(botThreadManager.get(), &BotThreadManager::signalOnCommand, this, std::bind(&BotMain::slotOnCommand, this, std::placeholders::_1));
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
