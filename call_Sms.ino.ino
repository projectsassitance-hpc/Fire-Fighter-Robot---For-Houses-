#include <SoftwareSerial.h>

// HarishProjects.com
const String PHONE_1 = "+919131808247"; // Receiver number
const String PHONE_2 = ""; // Optional
const String PHONE_3 = ""; // Optional

#define rxPin 4
#define txPin 3
SoftwareSerial sim800L(rxPin, txPin);

#define soilPin 2      // Soil moisture sensor on D2
#define buzzer_pin 5

boolean soil_flag = 0; // To prevent multiple alerts

void setup() {
  Serial.begin(115200);
  sim800L.begin(9600);

  pinMode(soilPin, INPUT);
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, LOW);

  Serial.println("Initializing GSM module...");
  sim800L.println("AT");
  delay(1000);
  sim800L.println("AT+CMGF=1");
  delay(1000);
  Serial.println("Setup done. Waiting for soil sensor...");
}

void loop() {
  while (sim800L.available()) {
    Serial.println(sim800L.readString());
  }

  int soilState = digitalRead(soilPin); // Read soil sensor

  Serial.print("Soil Sensor (D2) State: ");
  Serial.println(soilState);

  // ⚠️ Most sensors: LOW = Wet (trigger condition)
  if (soilState == LOW && soil_flag == 0) {
    Serial.println("Moisture detected! Triggering alert...");
    soil_flag = 1;
    digitalWrite(buzzer_pin, HIGH);

    send_multi_sms();
    make_multi_call();

    delay(10000); // Buzzer ON for 10 seconds
    digitalWrite(buzzer_pin, LOW);
  }

  // Reset flag when soil becomes dry
  else if (soilState == HIGH) {
    if (soil_flag == 1) {
      Serial.println("Soil dry again, system reset.");
    }
    soil_flag = 0;
    digitalWrite(buzzer_pin, LOW);
  }

  delay(1000);
}

// --- Helper Functions ---

void send_multi_sms() {
  if (PHONE_1 != "") {
    send_sms("Alert: Fire Detected", PHONE_1);
  }
  if (PHONE_2 != "") {
    send_sms("Alert: Soil moisture detected!", PHONE_2);
  }
  if (PHONE_3 != "") {
    send_sms("Alert: Soil moisture detected!", PHONE_3);
  }
}

void make_multi_call() {
  if (PHONE_1 != "") {
    make_call(PHONE_1);
  }
  if (PHONE_2 != "") {
    make_call(PHONE_2);
  }
  if (PHONE_3 != "") {
    make_call(PHONE_3);
  }
}

void send_sms(String text, String phone) {
  Serial.println("Sending SMS...");
  sim800L.print("AT+CMGF=1\r");
  delay(1000);
  sim800L.print("AT+CMGS=\"" + phone + "\"\r");
  delay(1000);
  sim800L.print(text);
  delay(100);
  sim800L.write(0x1A); // Ctrl+Z
  delay(5000);
  Serial.println("SMS sent.");
}

void make_call(String phone) {
  Serial.println("Calling...");
  sim800L.println("ATD" + phone + ";");
  delay(20000); // 20 seconds
  sim800L.println("ATH"); // Hang up
  delay(1000);
  Serial.println("Call ended.");
}