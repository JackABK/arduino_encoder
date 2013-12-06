/* This Project is easy PID control with encoder,
 * it apply in the motor controller and caclulator the RPM.
 *
 * 
 * The encoder signal is A leader to B 90 angle , so that like as picture is CW.
 *				__	  __
 *         A __|  |__|	|__
 *             	  __    __ 
 *		   B   __|  |__|  |__
 *
 *
 * JackABK
 * Github: https://github.com/JackABK 
 * E-mail: gn771018@hotmail.com
 *
 */


/*define the all pin setting*/
#define interruptNumber_0   0 /*interrupt 0 on the number pin2*/
#define interruptNumber_1   1 /*interrupt 1 on the number pin3*/
#define PIN_EncoderLeftPhase_A  2  /*encoder_left phase A  signal , will trigger interrupt 0*/ 
#define PIN_EncoderLeftPhase_B  4  /*encoder_left phase B  signal*/ 
#define PIN_EncoderRightPhase_A  3  /*encoder_right phase A signal , will trigger interrupt 1*/
#define PIN_EncoderRightPhase_B  5  /*encoder_right phase A signal*/

/*Motor_left and Motor_right setting , on the testing wheel chair , it's driver only need by PWM.*/
//#define INA  8 /*motor A reversing control pin*/
//#define INB  12 /*motor B reversing control pin*/
#define PIN_Motor_left  11 /*motor left speed control pin*/
#define PIN_Motor_right  10 /*motor right speed control pin*/
#define SpeedValue 50

/*encoder1's var. setting*/
int rpm1 = 0;
int rpm1_HIGH = 0;
int rpm1_LOW = 0;
int count1 = 0 ;
/*encoder2's var. setting*/
int rpm2 = 0;
int rpm2_HIGH = 0;
int rpm2_LOW = 0;
int count2 = 0 ;

/*pwm regulate of two motor */
int SpeedValue_left = 80; /*default speedvalue*/
int SpeedValue_right;

unsigned long time = 0;
unsigned long old_time = 0;
unsigned long time_delay = 0;
byte CHECK_CW_OR_CCW; /*use for check the motor is CW or CCW*/
byte flag;
/*Global var setting*/
char val;
char acpt_cmd = 'f';  /* parse the command from uart*/
/*just a counter*/
int counter1=0; 
int counter2=0; 

void setup()
{
	/*MotorA and MotorB*/
	pinMode(INA,OUTPUT);
	pinMode(INB,OUTPUT);
	pinMode(PIN_Motor_left,OUTPUT);
	pinMode(PIN_Motor_right,OUTPUT);
	/*encoder_left and encoder_right */
	pinMode(PIN_EncoderLeftPhase_A,INPUT);	
	pinMode(PIN_EncoderLeftPhase_B,INPUT);	
	pinMode(PIN_EncoderRightPhase_A,INPUT);	
	pinMode(PIN_EncoderRightPhase_B,INPUT);	

	attachInterrupt(0 , Code1 , FALLING);
	attachInterrupt(1 , Code2 , FALLING);
	Serial.begin(9600);
}

