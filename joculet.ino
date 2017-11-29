#include "LedControl.h" //  need the library
#include<LiquidCrystal.h>

#define JOY_X A0
#define JOY_Y A1
#define JOY_SWITCH 2
#define PIN_BUTTON 1
//pt LCD
#define V0 3
#define RS 4
#define E 5
#define D4 6
#define D5 7
#define D6 8 
#define D7 9

// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219

LedControl lc = LedControl(12,11,10,1);
LiquidCrystal lcd(RS,E,D4,D5,D6,D7); 

//jucator:
//   ___ 
//  _|X|_
// |_|_|_|

int linie, coloana;  //linie, coloana = coordonatele jucatorului in punctul X
int obstacole[16][8], rachete[8][8]; //obstacole are dimensiunea 16X8 pt ca la primul nivel las 16 linii libere minim intre obstacole, apoi tot scad liniile de la un nivel la altul
int scor, vieti, dificultate;
int jocInceput;  //jocInceput = 0 initial. Se face 1 cand trec de partea cu meniuri
int primulLoop, primulLoopDinJoc;
int randMeniu;   //rand selectat din meniul de la inceputul jocului
int stareButtonPrecedenta = 1, stareButton = 1, ultimulDebounce = 0;

unsigned long timpUltimaActualizareMatriceRachete = 0, timpUltimaRacheta = 0;
unsigned long timpUltimulObstacol = 0, timpUltimaActializareObstacole = 0;
unsigned long timpUltimeleCoordonateJucator = 0; //ultima citire de la joystick si actualizarea coordonatelor pt jucator
unsigned long timpUltimaAfisareLCD =  0; 


byte jumatateInimaStanga[8] = {
  B00110,
  B01111,
  B11111,
  B11111,
  B01111,
  B00111,
  B00011,
  B00001,
};
byte jumatateInimaDreapta[8] = {
  B01100,
  B11110,
  B11111,
  B11111,
  B11110,
  B11100,
  B11000,
  B10000,
};

void setup()
{
    lc.shutdown(0,false);// turn off power saving, enables display
    lc.setIntensity(0,8);// sets brightness (0~15 possible values)
    lc.clearDisplay(0);// clear screen
    
    scor = 0;
    vieti = 10;
    dificultate = 1;
    //pozitie initiala jucator
    linie = 6;
    coloana = 3;
    randMeniu = 0;
    primulLoop = 1;
    primulLoopDinJoc = 1;

    pinMode(JOY_SWITCH, INPUT);
    digitalWrite(JOY_SWITCH, HIGH);
    pinMode(PIN_BUTTON,INPUT);

    //pt LCD
    lcd.begin(16,2);
    analogWrite(V0,100);
    lcd.createChar(0,jumatateInimaStanga);
    lcd.createChar(1,jumatateInimaDreapta);
}
void loop()
{   
  
    if(jocInceput == 0)
    {
        afiseazaMeniu(); // meniu cu start si schimba nivel   
    }
    else
    {
        if(primulLoopDinJoc == 1)
            afiseazaDetaliiJoc(); //afisez pe LCD: vieti, nivel, scor
            
        startJoc();   

        if(vieti == 0)
        {
            afiseazaFataTrista();
            afiseazaMeniuFinal();
            reseteazaValori();   
        }       
    }
    primulLoop=0;
}
void afiseazaMeniu()
{
    /*
     *meniu pe LCD:    
     *    >Start!
     *     Schimba nivel: 1
     *Schimb randul cu joystick-ul
     *selectez randul cu buttonul: daca e pe randul 0 incepe jucul
     *                             daca e pe randul 1 creste dificultatea pana la 7 apoi reincepe de la 1
    */
    int xJoyStick, valoareSwitch;
    int intervalClear = 600;
   
    int intervalDebounce = 50;
        
    lcd.setCursor(1,0);
    lcd.print("Start!"); 
    lcd.setCursor(1,1);
    lcd.print("Schimba nivel:");
    lcd.print(dificultate);
  
    xJoyStick = analogRead(JOY_X);
        
    if(xJoyStick > 800)
    {
        randMeniu = 0;
    }
        
    if(xJoyStick < 200)
    {
        randMeniu = 1;
    }

    if(randMeniu == 0)
    {      
        lcd.setCursor(0,0);
        lcd.print(">");
        lcd.setCursor(0,1);
        lcd.print(" ");
          
    }
    else
    {
        lcd.setCursor(0,1);
        lcd.print(">");
        lcd.setCursor(0,0);
        lcd.print(" ");
    }
      
    if((millis() - timpUltimaAfisareLCD) >= intervalClear)
    {
        timpUltimaAfisareLCD = millis();
        lcd.clear();
    }

    int stare = digitalRead(PIN_BUTTON);
    // 0 cand e apasat, 1 pe liber

    if(stare != stareButtonPrecedenta)
    {
        ultimulDebounce = millis();  
    }

    if((millis() - ultimulDebounce) > intervalDebounce)
    {
        if(stare != stareButton)
        {
            stareButton = stare;
                
            if(stareButton == 0 && randMeniu == 0)
            {
                jocInceput = 1;
            }
                
            if(stareButton == 0 && randMeniu == 1)
            {
                if(dificultate < 7) 
                    dificultate++;
                else
                    dificultate = 1;
            }               
        }
    }

    stareButtonPrecedenta = stare;

}

