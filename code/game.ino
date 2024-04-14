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

// Layer for falling block
uint8_t blockFrame[8] = {
	0x6,0x3,0x0,0x0,0x0,0x0,0x0,0x0,
};
// Layer for piling blocks
uint8_t pileFrame[8] = {
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
};
// Merged frame
uint8_t frame[8];

void loop()
{
    bool dir[3] = {true, false, true};
    move(dir);
    // for (int rowIndx=0; rowIndx<8; rowIndx++) {
    //     Serial.println(blockFrame[rowIndx]);
    // }
    drawFrame(20);

    delay(10);
}

// Function for drawing single given frame, and leave frame on matrix for 
// t ms
void drawFrame(int t) {
    mergeFrames();
    int timePassed = 0;
    while (timePassed < t) {
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
        delay(1);
        timePassed++;
    }
}

// Function for merging blockFrame and pileFrame
void mergeFrames() {
    for (int i=0; i<8; i++) {
        frame[i] = blockFrame[i] + pileFrame[i];
    }
}


// Function to move image
// False, False, False : left, right, down
void move(bool dir[]) {
    // Left and right
    for (int rowIndx=0; rowIndx<8; rowIndx++) {
        if (dir[0]) {
            blockFrame[rowIndx] = blockFrame[rowIndx]<<1;
        } 
        if (dir[1]) {
            blockFrame[rowIndx] = blockFrame[rowIndx]>>1;
        }
    }

    // Down
    if (dir[2]) {
        uint8_t tempFrame[8];
        for (int i = 0; i < 8; i++) {
            tempFrame[i] = blockFrame[i];
        }
        blockFrame[0] = 0x0;
        for (int colIndex=1; colIndex<7; colIndex++) {
            // Serial.println(blockFrame[colIndex]);
            blockFrame[colIndex] = tempFrame[colIndex-1];
            Serial.println(blockFrame[colIndex]);
        } 
    }
}
