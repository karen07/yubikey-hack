int incomingByte = 0;

#define RELAY_IN 5
void setup()
{
    Serial.begin(9600);
    pinMode(RELAY_IN, OUTPUT);
}

void loop()
{
    if (Serial.available() > 0) {
        incomingByte = Serial.read();

        if (incomingByte == 'k') {
            Serial.print((char)incomingByte);
            Serial.print('\n');
            delay(500);
            digitalWrite(RELAY_IN, HIGH);
            delay(500);
            digitalWrite(RELAY_IN, LOW);
        }
    }
}