void afiseazaDetaliiJoc()
{
    //functie apelata la inceputul jocului si de fiecare data cand se modifica un parametru(vieti, nivel, dificultate)
    lcd.clear();  
    lcd.setCursor(0,0);
    lcd.print(char(0));  // 1/2 inima
    lcd.print(char(1));  // +1/2 inima
    lcd.setCursor(3,0);
    lcd.print(vieti);
    lcd.setCursor(7,0);
    lcd.print("Nivel: ");
    lcd.print(dificultate);
    
    lcd.setCursor(0,1);
    lcd.print("Scor:");
    lcd.setCursor(6,1);
    lcd.print(scor);
}
void afiseazaMeniuFinal()
{
    //meniu dupa ce pierzi, apare pana la un input de la joystick
    lcd.clear();
    int valoareSwitch;
    do{
        lcd.setCursor(0,0);
        lcd.print("Scor: ");
        lcd.setCursor(7,0);
        lcd.print(scor);
        lcd.setCursor(0,1);
        lcd.print("Nivel: ");
        lcd.print(dificultate);
    }while((valoareSwitch = digitalRead(JOY_SWITCH)) == 1);
    lcd.clear();  
}
void reseteazaValori()
{
    //functie apelata dupa ce pierzi pt a reseta valorile pt jocul urmator
    scor = 0;
    vieti = 10;
    dificultate = 1;
    randMeniu = 0;
    primulLoop = 1;
    primulLoopDinJoc = 1;
      
    //pozitie initiala jucator
    linie = 6;
    coloana = 3;
    
    jocInceput = 0;

    int i, j;

    for(i = 0; i < 16; i++)
        for(j = 0; j < 8; j++)
            obstacole[i][j] = 0;

    for(i = 0; i < 8; i++)
        for(j = 0; j < 8; j++)
            rachete[i][j] = 0;  
}
void afiseazaFataTrista()
{
    lc.setColumn(0, 0, B00000000);
    lc.setColumn(0, 1, B01000010);
    lc.setColumn(0, 2, B01000010);
    lc.setColumn(0, 3, B00000000);
    lc.setColumn(0, 4, B00000000);
    lc.setColumn(0, 5, B00111100);
    lc.setColumn(0, 6, B01000010);
    lc.setColumn(0, 7, B00000000); 
}
void startJoc()
{
    calculeazaCoordonateJucator();
    
    genereazaObstacole();

    lanseazaRachete();

    afisareRacheteObstacoleJucator();

    verificaColiziuniRachete();

    verificaColiziuniJucator();
  
    recalculeazaParametri();

    primulLoopDinJoc = 0;
}

