#include "BotThreadManager.hpp"

BotThreadManager::BotThreadManager(const QString& token, const QStringList& commandsWithSlash, QObject* parent) : QThread{parent}
  , _bot(token.toStdString())
  , _commandsWithSlash(commandsWithSlash)
{
    start(LowPriority);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qRegisterMetaType<TgBot::Message::Ptr>("TgBot::Message::Ptr");
    qRegisterMetaType<TgBot::CallbackQuery::Ptr>("TgBot::CallbackQuery::Ptr");
#endif
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
    while (true) {
        try {
            for (const QString& command : _commandsWithSlash) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                QString commandCopy = command;
                _bot.getEvents().onCommand(commandCopy.remove(0, 1).toStdString(),
                                           std::bind(&BotThreadManager::slotOnCommand, this, std::placeholders::_1, command));
#else
                _bot.getEvents().onCommand(command.sliced(1, command.size() - 1).toStdString(),
                                           std::bind(&BotThreadManager::slotOnCommand, this, std::placeholders::_1, command));
#endif
            }
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
            QThread::msleep(1000);
        }
    }
}

void BotThreadManager::slotOnCommand(const TgBot::Message::Ptr message, QString commandName)
{
    emit signalOnCommand(message, commandName);
}

void BotThreadManager::slotOnAnyMessage(const TgBot::Message::Ptr message)
{
    emit signalOnAnyMessage(message);
}

void BotThreadManager::slotOnCallbackQuery(const TgBot::CallbackQuery::Ptr callbackQuery)
{
    emit signalOnCallbackQuery(callbackQuery);
}
