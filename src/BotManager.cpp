#include "BotManager.h"


BotManager::BotManager(QObject *parent) : QObject(parent)
  , bot(new Bot(getTokenFromFile().toStdString()))
  , petitionManager(new PetitionManager())
  , signalEmitterManager(nullptr)
{
    petitionManager->moveToThread(&threadPetitionManager);
    petitionManager->init();

    setSettings();


//    QTimer::singleShot(4000, [this](){
//        RequestToBot rq;
//        rq.chat_id = -1;
//        rq.request_text = "Сонгров";
//        petitionManager->checkRequestToBot(rq);
//        qDebug() << "SingleShot1" << Qt::endl;
//        QTimer::singleShot(4000, [this](){
//            qDebug() << "SingleShot2" << Qt::endl;
//        });
//    });
}

void BotManager::startBot()
{
    //    signal(SIGINT, [](int s) {
    //        printf("SIGINT got\n");
    //        exit(0);
    //    });

    qDebug("Token: %s\n", bot->getToken().c_str());
    const auto ret = QtConcurrent::run([this](){
        try {
            signalEmitterManager.reset(new SignalEmitterManager());
            connect(signalEmitterManager.get(), &SignalEmitterManager::signalCheckRequestToBot,
                    petitionManager.get(), &PetitionManager::checkRequestToBot);

            qDebug("Bot username: %s\n", bot->getApi().getMe()->username.c_str());
            bot->getApi().deleteWebhook();

            TgLongPoll longPoll(*bot.get());
            while (true) {
                qDebug("Long poll started\n");
                longPoll.start();
            }
        } catch (std::exception& e) {
            qDebug("error: %s\n", e.what());
        }
    });
}

void BotManager::slotOnAnyMessageWasWrite(const Message::Ptr message)
{
    qDebug() << "print_function:" << __FUNCTION__ << "message->chat->id:" << message->chat->id << Qt::endl;
    qDebug() << "print_function:" << __FUNCTION__ << "message->text:" << message->text.c_str() << Qt::endl;
    if (StringTools::startsWith(message->text, "/start")) {
        return;
    }
    RequestToBot rq;
    rq.chat_id      = message->chat->id;
    rq.request_text = message->text.c_str();
    emit signalEmitterManager->signalCheckRequestToBot(rq);
//    petitionManager->checkRequestToBot(rq);
}

void BotManager::slotOnCallbackQueryWasWrite(const CallbackQuery::Ptr callbackQuery)
{
    qDebug() << "print_function:" << __FUNCTION__ << "callbackQuery->message->chat->id:" << callbackQuery->message->chat->id << Qt::endl;
    qDebug() << "print_function:" << __FUNCTION__ << "callbackQuery->data:" << callbackQuery->data.c_str() << Qt::endl;
    bot->getApi().sendMessage(callbackQuery->id, "Your message is: " + callbackQuery->data);
}

void BotManager::slotListResultReady(QSharedPointer<ResponseFromBot> rs)
{
    bot->getApi().sendMessage(rs->chat_id, "Знайдені збіги:\n" + rs->response_text);
}

void BotManager::slotOnCommand(const Message::Ptr message)
{
    qDebug() << "print_function:" << __FUNCTION__ << "message->chat->id:" << message->chat->id << Qt::endl;
    qDebug() << "print_function:" << __FUNCTION__ << "message->text:" << message->text.c_str() << Qt::endl;
    bot->getApi().sendMessage(message->chat->id, "Напишіть фамілію:");
}

void BotManager::setSettings()
{
    bot->getEvents().onCommand("start", std::bind(&BotManager::slotOnCommand, this, std::placeholders::_1));
    bot->getEvents().onAnyMessage(std::bind(&BotManager::slotOnAnyMessageWasWrite, this, std::placeholders::_1));
    bot->getEvents().onCallbackQuery(std::bind(&BotManager::slotOnCallbackQueryWasWrite, this, std::placeholders::_1));

    connect(petitionManager.get(), &PetitionManager::signalListResultReady, this, &BotManager::slotListResultReady);
//    connect(this, &BotManager::signalCheckRequestToBot, petitionManager.get(), &PetitionManager::checkRequestToBot);
}

QString BotManager::getTokenFromFile()
{
#ifdef QT_DEBUG
    const QJsonDocument doc = FileWorker::readFileJson("../../config.json");
#else
    const QJsonDocument doc = FileWorker::readFileJson("config.json");
#endif
    const QJsonObject obj = doc.object();
    return obj.value("token").toString();
}
