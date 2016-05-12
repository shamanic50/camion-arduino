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


#define pinVentil 3 // relais ventil sur pin digitale 3


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
float tmax=0;
float h;
float hmax=0;


// pour l'ethilometre
int gazcapteur;
float alcool = 2;

// pour la ventil
boolean x;

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

// creation des fonction :
  /*rappel des fonctions :
    bouton() : donne la valeur droite,gauche,.. a choix
    ethylometre () donne une valeur a alcool
    temperature() valeur a : h et t 
    ventil() : x=0 ventil eteinte x=1 allumé
     eau() : donne une valeur a cm
    batterie() : donne une valeur a batterie en volt */

void bouton() {   // fonction qui retourne le bouton appuyer
for (int i=1; i<10; i++){
  boutonLCD = analogRead(0); // 5 boutons sur le shield defini sur la valeur analogique pin 0
  if (boutonLCD < 50) {
    choix = droite;   }
  else if (boutonLCD < 195) {
    choix = haut;     }
  else if (boutonLCD < 380) {
    choix = bas;      }
  else if (boutonLCD < 560) {
    choix = gauche;   }
  else if (boutonLCD < 790) {
    choix = select;   }
  else if (boutonLCD > 789) {
    choix = rien ;    }
}
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
  if (t > tmax){tmax=t;}
  if (h > hmax) {hmax=h;}


}

void ventil() { // fonction pour la ventil
  while (choix != haut){
  if (x = true) {
    digitalWrite (pinVentil, 0);
    lcd.write("extracteur off");
  }
  else {
    digitalWrite (pinVentil, 1);
    lcd.write("extracteur on");
  }
  bouton();

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
  int lectureBatterie = analogRead (batteriePin);//pont diciseur de tension batterie branché sur pin A4
  batterie = map(lectureBatterie, 0, 1023, 0, 5) * coeff ;
    if (batterie>batteriemax){
        batteriemax= batterie;}
  // l'entrée lit une valeur de 0 a 1024 qui est remis dans la plage de lecture analogique de 0 a 5v et mulitiplié par le coeff du pont diviseur
  delay(500);
}


//les fonctions suivantes gèrent l'affichage :

void titre(){ 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write("Tu veux ?"); 
  lcd.setCursor(0, 1); }
void menu2(){
    while  (choix != haut){
      bouton();
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
  lcd.setCursor(7,1);
  lcd.print("max:");
  lcd.print(hmax);
  lcd.print("%");  
      break;
    case 4:
      ventil();
      break;
    case 5:
      ethylometre();
      break; }
  }
}
void menu1() {
   while (choix =!haut){
    bouton();
  if (choix == bas) {menu2();}
  if (choix == gauche) {fonction --;}
  if (choix == droite) {fonction++;}
  if (fonction > 5) {fonction = 1;}
  if (fonction < 1) {fonction = 5;}
   
  if (fonctionBis!=fonction) {
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
      break; }
  fonctionBis = fonction ;
  delay(200); } 
}
}

void bilan(){
  lcd.clear();
  lcd.setCursor(0, 0);
     Batterie();
  lcd.print(batterie); 
  lcd.print("V");
  lcd.setCursor(7, 0);
    temperature();
  lcd.print(t);
  lcd.print("°C");        
  lcd.setCursor(0, 1);
    eau();
  lcd.print(cm);
  lcd.print("L");
  lcd.setCursor(7,1);
  lcd.print(h);
  lcd.print("%H2O"); 
    delay(200);
    }
    
    

    
    
    




void setup() {

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.write("Une ambulance"); //message d'intro
  delay (1000);
  lcd.setCursor(0, 1);
  lcd.write("De OUF !!"); // de OUF !!
  delay(2000);

  //initialisation ventil:

  pinMode(pinVentil, OUTPUT); // ventilateur camion branché sur 12v
  //commandé par un relais relié a pin digitale pinVentil

  digitalWrite (pinVentil, 0); // initialisation ventil eteinte

  //initialisation niveau d'eau :
  pinMode (tirPin, OUTPUT);
  digitalWrite(tirPin, LOW);
  pinMode (echoPin, INPUT);


}
void loop() {
bouton();
    if (choix==rien) {
        bilan();
        if (millis()- temps > 60000){
        lcd.noDisplay(); } 
        else {lcd.display();}
         }
  
    else{
        temps = millis(); 
    if (choix == bas){
        titre();
        menu1();
        menu2();}
            }
}







void checkbouton () { // fonction pour verifier les boutons

  lcd.setCursor(0, 1);
  bouton();
  if (choix != rien) {
    lcd.clear ();
    lcd.write (choix);
  }

}

