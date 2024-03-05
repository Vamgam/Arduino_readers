#define B 3950 // B-коэффициент
#define SERIAL_R 100000 // сопротивление последовательного резистора, 102 кОм
#define THERMISTOR_R 100000 // номинальное сопротивления термистора, 100 кОм
#define NOMINAL_T 25 // номинальная температура (при которой TR = 100 кОм)
#define R_ref 890
#define R0 1000
#define alpha 3850

boolean cold_start=true;
boolean start =false;

int set_temp_arr[] = {30,35,40};


int lid_temp_lower[]={33, 43, 50};
int lid_temp_upper[]={35, 45, 55};

int table_temp_cold_start[] =  {47, 63, 75};
int table_temp_control_lower[]={33, 41, 48};
int table_temp_control_upper[]={35, 43, 50};


int i = 2;

const int numReadings = 20;
int interval = 100;
int count=0;

float table_temp = 0;
float plate1_temp = 0;
float plate2_temp = 0;
float lid1_temp = 0;
float lid2_temp = 0;

int table_pin = A7;
int plate1_pin = A2;
int plate2_pin = A3;
int lid1_pin = A5;
int lid2_pin = A6;
int lid_relay_pin = 8;
int plate_relay_pin = 2;


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


void setup() {
  Serial.begin( 9600 );


  pinMode(table_pin, INPUT);
  pinMode(plate1_pin, INPUT);
  pinMode(plate2_pin, INPUT);
  pinMode(lid1_pin, INPUT);
  pinMode(lid2_pin, INPUT);
  pinMode(lid_relay_pin, OUTPUT);
  pinMode(plate_relay_pin, OUTPUT);
  digitalWrite(plate_relay_pin, LOW);
  digitalWrite(lid_relay_pin, LOW);

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
  
void read_temp(){
  table_temp = 0;
  plate1_temp = 0;
  plate2_temp = 0;
  lid1_temp = 0;
  lid2_temp = 0;
  for (int j = 0; j < numReadings; j++){
      table_temp += volt_to_temp_ntc(analogRead(table_pin));
      //plate1_temp += volt_to_temp_dtc(analogRead(plate1_pin));
      plate2_temp += volt_to_temp_dtc(analogRead(plate2_pin));
      //lid1_temp += volt_to_temp_ntc(analogRead(lid1_pin));
      lid2_temp += volt_to_temp_ntc(analogRead(lid2_pin));
      delay(interval);
    }
  table_temp /= numReadings;
  plate1_temp /= numReadings;
  plate2_temp /= numReadings;
  lid1_temp /= numReadings;
  lid2_temp /= numReadings;



  Serial.print("table=");
  Serial.print(table_temp);
  Serial.print(" plate1=");
  Serial.print(plate1_temp);
  Serial.print(" lid1=");
  Serial.print(lid1_temp);
  Serial.print(" plate2=");
  Serial.print(plate2_temp);
  Serial.print(" lid2=");
  Serial.println(lid2_temp);
}

void control_table_relay(){
  if (cold_start){
        digitalWrite(plate_relay_pin, HIGH);
        Serial.println("Table ON cold start");
        if (table_temp > table_temp_cold_start[i]){
          digitalWrite(plate_relay_pin, LOW);
          cold_start = false;
          Serial.println("Table OFF cold start OFF");
        }
    }else{
  
    if (table_temp < table_temp_control_lower[i]){
      digitalWrite(plate_relay_pin, HIGH);
      Serial.println("Table ON");
    } else if (table_temp > table_temp_control_upper[i]){
      digitalWrite(plate_relay_pin, LOW);
      Serial.println("Table OFF");
    }
    }
}

void control_lid_relay(){
  if (lid2_temp < lid_temp_lower[i]){
      digitalWrite(lid_relay_pin, LOW);
      Serial.println("Lid ON");
    } else if (lid2_temp > lid_temp_upper[i]) {
      digitalWrite(lid_relay_pin, HIGH);
      Serial.println("Lid OFF");
    }
}

void loop() {
    read_temp();
    control_table_relay();
    control_lid_relay();

    // uint32_t sec = millis() / 1000ul;
  
    // if ((sec%1800==0) and sec>1){
    //   i+=1;
    //   cold_start=true;
    //   cool=false;
    //   control=false;
    // }
    
    
    }
