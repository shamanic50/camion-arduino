// Code pour camion amennagé en cours de creation
/* ce code est realiser pour le controle de différents parametre dans le camion
  les systemes prévus :
  -controle de la temperature et de l'humidité
  -ethylomtre approximatifs pour le conducteur
  -controle du niveau de batterie
  -marche/arret du ventilateur d'extraction du camion
  -controle du niveau dans potable en reserve
  - eventuellement automatiser la mise en marche de la ventil pour reguler temperature et pour reguler la quantité de gaz dans l'air

  ces fonctions seront affiché sur un ecran LCD a 5 boutons */



#include <LiquidCrystal.h> // librairie pour ecran LCD 
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //defini les pin de l'ecran LCD

#include <DHT.h> // librairie pour capteur DHT
#define DHTPIN 3 // capteur DHT supin analogique 3
#define DHTTYPE DHT22 // definie le type de capteur DHT
DHT dht(DHTPIN, DHTTYPE);// initialise capteur


//variable pour les boutons :
int boutonLCD;

const int haut = 1;
const int bas = 2;
const int gauche = 3;
const int droite = 4;
const int select = 5;
const int rien = 0;
int choix;
int fonction = 1;
int fonctionBis = 2;

//pour la temperature :
float t;
float tmax = 0;
float h;
float hmax = 0;


// pour l'ethilometre
int gazcapteur;
float alcool = 2;

// pour la ventil
boolean x;
const int pinVentil = 3; // relais ventil sur pin digitale 3

//pour le niveau d'eau
const int tirPin = 12; // brocha 12 emet le son
const int echoPin = 11; // broche 11 recoit l'echo
long echo;
long cm;

//pour la jauge de batterie
const int batteriePin = 1 ; //pont diviseur de tension batterie relié analogique 4
int coeff = 4; // definie le  coeff de division du pont diviseur, a calculer avec les resistances
float batterie;
float batteriemax = 0;

//pour l'extinction automatique
unsigned long temps;
unsigned long tempsbis;

// creation des fonction :
/*rappel des fonctions :
  bouton() : donne la valeur droite,gauche,.. a choix
  ethylometre () donne une valeur a alcool
  temperature() valeur a : h et t
  ventil() : x=0 ventil eteinte x=1 allumé
   eau() : donne une valeur a cm
  batterie() : donne une valeur a batterie en volt */

int bouton() {   // fonction qui retourne le bouton appuyer
    boutonLCD = analogRead(A0); // 5 boutons sur le shield defini sur la valeur analogique pin 0
    if (boutonLCD < 50)  return droite; 
    if (boutonLCD < 195) return haut;
if (boutonLCD < 380) return bas;
   if (boutonLCD < 560) return gauche;
if (boutonLCD < 790) return select;
 if (boutonLCD > 789) return rien ;
    
  delay(200);
}



void ethylometre() {  //fonction pour donner une valeur à l'ethylometre
  gazcapteur = analogRead (A2); // capteur MQ3 branchée sur pin analogique A2
  alcool = map(gazcapteur, 0, 1023, 0, 10);
  // capteur mesure entre 0.05 et 10 mg/l d'alcool dans l'air
  delay(200);

}
void temperature() { // fonction pour le capteur temperarure
  // valeur de -40 a 80 degres temperature
  // valeur de 0 a 100% humidité
  dht.begin();
  delay(2000);

  h = dht.readHumidity();
  t = dht.readTemperature();
  if (t > tmax) {
    tmax = t;
  }
  if (h > hmax) {
    hmax = h;
  }


}

void ventil() { // fonction pour la ventil
  while (choix != haut) {
    if (x = true) {
      digitalWrite (pinVentil, 0);
      lcd.write("extracteur off");
    }
    else {
      digitalWrite (pinVentil, 1);
      lcd.write("extracteur on");
    }
    choix = bouton();

    if (choix == gauche) {
      x = true;
    }
    if (choix == droite) {
      x = false ;
    }

    // x defini si la ventil est allumer ou eteinte ventil() execute la commande
    delay(200);
  }
}

void eau() {
  digitalWrite (tirPin, HIGH);
  delayMicroseconds (10);
  digitalWrite (tirPin, LOW);
  echo = pulseIn(echo, HIGH);
  cm = echo / 58; //calcul la distance en cm de l'obstacle rencontré
  delay (1000);
}