void loop()
{
                
		/*if(Serial.available()> 0){
				acpt_cmd = Serial.read();
				PerformCommand();
		}*/
    
                
		while(counter1 < 1){
				PerformCommand();
				counter1= counter1 + 1;
		}


		time = millis();
		/*caclulator the encoder counter by 20msec once */
		if(abs(time - old_time) >= 1000) /*if the time over the 20 msec*/
		{
				detachInterrupt(interruptNumber_0); /*close external interrupt 0*/ 
				detachInterrupt(interruptNumber_1); /*close external interrupt 1*/ 
				old_time= millis();  /*record the time of every testing speed*/
				/*use the count total by  1 sec to caclulator RPM */
				/*the encoder spec is  500 pulse/1 rev */
				rpm1 =(float)count1*60/500;
				rpm2 =(float)count2*60/500;
				/*divide the rpm to HIGH part and LOW part.*/
				rpm1_HIGH=rpm1/256;
				rpm1_LOW=rpm1%256;  
				rpm2_HIGH=rpm2/256;
				rpm2_LOW=rpm2%256;  


				/*calculator the calibration of right motor based on left motor */
				SpeedValue_right = (float)SpeedValue_right + (rpm1 - rpm2)*0.4;
				/*print the message to stdout*/
				Serial.println("----motor left----"); 
				Serial.println(rpm1); 
				Serial.println(rpm1_HIGH);
				Serial.println(rpm1_LOW); 
				Serial.println(count1); 
				Serial.println("----motor right----"); 
				Serial.println(rpm2); 
				Serial.println(rpm2_HIGH);
				Serial.println(rpm2_LOW); 
				Serial.println(count2); 
				/*restart motor calibration and re-count encoder count*/
				if(flag == 'f') go_forward();
				else if(flag == 'b') go_reverse();
				count1 = 0;   
				count2 = 0;   
				/*restart enable the external interrupt*/
				attachInterrupt(interruptNumber_0, Code1,FALLING); 
				attachInterrupt(interruptNumber_1, Code2,FALLING);
                                
                                counter2 += 1;
		}

			/*only run 5 times*/
            if(counter2 > 5) {
              stop_motion() ;
              while(1);
            }
            

                
}

/*determine the command from Serail port*/
void PerformCommand()
{
		if(acpt_cmd == 'f'){
				SpeedValue_right=SpeedValue_left;
				go_forward();
				flag = 'f'; /*means forward*/
				count1=0;
				count2=0;
				old_time = millis();	
		}
		else if(acpt_cmd == 's'){
				stop_motion();
				flag = 's';
		}
		else if(acpt_cmd == 'b'){
				go_reverse();
				flag = 'b'; /*means reverse*/
				count1=0;
				count2=0;
				old_time = millis();	
		}
		else if(acpt_cmd == 'l'){
				go_left();
		}
		else if(acpt_cmd == 'r'){
				go_right();
		}
		else{
				/*do not anything*/
		}
}
void go_forward()
{
		digitalWrite(INA,HIGH);   
		//digitalWrite(INB,HIGH);
		analogWrite(PIN_Motor_left,SpeedValue_left);
		analogWrite(PIN_Motor_right,SpeedValue_right);
}
void go_reverse()
{
		digitalWrite(INA,LOW);   
		//digitalWrite(INB,LOW);
		analogWrite(PIN_Motor_left,SpeedValue_left);
		analogWrite(PIN_Motor_right,SpeedValue_right);

}
void go_left()
{
		digitalWrite(INA,LOW);   
		//digitalWrite(INB,HIGH);
		analogWrite(PIN_Motor_left,SpeedValue);
		//analogWrite(PIN_Motor_right,SpeedValue);
}
void go_right()
{
		digitalWrite(INA,HIGH);   
		//digitalWrite(INB,LOW);
		analogWrite(PIN_Motor_left,SpeedValue);
		//analogWrite(PIN_Motor_right,SpeedValue);
}
void go_forward_L()
{

}
void go_forward_R()
{

}
void go_reverse_L()
{

}
void go_reverse_R()
{

}
void stop_motion()
{
		digitalWrite(INA,LOW);   
		digitalWrite(INB,LOW);
		digitalWrite(PIN_Motor_left,LOW);
		digitalWrite(PIN_Motor_right,LOW);
}
/*************End Action Code*******************/
void Code1()
{
		/*because our encoder is A leading B 90 angle*/
		if(digitalRead(PIN3)==HIGH)
		{
				CHECK_CW_OR_CCW=0x7F;/* flag is 127 , means of CW*/
		}
		else
		{
				CHECK_CW_OR_CCW=0xFF; /* flag is 255 , means of CCW*/
		}  
		//if((millis()-time_delay) > 5)
				count1 += 1; 
		time_delay=millis(); 
}
void Code2()
{
		/*because our encoder is A leading B 90 angle*/
		if(digitalRead(PIN3)==HIGH)
		{
				CHECK_CW_OR_CCW=0x7F;/* flag is 127 , means of CW*/
		}
		else
		{
				CHECK_CW_OR_CCW=0xFF; /* flag is 255 , means of CCW*/
		}  
		//if((millis()-time_delay) > 5)
				count2 += 1; 
		time_delay=millis(); 
}
