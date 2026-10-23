#include <LiquidCrystal.h>
#include <Keypad.h>
#include <EEPROM.h>

// --- CONFIGURARE HARDWARE ---
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const byte ROWS = 4;
const byte COLS = 4;
char taste[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPin[ROWS] = {13, 10, 9, 8};
byte colPins[COLS] = {7, 6, A0, A1};
Keypad customKeypad = Keypad(makeKeymap(taste), rowPin, colPins, ROWS, COLS);

// Pini Periferice
const int pinButonStartStop = A3;
const int pinLedVerde       = A5;
const int pinLedRosu        = A4;
const int pinBuzzer         = A2;

// --- AUTOMAT DE STĂRI (FSM) ---
enum StareJoc {
  STARE_OPRIT,
  STARE_JOC,
  STARE_VICTORIE,
  STARE_GAME_OVER
};

StareJoc stareCurenta = STARE_OPRIT;

// --- VARIABILE GLOBALE JOC ---
int numarSecret;
int numarIntrodus = 0;
bool introducereInceputa = false;
int incercariRamase = 7;
const int MAX_INCERCARI = 7;

unsigned long momentStartJoc = 0;
unsigned long celMaiBunTimp = 999;

// --- VARIABILE DEBOUNCE BUTON ---
int stareFizicaAnterioara = HIGH;
int stareStabilizataButon = HIGH;
unsigned long ultimulTimpSchimbare = 0;
const unsigned long TIMP_DEBOUNCE = 50;

// --- FUNCȚII SUNET ---
void emitBipScurt() {
  digitalWrite(pinBuzzer, HIGH);
  delay(25);
  digitalWrite(pinBuzzer, LOW);
}

void emitSunetPreaMic() {
  digitalWrite(pinLedRosu, HIGH);
  digitalWrite(pinBuzzer, HIGH); delay(50);
  digitalWrite(pinBuzzer, LOW);  delay(40);
  digitalWrite(pinBuzzer, HIGH); delay(50);
  digitalWrite(pinBuzzer, LOW);
  digitalWrite(pinLedRosu, LOW);
}

void emitSunetPreaMare() {
  digitalWrite(pinLedRosu, HIGH);
  digitalWrite(pinBuzzer, HIGH); delay(250);
  digitalWrite(pinBuzzer, LOW);
  digitalWrite(pinLedRosu, LOW);
}

void emitSunetVictorie() {
  digitalWrite(pinLedVerde, HIGH);
  for (int i = 0; i < 3; i++) {
    digitalWrite(pinBuzzer, HIGH); delay(60);
    digitalWrite(pinBuzzer, LOW);  delay(50);
  }
  digitalWrite(pinBuzzer, HIGH); delay(300);
  digitalWrite(pinBuzzer, LOW);
  digitalWrite(pinLedVerde, LOW);
}

void emitSunetIndiciu() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(pinBuzzer, HIGH); delay(100);
    digitalWrite(pinBuzzer, LOW);  delay(80);
  }
}

// --- LOGICĂ JOC ȘI EEPROM ---
void redesenareEcranJoc() {
  lcd.clear();
  lcd.print("Ghiceste (0-100):");
  lcd.setCursor(0, 1);
  lcd.print("N:");
  if (introducereInceputa) {
    lcd.print(numarIntrodus);
  }
  lcd.setCursor(10, 1);
  lcd.print("V:");
  lcd.print(incercariRamase);
}

void resetareJoc() {
  numarSecret = random(0, 101);
  numarIntrodus = 0;
  introducereInceputa = false;
  incercariRamase = MAX_INCERCARI;
  momentStartJoc = millis();
  
  redesenareEcranJoc();
}

void citesteRecordEEPROM() {
  byte recordSalvat = EEPROM.read(0);
  if (recordSalvat == 255 || recordSalvat == 0) {
    celMaiBunTimp = 999;
  } else {
    celMaiBunTimp = recordSalvat;
  }
}

void salveazaRecordEEPROM(unsigned long timpNou) {
  if (timpNou < celMaiBunTimp) {
    celMaiBunTimp = timpNou;
    EEPROM.update(0, (byte)timpNou); 
  }
}

// --- SETUP ---
void setup() {
  randomSeed(analogRead(pinBuzzer));

  pinMode(pinLedVerde, OUTPUT);
  pinMode(pinLedRosu, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinButonStartStop, INPUT_PULLUP);

  digitalWrite(pinLedVerde, LOW);
  digitalWrite(pinLedRosu, LOW);
  digitalWrite(pinBuzzer, LOW);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.noDisplay();
  
  citesteRecordEEPROM();
}

