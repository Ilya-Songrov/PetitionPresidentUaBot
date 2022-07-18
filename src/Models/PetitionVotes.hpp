#pragma once

#include <QSerializer>

#include "PetitionVotesNode.hpp"

class PetitionVotes : public QSerializer
{
    Q_GADGET
    QS_SERIALIZABLE
    QS_COLLECTION_OBJECTS   (QVector, PetitionVotesNode, rows)

public:
    PetitionVotes()
        {}
};
