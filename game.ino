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

int totalMissions = 2;
// statisics
int totalNrOfRulesPrints = 0;
int totalNrOfMissionPrints = 0;

char buffer[300];

void setup() {
  mySerial.begin(9600);
  printer.begin();

  pinMode(RULES_BUTTON_PIN, INPUT);
  digitalWrite(RULES_BUTTON_PIN, HIGH);

  pinMode(MISSION_BUTTON_PIN, INPUT);
  digitalWrite(MISSION_BUTTON_PIN, HIGH);

  printer.sleep();
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

void printMission() {
  if (isPrinting) {
    return;
  }
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
  printer.feed(1); // should me 5

  totalNrOfMissionPrints = totalNrOfMissionPrints + 1;
  isPrinting = false;
  printer.setDefault();
  printer.sleep();
}

void printRules() {
  if (isPrinting) {
    return;
  }
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
  printer.println("Denna leken gar ut pa att gora sa manga \"uppdrag\" som mojligt utan att nagon annan forstar vad du haller pa med.");
  printer.setSize('M');
  printer.justify('C');
  printer.underlineOn();
  printer.println("Regler");
  printer.underlineOff();
  printer.justify('L');
  printer.setSize('S');
  printer.println("1. Du far inte prata / beratta om spelet eller visa reglerna for nagon. Du lassas som om du inte vet nagot om detta spel.");
  printer.feed(1);
  printer.println("2. Du far inte visa dinna lappar for nagon.");
  printer.feed(1);
  printer.println("3. Om du upptacker att nagon gor / nyss gjorde ett uppdrag sager du \"kalaspuffar\" deras uppdrag blir da ogiltligt.");
  printer.feed(1);
  printer.println("4. Du far gora sa manga uppdrag du bara kan.");
  printer.feed(1);
  printer.println("5. Du far max ha 3st aktiva updrag samtidigt pa handen.");
  printer.feed(1);
  printer.println("6. Nagon maste alltid vittna det du gor");
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


