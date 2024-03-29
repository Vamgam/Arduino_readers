// Блок инициации переменных
#define B 3950 // B-коэффициент ntc
#define SERIAL_R 100000 // сопротивление последовательного резистора, 102 кОм
#define THERMISTOR_R 100000 // номинальное сопротивления термистора, 100 кОм
#define NOMINAL_T 25 // номинальная температура (при которой TR = 100 кОм)
#define R_ref 890 // Номинальное сопротивление резистора
#define R0 1000 // Номинальное сопротивление термистора
#define alpha 3850 //alpha-коэффициент dtc 

float T_plate = 0;
float T_lid = 0;
float T_table = 0;

int plate_last_val = 0;
int lid_last_val = 0;
int table_last_val = 0;

//Блок режимов работы
boolean cold_start=false;
boolean cool=true;
boolean control=false;
boolean start =false;
boolean end_flag = false;
boolean lid_state = false;
boolean plate_state = false;

int table_pin = 2;
int lid_pin = 8;


int time_interval = 600;// Времени на каждую температурную точку в секундах
int set[] ={27,29,31,33,35,37,39};// Установленная температура
int cr_temp[]={30,32,34,36,38,40,42}; // температура поддержания крышки
int t_stol[]={27,29,31,33,35,37,39}; // температура поддержания стола
int temp_index=0; // С какого шага начинается программа


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


uint16_t lowpass_filtr_u16(int input , int *last , int Num){ // Yn = 1/8 Xn + 7/8 Yn-1  ;  FILTR_BITS 3  ->  1/8

    if( Num == 0 ){ return input; }
    if( Num > 15 ){ Num = 15; }

    *last -=((*last) >> Num);   // 7/8 Yn-1
    *last += (uint32_t)(input); // 1/8 Xn

    return (uint16_t)((*last) >> Num);
}



void contorl_of_regimes(){
  if (T_lid<cr_temp[temp_index]+5){
      lid_state = true;
      digitalWrite(lid_pin,LOW);
    }
    else{
      lid_state = false;
      digitalWrite(lid_pin,HIGH);
    }
  if (cold_start){
        plate_state = true;
        digitalWrite(2,HIGH);
    }else if (cool){
        plate_state = false;
        digitalWrite(table_pin,LOW);
    }else if (control){
        plate_state = true;
        digitalWrite(table_pin,HIGH);
    }else{
        plate_state = false;
        digitalWrite(table_pin,LOW);
    }

  if (T_table>(set[temp_index]+3) and cold_start){
        cold_start=false;
        cool=true;
    }else if (cool and T_table<t_stol[temp_index]){
        cool=false;
        control=true;
    }else if (T_plate<t_stol[temp_index]-1 and !cold_start and !cool){
        control=true;
    }else {
        control=false;
    }
}

void time_count(){
  uint32_t sec = millis() / 1000ul;
  
  if ((sec>=time_interval*(temp_index+1)) && (!end_flag)){
    if (temp_index==4){
      end_flag = true;
      cold_start=false;
      cool=true;
      control =false;
    }
    else{
      temp_index+=1;
      cold_start=true;
      cool=false;
      control=false;}
  }
  
}

uint8_t analog_pins[] = {A2,A5,A3,A6};
bool pins[]={true,true,true,true};
void setup() {
  // for (int i = 0; i < 4; i++) { //or i <= 4
  //     int k =volt_to_temp_ntc(analog_pins[i]);
  //     if (k<0){
  //       pins[i]=false;

  //     }
//}
    Serial.begin( 9600 );
    for (int i=0;i<4;i++){
      if (pins[i]){
        pinMode(analog_pins[i],INPUT);
        Serial.print("Found port");
        Serial.println(analog_pins[i]);

      }
    }
    pinMode(A7,INPUT);
    pinMode(lid_pin,OUTPUT);
    pinMode(table_pin,OUTPUT);
    digitalWrite(lid_pin, HIGH);
    digitalWrite(table_pin, LOW);

    for (int j = 0; j <20; j++){
      lowpass_filtr_u16(analogRead(A5), &plate_last_val, 2);
      lowpass_filtr_u16(analogRead(A6), &lid_last_val, 2);
      lowpass_filtr_u16(analogRead(A7), &table_last_val, 2);
    }

// Ожидание команды для старта
  while (!start){  
    if (Serial.available()>0){
      temp_index = Serial.parseInt();
      start=true;
      cold_start=true;
      cool=false;

    }
    }
    
  }
  

  
// Главный цикл программы
void loop() {

    if (!end_flag){
    
    T_plate = volt_to_temp_ntc(lowpass_filtr_u16(analogRead(A5), &plate_last_val, 2));
    T_lid = volt_to_temp_ntc(lowpass_filtr_u16(analogRead(A6), &lid_last_val, 2));
    T_table = volt_to_temp_ntc(lowpass_filtr_u16(analogRead(A7), &table_last_val, 2));
    
    Serial.print("plate=");
    Serial.print(T_plate,2);
    Serial.print(";table=");
    Serial.print(T_table,2);
    Serial.print(";lid=");
    Serial.print(T_lid,2);
    Serial.print(";plate_state=");
    Serial.print(plate_state);
    Serial.print(";lid_state=");
    Serial.print(lid_state);
    Serial.print(";set_temp=");
    Serial.println(set[temp_index]);


    contorl_of_regimes();
    time_count();
    }else{
    Serial.println('termination');
    }
    delay(500);
  }