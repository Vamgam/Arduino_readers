#define B 3950 // B-коэффициент
#define SERIAL_R 100000 // сопротивление последовательного резистора, 102 кОм
#define THERMISTOR_R 100000 // номинальное сопротивления термистора, 100 кОм
#define NOMINAL_T 25 // номинальная температура (при которой TR = 100 кОм)
boolean cold_start=true;
boolean cool=false;
boolean control=false;
float set=40;

const int numReadings = 10;

float readings[numReadings];  // массив для хранения значений полученных на аналоговом входе 
int readIndex = 0;          // индекс последнего значения
float total = 0;              // сумма значений
float average = 0;            // скользящее среднее
const byte tempPin = A0;
float volt_to_temp(int t){
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

void setup() {
    Serial.begin( 9600 );
    pinMode( A2, INPUT );
    pinMode( A3, INPUT );
    pinMode(A4,INPUT);
    pinMode(2,OUTPUT);
    pinMode(8,OUTPUT);
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
    
    

    
}

void loop() {
    float T_plansh =volt_to_temp( analogRead(A2));
    
    /*Сделать чтение земли что бы не было корреляции между соседними измерениями и датчиками*/
    
    Serial.print("PLN=");
    Serial.println(T_plansh,2);
    // t=analogRead(A1);
    // int t2= analogRead(A3);
    // Serial.print("t_2=");
    // Serial.println(volt_to_temp(t2),2);
    float T_stol= volt_to_temp(analogRead(A3));
    analogRead(A1);
    float T_cryshka=volt_to_temp(analogRead(A4));
    Serial.print("TBL=");
    Serial.println(average,2);
    Serial.print("CRS=");
    Serial.println(T_cryshka,2);
    total = total - readings[readIndex];
  // считываем значение 
    readings[readIndex] = T_stol;
  // добавляем значение к сумме
    total = total + readings[readIndex];
  // переставляем индекс на следующую позицию
    readIndex = readIndex + 1;

  // проверяем если мы выскочили индексом за пределы массива
    if (readIndex >= numReadings) {
    // если да, то индекс на ноль
        readIndex = 0;
    }

  // считаем среднее:
    average = total / numReadings;
    
    Serial.print(cold_start);
    Serial.print(cool);
    Serial.println(control);
    if (T_cryshka<set-1){
      digitalWrite(8,HIGH);
    }
    else{
      digitalWrite(8,LOW);
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
    if (average>(set+30) and cold_start){
        cold_start=false;
        cool=true;
        
    }else if (cool and average<set){
        cool=false;
        control=true;
        
    }else if ( T_plansh<set-0.5 and !cold_start and !cool){
        control=true;
        
    }else {
        control=false;
    }
    
 
    delay(500);}