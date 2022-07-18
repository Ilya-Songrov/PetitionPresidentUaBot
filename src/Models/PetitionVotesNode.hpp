#pragma once

#include <QSerializer>

class PetitionVotesNode : public QSerializer
{
    Q_GADGET
    QS_SERIALIZABLE
    QS_FIELD                (QString, number)
    QS_FIELD                (QString, name)
    QS_FIELD                (QString, date)

public:
    PetitionVotesNode() {}
    PetitionVotesNode(const QString& a_number,
                      const QString& a_name,
                      const QString& a_date)
        : number(a_number)
        , name(a_name)
        , date(a_date)
    {}
};
