#include "BotThreadManager.hpp"

BotThreadManager::BotThreadManager(const std::string& token, QObject* parent) : QThread{parent}
  , _bot(token)
{
    start(LowPriority);
}

BotThreadManager::~BotThreadManager()
{
    mutex.lock();
    condition.wakeOne();
    mutex.unlock();

    wait();
}

TgBot::Bot& BotThreadManager::bot()
{
    return _bot;
}

void BotThreadManager::run()
{
    try {
        _bot.getEvents().onCommand("start", std::bind(&BotThreadManager::slotOnCommand, this, std::placeholders::_1));
        _bot.getEvents().onAnyMessage(std::bind(&BotThreadManager::slotOnAnyMessage, this, std::placeholders::_1));
        _bot.getEvents().onCallbackQuery(std::bind(&BotThreadManager::slotOnCallbackQuery, this, std::placeholders::_1));

        qDebug("Bot username: %s\n", _bot.getApi().getMe()->username.c_str());
        _bot.getApi().deleteWebhook();

        TgBot::TgLongPoll longPoll(_bot);
        while (true) {
            qDebug("Long poll started\n");
            longPoll.start();
        }
    } catch (std::exception& e) {
        qDebug("error: %s\n", e.what());
    }
}

void BotThreadManager::slotOnCommand(const TgBot::Message::Ptr message)
{
    emit signalOnCommand(message);
}

void BotThreadManager::slotOnAnyMessage(const TgBot::Message::Ptr message)
{
    emit signalOnAnyMessage(message);
}

void BotThreadManager::slotOnCallbackQuery(const TgBot::CallbackQuery::Ptr callbackQuery)
{
    emit signalOnCallbackQuery(callbackQuery);
}
