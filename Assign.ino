#include <TinyGPS++.h>
#include<SoftwareSerial.h>

SoftwareSerial myserial(9,10);
int data=0;
int flag;
int button1 = 6;
int ledd = 13;
int button2 = 7;
float AlcoholSmoke=650;
float HeartAlcohol=355.5;
float HeartSmoke=200;
float TotalAverage=350;
int relay = 5;


int yess=0,Noo=1,emergency = 0;

char *numbers[4]={"7588410982","9172421267"};
//char *numbers[1]={"9172421267"};
String callnumber1 = "+919172421267";

char text[ 64 ];
SoftwareSerial gpsSerial(3,4);
//TinyGPS gps;

int alcohol = A3;
int smoke = A1;
int sensorThres = 400;
int buzzer=13;
int UpperThreshold = 518;
int LowerThreshold = 490;
int reading = 0;                        //HEART
float BPM = 0.0;
bool IgnoreReading = false;
bool FirstPulseDetected = false;
unsigned long FirstPulseTime = 0;
unsigned long SecondPulseTime = 0;
unsigned long PulseInterval = 0;


void setup()
{
  pinMode(alcohol, INPUT);
  pinMode(reading, INPUT);
   pinMode(buzzer, OUTPUT);
   pinMode(smoke, INPUT);
   pinMode(button1,INPUT);
  pinMode(button2,INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
//  pinMode(button3,INPUT);
  pinMode(ledd,OUTPUT);
   myserial.begin(9600);               ////setting baud rate og GSM module
  Serial.begin(9600);                 //setting baud rate of serial monitor
  delay(100);
}  

void loop() {
  int Emercount=0;
  if(digitalRead(button1))
  {
    while(Emercount<60)
    {
      if(digitalRead(button1))
      {
       Emercount++; 
       Serial.println("Emergency Action");
      }
      else
        break;
    }
    if(Emercount>=55)
    {
      Action();
      spread();
      Serial.println("Emergency Action taken");
      Serial.println(Emercount);
      delay(1000);
      exit(0);
    }
  }

int analogSensor = analogRead(alcohol);
int D2=analogSensor;

reading = analogRead(0); 
// Heart beat leading edge detected.
if(reading > UpperThreshold && IgnoreReading == false){
        if(FirstPulseDetected == false){
          FirstPulseTime = millis();
          FirstPulseDetected = true;
        }
        else{
          SecondPulseTime = millis();
          PulseInterval = SecondPulseTime - FirstPulseTime;
          FirstPulseTime = SecondPulseTime;
        }
        IgnoreReading = true;
      }

      // Heart beat trailing edge detected.
      if(reading < LowerThreshold){
        IgnoreReading = false;
      }  

      BPM = (1.0/PulseInterval) * 60.0 * 1000;

     /* Serial.print(reading);
      Serial.print("\t");
      Serial.print(PulseInterval);
      Serial.print("\t");
      Serial.print(BPM);
      Serial.println(" BPM");
      Serial.flush();
    */
      int D3 = analogRead(smoke);

      // Checks if it has reached the threshold value
  
      int D1=reading;
      float avg=(D1+D2+D3)/3;
      float avg1=(D1+D2)/2;
      float avg2=(D1+D3)/2;
      float avg3=(D2+D3)/2;
      /*Serial.println(avg);
      Serial.print("\t");
      Serial.print(avg1);
      Serial.print("\t");
      Serial.print(avg2);
      Serial.print("\t");
      Serial.print(avg3);
      Serial.println(D1);
      Serial.print("\t");
      Serial.print(PulseInterval);
      Serial.print("\t");
      Serial.print(BPM);      
      Serial.print("\t");
      Serial.print(D2);
      Serial.print("\t");
      Serial.print(D3);
      Serial.print("\t");*/
      delay(100);
     
      if (avg > TotalAverage||avg1>HeartAlcohol||avg2>HeartSmoke||avg3>AlcoholSmoke||D1>1500||D1<55)
      {  
        digitalWrite(buzzer, HIGH);
        delay(1000);
        digitalWrite(buzzer, LOW);
        Serial.println("IN DANGER.......");
      /*
        if(avg > 400)
          Serial.println("ALL");
        else if(avg1 > 400)
          Serial.println("Heartbeat and alcohol");
        else if(avg2 > 4000)
          Serial.println("Heartbeat and smoke");
        else if(avg3 > 400)
          Serial.println("Alocohol and smoke");
        else
          Serial.println("Heartbeat");
          */
        Serial.println("TAKE ACTION ??(y/n)");        
        int timer = 0;
        while(timer<100)
        {
            if(digitalRead(button1) || digitalRead(button2) )
              {
                    if(digitalRead(button1)==HIGH)
                    {      
                        //digitalWrite(ledd,HIGH);
                        Serial.println("ACTION....");
                        Action();
                        break;
                    }
                    else if(digitalRead(button2)==HIGH)
                    {
                        Serial.println("STOP ACTION...");
                        //digitalWrite(ledd,LOW);
                        break;
                    }
              }           
           else
           {
               timer++;
               delay(100);
           } 
        }
        if(flag == 1||timer >= 1000000000)
        {
          Serial.println("AUTOMATIC ACTION TAKEN...");
            Action();
        }
        else
        {
            avg = 0;
            //continue;
        }
        
      }
      else
      {
        digitalWrite(buzzer, LOW);
        Serial.println("SAFE....");
      }
      // Please don't use delay() - this is just for testing purposes.
     //  delay(5000); 
}

void Action() 
{
  appenddata();
  //data = Serial.available();
             for(int i=0;i<4;i++){
            SendMessege(numbers[i]);
             cleanBuffer();}             
            callNumber();
            cleanBuffer();
            delay(10000);
            spread();
}
void appenddata()
{
  
  String latitude = String(lat,6);
  String longitude = String(lon,6);

  float l1 = latitude.toFloat();
  float l2 = longitude.toFloat();
  
    // Make sure we have a location to send
      //char text[ 64 ];

      strcpy_P( text, (const char *) F("https://maps.google.com/maps/place/") );
      char *ptr = &text[ strlen(text) ];

      dtostrf( l1, 4, 6, ptr ); // append the latitude
      ptr = &ptr[ strlen(ptr) ];

      *ptr++ = ',';  // append one character

      dtostrf( l2, 4, 6, ptr ); // append the longitude

}
void SendMessege(String num)
{
  myserial.println("AT+CMGF = 1");          //sets the GSM module in text mode
  delay(1000);
  myserial.println("AT+CMGS=\"+91"+num+"\"\r");
  delay(1000);
  myserial.println("I AM In a danger");     // TEXT MESSEGE TO BE SEND
  myserial.write(text);
  delay(1000);
  myserial.println((char)26);
  delay(1000);
 
}
void cleanBuffer()
{ 
  delay( 250 );
  while ( myserial.available() > 0) 
  { myserial.read(); // Clean the input buffer delay(50); 
  }
}
void callNumber() {
  myserial.print (F("ATD"));
  myserial.print (callnumber1);
  myserial.print (F(";\r\n"));
}

void spread()
{
  digitalWrite(relay, LOW);
  delay(15000);
  digitalWrite(relay, HIGH);
  delay(1000);
  exit(0);
}
