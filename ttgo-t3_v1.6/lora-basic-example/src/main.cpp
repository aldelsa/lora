/*********
  Alejandro Delgado - Ejemplo básico de envío/recepción LORA
  Basado en https://RandomNerdTutorials.com/ttgo-lora32-sx1276-arduino-ide/
*********/

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pines LoRa
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

// Frecuencia LoRa
#define BAND 866E6

// Pines OLED
#define OLED_SDA 21
#define OLED_SCL 22 
#define OLED_RST 23
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// Contador de paquetes (solo para TX)
int counter = 0;

void setup() {
  Serial.begin(115200);

  // Reinicio de OLED
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  // Inicializar OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);

  // Configurar LoRa
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  LoRa.setSpreadingFactor(10);
  Serial.println("LoRa Initializing OK!");

  #if MODE == TRANSMITTER
    display.print("LORA TRANSMITTER");
  #elif MODE == RECEIVER
    display.print("LORA RECEIVER");
  #endif

  display.display();
  delay(2000);
}

// Función para enviar datos
void enviarPaquete() {
  Serial.print("Enviando paquete: ");
  Serial.println(counter);

  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("LORA TRANSMITTER");
  display.setCursor(0,20);
  display.print("LoRa packet sent.");
  display.setCursor(0,30);
  display.print("Counter:");
  display.setCursor(50,30);
  display.print(counter);
  display.display();

  counter++;
}

// Función para recibir datos
void recibirPaquete() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Paquete recibido: ");

    String receivedText = "";
    while (LoRa.available()) {
      receivedText += (char)LoRa.read();
    }

    Serial.println(receivedText);
    
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("LORA RECEIVER");
    display.setCursor(0,20);
    display.print("Packet:");
    display.setCursor(50,20);
    display.print(receivedText);
    display.display();
  }
}

void loop() {
  #if MODE == TRANSMITTER
    enviarPaquete();
  #elif MODE == RECEIVER
    recibirPaquete();
  #endif

  delay(10000);
}

