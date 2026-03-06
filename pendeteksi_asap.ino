
#define BLYNK_TEMPLATE_ID "###########"
#define BLYNK_TEMPLATE_NAME "#########"
#define BLYNK_AUTH_TOKEN "###################"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

int smokeSensorPin = 34;   
int relayFanPin = 19;      
int relayVapePin = 18;     

bool alatOn = false;       
bool warmingUpMode = false; 

char auth[] = "#############";
char ssid[] = "###########";
char pass[] = "##########";

BlynkTimer timer;

const int smokeThreshold = 500;

void setup() {
  Serial.begin(9600);
  pinMode(smokeSensorPin, INPUT);
  pinMode(relayFanPin, OUTPUT);
  pinMode(relayVapePin, OUTPUT);

  digitalWrite(relayFanPin, HIGH);
  digitalWrite(relayVapePin, HIGH);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  timer.setInterval(1000L, sendSensor);

  Serial.println("Monitoring dimulai...");
}

void loop() {
  Blynk.run();
  timer.run();
}

void sendSensor() {
  int smokeValue = analogRead(smokeSensorPin);

  Blynk.virtualWrite(V14, smokeValue);
  Serial.print("Smoke Value: ");
  Serial.println(smokeValue);

  if (alatOn && !warmingUpMode) {
    if (smokeValue > smokeThreshold) {
      digitalWrite(relayFanPin, HIGH); 
      digitalWrite(relayVapePin, HIGH);
      Serial.println("Smoke detected! Kipas dan vape OFF.");
    } else {
      digitalWrite(relayFanPin, LOW); 
      digitalWrite(relayVapePin, LOW); 
      Serial.println("Air clear. Kipas dan vape ON.");
    }
  }
}

BLYNK_WRITE(V0) {
  alatOn = param.asInt(); 

  if (alatOn) {
    Serial.println("Alat: ON");
    if (!warmingUpMode) {
      digitalWrite(relayFanPin, LOW); 
      digitalWrite(relayVapePin, LOW); 
    }
  } else {
    Serial.println("Alat: OFF");
    digitalWrite(relayFanPin, HIGH); 
    digitalWrite(relayVapePin, HIGH); 
  }
}

BLYNK_WRITE(V1) {
  warmingUpMode = param.asInt();

  if (warmingUpMode) {
    Serial.println("Warming Up dimulai...");
    digitalWrite(relayFanPin, HIGH);
    digitalWrite(relayVapePin, HIGH); 

    timer.setTimeout(300000L, []() {
      warmingUpMode = false;
      Serial.println("Warming Up selesai...");
      if (alatOn) {
        digitalWrite(relayFanPin, LOW); 
        digitalWrite(relayVapePin, LOW); 
      }
    });
  } else {
    Serial.println("Warming Up dihentikan...");
    if (alatOn) {
      digitalWrite(relayFanPin, LOW); // Hidupkan kipas
      digitalWrite(relayVapePin, LOW); // Hidupkan vape
    }
  }
}
