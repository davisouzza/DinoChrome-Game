#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BUTTON_PIN 4 // Pino D4 do botao

Preferences prefs;
int score = 0;
int highScore = 0;
int dinoY = 48;
int dinoV = 0;
bool isJumping = false;
bool gameStarted = false; // Controla o estado da tela inicial

// Obstaculos
int obstacleX = 128;
int obstacleY = 48;
int obstacleType = 0; 
int obstacleSpeed = 5;

bool legState = false;
bool wingState = false;

// Bitmaps do Dino
static const unsigned char PROGMEM dino_run1[] = {
  0x07, 0x80, 0x1f, 0xc0, 0x3f, 0xe0, 0x3f, 0xe0, 0x3f, 0xe0, 0x38, 0x00, 0x7e, 0x00, 0x7e, 0x00, 
  0x7e, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x60, 0x00, 0x70, 0x00, 0x30, 0x00, 0x18, 0x00, 0x08, 0x00
};

static const unsigned char PROGMEM dino_run2[] = {
  0x07, 0x80, 0x1f, 0xc0, 0x3f, 0xe0, 0x3f, 0xe0, 0x3f, 0xe0, 0x38, 0x00, 0x7e, 0x00, 0x7e, 0x00, 
  0x7e, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x06, 0x00, 0x0e, 0x00, 0x0c, 0x00, 0x18, 0x00, 0x10, 0x00
};

// Bitmaps dos Cactos
static const unsigned char PROGMEM cactus1_bmp[] = {
  0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x11, 0x10, 0x31, 0x10, 0x31, 0x10, 0x31, 0x10, 
  0x11, 0x10, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00
};

static const unsigned char PROGMEM cactus2_bmp[] = {
  0x10, 0x82, 0x10, 0x82, 0x10, 0x82, 0x10, 0x82, 0x91, 0x8b, 0xd1, 0x8b, 0xd1, 0x8b, 0xd1, 0x8b, 
  0x91, 0x8b, 0x10, 0x82, 0x10, 0x82, 0x10, 0x82, 0x10, 0x82, 0x10, 0x82, 0x10, 0x82, 0x10, 0x82
};

// Bitmaps do Passaro
static const unsigned char PROGMEM bird1_bmp[] = {
  0x00, 0x00, 0x20, 0x00, 0x30, 0x00, 0x38, 0x00, 0x3f, 0x00, 0x3f, 0xf8, 0x1f, 0xfc, 0x0f, 0xfe, 
  0x07, 0xf0, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM bird2_bmp[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf8, 0x1f, 0xfc, 0x0f, 0xfe, 
  0x07, 0xf0, 0x03, 0x38, 0x01, 0x30, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Bitmap do Trofeu
static const unsigned char PROGMEM trophy_bmp[] = {
  0x7f, 0xfe, 0xff, 0xff, 0x9f, 0xf9, 0x9f, 0xf9, 0x4f, 0xe2, 0x27, 0xc4, 0x13, 0xc8, 0x03, 0xc0, 
  0x01, 0x80, 0x01, 0x80, 0x03, 0xc0, 0x00, 0x00, 0x07, 0xe0, 0x1f, 0xf8, 0x3f, 0xfc, 0x00, 0x00
};

void resetObstacle() {
  obstacleX = 128;
  obstacleType = random(0, 3);
  
  if (obstacleType == 0 || obstacleType == 1) {
    obstacleY = 48;
  } else {
    obstacleY = 44;
  }
}

void showStartScreen() {
  display.clearDisplay();
  
  display.setTextSize(2);
  display.setCursor(10, 8);
  display.print("DINO GAME");

  // Desenha o Dino parado na tela de inicio
  display.drawBitmap(56, 26, dino_run1, 16, 16, WHITE);

  display.setTextSize(1);
  display.setCursor(12, 45);
  display.print("Aperte para iniciar");

  display.setCursor(35, 56);
  display.print("HI: ");
  display.print(highScore);

  display.display();
}

void showVictoryScreen() {
  if (score > highScore) {
    highScore = score;
    prefs.putInt("highscore", highScore);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 2);
  display.print("PARABENS!");

  display.drawBitmap(56, 22, trophy_bmp, 16, 16, WHITE);

  display.setTextSize(1);
  display.setCursor(12, 42);
  display.print("VOCE ZEROU O JOGO!");
  
  display.setCursor(25, 53);
  display.print("Score: 100 PTS");

  display.display();
  delay(4000);

  gameStarted = false; // Retorna para a tela de Start
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;);
  }
  
  randomSeed(analogRead(0));
  
  prefs.begin("dino-game", false);
  highScore = prefs.getInt("highscore", 0);

  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop() {
  // Tela Inicial (Aguarda clique do jogador)
  if (!gameStarted) {
    showStartScreen();
    
    if (digitalRead(BUTTON_PIN) == LOW) {
      score = 0;
      dinoY = 48;
      resetObstacle();
      gameStarted = true;
      delay(300); // Pausa para evitar que o clique de start vire um pulo instantaneo
    }
    delay(50);
    return;
  }

  display.clearDisplay();

  // Placar
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("PTS:");
  display.print(score);
  display.setCursor(65, 0);
  display.print("HI:");
  display.print(highScore);

  // Leitura do Botao (Pulo)
  if (digitalRead(BUTTON_PIN) == LOW && !isJumping) {
    dinoV = -8;
    isJumping = true;
  }

  // Gravidade
  if (isJumping) {
    dinoY += dinoV;
    dinoV += 1;
    if (dinoY >= 48) {
      dinoY = 48;
      isJumping = false;
      dinoV = 0;
    }
  }

  // Velocidade e Aceleração
  obstacleSpeed = 5 + (score / 15);
  if (obstacleSpeed > 9) obstacleSpeed = 9;
  
  obstacleX -= obstacleSpeed;

  if (obstacleX < -16) {
    score++;
    if (score >= 100) {
      showVictoryScreen();
      return;
    }
    resetObstacle();
  }

  // Anima pernas
  if (!isJumping) legState = !legState;
  if (legState) {
    display.drawBitmap(16, dinoY, dino_run1, 16, 16, WHITE);
  } else {
    display.drawBitmap(16, dinoY, dino_run2, 16, 16, WHITE);
  }

  // Anima asas
  wingState = !wingState;

  // Desenha Obstaculo
  if (obstacleType == 0) {
    display.drawBitmap(obstacleX, obstacleY, cactus1_bmp, 16, 16, WHITE);
  } else if (obstacleType == 1) {
    display.drawBitmap(obstacleX, obstacleY, cactus2_bmp, 16, 16, WHITE);
  } else {
    if (wingState) {
      display.drawBitmap(obstacleX, obstacleY, bird1_bmp, 16, 16, WHITE);
    } else {
      display.drawBitmap(obstacleX, obstacleY, bird2_bmp, 16, 16, WHITE);
    }
  }

  // Sistema de Colisao
  bool hit = false;
  if (obstacleX < 28 && obstacleX > 4) {
    if (dinoY < (obstacleY + 13) && (dinoY + 13) > obstacleY) {
      hit = true;
    }
  }

  if (hit) {
    if (score > highScore) {
      highScore = score;
      prefs.putInt("highscore", highScore);
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(10, 15);
    display.print("GAME OVER");
    
    display.setTextSize(1);
    display.setCursor(10, 40);
    display.print("Score: ");
    display.print(score);
    display.setCursor(10, 52);
    display.print("Recorde: ");
    display.print(highScore);
    
    display.display();
    delay(2500);

    gameStarted = false; // Retorna para a tela de Start
  }

  display.display();
  delay(25);
}