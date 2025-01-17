#include "deviceControl.h"

extern ManualConfig manualConfig; // Globální definice, která je přístupná ve všech souborech


void updateZap() {
    Zap = 0;
    if ((manualConfig.DeviceType & LED_WHITE1) && led1State) {
        Zap |= LED_WHITE1;
    }
    if ((manualConfig.DeviceType & LED_WHITE2) && led2State) {
        Zap |= LED_WHITE2;
    }
    if ((manualConfig.DeviceType & LED_WHITE3) && led3State) {
        Zap |= LED_WHITE3;
    }
    if ((manualConfig.DeviceType & LED_RGB) && ledRGBState) {
        Zap |= LED_RGB;
    }
    if ((manualConfig.DeviceType & DEVICE_RELAY) && relayState) {
        Zap |= DEVICE_RELAY;
    }
}

void changeState() {
    if (Zap == 0) {
        if (manualConfig.Stisk & LED_WHITE1) {
            led1State = true;
        }
        if (manualConfig.Stisk & LED_WHITE2) {
            led2State = true;
        }
        if (manualConfig.Stisk & LED_WHITE3) {
            led3State = true;
        }
        if (manualConfig.Stisk & LED_RGB) {
            ledRGBState = true;
        }
        if (manualConfig.Stisk & DEVICE_RELAY) {
            relayState = true;
        }
    } else {
        if (manualConfig.DeviceType & LED_WHITE1) {
            led1State = false;
        }
        if (manualConfig.DeviceType & LED_WHITE2) {
            led2State = false;
        }
        if (manualConfig.DeviceType & LED_WHITE3) {
            led3State = false;
        }
        if (manualConfig.DeviceType & LED_RGB) {
            ledRGBState = false;
        }
        if (manualConfig.DeviceType & DEVICE_RELAY) {
            relayState = false;
        }
    }
}

void ledKontolaZapnuti() {
    if (Zap == 0) {
        analogWrite(PwrSw, 0);
    } else {
        analogWrite(PwrSw, LedL);
    }
}

void aktivaceZarizeni() {
    if (manualConfig.DeviceType & LED_WHITE1) {
        analogWrite(PwrRed, led1State ? led1Brightness : 0);
    }
    if (manualConfig.DeviceType & LED_WHITE2) {
        analogWrite(PwrGreen, led2State ? led2Brightness : 0);
    }
    if (manualConfig.DeviceType & LED_WHITE3) {
        analogWrite(PwrBlue, led3State ? led3Brightness : 0);
    }
    if (manualConfig.DeviceType & LED_RGB) {
        if (ledRGBState) {
            analogWrite(PwrRed, Red);
            analogWrite(PwrGreen, Green);
            analogWrite(PwrBlue, Blue);
        } else {
            analogWrite(PwrRed, 0);
            analogWrite(PwrGreen, 0);
            analogWrite(PwrBlue, 0);
        }
    }
    if (manualConfig.DeviceType & DEVICE_RELAY) {
        digitalWrite(Re, relayState ? HIGH : LOW);
    }
    updateZap();
    ledKontolaZapnuti();
}