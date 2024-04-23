// Row pins of matrix
int cathode[8] = {9,8,7,6,5,4,3,2};

// Buttons pins
int buttons[3] = {A0, A1, A2};

// Pins for controlling shift register
int dataPin = 10;
int clockPin = 11;
int latchPin = 12;

// Blocks
uint8_t blocks[7][8] = {
    {0xF0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}, // I
    {0x80,0xE0,0x0,0x0,0x0,0x0,0x0,0x0}, // J
    {0x20,0xE0,0x0,0x0,0x0,0x0,0x0,0x0}, // L
    {0xC0,0xC0,0x0,0x0,0x0,0x0,0x0,0x0}, // O
    {0xC0,0x60,0x0,0x0,0x0,0x0,0x0,0x0}, // S
    {0x40,0xE0,0x0,0x0,0x0,0x0,0x0,0x0}, // T
    {0xC0,0x60,0x0,0x0,0x0,0x0,0x0,0x0} // Z
};


void setup() {
    Serial.begin(9600);

    // Setting up register pins
    pinMode(dataPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    digitalWrite(latchPin, HIGH);

    // Setting up matrix cathode pins
    for (int i = 0; i < (sizeof(cathode)/sizeof(*cathode)); i++) {
        pinMode(cathode[i], OUTPUT);
        digitalWrite(cathode[i], HIGH);
    }

}

// Layer for falling block
uint8_t blockFrame[8];
// Layer for piling blocks
uint8_t pileFrame[8] = {
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
};
// Merged frame
uint8_t frame[8];

// Movement direction
bool dir[3] = {false,false,false};


bool chooseBlock = true;

void loop()
{ 
    chooseRandomBlock();

    readButtons();
    move(dir);
    
    drawFrame(70);
}

void chooseRandomBlock() {
    if (chooseBlock) {
        for (int i=0; i<(sizeof(frame)/sizeof(*frame)); i++) {
            frame[i] = 0x0;
            blockFrame[i] = 0x0;
        }
        int blockNumb = randomGen();

        for (int i=0; i<(sizeof(blockFrame)/sizeof(*blockFrame)); i++) {
            blockFrame[i] = blocks[blockNumb][i];
        }

        chooseBlock = false;
    }
}


void readButtons() {
    if (analogRead(buttons[0]) > 512) {
        dir[1] = true;
    } else {
        dir[1] = false;
    }

    if (analogRead(buttons[1]) > 512) {
        dir[0] = true;
    } else {
        dir[0] = false;
    }

    if (analogRead(buttons[2]) > 512) {
        dir[2] = true;
    } else {
        dir[2] = false;
    }
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

// TODO: change so it would just set piece in needed coordinates, not move it around
void move(bool dir[]) {
    uint8_t tempBlock[8];
    for (int i=0; i<(sizeof(blockFrame)/sizeof(*blockFrame)); i++) {
      tempBlock[i] = blockFrame[i];
    }

    // Left and right
    for (int rowIndx=0; rowIndx<8; rowIndx++) {
        if (dir[0]) {
            blockFrame[rowIndx] = blockFrame[rowIndx]<<1;
        } 
        if (dir[1]) {
            blockFrame[rowIndx] = blockFrame[rowIndx]>>1;
        }
    }
    if (!checkMove(blockFrame, tempBlock)) {
        for (int i=0; i<(sizeof(tempBlock)/sizeof(*tempBlock)); i++) {
            blockFrame[i] = tempBlock[i];
        } 
    }
    

    for (int i=0; i<(sizeof(blockFrame)/sizeof(*blockFrame)); i++) {
      tempBlock[i] = blockFrame[i];
    }

    // Down
    if (dir[2]) {
        int size = (sizeof(blockFrame)/sizeof(*blockFrame));
        uint8_t temp = blockFrame[size-1];
        for (int i=size-1; i>0; --i) {
            blockFrame[i] = blockFrame[i-1];
        } 
        blockFrame[0] = temp;
    }
    if (!checkMove(blockFrame, tempBlock)) {
        for (int i=0; i<(sizeof(tempBlock)/sizeof(*tempBlock)); i++) {
            blockFrame[i] = tempBlock[i];
        } 
    }
    
}

// Function to rotate the matrix 90 degree clockwise
void rotate(int a[8][8]) {
    for (int i = 0; i < 8 / 2; i++) {
        for (int j = i; j < 8 - i - 1; j++) {
            int temp = a[i][j];
            a[i][j] = a[8-1-j][i];
            a[8-1-j][i] = a[8-1-i][8-1-j];
            a[8-1-i][8-1-j] = a[j][8-1-i];
            a[j][8-1-i] = temp;
        }
    }
}

// Check for legality of a move
bool checkMove(uint8_t* initFrame, uint8_t* modFrame) {
    // Check for side to side moves
    int initNnonZeroBytes = 0;
    int modNnonZeroBytes = 0;
    for (int i=0; i<8; i++) {
        bool bit;
        for(int j = 0; j < 8; j++) {
            bit = ((initFrame[i] >> j) & 0x01);
            if (bit) {
              initNnonZeroBytes++;
            }
        }

        for(int j = 0; j < 8; j++) {
            bit = ((modFrame[i] >> j) & 0x01);
            if (bit) {
              modNnonZeroBytes++;
            }
        }
    }

    if (initNnonZeroBytes != modNnonZeroBytes) {
        return false;
    }

    return true;
}

// Function to generate random block number
int randomGen() {
    int time = millis();
    srand(time);
    int r;
    r=rand()%7;
    
    return r;
}