void Batterie() {//mesure du niveau de batterie
  int lectureBatterie = analogRead (batteriePin);//pont diciseur de tension batterie branché sur pin A1
  batterie = map(lectureBatterie, 0, 1023, 0, 5) * coeff ;
  if (batterie > batteriemax) {
    batteriemax = batterie;
  }
  // l'entrée lit une valeur de 0 a 1024 qui est remis dans la plage de lecture analogique de 0 a 5v et mulitiplié par le coeff du pont diviseur
  delay(500);
}


//les fonctions suivantes gèrent l'affichage :

void titre() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write("Tu veux ?");
  lcd.setCursor(0, 1);
}
void menu2() {
  while  (choix != haut) {
  choix =  bouton();
    delay (200);
    switch (fonction) {
      case 1 :
        Batterie();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Volt : ");
        lcd.print(batterie);
        lcd.print("V");
        lcd.setCursor(0, 1);
        lcd.print("Vmax : ");
        lcd.print(batteriemax);
        break;
      case 2 :
        eau();
        break;
      case 3:
        temperature();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T: ");
        lcd.print(t);
        lcd.print("°C");
        lcd.setCursor(7, 0);
        lcd.print("max:") ;
        lcd.print(tmax);
        lcd.print("°C");
        lcd.setCursor(0, 1);
        lcd.print("H2O:");
        lcd.print(h);
        lcd.print("%");
        lcd.setCursor(7, 1);
        lcd.print("max:");
        lcd.print(hmax);
        lcd.print("%");
        break;
      case 4:
        ventil();
        break;
      case 5:
        ethylometre();
        break;
    }
  }
}
void menu1() {
  while (choix = !haut) {
   choix = bouton();
    if (choix == bas) {
      menu2();
    }
    if (choix == gauche) {
      fonction --;
    }
    if (choix == droite) {
      fonction++;
    }
    if (fonction > 5) {
      fonction = 1;
    }
    if (fonction < 1) {
      fonction = 5;
    }

    if (fonctionBis != fonction) {
      switch (fonction) {
        case 1 : titre();
          lcd.write ("< La Batterie >");
          break;
        case 2 : titre();
          lcd.write ("< L'eau >");
          break;
        case 3: titre();
          lcd.write ("< Le Climat  >");
          break;
        case 4: titre();
          lcd.write ("< La ventile !>");
          break;
        case 5: titre();
          lcd.write ("< ethylometre >");
          break;
      }
      fonctionBis = fonction ;
      delay(200);
    }
  }
}

void bilan() {
  Batterie();
  temperature();
  eau();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(batterie);
  lcd.print("V");
  lcd.setCursor(8, 0);
  lcd.print(t);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print(cm);
  lcd.print("L");
  lcd.setCursor(8, 1);
  lcd.print(h);
  lcd.print("%H2O");
}










void setup() {
  pinMode(10,OUTPUT);
  digitalWrite (10,LOW);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.write("Une ambulance"); //message d'intro
  delay (1000);
  lcd.setCursor(0, 1);
  lcd.write("De OUF !!"); // de OUF !!
  delay(500);

  //initialisation ventil:

  pinMode(pinVentil, OUTPUT); // ventilateur camion branché sur 12v
  //commandé par un relais relié a pin digitale pinVentil

  digitalWrite (pinVentil, 0); // initialisation ventil eteinte

  //initialisation niveau d'eau :
  pinMode (tirPin, OUTPUT);
  digitalWrite(tirPin, LOW);
  pinMode (echoPin, INPUT);
 

}
void checkbouton () { // fonction pour verifier les boutons

  lcd.setCursor(0, 1);
  choix = bouton();
  if (choix != rien) {
    lcd.clear ();
    lcd.write (choix);
  delay(500);
  }
bilan();
}

void loop() {
  choix = bouton();
  unsigned long ref = millis();
  if ( ref - temps > 60000 ) {
      digitalWrite (10,HIGH);
    }
     if (ref - tempsbis > 10000) {
      lcd.clear();
      bilan();
      tempsbis = ref;
    }
  }

/*  if (choix =! rien) {
    
      digitalWrite (10,LOW);
      temps = ref;
  }
    
    if (choix == bas) {
      titre();
      menu1();
      menu2();
    }*/









