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
volatile int boutonLCD;

const int haut = 1;
const int bas = 2;
const int gauche = 3;
const int droite = 4;
const int select = 5;
const int rien = 0;
const int interruptPin = 2 ; // pin digital 2 pour interruption numero0
volatile int choix;
int fonction = 1;
int fonctionBis = 2;


//pour la temperature :
float t;
float tmax = 0;
float h;
float hmax = 0;


// pour l'ethilometre
int gazcapteur;
float alcool;

// pour la ventil
int x = 0;
const int pinVentil = 3; // relais ventil sur pin digitale

//pour le niveau d'eau
const int tirPin = 12; // broche 12 emet le son
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
const int pinLcd = 10;
unsigned long ref = millis();

// creation des fonction :
/*rappel des fonctions :
  bouton() : donne la valeur droite,gauche,.. a choix
  ethylometre () donne une valeur a alcool
  Temperature() valeur a : h et t
  ventil() : x=0 ventil eteinte x=1 allumé
   Eau() : donne une valeur a cm
  batterie() : donne une valeur a batterie en volt */

void bouton() {   // fonction qui retourne le bouton appuyer
  delay(100);
  boutonLCD = analogRead(A0); // 5 boutons sur le shield defini sur la valeur analogique pin 0
  if (boutonLCD < 100)  {
    choix = droite;
  }
  else if (boutonLCD < 250) {
    choix = haut;
  }
  else if (boutonLCD < 400) {
    choix = bas;
  }
  else if (boutonLCD < 560) {
    choix = gauche;
  }
  else if (boutonLCD < 790) {
    choix = select ;
  }
  else if (boutonLCD > 789) {
    choix = rien ;
  }
  delay(150);
}



void ethylometre() {  //fonction pour donner une valeur à l'ethylometre
  RetourMenu();
  gazcapteur = analogRead (A2); // capteur MQ3 branchée sur pin analogique A2
  alcool = map(gazcapteur, 55, 1023, 0, 1000);
  // capteur mesure entre 0.05 et 10 mg/l d'alcool dans l'air
  lcd.setCursor(0, 0);
  lcd.print("MQ3:");
  lcd.print(int(alcool));
  lcd.print("  0,25mg/l");
  lcd.setCursor(0, 1);
  lcd.print("taux:") ;
  lcd.print( abs(alcool / 1000));
  lcd.print("mg/l    ");
}
void Temperature() { // fonction pour le capteur temperarure
  // valeur de -40 a 80 degres temperature
  // valeur de 0 a 100% humidité
  // ne pas effectue plus d'une fois toutes les 2 secondes
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (t > tmax) {
    tmax = t;
  }
  if (h > hmax) {
    hmax = h;
  }
}
void lcdTemperature() {
  RetourMenu();
  Temperature();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);
  lcd.print("C    ");
  lcd.setCursor(7, 0);
  lcd.print("Max:") ;
  lcd.print(tmax);
  lcd.print("C    ");
  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(h);
  lcd.print("%     ");
  lcd.setCursor(7, 1);
  lcd.print("Max:");
  lcd.print(hmax);
  lcd.print("%    ");
}

void ventil() { // fonction pour la ventil
  RetourMenu();
  if (choix == droite) {
    x=x+1;
  }
  if ((x > 3) || (x < 0)) {
    x = 0;
  }
  lcd.print("extracteur  :   ");
  lcd.setCursor(0, 1);
  if (x == 0) {
    analogWrite (pinVentil, 255);
    lcd.write("     arret      ");
  }
  else if (x == 1) {
    analogWrite (pinVentil, 170);
    lcd.write("    marche 1/3  ");
  }
  else if (x == 2) {
    analogWrite (pinVentil, 85);
    lcd.write("    marche 2/3  ");
  }
  else if (x == 3) {
    analogWrite (pinVentil, 0);
    lcd.write("    marche 3/3  ");
  }
  // x defini si la ventil est allumer ou eteinte ventil() execute la commande
  ventil();
}

void Eau() {
  digitalWrite (tirPin, HIGH);
  delayMicroseconds (10);
  digitalWrite (tirPin, LOW);
  echo = pulseIn(echo, HIGH);
  cm = echo / 58; //calcul la distance en cm de l'obstacle rencontré
  //ne pas faire plus d'une mesure par seconde
}
void lcdEau() {
  RetourMenu();
  Eau();
  lcd.setCursor(0, 0);
  lcd.print("Eau: ");
  lcd.print(cm); //volume à definir selon le jerricane : cm = distance niveu eau
  lcd.print("L  ");
  lcd.setCursor(0, 1);
  lcd.print("   Vmax a 20L   ");
}

