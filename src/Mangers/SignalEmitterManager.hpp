#pragma once

#include <QObject>
#include <QThread>

#include "RequestToBot.hpp"

class SignalEmitterManager : public QObject
{
    Q_OBJECT
public:
    explicit SignalEmitterManager(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void signalCheckRequestToBot(const RequestToBot rq);
};

