#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// settings
const uint8_t
  RADIO_CE_PIN = 8,
  RADIO_CSN_PIN = 7,
  BTN_UP_PIN = 2,
  BTN_DOWN_PIN = 3,
  BTN_LEFT_PIN = 4,
  BTN_RIGHT_PIN = 5,
  BTN_SELECT_PIN = 6,
  BTN_MODE_PIN = 9;

const byte RADIO_ADDRESS[6] = "00001";

// display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// kb
struct Key {
  char symbol;
  bool isSpecial;
};

const Key KEYBOARD[4][13] = { //kb buttons
  { { '1' }, { '2' }, { '3' }, { '4' }, { '5' }, { '6' }, { '7' }, { '8' }, { '9' }, { '0' }, { '-' }, { '=' }, { ' ' } },
  { { 'q' }, { 'w' }, { 'e' }, { 'r' }, { 't' }, { 'y' }, { 'u' }, { 'i' }, { 'o' }, { 'p' }, { ' ' }, { ' ' }, { 'B', true } },
  { { 'a' }, { 's' }, { 'd' }, { 'f' }, { 'g' }, { 'h' }, { 'j' }, { 'k' }, { 'l' }, { '+' }, { ' ' }, { ' ' }, { 'C', true } },
  { { 'z' }, { 'x' }, { 'c' }, { 'v' }, { 'b' }, { 'n' }, { 'm' }, { ',' }, { '.' }, { '/' }, { ' ' }, { ' ' }, { 'S', true } }
};

const uint8_t
  KEY_WIDTH = 8,
  KEY_HEIGHT = 10,
  KEY_SPACING = 1,
  TEXT_OFFSET_X = 1,
  TEXT_OFFSET_Y = 2,
  MAX_MESSAGES = 3;  //3 msgs in log

//vars
RF24 radio(RADIO_CE_PIN, RADIO_CSN_PIN);

struct {
  int8_t x = 0;
  int8_t y = 0;
} cursor;

char inputBuffer[32] = "";
String messageLog[MAX_MESSAGES];  //log
uint8_t bufferIndex = 0;
bool
  lastSelectState = HIGH,
  lastModeState = HIGH,
  isTransmitterMode = true,
  newMessageFlag = false,
  displayInverted = false;

//func
void initRadio() {
  if (!radio.begin()) {
    while (true) {
      oled.clearDisplay();
      oled.println("Radio error!");
      oled.display();
      delay(1000);
    }
  }
  radio.openWritingPipe(RADIO_ADDRESS);
  radio.openReadingPipe(1, RADIO_ADDRESS);
  radio.startListening();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(1);
  radio.setRetries(15, 15);
}

void addToMessageLog(String msg) {
  //move old msgs
  for (int i = MAX_MESSAGES - 1; i > 0; i--) {
    messageLog[i] = messageLog[i - 1];
  }
  messageLog[0] = msg;
}

void checkForMessages() {
  if (radio.available()) {
    char incoming[32] = "";
    radio.read(&incoming, sizeof(incoming));

    addToMessageLog(String(incoming));
    newMessageFlag = true;

    //blink
    if (isTransmitterMode) {
      for (int i = 0; i < 3; i++) {
        oled.invertDisplay(true);
        delay(150);
        oled.invertDisplay(false);
        delay(150);
      }
    }
  }
}

void drawTransmitterScreen() {
  oled.clearDisplay();

  // Отрисовка клавиатуры
  for (uint8_t y = 0; y < 4; y++) {
    for (uint8_t x = 0; x < 13; x++) {
      const uint8_t xPos = x * (KEY_WIDTH + KEY_SPACING);
      const uint8_t yPos = y * (KEY_HEIGHT + KEY_SPACING);



      if (x == cursor.x && y == cursor.y) {
        oled.fillRect(xPos, yPos, KEY_WIDTH, KEY_HEIGHT, WHITE);
        oled.setTextColor(BLACK);
      } else {
        oled.setTextColor(WHITE);
      }

      oled.setCursor(xPos + TEXT_OFFSET_X, yPos + TEXT_OFFSET_Y);
      oled.write(KEYBOARD[y][x].symbol);
    }
  }

  // Область ввода
  oled.drawRect(0, 50, SCREEN_WIDTH, 14, WHITE);
  oled.setCursor(2, 52);
  if (strlen(inputBuffer) > 18) {
    oled.print("...");
    oled.print(inputBuffer + strlen(inputBuffer) - 15);
  } else {
    oled.print(inputBuffer);
  }

  oled.display();
}