void Batterie() {//mesure du niveau de batterie
  int lectureBatterie = analogRead (batteriePin);//pont diciseur de tension batterie branché sur pin A1
  batterie = map(lectureBatterie, 0, 1023, 0, 5) * coeff ;
  if (batterie > batteriemax) {
    batteriemax = batterie;
  }
  // l'entrée lit une valeur de 0 a 1024 qui est remis dans la plage de lecture analogique de 0 a 5v et mulitiplié par le coeff du pont diviseur
  //ne pas faire plus d'une mesure par 0,5seconde
}
void lcdBatterie() {
  RetourMenu();
  Batterie();
  lcd.setCursor(0, 0);
  lcd.print("Volt : ");
  lcd.print(batterie);
  lcd.print("V  ");
  lcd.setCursor(0, 1);
  lcd.print("Vmax : ");
  lcd.print(batteriemax);
}


//les fonctions suivantes gèrent l'affichage :
void RetourMenu() {
  ref = millis();
  choix = rien;
  delay(200);
  if (choix == haut) {
    lcd.clear();
    fonctionBis = 0;
    menu1();
  }
}
void titre() {
  lcd.setCursor(0, 0);
  lcd.write("Tu veux ?");
  lcd.setCursor(0, 1);
}
void menu2() {
  RetourMenu();
  switch (fonction) {
    case 1 :
      lcdBatterie();
      break;
    case 2 :
      lcdEau();
      break;
    case 3:
      lcdTemperature();
      break;
    case 4:
      ventil();
      break;
    case 5:
      ethylometre();
      break;
  }
  menu2();
}

void menu1() {
  titre();
  choix = rien;
  delay(200);
  if (choix == haut) {
    lcd.clear();
    fonctionBis = 0;
    bilan();
    principale();
  }
  if (choix == bas) {
    lcd.clear();
    choix = rien;
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
        lcd.write ("< La Batterie  >");
        break;
      case 2 : titre();
        lcd.write ("<    L'eau     >");
        break;
      case 3: titre();
        lcd.write ("<  Le Climat   >");
        break;
      case 4: titre();
        lcd.write ("< La ventile ! >");
        break;
      case 5: titre();
        lcd.write ("< Ethylometre  >");
        break;
    }
    fonctionBis = fonction ;

  }

  menu1();
}



void bilan() {
  Batterie();
  Temperature();
  Eau();
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
  fonctionBis = 0;
}



void extinction() {
  ref = millis();
  if ( choix == rien && ref - temps > 180000 ) {
    digitalWrite (pinLcd, HIGH);
  }
  else if ( choix == rien && ref - tempsbis > 10000) {
    bilan();
    tempsbis = ref;
  }
}







void setup() {
  attachInterrupt(0, bouton, FALLING);
  pinMode(10, OUTPUT);
  digitalWrite (10, LOW);
  dht.begin();// mise en marche sonde dht
  lcd.begin(16, 2); //initialisation ecran
  lcd.setCursor(0, 0);
  lcd.write("He! Bienvenue"); //message d'intro
  delay (1000);
  lcd.setCursor(0, 1);
  lcd.write("Hein !!!"); // de OUF !!
  delay(1000);

  //initialisation ventil:

  pinMode(pinVentil, OUTPUT); // ventilateur camion branché sur 12v
  //commandé par un relais relié a pin digitale pinVentil

  digitalWrite (pinVentil, HIGH); // initialisation ventil eteinte

  //initialisation niveau d'eau :
  pinMode (tirPin, OUTPUT);
  digitalWrite(tirPin, LOW);
  pinMode (echoPin, INPUT);

  bilan();
  choix = rien;
  delay(200);

}

void principale() {
  choix = rien;
  delay(200);
  extinction();
  if (choix == bas) {
    choix = rien;
    lcd.clear();
    menu1();
  }
  else {
    principale();
  }
}
void loop() {
  principale();
}









void checkbouton () { // fonction pour verifier les boutons

  lcd.setCursor(0, 1);
  lcd.clear ();
  lcd.print (choix);
  lcd.print ("   ");
  lcd.print (boutonLCD);
  boutonLCD = analogRead(A0); // 5 boutons sur le shield defini sur la valeur analogique pin 0
  if (boutonLCD < 100)  {
    choix = droite;
  }
  else if (boutonLCD < 240) {
    choix = haut;
  }
  else if (boutonLCD < 400) {
    choix = bas;
  }
  else if (boutonLCD < 560) {
    choix = gauche;
  }
  else if (boutonLCD < 790) {
    choix = select ;
  }
  else if (boutonLCD > 789) {
    choix = rien ;
  }
  delay(500);

}






