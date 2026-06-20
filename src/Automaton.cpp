#include "../include/Automaton.h"

Automaton::Automaton(QJsonArray statesArr, QJsonArray edgesArr, bool log_enable):
    m_statesArr(statesArr),
    m_edgesArr(edgesArr),
    m_log_enable(log_enable)    {}

void Automaton::run()
{
    // At first we need to find state with name "S1" because it is the start point
    QJsonObject startState;
    for(int i = 0; i < m_statesArr.count(); i++)
    {
        QJsonObject tempObj = m_statesArr[i].toObject();

        if(tempObj.contains("name"))
        {
            if(tempObj["name"] == "S1")
                startState = tempObj;
        }
    }

    if(startState.isEmpty())
    {
        if(m_log_enable)
            qDebug() << "Could not find state with name = 'S1'";
        return;
    }

    m_currStateID = startState["id"].toString();
    m_currStateName = startState["name"].toString();
    m_currX = startState["posX"].toInt();
    m_currY = startState["posY"].toInt();

    int count = 0;

    while(true)
    {
        count++;
        click();

        grabUrlFromBrowser();
        determineNextState();

        QThread::msleep(m_waitTime);
        if (count == 3) {
            break;
        }
    }
}

void Automaton::click()
{
    SetCursorPos(m_currX, m_currY);

    INPUT inputs[2] = {0};

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    SendInput(2, inputs, sizeof(INPUT));
}

void Automaton::sendModernKeystroke(WORD modifier, WORD key) {
    INPUT inputs[4] = {};
    int inputCount = 0;

    // 1. Press Modifier (if any)
    if (modifier != 0) {
        inputs[inputCount].type = INPUT_KEYBOARD;
        inputs[inputCount].ki.wVk = modifier;
        inputCount++;
    }

    // 2. Press Key
    inputs[inputCount].type = INPUT_KEYBOARD;
    inputs[inputCount].ki.wVk = key;
    inputCount++;

    // 3. Release Key
    inputs[inputCount].type = INPUT_KEYBOARD;
    inputs[inputCount].ki.wVk = key;
    inputs[inputCount].ki.dwFlags = KEYEVENTF_KEYUP;
    inputCount++;

    // 4. Release Modifier (if any)
    if (modifier != 0) {
        inputs[inputCount].type = INPUT_KEYBOARD;
        inputs[inputCount].ki.wVk = modifier;
        inputs[inputCount].ki.dwFlags = KEYEVENTF_KEYUP;
        inputCount++;
    }

    SendInput(inputCount, inputs, sizeof(INPUT));
}

void Automaton::grabUrlFromBrowser()
{
    // Find the Firefox window and force it to the absolute front
    HWND firefoxWindow = FindWindowA("MozillaWindowClass", NULL);
    if (firefoxWindow) {
        SetForegroundWindow(firefoxWindow);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    // Release all modifier keys virtually just in case your physical fingers
    // are still holding them down while launching the app
    sendModernKeystroke(0, VK_SHIFT);
    sendModernKeystroke(0, VK_CONTROL);
    sendModernKeystroke(0, VK_MENU); // Alt

    // Focus Address Bar: Ctrl + L
    sendModernKeystroke(VK_CONTROL, 'L');
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Copy Address Bar: Ctrl + C
    sendModernKeystroke(VK_CONTROL, 'C');
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    QString url = "";

    if (OpenClipboard(nullptr))
    {
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (hData != nullptr)
        {
            wchar_t* text = static_cast<wchar_t*>(GlobalLock(hData));
            if (text != nullptr)
            {
                url = QString::fromWCharArray(text);
                GlobalUnlock(hData);
            }
        }
        CloseClipboard();
    }

    m_currURL = url;
}

void Automaton::determineNextState()
{
    QVector<QJsonObject> candidateEdges;

    for (int i = 0; i < m_edgesArr.count(); ++i)
    {
        QJsonObject tempObj = m_edgesArr[i].toObject();

        if(tempObj.contains("fromId"))
        {
            if(tempObj["fromId"] == m_currStateID)
                candidateEdges.append(tempObj);
        }
    }

    if(m_currStateName == "S1") // In start state we do not check any URLs
    {
        QJsonObject finalEdge = candidateEdges[0];  // In start state we assume that always we have one outgoing edge
        m_currStateID = finalEdge["toId"].toString();
        m_waitTime = finalEdge["waitTime"].toInt();

        for(int i = 0; i < m_statesArr.count(); i++)
        {
            QJsonObject tempObj = m_statesArr[i].toObject();

            if(tempObj.contains("id"))
            {
                if(tempObj["id"] == m_currStateID)
                {
                    m_currStateName = tempObj["name"].toString();
                    m_currX = tempObj["posX"].toInt();
                    m_currY = tempObj["posY"].toInt();
                }
            }
        }

        return;
    }

    // If state is not "start state" then we do this ...

    QJsonObject finalEdge;
    for (int i = 0; i < candidateEdges.count(); ++i)
    {
        if(candidateEdges[i]["url"] == m_currURL)
            finalEdge = candidateEdges[i];
    }

    m_currStateID = finalEdge["toId"].toString();
    m_waitTime = finalEdge["waitTime"].toInt();

    for(int i = 0; i < m_statesArr.count(); i++)
    {
        QJsonObject tempObj = m_statesArr[i].toObject();

        if(tempObj.contains("id"))
        {
            if(tempObj["id"] == m_currStateID)
            {
                m_currStateName = tempObj["name"].toString();
                m_currX = tempObj["posX"].toInt();
                m_currY = tempObj["posY"].toInt();
            }
        }
    }
}

