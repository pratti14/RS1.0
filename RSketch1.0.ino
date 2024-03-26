#include <EEPROM.h>
#include <VarSpeedServo.h>


/*

Comandi M
M1 Manda in Home Position
M3 Incrementa feedrate attuale


EEPROM
EEPROM[0]=Feedrate attuale
EEPROM[10]=Ultima posizione J1
EEPROM[11]=Ultima posizione J2
EEPROM[12]=Ultima posizione J3
EEPROM[13]=Ultima posizione J4
*/


#pragma region ROBOT

//Assi
VarSpeedServo J1;
VarSpeedServo J2;
VarSpeedServo J3;
VarSpeedServo J4;


//Variabili interne
//Assi
unsigned short precvalJ1 = 0;
unsigned short precvalJ2 = 0;
unsigned short precvalJ3 = 0;
unsigned short precvalJ4 = 0;
#define homeposJ1 0  //Vedere insieme a gruppo
#define homeposJ2 0  //Vedere insieme a gruppo
#define homeposJ3 0  //Vedere insieme a gruppo
#define homeposJ4 0  //Vedere insieme a gruppo
float smoothprev;
//Movimento manuale

#define POT1 A?
#define POT2 A?
#define POT3 A?
#define POT4 A?

unsigned short JogJ1;
unsigned short JogJ2;
unsigned short JogJ3;
unsigned short JogJ4;
//Finecorsa
/*
enum finecorsa {
  finecorsaj2piu = ?,
  finecorsaj2meno = ?,
  finecorsaj3piu = ?,
  finecorsaj3meno = ?,
  finecorsaj4piu = ?,
  finecorsaj4meno = ?
};
*/
//Organo di presa
int utensile;
bool precvalpressostato = true;
#define pressostato 35
bool relay1, relay2;

#define maxposizioni 10  // Vedere insieme a gruppo
#define attuatori 6      //Assi + Ventosa + (Feedrate)

unsigned int ContatorePosizioni = 0;              //Usato per scorrere matrice di posizioni
unsigned int Posizioni[maxposizioni][attuatori];  //Matrice di posizioni

#pragma endregion ROBOT


//Nastro

#pragma region NASTRO

//Assi
#define MotNastro 50

//Finecorsa
#define endstopPLUSNastro 51
#define endstopMINUSNastro 52

//Sensori

#define fc1 53
#define fc2 54

#pragma endregion NASTRO

void (*resetFunc)(void) = 0;  //Per resettare Arduino da codice

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(POT1, INPUT);
  pinMode(POT2, INPUT);
  pinMode(POT3, INPUT);
  pinMode(POT4, INPUT);
  /*
  J1.attach(2, 400, 2600);
  J2.attach(7, 400, 2600);
  J3.attach(13, 400, 2600);
  J1.write(EEPROM.read(10));  //Inizializza tutti gli assi sull'ultima posizione nota, per evitare movimenti bruschi allo startup
  J2.write(EEPROM.read(11));
  J3.write(EEPROM.read(12));
  J4.write(EEPROM.read(13));

  //EEPROM.write(0, 0);     //Inizializzo feedrate a 0 ogni volta che arduino viene acceso

  */
}


