

String readString;

int ch1,ch2,ch3,ch4,ch5,ch6,ch7,ch8,leer;

//////////////////////CONFIGURATION///////////////////////////////
#define channel_number 8  //set the number of chanels
#define default_servo_value 1500  //set the default servo value
#define PPM_FrLen 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PPM_PulseLen 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 10  //set PPM signal output pin on the arduino
//////////////////////////////////////////////////////////////////

int ppm[chanel_number];

void setup() {
  Serial.begin(115200);
  pinMode(9,OUTPUT);
  //initiallize default ppm values
  for(int i=0; i<chanel_number; i++){
    ppm[i]= default_servo_value;
  }

  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)
  
  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;
  
  OCR1A = 100;  // compare match register, change this
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();
}

void loop() 
{
    if (Serial.available() > 0)
    {   leer = Serial.readStringUntil(';').toInt();
        ch1 = Serial.readStringUntil(';').toInt();
        ch2 = Serial.readStringUntil(';').toInt();
        ch3 = Serial.readStringUntil(';').toInt();
        ch4 = Serial.readStringUntil(';').toInt();
        ch5 = Serial.readStringUntil(';').toInt();
        ch6 = Serial.readStringUntil(';').toInt();
        ch7 = Serial.readStringUntil(';').toInt();
        ch8 = Serial.readStringUntil('\n').toInt();


     
    }

 
  static int val = 1;
  
  ppm[0] = ch1;
  ppm[1] = ch2;
  ppm[2] = ch3;
  ppm[3] = ch4;
  ppm[4] = ch5;
  ppm[5] = ch6;
  ppm[6] = ch7;
  ppm[7] = ch8;
    
}

ISR(TIMER1_COMPA_vect){  //leave this alone
  static boolean state = true;
  
  TCNT1 = 0;
  
  if(state) {  //start pulse
    digitalWrite(sigPin, onState);
    OCR1A = PPM_PulseLen * 2;
    state = false;
  }
  else{  //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;
  
    digitalWrite(sigPin, !onState);
    state = true;

    if(cur_chan_numb >= chanel_number){
      cur_chan_numb = 0;
      calc_rest = calc_rest + PPM_PulseLen;// 
      OCR1A = (PPM_FrLen - calc_rest) * 2;
      calc_rest = 0;
    }
    else{
      OCR1A = (ppm[cur_chan_numb] - PPM_PulseLen) * 2;
      calc_rest = calc_rest + ppm[cur_chan_numb];
      cur_chan_numb++;
    }     
  }
    readString=""; //empty for next input
  } 

