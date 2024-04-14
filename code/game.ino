// Row pins of matrix
int cathode[8] = {9,8,7,6,5,4,3,2};

// Pins for controlling shift register
int dataPin = 10;
int clockPin = 11;
int latchPin = 12;


void setup() {
    Serial.begin(9600);

    pinMode(dataPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    
    for (int i = 0; i < (sizeof(cathode)/sizeof(*cathode)); i++) {
        pinMode(cathode[i], OUTPUT);
        digitalWrite(cathode[i], HIGH);
    }

    digitalWrite(latchPin, HIGH);
}

uint8_t testFrame[8] = {
	0x30,0x18,0x0,0x0,0x0,0x0,0x0,0x0,
};
void loop()
{
    drawFrame(testFrame);
}

// Function for drawing single given frame
void drawFrame(uint8_t frame[8]) {
    for (int i=0; i<(sizeof(cathode)/sizeof(*cathode)); i++) {
        if (i != 0){
            digitalWrite(cathode[i-1], HIGH);
        } else {
            digitalWrite(cathode[7], HIGH);
        };
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, LSBFIRST, frame[i]);  
        digitalWrite(latchPin, HIGH);
        digitalWrite(cathode[i], LOW);
    }
}