void loop() {
  // put your main code here, to run repeatedly:
    /*
    JogJ1 = map(analogRead(POT1), 0, 1023, 0, ?);  //Da testare
    JogJ2 = map(analogRead(POT2), 0, 1023, 0, ?);  //Da testare
    JogJ3 = map(analogRead(POT3), 0, 1023, 0, ?);  //Da testare
    JogJ4 = map(analogRead(POT4), 0, 1023, 0, ?);  //Da testare
    */

    JogJ1 = analogRead(A0);   //Da testare
    JogJ2 = analogRead(A2);   //Da testare
    JogJ3 = analogRead(A7);   //Da testare
    JogJ4 = analogRead(A11);  //Da testare
    Serial.print("JOG1: ");
    Serial.print(JogJ1);
    Serial.print(", ");
    Serial.print("JOG2: ");
    Serial.print(JogJ2);
    Serial.print(", ");
    Serial.print("JOG3: ");
    Serial.print(JogJ3);
    Serial.print(", ");
    Serial.print("JOG4: ");
    Serial.println(JogJ4);

    /*
    if (precvalpressostato != digitalRead(pressostato)) {
      WriteAxisPositionAndMessages("[14]=", digitalRead(pressostato));
      precvalpressostato = digitalRead(pressostato);
    }
    */
    /*} else {                                    //Se ho messaggi dall'HMI, cataloga il codice del messaggio
    String code = Serial.readString();
    delay(100);
    code.trim();
    if (code.equalsIgnoreCase("M1")) {
      FlagHomePosition();
    } else if (code.equalsIgnoreCase("M3")) {   //Incremento del feedrate di 10% alla volta
      EEPROM.write(0, EEPROM.read(0) + 10);
      if (EEPROM.read(0) > 100) {
        EEPROM.write(0, 0);
      }
      WriteAxisPositionAndMessages("[0]=", EEPROM.read(0));
    } else if (code.equalsIgnoreCase("M11")) {
      utensile = 0;
      FlagRecordPosition(utensile);
    } else if (code.equalsIgnoreCase("M12")) {
      utensile = 1;
      FlagRecordPosition(utensile);
    } else if (code.equalsIgnoreCase("M13")) {
      utensile = 2;
      FlagRecordPosition(utensile);
    } else if (code.equalsIgnoreCase("M19")) {
      FlagDeletePositions();
    } else if (code.equalsIgnoreCase("M20")) {
      FlagExecute();
    } else if (code.equalsIgnoreCase("M22")) {
      J1.write(110, 80);
    } else if (code.equalsIgnoreCase("M23")) {
      AxisMovement(J1, 180, 2, 3, 100);
    } else if (code.equalsIgnoreCase("M99")) {
      resetFunc();
    } else {
      // Da implementare
    }
    */
  
}
void FlagHomePosition() {  //Manda in home position gli assi
  AxisMovement(J1, homeposJ1, 2, 3, 100);
  AxisMovement(J2, homeposJ2, 4, 5, 50);
  AxisMovement(J3, homeposJ3, 6, 7, 50);
  AxisMovement(J4, homeposJ4, 8, 9, 50);
}
void FlagRecordPosition(int utensile) {  //Registra la posizione attuale dei giunti e se in quella posizione deve prendere o rilasciare
  if (ContatorePosizioni < maxposizioni) {
    Posizioni[ContatorePosizioni][0] = J1.read();
    Posizioni[ContatorePosizioni][1] = J2.read();
    Posizioni[ContatorePosizioni][2] = J3.read();
    Posizioni[ContatorePosizioni][3] = J4.read();
    Posizioni[ContatorePosizioni][4] = utensile;
    Posizioni[ContatorePosizioni][5] = EEPROM.read(0);
    ContatorePosizioni++;
  }
}
void FlagDeletePositions() {  //Cancella tutte le posizioni memorizzate
  for (int i = 0; i < maxposizioni; i++) {
    for (int j = 0; j < attuatori; j++) {
      Posizioni[i][j] = 0;
    }
  }
}
/*
void FlagExecute() {
  for (int i = 0; i < maxposizioni; i++) {
    for (int j = 0; j < attuatori - 1; j++) {
      switch (j) {
        case 0:
          AxisMovement(J1, Posizioni[i][j], 0, 0, Posizioni[i][5]);  //Posizioni[i][j]=gradi asse | Posizioni[i][5]=feedrate
          break;
        case 1:
          AxisMovement(J2, Posizioni[i][j], finecorsaj2meno, finecorsaj2piu, Posizioni[i][5]);
          break;
        case 2:
          AxisMovement(J3, Posizioni[i][j], finecorsaj3meno, finecorsaj3piu, Posizioni[i][5]);
          break;
        case 3:
          AxisMovement(J4, Posizioni[i][j], finecorsaj4meno, finecorsaj4piu, Posizioni[i][5]);
          break;
        case 4:
          if (Posizioni[i][j] == 1) {  //Prendi
            while (digitalRead(pressostato) != HIGH) {
              digitalWrite(relay1, LOW);
              delay(5);  //Testare bene
            }
            digitalWrite(relay1, HIGH);
          } else if (Posizioni[i][j] == 2) {  //Rilascia
            while (digitalRead(pressostato) != LOW) {
              digitalWrite(relay2, LOW);
              delay(5);  //Testare bene
            }
            digitalWrite(relay2, HIGH);
          } else {
            //Non fare niente
          }
          break;
      }
    }
  }
}
*/
void AxisMovement(VarSpeedServo ax, float destination, int enstopvalueminus, int endstopvalueplus, int feedrate) {
  bool axenable = true;
  while (abs(ax.read() - destination) > 1 && digitalRead(enstopvalueminus) == 1 && digitalRead(endstopvalueplus) == 1) {  //Testato
    smoothprev = ax.read();
    while (ax.read() > destination + 1 && digitalRead(enstopvalueminus) == 1 && axenable) {
      if (Serial.available() > 0) {  //Se ricevo messaggio di interruzione da HMI aspetto finche non ricevo start
        axenable = false;
      }
      float smooth = (destination * 0.02) + (smoothprev * 0.98);  //Usato per far frenare dolcemente gli assi

      smoothprev = smooth;
      ax.write(smooth, feedrate);
      if (digitalRead(enstopvalueminus) != 1) {
        break;
      }
      delay(25);
    }
    while (ax.read() < destination - 1 && digitalRead(endstopvalueplus) == 1 && axenable) {
      if (Serial.available() > 0) {  //Se ricevo messaggio di interruzione da HMI aspetto finche non ricevo start
        axenable = false;
      }
      float smooth = (destination * 0.02) + (smoothprev * 0.98);  //Usato per far frenare dolcemente gli assi

      smoothprev = smooth;
      ax.write(smooth, feedrate);

      if (digitalRead(endstopvalueplus) != 1) {
        break;
      }
      delay(25);
    }
    delay(150);
    if (!axenable) {  //Se non ho enable dell'asse aspetto finche non ricevo uno start
      if (Serial.available() > 0) {
        String code = Serial.readString();
        delay(100);
        code.trim();
        if (code.equalsIgnoreCase("M20")) {
          axenable = 1;
        }
      }
    }

    delay(50);
  }
}



/*void WriteAxesPositionToPC(unsigned short ax) {  //Utilizzata per aggiornare HMI sulla posizione degli assi
  const int delayTime = 250;
  switch (ax) {
    case 1:
      WriteAxisPositionAndMessages("[10]=", J1.read());
      precvalJ1 = J1.read();
      EEPROM.write(10, J1.read());
      break;
    case 2:
      WriteAxisPositionAndMessages("[11]=", J2.read());
      precvalJ2 = J2.read();
      EEPROM.write(11, J1.read());
      break;
    case 3:
      WriteAxisPositionAndMessages("[12]=", J3.read());
      precvalJ3 = J3.read();
      EEPROM.write(12, J1.read());
      break;
    case 4:
      WriteAxisPositionAndMessages("[13]=", J4.read());
      precvalJ4 = J4.read();
      EEPROM.write(13, J1.read());
      break;
    default:
      break;
  }
  delay(delayTime);
}
*/

/*void WriteAxisPositionAndMessages(String label, int value) {  //Utilizzata per aggiornare HMI sulla posizione degli assi
  Serial.print(label);
  Serial.print(value);
  Serial.print("END");
}
*/
