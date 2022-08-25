#include <Arduino.h>
#include "task3.h"
#include "events.h"

static bool compareKeys(BUTTONS *pSecret, BUTTONS *pKey)
{
    bool correct = true;
    for (uint8_t i = 0; i < 5; i++)
    {
        if (pSecret[i] != pKey[i])
        {
            correct = false;
            break;
        }
    }

    return correct;
}

void task3()
{

    enum class TaskStates
    {
        INIT,
        WAIT_CONFIG,
        MODOLENTO,
        MODOMEDIO,
        MODORAPIDO,
        PAPAGADO,
        PENCENDIDO,

    };
    static TaskStates taskState = TaskStates::INIT;
    //
    static uint32_t lasTime;
    static constexpr uint32_t INTERVAL1 = 1000;
    static constexpr uint32_t INTERVAL2 = 500;
    static constexpr uint32_t INTERVAL3 = 125;

    static constexpr uint8_t ledRed = 14;
    static bool ledStatus = false;
    static bool estabaenPAPAGADO = false;

    static BUTTONS secret[5] = {BUTTONS::UP_BTN, BUTTONS::UP_BTN,
                                BUTTONS::DOWN_BTN, BUTTONS::DOWN_BTN,
                                BUTTONS::UP_BTN};

    static BUTTONS disarmKey[5] = {BUTTONS::NONE};
    static uint8_t keyCounter = 0;

    switch (taskState)
    {
    case TaskStates::INIT:
    {

        pinMode(ledRed, OUTPUT);
        digitalWrite(ledRed, LOW);
        ledStatus = true;
        digitalWrite(ledRed, ledStatus);

        taskState = TaskStates::MODOLENTO;
        lasTime = millis();
        break;
    }
    case TaskStates::MODOLENTO:
    {

        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= INTERVAL1)
        {
            lasTime = currentTime;

            ledStatus = !ledStatus;
            digitalWrite(ledRed, ledStatus);

            if (buttonEvt.trigger == true)
            {
                buttonEvt.trigger = false;
                if (buttonEvt.whichButton == BUTTONS::UP_BTN)
                {
                    ledStatus = false;
                    digitalWrite(ledRed, ledStatus);
                    taskState = TaskStates::PAPAGADO;
                }
                else if (buttonEvt.whichButton == BUTTONS::DOWN_BTN)
                {
                    ledStatus = true;
                    digitalWrite(ledRed, ledStatus);

                    lasTime = millis();
                    taskState = TaskStates::MODOMEDIO;
                }
            }
        }

        break;
    }
    case TaskStates::PAPAGADO:
    {

        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::UP_BTN)
            {
                ledStatus = true;
                digitalWrite(ledRed, ledStatus);

                lasTime = millis();
                taskState = TaskStates::MODOLENTO;
            }
            else if (buttonEvt.whichButton == BUTTONS::DOWN_BTN)
            {
                ledStatus = true;
                digitalWrite(ledRed, ledStatus);

                lasTime = millis();
                estabaenPAPAGADO = true;
                taskState = TaskStates::MODORAPIDO;
            }
        }

        break;
    }

    case TaskStates::MODOMEDIO:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= INTERVAL2)
        {
            lasTime = currentTime;
            ledStatus = !ledStatus;
            digitalWrite(ledRed, ledStatus);

            if (buttonEvt.trigger == true)
            {
                buttonEvt.trigger = false;
                if (buttonEvt.whichButton == BUTTONS::UP_BTN)
                {
                    ledStatus = true;
                    digitalWrite(ledRed, ledStatus);

                    taskState = TaskStates::PENCENDIDO;
                }
                else if (buttonEvt.whichButton == BUTTONS::DOWN_BTN)
                {
                    ledStatus = true;
                    digitalWrite(ledRed, ledStatus);

                    lasTime = millis();
                    taskState = TaskStates::MODOLENTO;
                }
            }
        }

        break;
    }
    case TaskStates::PENCENDIDO:
    {

        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::UP_BTN)
            {
                ledStatus = true;
                digitalWrite(ledRed, ledStatus);

                lasTime = millis();
                taskState = TaskStates::MODOMEDIO;
            }
            else if (buttonEvt.whichButton == BUTTONS::DOWN_BTN)
            {
                ledStatus = true;
                digitalWrite(ledRed, ledStatus);
                lasTime = millis();
                estabaenPAPAGADO = false;
                taskState = TaskStates::MODORAPIDO;
            }
        }

        break;
    }
    case TaskStates::MODORAPIDO:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= INTERVAL3)
        {
            lasTime = currentTime;
            ledStatus = !ledStatus;
            digitalWrite(ledRed, ledStatus);
        }
        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            disarmKey[keyCounter] = buttonEvt.whichButton;
            keyCounter++;
            if (keyCounter == 5)
            {
                keyCounter = 0;
                if (compareKeys(secret, disarmKey) == true)
                {

                    Serial.print("salir de modo rápido\n");

                    if (estabaenPAPAGADO == true)
                    {
                        ledStatus = false;
                        digitalWrite(ledRed, ledStatus);
                        taskState = TaskStates::PAPAGADO;
                    }
                    else
                    {
                        ledStatus = true;
                        digitalWrite(ledRed, ledStatus);

                        taskState = TaskStates::PENCENDIDO;
                    }
                }
                else
                {
                    Serial.print("código equivocado\n");
                }
            }
        }

        break;
    }

    default:
    {
        break;
    }
    }
}
