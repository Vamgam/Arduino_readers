// Блок инициации переменных
#define B 3950 // B-коэффициент ntc
#define SERIAL_R 100000 // сопротивление последовательного резистора, 102 кОм
#define THERMISTOR_R 100000 // номинальное сопротивления термистора, 100 кОм
#define NOMINAL_T 25 // номинальная температура (при которой TR = 100 кОм)
#define R_ref 890 // Номинальное сопротивление резистора
#define R0 1000 // Номинальное сопротивление термистора
#define alpha 3850 //alpha-коэффициент dtc 

//Блок режимов работы
boolean cold_start=false;
boolean cool=true;
boolean control=false;
boolean start =false;
int set[] ={25,30,35,40};// Установленная температура
int cr_temp[]={23,30,40,45}; // температура поддержания крышки
int t_stol[]={23,28,30,35}; // температура поддержания стола
int i=0; // С какого шага начинается программа

// Блок инициаиции скользящего среднего для крышки и стола
const int numReadings = 10;
int count=0;
float readings[numReadings];
float readings_cr[numReadings];  // массив для хранения значений полученных на аналоговом входе 
int readIndex = 0;          // индекс последнего значения
float total = 0;   
float total1 = 0;           // сумма значений
float average = 0;
float average1 =0 ;            // скользящее среднее

// Функция перевода напрядения для негативного термистора
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

//функция перевода для прямого термистора
float volt_to_temp_dtc(int phi){
  float R_therm = (float)phi*R_ref/(1023.0 - (float)phi);
  float temp = (R_therm - R0);
  temp/=alpha;
  temp=temp*1000000;
  temp/=R0;
  return temp;
}
//скользящее среднее
float move_average(float temp1,float temp2){
    total = total - readings[readIndex];
    total1=total1-readings_cr[readIndex];
    readings[readIndex] = temp1;
    readings_cr[readIndex]=temp2;

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


}
//Контроль режимов работы и температуры столика и крышки с учётом температуры стола
bool contorl_of_regimes(float temp3){
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
  if (average>(2*set[i]-25) and cold_start){
        cold_start=false;
        cool=true;
    }else if (cool and average<t_stol[i]){
        cool=false;
        control=true;
    }else if ( temp3<t_stol[i] and !cold_start and !cool){
        control=true;
    }else {
        control=false;
    }
}
// Таймер на время
bool time_count(){
  uint32_t sec = millis() / 1000ul;
  
  if ((sec%1800==0) and sec>1){
      i+=1;
      cold_start=true;
      cool=false;
      control=false;
  }
}
///  Начало программы
uint8_t analog_pins[] = {A2,A5,A3,A6};
bool pins[]={true,true,true,true};
void setup() {
  for (int i = 0; i < 4; i++) { //or i <= 4
      int k =volt_to_temp_ntc(analog_pins[i]);
      if (k<0){
        pins[i]=false;

      }
}
    Serial.begin( 9600 );
    for (int i=0;i<4;i++){
      if (pins[i]){
        pinMode(analog_pins[i],INPUT);
        Serial.print("Found port");
        Serial.println(analog_pins[i]);

      }
    }
    pinMode(A7,INPUT);
    pinMode(2,OUTPUT);
    pinMode(8,OUTPUT);
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
    readings_cr[thisReading]=0;
  }
// Ожидание команды для старта
  while (!start){  
    if (Serial.available()>0){
      char incomingCharacter = Serial.read();
      switch (incomingCharacter) {
        case '1':
        start=true;
        i=1;
        cold_start=true;
        cool=false;
        break;
        case '0':
        start=true;
        i=0;
        break;
        case '2':
        start=true;
        i=2;
        break;
        case '3':
        start=true;
        i=3;
        break;
  
      }
    }
    }
    
    
    
  }
  

  
// Главный цикл программы
void loop() {
                                // ЗАДЕРЖКА. Без неё работает некорректно!
    
    
    float T_plansh =volt_to_temp_ntc(analogRead(A5));
    analogRead(A1);
    float T_stol= volt_to_temp_ntc(analogRead(A7));
    float T_cryshka=volt_to_temp_ntc(analogRead(A6));
    
    Serial.print("PLN=");
    Serial.print(T_plansh,2);
    Serial.print(" TBL=");
    Serial.print(average,2);
    Serial.print(" CRS=");
    Serial.print(average1,2);
    Serial.print(" ");
    Serial.print(cold_start);
    Serial.print(cool);
    Serial.print(control);
    Serial.print(" ");
    Serial.println(set[i]);


    move_average(T_stol,T_cryshka);
    contorl_of_regimes(T_plansh);
    time_count();
    delay(500);}