// --- LOOP PRINCIPAL ---
void loop() {
  // 1. Filtru Debounce pentru butonul Start/Stop (A3)
  int lecturaFizicaCurenta = digitalRead(pinButonStartStop);

  if (lecturaFizicaCurenta != stareFizicaAnterioara) {
    ultimulTimpSchimbare = millis();
  }

  if ((millis() - ultimulTimpSchimbare) > TIMP_DEBOUNCE) {
    if (lecturaFizicaCurenta != stareStabilizataButon) {
      stareStabilizataButon = lecturaFizicaCurenta;

      if (stareStabilizataButon == LOW) {
        if (stareCurenta == STARE_OPRIT) {
          stareCurenta = STARE_JOC;
          lcd.display();
          resetareJoc();
        } else {
          stareCurenta = STARE_OPRIT;
          lcd.clear();
          lcd.noDisplay();
          digitalWrite(pinLedVerde, LOW);
          digitalWrite(pinLedRosu, LOW);
          digitalWrite(pinBuzzer, LOW);
        }
      }
    }
  }
  stareFizicaAnterioara = lecturaFizicaCurenta;

  // 2. Automatul de Stări (FSM)
  switch (stareCurenta) {
    case STARE_OPRIT:
      break;

    case STARE_JOC: {
      char tasta = customKeypad.getKey();

      if (tasta) {
        emitBipScurt();

        // A. Cifre (0 - 9)
        if (tasta >= '0' && tasta <= '9') {
          if (!introducereInceputa) {
            numarIntrodus = 0;
            introducereInceputa = true;
          }
          if (numarIntrodus * 10 + (tasta - '0') <= 100) {
            numarIntrodus = numarIntrodus * 10 + (tasta - '0');
            lcd.setCursor(3, 1);
            lcd.print(numarIntrodus);
            lcd.print("   ");
          }
        } 
        // B. Tasta # -> Enter / Validare
        else if (tasta == '#') {
          if (introducereInceputa) {
            lcd.clear();
            
            if (numarIntrodus == numarSecret) {
              // VICTORIE
              unsigned long timpSecunde = (millis() - momentStartJoc) / 1000;
              salveazaRecordEEPROM(timpSecunde);
              
              emitSunetVictorie();
              
              lcd.print("AI GHICIT! 🎉");
              lcd.setCursor(0, 1);
              lcd.print("T:");
              lcd.print(timpSecunde);
              lcd.print("s Rec:");
              lcd.print(celMaiBunTimp);
              lcd.print("s");
              
              delay(3000);
              resetareJoc();
            } 
            else {
              // RĂSPUNS GREȘIT
              incercariRamase--;

              if (incercariRamase <= 0) {
                // GAME OVER
                emitSunetPreaMare();
                stareCurenta = STARE_GAME_OVER;
                lcd.print("GAME OVER! :(");
                lcd.setCursor(0, 1);
                lcd.print("Numar era: ");
                lcd.print(numarSecret);
                delay(3000);
                resetareJoc();
                stareCurenta = STARE_JOC;
              } else {
                if (numarIntrodus < numarSecret) {
                  lcd.print("Prea mic! ^");
                  emitSunetPreaMic();
                } else {
                  lcd.print("Prea mare! v");
                  emitSunetPreaMare();
                }
                delay(1000);

                numarIntrodus = 0;
                introducereInceputa = false;
                redesenareEcranJoc();
              }
            }
          }
        } 
        // C. Tasta * -> Ștergere cifră curentă
        else if (tasta == '*') {
          numarIntrodus = 0;
          introducereInceputa = false;
          lcd.setCursor(3, 1);
          lcd.print("   ");
        }
        // D. Tasta D -> FUNCTIA DE INDICIU / TRISARE (PEEK)
        else if (tasta == 'D') {
          if (incercariRamase > 2) { // Este nevoie de cel puțin 3 vieți pentru a folosi indiciul
            incercariRamase -= 2;   // Penalizare 2 încercări
            
            emitSunetIndiciu();

            lcd.clear();
            lcd.print("SECRETUL ESTE:");
            lcd.setCursor(0, 1);
            lcd.print(">>> ");
            lcd.print(numarSecret);
            lcd.print(" <<< (-2V)");
            delay(2000);

            redesenareEcranJoc();
          } else {
            // Nu sunt suficiente vieți pentru penalizare
            lcd.clear();
            lcd.print("Prea putine V!");
            lcd.setCursor(0, 1);
            lcd.print("Cost indiciu: 2V");
            delay(1500);

            redesenareEcranJoc();
          }
        }
      }
      break;
    }

    case STARE_VICTORIE:
    case STARE_GAME_OVER:
      break;
  }
}