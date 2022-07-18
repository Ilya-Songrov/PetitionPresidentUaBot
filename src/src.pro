QT -= gui
QT += concurrent network

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = PetitionPresidentUaBot

DEFINES += QS_HAS_JSON
DEFINES += QS_HAS_XML

include(../petitionpresidentuabot.pri)
include($$PETITION_PRESIDENT_UA_BOT_SOURCE_TREE/QSerializer/qserializer.pri)

SOURCES += \
        ApiClient/ApiClientAbstract.cpp \
        ApiClient/ApiClientPetition.cpp \
        BotManager.cpp \
        Mangers/PetitionManager.cpp \
        Utils/FileWorker.cpp \
        main.cpp

HEADERS += \
    $$PETITION_PRESIDENT_UA_BOT_SOURCE_TREE/tgbot-cpp/include/tgbot/tgbot.h \
    ApiClient/ApiClientAbstract.hpp \
    ApiClient/ApiClientPetition.hpp \
    BotManager.h \
    Mangers/LogManager.hpp \
    Mangers/PetitionManager.hpp \
    Mangers/SignalEmitterManager.hpp \
    Models/PetitionVotes.hpp \
    Models/PetitionVotesNode.hpp \
    Models/RequestToBot.hpp \
    Models/ResponseFromBot.hpp \
    Utils/FileWorker.h

INCLUDEPATH += \
    $$PWD/Utils \
    $$PWD/ApiClient \
    $$PWD/Mangers \
    $$PWD/Models \
    $$PETITION_PRESIDENT_UA_BOT_SOURCE_TREE/tgbot-cpp/include


#-lTgBot
unix:!macx: LIBS += -L$$PETITION_PRESIDENT_UA_BOT_SOURCE_TREE/tgbot-cpp -lTgBot
INCLUDEPATH += $$PETITION_PRESIDENT_UA_BOT_SOURCE_TREE/tgbot-cpp
DEPENDPATH  += $$PETITION_PRESIDENT_UA_BOT_SOURCE_TREE/tgbot-cpp
unix:!macx: PRE_TARGETDEPS += $$PETITION_PRESIDENT_UA_BOT_SOURCE_TREE/tgbot-cpp/libTgBot.a

#-lTgBot -lboost_system -lssl -lcrypto -lpthread
unix:!macx: LIBS += -lboost_system
unix:!macx: LIBS += -lssl
unix:!macx: LIBS += -lcrypto
unix:!macx: LIBS += -lpthread

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


### cqtdeployer ###
CONFIG(release, debug | release) {
QMAKE_POST_LINK=/bin/bash -c \"cqtdeployer -bin $${OUT_PWD}/$${TARGET} -targetDir DeployKit_$${TARGET} -qmake $${QMAKE_QMAKE} -libDir $${PWD} \
                               -recursiveDepth 10 -qmlDir $${PWD} force-clear ; \"
}
### cqtdeployer ###