void calculeazaCoordonateJucator()
{
    int intervalRecalculare = 60;
    int xJoyStick, yJoyStick;

    if((millis() - timpUltimeleCoordonateJucator) >= intervalRecalculare)
    {
        timpUltimeleCoordonateJucator = millis();  

        xJoyStick = analogRead(JOY_X);
        yJoyStick = analogRead(JOY_Y);

        if(xJoyStick < 340)
        {
            if(linie < 6)
                linie++;
        }
        else
            if (xJoyStick > 680)
            {
                if(linie > 0)
                linie--;
            }
        if(yJoyStick < 340)
        {
            if(coloana > 1)
                coloana--;
        }
        else 
            if (yJoyStick > 680)
                if(coloana < 6)
                  coloana++;
    }
}

void genereazaObstacole()
{
    /* 
     *in functie de dificultate o sa pastrez un numar minim de linii
     *intre obstacole si o sa variez timpul dintre adaugarea a doua obstacole
     */
    int minimLiniiLibere, delayObstacole;
    int delayActualizareObstacole = 100;
    
    int i, j;

    //fac update la matricea de obstacole (le copiez pe cele de pe linia precedenda sa creez impresia de miscare)
    if((millis() - timpUltimaActializareObstacole) >= delayActualizareObstacole)
    {
        timpUltimaActializareObstacole = millis();
        
        for( j = 0; j <= 7; j++)
        {
            for(i = 7; i >= 1; i--)
                obstacole[i][j] = obstacole[i-1][j];

            obstacole[0][j] = 0; //pe prima linie pun 0 si adaug dupa obstacole unde e cazul
        }
    }
    switch(dificultate){
        case 1:
            minimLiniiLibere = 16;
            delayObstacole = 500;
            break;
        case 2:
            minimLiniiLibere = 14;
            delayObstacole = 450;
            break;
        case 3:
            minimLiniiLibere = 12;
            delayObstacole = 400;
            break;
        case 4:
            minimLiniiLibere = 10;
            delayObstacole = 350;
            break;
        case 5:
            minimLiniiLibere = 8;
            delayObstacole = 300;
            break;
        case 6:
            minimLiniiLibere = 6;
            delayObstacole = 250;
            break;
        case 7:
            minimLiniiLibere = 4;
            delayObstacole = 200;
            break;       
    }
    
    int obstacolePerIteratie = dificultate/2+1;
    int okSaPunObstacol = 1;

   while(obstacolePerIteratie > 0)
   {  
        //aleg random o coloana pe care pun un obstacol
        int coloanaObstacol = rand() % 8; //valori posibile: 0, 1, 2..,7

        //verific ca pe coloana aleasa sa fie un numar ales de linii libere
        for(i = 0; i < minimLiniiLibere; i++)       
            if( obstacole[i][coloanaObstacol] != 0) 
            {
                okSaPunObstacol = 0;
                break; 
            }
         
        if((okSaPunObstacol == 1) && ((millis() - timpUltimulObstacol) >= delayObstacole))
        {
            obstacole[0][coloanaObstacol] = 1;
            timpUltimulObstacol = millis();
            obstacolePerIteratie--;
        }
   }    
}
void lanseazaRachete()
{
    int valoareButton, valoareSwitch;
    
    const int intervalIntreRachete = 100;
    const int delayRachete = 100; // la cate milisecunde actualizez matricea cu rachete: cu cat valoarea e mai mica rachetele se misca mai repede
    //pot sa trag si cu buttonul si cu sw de la joystick
      
    // cu button
    valoareButton = digitalRead(PIN_BUTTON);
    
    if((valoareButton == 0)&&((millis() - timpUltimaRacheta) >= intervalIntreRachete))
    {
        timpUltimaRacheta = millis();
        rachete[linie][coloana-1] = 1;
        rachete[linie][coloana+1] = 1;   
    }
    
    //cu sw de la joystick 
    valoareSwitch = digitalRead(JOY_SWITCH);

    if((valoareSwitch == 0)&&((millis() - timpUltimaRacheta) >= intervalIntreRachete))
    {
        timpUltimaRacheta = millis();
        rachete[linie][coloana-1] = 1;
        rachete[linie][coloana+1] = 1; 
    }

    int i, j;
    if((millis() - timpUltimaActualizareMatriceRachete) >= delayRachete)
    {
      timpUltimaActualizareMatriceRachete = millis();
        
        for( j = 0; j <= 7; j++)
        {  
            for(i = 0; i < 7; i++)
                rachete[i][j] = rachete[i+1][j];

            rachete[7][j] = 0;
        }
     }   
}
void afisareRacheteObstacoleJucator()
{

    /*ledurile jucatorului:
    coloana,  linie
    coloana-1,linie+1
    coloana,  linie+1
    coloana+1,linie+1*/
    
    int i,j;
    for(i = 0; i <=7; i++)
        for(j = 0; j <= 7; j++)
        {
            //daca e o racheta sau un obstacol la pozitia i,j aprind ledul
            if((rachete[i][j] == 1) || (obstacole[i][j] == 1)) 
                lc.setLed(0,j,i,true);
            else
                //aprind ledurile jucatorului
                if(((i == linie)&&(j==coloana))||((i==linie+1)&&((j==coloana)||(j==coloana+1)||(j==coloana-1))))
                    lc.setLed(0,j,i,true);
                else
                    //sting in rest tot
                    lc.setLed(0,j,i,false);
        }   
}
void verificaColiziuniRachete()
{
    /*
      verifica daca vreo racheta a lovit vreun obstacol
      daca da, sterg obstacolul si racheta respectiva din matricele lor
        adaug 10 puncte la scor
          recalculez parametri (daca am trecut la nivelul urmator sau nu(trec la urmatorul nivel cand scorul e multiplu de 1000))
            reafisez valorile pe LCD
    */
    int i, j;

    for(i = 1; i <=7; i++)
        for(j = 0; j <= 7; j++)
        {
          /////////////////////DE STERS UN IF si verificat//////////////////////////////////////////////////////////////////////////
            if(rachete[i][j] == 1 && obstacole[i-1][j] == 1)
            {
                rachete[i][j] = 0;
                obstacole[i-1][j] = 0;

                scor = scor + 10;
                recalculeazaParametri();
                afiseazaDetaliiJoc();
            }
            if(rachete[i][j] == 1 && obstacole[i][j] == 1)
            {
                rachete[i][j] = 0;
                obstacole[i][j] = 0;  

                scor = scor + 10;
                recalculeazaParametri();
                afiseazaDetaliiJoc();
            }
        }
   
}
void verificaColiziuniJucator()
{
    int iColiziune, jColiziune, lovit = 0;
    int i, j;

    for(i = 0; i <=7; i++)
        for(j = 0; j <= 7; j++)
            if(obstacole[i][j] == 1)
            {
                if( i == linie && j == coloana)
                {
                    lovit = 1;
                    iColiziune = i;
                    jColiziune = j;
                }        

                if ((i == linie+1 && j == coloana-1)||(i == linie+1 && j == coloana+1))
                { 
                    lovit = 1;
                    iColiziune = i;
                    jColiziune = j;
                }  
            } 
    if(lovit == 1)
    {
        for(i = iColiziune-2; i <= iColiziune+2; i++)
            for(j = jColiziune-2; j <= jColiziune+2; j++) 
            {
                lc.setLed(0,j,i,true);
                obstacole[i][j] = 0;
                rachete[i][j] = 0;
            }   
        vieti--;
        afiseazaDetaliiJoc();
        
        //dupa ce pierzi o viata e necesara o mica pauza
          //pana incep obstacolele sa te loveasca din nou
        delay(400);

    }
}
void recalculeazaParametri()
{
    int i;
    for(i = 1; i <= 20; i++)
        if(scor == (i*1000))
        {
            scor = scor + 10;
            if(dificultate < 7)
                dificultate++;
            vieti++;
            afiseazaDetaliiJoc();
            break;
        }      
}
