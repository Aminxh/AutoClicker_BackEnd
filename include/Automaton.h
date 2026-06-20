#ifndef AUTOMATON_H
#define AUTOMATON_H
#include <windows.h>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>
#include <QCursor>
#include <QClipboard>
#include <QGuiApplication>
#include <QVector>

class Automaton
{
public:
    Automaton(QJsonArray statesArr, QJsonArray edgesArr, bool log_enable);

    void run();
    void sendModernKeystroke(WORD modifier, WORD key);
    void grabUrlFromBrowser();
    void determineNextState();

private:
    void click();

    QJsonArray m_statesArr;
    QJsonArray m_edgesArr;
    QString m_currStateID = "";
    QString m_currStateName = "";
    QString m_currURL = "";
    uint m_currX = 0;
    uint m_currY = 0;
    uint m_waitTime = 0;
    bool m_log_enable = true;
};

#endif // AUTOMATON_H
