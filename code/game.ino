// Row pins of matrix
int cathode[8] = {9,8,7,6,5,4,3,2};

// Buttons pins
int buttons[4] = {A0, A1, A2, A3};

// Pins for controlling shift register
int dataPin = 10;
int clockPin = 11;
int latchPin = 12;

// Blocks
uint8_t blocks[7][8] = {
    {0x80,0xE0,0x0,0x0,0x0,0x0,0x0,0x0}, // J
    {0xE0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}, // I
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
bool dir[4] = {false,false,false,false};

int currentBlock = 0;
bool chooseBlock = true;


int frameCounter = 0;
int gravityFrameNumber = 9;


void loop()
{ 
    if (chooseBlock) {
        chooseRandomBlock();
        chooseBlock = false;
    }

    readButtons();
    move(dir);
    
    drawFrame(70);
    frameCounter++;
}

void chooseRandomBlock() {
    for (int i=0; i<(sizeof(frame)/sizeof(*frame)); i++) {
        frame[i] = 0x0;
        blockFrame[i] = 0x0;
    }
    currentBlock = randomGen();

    for (int i=0; i<(sizeof(blockFrame)/sizeof(*blockFrame)); i++) {
        blockFrame[i] = blocks[currentBlock][i];
    }

}


void readButtons() {
    // Move left
    if (analogRead(buttons[0]) > 512) {
        dir[0] = true;
    } else {
        dir[0] = false;
    }

    // Move down
    if (frameCounter > gravityFrameNumber) {
        dir[1] = true;
        frameCounter = 0;
    } else if (analogRead(buttons[1]) > 512) {
        dir[1] = true;
    } else {
        dir[1] = false;
    }

    // Move right
    if (analogRead(buttons[2]) > 512) {
        dir[2] = true;
    } else {
        dir[2] = false;
    }

    // Rotate
    if (analogRead(buttons[3]) > 512) {
        dir[3] = true;
    } else {
        dir[3] = false;
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
            shiftOut(dataPin, clockPin, MSBFIRST, frame[i]);  
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
    // Saving current blockFrame
    uint8_t tempBlock[8];
    for (int i=0; i<(sizeof(blockFrame)/sizeof(*blockFrame)); i++) {
      tempBlock[i] = blockFrame[i];
    }

    // Side to side movement
    for (int rowIndx=0; rowIndx<8; rowIndx++) {
        // Left
        if (dir[0]) {
            blockFrame[rowIndx] = blockFrame[rowIndx]<<1;
        } 
        // Right
        if (dir[2]) {
            blockFrame[rowIndx] = blockFrame[rowIndx]>>1;
        }
    }
    if (!checkMove(blockFrame, tempBlock)) {
        for (int i=0; i<(sizeof(tempBlock)/sizeof(*tempBlock)); i++) {
            blockFrame[i] = tempBlock[i];
        } 
    }

    // Down
    if (dir[1]) {
        int size = (sizeof(blockFrame)/sizeof(*blockFrame));
        uint8_t temp = blockFrame[size-1];
        for (int i=size-1; i>0; --i) {
            blockFrame[i] = blockFrame[i-1];
        } 
        blockFrame[0] = temp;
    }

    // Check if block hit bottom
    if (checkCollision()) {
        for (int i=0; i<(sizeof(tempBlock)/sizeof(*tempBlock)); i++) {
            blockFrame[i] = tempBlock[i];
        }
        moveBlock2Pile();
        chooseRandomBlock();
    } else {
        // Check if block hit pile
        if (checkBottom()) {
            moveBlock2Pile();
            chooseRandomBlock();
        }

    }
    
}

// Check for hitting a bottom layer
bool checkBottom() {
    if (blockFrame[7] != 0x0) {
        return true;
    }
    return false;
}

// Move block to pile frame
void moveBlock2Pile() {
    for (int i=0; i<sizeof(pileFrame)/sizeof(*pileFrame); i++) {
        uint8_t pileBit;
        uint8_t blockBit;
        uint8_t byte = 0x0;

        for(int j = 0; j < 8; j++) {
            pileBit = ((pileFrame[i] >> j) & 0x01);
            blockBit = ((blockFrame[i] >> j) & 0x01);

            byte+=(pileBit+blockBit);
            // printByte(byte);
            if (j!=7) {
              byte<<=1;
            }
            // printByte(byte);
        }
        byte = reverse_byte(byte);
        pileFrame[i] = byte;
        blockFrame[i] = 0x0;
    }
}

bool checkCollision() {
    for (int i=0; i<sizeof(pileFrame)/sizeof(*pileFrame); i++) {
        uint8_t pileBit;
        uint8_t blockBit;
        uint8_t byte = 0x0;

        for(int j = 0; j < 8; j++) {
            pileBit = ((pileFrame[i] >> j) & 0x01);
            blockBit = ((blockFrame[i] >> j) & 0x01);

            byte+=(pileBit*blockBit);
            if (j!=7) {
              byte<<=1;
            }
        }

        if (byte != 0x0) {
            return true;
        }
    }

    return false;
}

    }
    Serial.println();
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
    bool bit;
    for (int i=0; i<8; i++) {
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

// Debugging function for printing bytes
void printByte(uint8_t byte) {
    bool bit;
    for(int j = 0; j < 8; j++) {
        bit = ((byte >> j) & 0x01);
        Serial.print(bit);
    }
    Serial.println();
}


unsigned char reverse_byte(unsigned char x)
{
    static const unsigned char table[] = {
        0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
        0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
        0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
        0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
        0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
        0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
        0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
        0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
        0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
        0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
        0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
        0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
        0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
        0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
        0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
        0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
        0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
        0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
        0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
        0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
        0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
        0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
        0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
        0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
        0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
        0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
        0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
        0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
        0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
        0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
        0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
        0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
    };
    return table[x];
}