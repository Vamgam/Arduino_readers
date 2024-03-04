#define B 3950 // B-коэффициент
#define SERIAL_R 100000 // сопротивление последовательного резистора, 102 кОм
#define THERMISTOR_R 100000 // номинальное сопротивления термистора, 100 кОм
#define NOMINAL_T 25 // номинальная температура (при которой TR = 100 кОм)
boolean cold_start=false;
boolean cool=true;
boolean control=false;
int set[] ={25,30,35,40};
int cr_temp[]={23,30,40,45};
int t_stol[]={23,28,30,35};
int i=1;


#define R_ref 890
#define R0 1000
#define alpha 3850



const int numReadings = 10;
int count=0;

float readings[numReadings];
float readings_cr[numReadings];  // массив для хранения значений полученных на аналоговом входе 
int readIndex = 0;          // индекс последнего значения
float total = 0;   
float total1 = 0;           // сумма значений
float average = 0;
float average1 =0 ;            // скользящее среднее
const byte tempPin = A0;
float volt_to_temp_ntc(int t){
    float tr = 1023.0 / t - 1;
    tr = SERIAL_R / tr;
    float steinhart=0;
    steinhart = tr / THERMISTOR_R; // (R/Ro)
    steinhart = log(steinhart); // ln(R/Ro)
    steinhart /= B; // 1/B * ln(R/Ro)
    steinhart += 1.0 / (NOMINAL_T + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart; // Invert
    steinhart -= 273.15; 
    return (steinhart);
}

float volt_to_temp_dtc(int phi){
  float R_therm = (float)phi*R_ref/(1023.0 - (float)phi);
  float temp = (R_therm - R0);
  temp/=alpha;
  temp=temp*1000000;
  temp/=R0;
  return temp;
}
boolean start =false;
void setup() {
    Serial.begin( 9600 );
    pinMode( A2, INPUT );
    pinMode( A3, INPUT );
    pinMode(A4,INPUT);
    pinMode(2,OUTPUT);
    pinMode(8,OUTPUT);
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
    readings_cr[thisReading]=0;
  }
  
  while (!start){
  
    if (Serial.available()>0){
      char incomingCharacter = Serial.read();
      switch (incomingCharacter) {
        case '1':
        start=true;
        break;
  
      }
    }
    }
  }
  
  

void loop() {
                                // ЗАДЕРЖКА. Без неё работает некорректно!
    
    float T_plansh =volt_to_temp_dtc( analogRead(A3));
    /*Сделать чтение земли что бы не было корреляции между соседними измерениями и датчиками*/
    Serial.print("PLN=");
    Serial.print(T_plansh,2);
    float T_stol= volt_to_temp_ntc(analogRead(A7));
    analogRead(A1);
    float T_cryshka=volt_to_temp_ntc(analogRead(A6));
    Serial.print(" TBL=");
    Serial.print(average,2);
    Serial.print(" CRS=");
    Serial.print(average1,2);
    total = total - readings[readIndex];
    total1=total1-readings_cr[readIndex];
  // считываем значение 
    readings[readIndex] = T_stol;
    readings_cr[readIndex]=T_cryshka;

  // добавляем значение к сумме
    total = total + readings[readIndex];
    total1=total1+readings_cr[readIndex];
  // переставляем индекс на следующую позицию
    readIndex = readIndex + 1;
  // проверяем если мы выскочили индексом за пределы массива
    if (readIndex >= numReadings) {
    // если да, то индекс на ноль
        readIndex = 0;
    }
  // считаем среднее:
    average = total / numReadings;
    average1=total1/numReadings;
    Serial.print(" ");
 
    Serial.print(cold_start);
    Serial.print(cool);
    Serial.print(control);
    Serial.print(" ");
    Serial.println(set[i]);


    if (average1<cr_temp[i]){
      digitalWrite(8,LOW);
    }
    else{
      digitalWrite(8,HIGH);
    }
    if (cold_start){
        digitalWrite(2,HIGH);
        // digitalWrite(8,HIGH);
    }else if (cool){
        digitalWrite(2,LOW);
        // digitalWrite(8,LOW);
    }else if (control){
        digitalWrite(2,HIGH);
        // digitalWrite(8,HIGH);
    }else{
        digitalWrite(2,LOW);
        // digitalWrite(8,LOW);
    }
    // digitalWrite(8,HIGH);
    if (average>(2*set[i]-25) and cold_start){
        cold_start=false;
        cool=true;
    }else if (cool and average<t_stol[i]){
        cool=false;
        control=true;
    }else if ( T_plansh<t_stol[i] and !cold_start and !cool){
        control=true;
    }else {
        control=false;
    }
    uint32_t sec = millis() / 1000ul;
  
    if ((sec%1800==0) and sec>1){
      i+=1;
      cold_start=true;
      cool=false;
      control=false;
    }
    delay(500);}