void drawReceiverScreen() {
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.println("Received messages:");
  oled.drawFastHLine(0, 10, SCREEN_WIDTH, WHITE);

  // Вывод лога сообщений
  for (int i = 0; i < MAX_MESSAGES; i++) {
    if (messageLog[i].length() > 0) {
      oled.setCursor(0, 15 + i * 15);
      if (messageLog[i].length() > 21) {
        oled.print(messageLog[i].substring(0, 21));
        oled.print("...");
      } else {
        oled.print(messageLog[i]);
      }
    }
  }

  // Индикация новых сообщений
  if (newMessageFlag) {
    oled.fillRect(120, 0, 8, 8, WHITE);
    newMessageFlag = false;
  }

  oled.display();
}

void handleModeSwitch() {
  bool currentModeState = digitalRead(BTN_MODE_PIN);
  if (currentModeState == LOW && lastModeState == HIGH) {
    delay(50);
    isTransmitterMode = !isTransmitterMode;
    if (isTransmitterMode) {
      drawTransmitterScreen();
    } else {
      drawReceiverScreen();
    }
    delay(200);
  }
  lastModeState = currentModeState;
}

void handleTransmitterInput() {
  // Навигация
  if (digitalRead(BTN_UP_PIN) == LOW) {
    cursor.y = (cursor.y - 1 + 4) % 4;
    drawTransmitterScreen();
    delay(200);
  }
  if (digitalRead(BTN_DOWN_PIN) == LOW) {
    cursor.y = (cursor.y + 1) % 4;
    drawTransmitterScreen();
    delay(200);
  }
  if (digitalRead(BTN_LEFT_PIN) == LOW) {
    cursor.x = (cursor.x - 1 + 13) % 13;
    drawTransmitterScreen();
    delay(200);
  }
  if (digitalRead(BTN_RIGHT_PIN) == LOW) {
    cursor.x = (cursor.x + 1) % 13;
    drawTransmitterScreen();
    delay(200);
  }

  //symbol choose
  bool currentSelectState = digitalRead(BTN_SELECT_PIN);
  if (currentSelectState == LOW && lastSelectState == HIGH) {
    delay(50);
    const Key currentKey = KEYBOARD[cursor.y][cursor.x];

    if (currentKey.symbol == 'B' && bufferIndex > 0) {  //bksp
      inputBuffer[--bufferIndex] = '\0';
    } else if (currentKey.symbol == 'C') {  //clr
      bufferIndex = 0;
      inputBuffer[0] = '\0';
    } else if (currentKey.symbol == 'S') {  //send
      sendMessage();
    } else if (bufferIndex < sizeof(inputBuffer) - 1) {  //regular simbl
      inputBuffer[bufferIndex++] = currentKey.symbol;
      inputBuffer[bufferIndex] = '\0';
    }

    drawTransmitterScreen();
    delay(200);
  }
  lastSelectState = currentSelectState;
}

void sendMessage() {
  if (bufferIndex == 0) return;

  radio.stopListening();
  bool success = radio.write(&inputBuffer, sizeof(inputBuffer));
  radio.startListening();

  if (success) {
    bufferIndex = 0;
    inputBuffer[0] = '\0';

    //blink success
    for (int i = 0; i < 2; i++) {
      oled.invertDisplay(true);
      delay(100);
      oled.invertDisplay(false);
      delay(100);
    }
  }

  drawTransmitterScreen();
}

//main
void setup() {
  //pin init
  pinMode(BTN_UP_PIN, INPUT_PULLUP);
  pinMode(BTN_DOWN_PIN, INPUT_PULLUP);
  pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
  pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BTN_SELECT_PIN, INPUT_PULLUP);
  pinMode(BTN_MODE_PIN, INPUT_PULLUP);

  //devices init
  initRadio();
  oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);

  //first draw
  drawTransmitterScreen();
}

void loop() {
  checkForMessages();
  handleModeSwitch();

  if (isTransmitterMode) {
    handleTransmitterInput();
  } else {
    drawReceiverScreen();
    delay(100);
  }
}