#include "../include/Automaton.h"
#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>

bool LOG_ENABLE = true;

int main(int argc, char *argv[])
{
    QString path_states = "../../state_config.json";
    QString path_edges = "../../edge_config.json";

    QFile file_states(path_states);
    if (!file_states.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Couldn't open file " << path_states;
        return -1;
    }

    QFile file_edges(path_edges);
    if (!file_edges.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Couldn't open file " << path_edges;
        return -1;
    }

    QByteArray jsonDataStates = file_states.readAll();
    file_states.close();

    QByteArray jsonDataEdges = file_edges.readAll();
    file_edges.close();

    QJsonParseError parseErrorStates;
    QJsonDocument jsonDocStates = QJsonDocument::fromJson(jsonDataStates, &parseErrorStates);

    QJsonParseError parseErrorEdges;
    QJsonDocument jsonDocEdges = QJsonDocument::fromJson(jsonDataEdges, &parseErrorEdges);

    if (parseErrorStates.error != QJsonParseError::NoError)
    {
        qWarning() << "States JSON parse error:" << parseErrorStates.errorString();
        return -2;
    }

    if (parseErrorEdges.error != QJsonParseError::NoError)
    {
        qWarning() << "Edges JSON parse error:" << parseErrorEdges.errorString();
        return -2;
    }

    if (!jsonDocStates.isArray())
    {
        qWarning() << "States JSON document does not contain an array at the root.";
        return -3;
    }

    if (!jsonDocEdges.isArray())
    {
        qWarning() << "Edges JSON document does not contain an array at the root.";
        return -3;
    }

    QJsonArray jsonArrayStates = jsonDocStates.array();
    QJsonArray jsonArrayEdges = jsonDocEdges.array();

    if(LOG_ENABLE)
    {
        qDebug() << "Successfully read state_config.json";
        qDebug() << "Successfully read edge_config.json";
    }

    Automaton automaton(jsonArrayStates, jsonArrayEdges, LOG_ENABLE);
    automaton.run();

    return 0;
}
