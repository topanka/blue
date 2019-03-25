#include "sh1tmr.h"
#include "Bounce2.h"

#define LED_PIN           13

#define BLUE_LDPWR_PIN         A21
#define BLUE_SYSVOLT_PIN       A0
#define BLUE_LDCURRENT_PIN     A1
#define BLUE_TEMPERATURE_PIN   A2

#define BLUE_BTN1_PIN          2
#define BLUE_BTN2_PIN          3
#define BLUE_BTN3_PIN          4
#define BLUE_BTN4_PIN          5

unsigned long g_millis=0;
unsigned long g_loop_cnt=0;         //loop counter
unsigned long g_loop_cps=0;         //loop counter per sec
unsigned long g_loop_ct=0;

int g_setpwr=0;
int g_prevpwr=0;
int g_sysvolt=0;
int g_temp=0;
int g_ldpwrV=0;
int g_ldpwrA=0;
MYTMR g_tmr_pwr={0};
MYTMR g_tmr_read={0};
MYTMR g_tmr_btn={0};

Bounce btn1 = Bounce(); 
Bounce btn2 = Bounce(); 
Bounce btn3 = Bounce(); 
Bounce btn4 = Bounce(); 

void loop_counter(void)
{
  g_millis=millis();
  g_loop_cnt++;
  if((g_millis-g_loop_ct) > 1000) {
    Serial.print("loopcps ");
    Serial.println(g_loop_cps);
    g_loop_cps=g_loop_cnt;
    g_loop_cnt=0;
    g_loop_ct=g_millis;
  }
}
  
void setup()
{
  Serial.begin(112500);
  analogWriteResolution(12);  
  analogReadResolution(12);  
  tmr_init(&g_tmr_pwr,8000);
  tmr_init(&g_tmr_read,1000);
  tmr_init(&g_tmr_btn,5);
  pinMode(LED_PIN,OUTPUT);
  pinMode(BLUE_LDPWR_PIN,OUTPUT);
  pinMode(BLUE_SYSVOLT_PIN,INPUT);
  pinMode(BLUE_LDPWR_PIN,OUTPUT);
  analogWrite(BLUE_LDPWR_PIN,0);
  digitalWrite(LED_PIN,LOW);

  btn1.attach(BLUE_BTN1_PIN,INPUT_PULLUP);
  btn1.interval(5); // interval in ms
  btn2.attach(BLUE_BTN2_PIN,INPUT_PULLUP);
  btn2.interval(5); // interval in ms
  btn3.attach(BLUE_BTN3_PIN,INPUT_PULLUP);
  btn3.interval(5); // interval in ms
  btn4.attach(BLUE_BTN4_PIN,INPUT_PULLUP);
  btn4.interval(5); // interval in ms
  
}

void loop()
{
  unsigned char b=255;
  bool fellx=false,fell1=false,fell2=false,fell3=false,fell4=false;
  
  loop_counter();
  if(tmr_do(&g_tmr_btn) == 1) {
    b=g_tmr_btn.cnt%4;
    if(b == 0) {
      btn1.update();
      fell1=btn1.fell();
    } else if(b == 1) {
      btn2.update();
      fell2=btn2.fell();
    } else if(b == 2) {
      btn3.update();
      fell3=btn3.fell();
    } else if(b == 3) {
      btn4.update();
      fell4=btn4.fell();
    }
    fellx=fell1|fell2|fell3|fell4;
  }

  if(tmr_do(&g_tmr_read) == 1) {
    g_sysvolt=analogRead(BLUE_SYSVOLT_PIN);
    Serial.print("sys: ");
    Serial.print(g_sysvolt);
    g_sysvolt=(66*1004*g_sysvolt)/409600;
    Serial.print(" (");
    Serial.print(g_sysvolt/100);
    Serial.print(".");
    if((g_sysvolt%100) < 10) {
      Serial.print("0");
    }
    Serial.print(g_sysvolt%100);
    Serial.print(" V) ");
    
    g_temp=analogRead(BLUE_TEMPERATURE_PIN);  
    g_temp=(3300*g_temp)/4096;
    Serial.print("temp: ");
    Serial.print(g_temp);
    g_temp=(6600*g_temp)/4096;
    Serial.print(" ");
    Serial.print(g_temp);
    Serial.print(" mV ");
    Serial.print(g_temp/10);
    Serial.print(".");
    Serial.print(g_temp%10);
    Serial.print(" C");
    Serial.println();

    g_ldpwrV=analogRead(BLUE_LDCURRENT_PIN);
    Serial.print(g_setpwr);
    Serial.print(" ");
    Serial.print(g_ldpwrV);
    Serial.print(" (");
    g_ldpwrA=(330000*g_ldpwrV)/(4096*83);
    g_ldpwrV=(33000*g_ldpwrV)/4096;
    Serial.print(g_ldpwrV/10);
    Serial.print(".");
    Serial.print(g_ldpwrV%10);
    Serial.print(" mV) ");
    Serial.print(" (");
    Serial.print(g_ldpwrA);
    Serial.print(" mA) ");
    Serial.println();
    
  }

  if(fell1 == true) {
    Serial.println("fell1");
    g_setpwr=0;
  }
  if(fell2 == true) {
    Serial.println("fell2");
    g_setpwr=84;
  }
  if(fell3 == true) {
    Serial.println("fell3");
    g_setpwr=845;
  }
  if(fell4 == true) {
    Serial.println("fell4");
    g_setpwr+=50;
    g_setpwr%=500;
  }
  if((fellx == true) || (g_setpwr != g_prevpwr)) {
    Serial.print("power set to: ");
    Serial.println(g_setpwr);
    analogWrite(BLUE_LDPWR_PIN,g_setpwr);
    g_prevpwr=g_setpwr;
  }

/*  
  if(tmr_do(&g_tmr_pwr) == 1) {
    Serial.print("power ");
    Serial.println(g_setpwr);
    g_setpwr+=100;
    g_setpwr%=900;
    analogWrite(BLUE_LDPWR_PIN,g_setpwr);
    if(g_setpwr == 0) {
      g_tmr_pwr.lct+=7500;
      digitalWrite(LED_PIN,HIGH);
    } else {
      digitalWrite(LED_PIN,LOW);
    }
  }
  if(tmr_do(&g_tmr_read) == 1) {
    g_sysvolt=analogRead(BLUE_SYSVOLT_PIN);
    g_ldpwrV=analogRead(BLUE_LDCURRENT_PIN);
    
    Serial.print("sys: ");
    Serial.print(g_sysvolt);
    g_sysvolt=(66*1004*g_sysvolt)/409600;
    Serial.print(" (");
    Serial.print(g_sysvolt/100);
    Serial.print(".");
    if((g_sysvolt%100) < 10) {
      Serial.print("0");
    }
    Serial.print(g_sysvolt%100);
    Serial.print(" V) ");
    
    Serial.print(g_setpwr);
    Serial.print(" ");
    Serial.print(g_ldpwrV);
    Serial.print(" (");
    g_ldpwrA=(330000*g_ldpwrV)/(4096*83);
    g_ldpwrV=(33000*g_ldpwrV)/4096;
    Serial.print(g_ldpwrV/10);
    Serial.print(".");
    Serial.print(g_ldpwrV%10);
    Serial.print(" mV) ");
    Serial.print(" (");
    Serial.print(g_ldpwrA);
    Serial.print(" mA) ");
    Serial.println();
  }
*/  
}
