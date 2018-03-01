#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#include "missions.h"
#define DELAY 20
#define TX_PIN 6
#define RX_PIN 5
SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Thermal printer(&mySerial);

boolean isPrinting = false;
int RULES_BUTTON_PIN = 13;
int MISSION_BUTTON_PIN = 2;
int maxButtonPressTime = 2000;
boolean buttonIsPressed = false;

int totalMissions = 104;
// statisics
int totalNrOfRulesPrints = 0;
int totalNrOfMissionPrints = 0;
int extraPrints = 0;
char buffer[300];

#define RED_LED 7
#define YELLOW_LED 8

void setup() {
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  lightYELLOW();

  mySerial.begin(9600);
  printer.begin();

  pinMode(RULES_BUTTON_PIN, INPUT);
  digitalWrite(RULES_BUTTON_PIN, HIGH);

  pinMode(MISSION_BUTTON_PIN, INPUT);
  digitalWrite(MISSION_BUTTON_PIN, HIGH);

  printer.sleep();
}

void lightRED () {
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, HIGH);
}

void lightYELLOW () {
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
}

void loadBuffer (int index) {
  if (totalNrOfMissionPrints == ((totalMissions*2)) ) {
    strcpy_P(buffer, (char*)pgm_read_word(&(endArray[0])));
    return;
  }

  if (totalNrOfMissionPrints >= totalMissions) {
    strcpy_P(buffer, (char*)pgm_read_word(&(missionsArray[totalNrOfMissionPrints - totalMissions])));
  } else {
    strcpy_P(buffer, (char*)pgm_read_word(&(missionsArray[totalNrOfMissionPrints])));
  }
}

void loadBufferRules (int index) {
  strcpy_P(buffer, (char*)pgm_read_word(&(rulesArray[index])));
}

void loadBufferExtra (int index) {
  strcpy_P(buffer, (char*)pgm_read_word(&(extraArray[index])));
}

void printMission() {
  if (isPrinting) {
    return;
  }
  lightRED();
  printer.wake();
  // man skulle kunna varigera reglerna lite ;)
  isPrinting = true;
  loadBuffer(1);
  printer.underlineOn();
  printer.setSize('M');
  printer.justify('C');
  printer.println("UPPDRAG");
  printer.underlineOff();
  printer.justify('L');
  printer.setSize('S');
  printer.println(buffer);

  if (totalNrOfMissionPrints == 20 || totalNrOfMissionPrints == 31 || totalNrOfMissionPrints == 49 || totalNrOfMissionPrints == 13 || totalNrOfMissionPrints == 83 || totalNrOfMissionPrints == 36) {
    printer.underlineOn();
    printer.println("detta ar uppdrag");
    printer.underlineOff();
    printer.setSize('M');
    printer.justify('C');
    printer.println(totalNrOfMissionPrints);
    printer.justify('L');
    printer.setSize('S');
  }

  if (totalNrOfMissionPrints > 20 && totalNrOfMissionPrints % 5 == 0) {
    printExtra();
  }

  printer.feed(5);

  totalNrOfMissionPrints = totalNrOfMissionPrints + 1;
  isPrinting = false;
  printer.setDefault();
  printer.sleep();
  lightYELLOW();
}

void printExtra () {
  printer.feed(2);
  if (extraPrints >= 11) {
    return;
  }
  loadBufferExtra(extraPrints);
  printer.underlineOn();
  printer.setSize('M');
  printer.justify('C');
  printer.println("EXTRA");
  printer.underlineOff();
  printer.justify('L');
  printer.setSize('S');
  printer.println(buffer);
  extraPrints = extraPrints + 1;
}

void printRules() {
  if (isPrinting) {
    return;
  }
  lightRED();
  printer.wake();
  // man skulle kunna varigera reglerna lite ;)
  isPrinting = true;
  printer.underlineOn();
  printer.setSize('M');
  printer.justify('C');
  printer.println("Sch tyst hemligt ;)");
  printer.setSize('L');
  printer.println("Spel regler");
  printer.feed(2);
  printer.setSize('M');
  printer.println("Forklaring");
  printer.underlineOff();

  printer.justify('L');
  printer.setSize('S');
  printer.println("Denna leken gar ut pa att gora sa manga uppdrag som mojligt utan att nagon annan marker.");
  printer.setSize('M');
  printer.justify('C');
  printer.underlineOn();
  printer.println("Regler");
  printer.underlineOff();
  printer.justify('L');
  printer.setSize('S');
  printer.println("1. Du far inte prata / beratta om spelet eller visa reglerna for nagon.");
  printer.feed(1);
  if (totalNrOfRulesPrints == 6 || 11) {
    loadBufferRules(0);
  } else {
    loadBufferRules(1);
  }

  printer.println(buffer);
  printer.feed(1);
  if (totalNrOfRulesPrints == 1 || 4) {
    loadBufferRules(2);
  } else {
    loadBufferRules(3);
  }
  printer.println(buffer);
  printer.feed(1);
  printer.println("4. Det finns ingen begransing i hur manga du far gora");
  printer.feed(1);
  if (totalNrOfRulesPrints == 0 || 5) {
    loadBufferRules(5);
  } else {
    loadBufferRules(4);
  }
  printer.println(buffer);
  printer.feed(1);
  printer.println("6. Alla uppdrag ska utforas sa att nagon ser.");
  printer.feed(1);
  printer.boldOn();
  printer.println("Spelet slutar kl 22:00.");
  printer.boldOff();

  // dom har kan man kanske ladda i MEM

  if (totalNrOfRulesPrints == 0) {
    printer.println("Du ar den forsta spelaren :O");
  }

  if (totalNrOfRulesPrints == 7) {
    printer.println("Du ligger efter! Nastan alla spelar redan spelet.");
  }

  if (totalNrOfRulesPrints == 4) {
    printer.println("Du ar varken forst eller sist med att borja spela");
  }

  totalNrOfRulesPrints = totalNrOfRulesPrints + 1;
  printer.feed(3);
  isPrinting = false;
  printer.setDefault();
  printer.sleep();
  lightYELLOW();
}

boolean handle_button(int buttonPin) {
  int button_pressed = !digitalRead(buttonPin);
  return button_pressed;
}

void loop() {
  boolean rules_button_pressed = handle_button(RULES_BUTTON_PIN);
  boolean mission_button_pressed = handle_button(MISSION_BUTTON_PIN);

  if (rules_button_pressed ) {
    printRules();
  } else if (mission_button_pressed) {
    printMission();
  }
  delay(DELAY);
}
