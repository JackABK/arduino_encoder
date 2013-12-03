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


/*define the pin set*/
const int interruptNumber_0  = 0; /*interrupt 0 on the number pin2*/
const int interruptNumber_1 =  1; /*interrupt 1 on the number pin3*/
const int PIN_2 = 2 ; /*encoder_left phase A  signal , will trigger interrupt 0*/ 
const int PIN_4 = 4 ; /*encoder_left phase B  signal*/ 
const int PIN_3 = 3 ; /*encoder_right phase A signal , will trigger interrupt 1*/
const int PIN_5 = 5 ; /*encoder_right phase A signal*/
/*pwm regulate of two motor */
int SpeedValue_right;
int SpeedValue_left;

#define INA  8 /*motor A reversing control pin*/
#define PWMA  11 //motor A speed control pin*/
#define INB  12 //motor B reversing control pin
#define PWMB  10 //motor B speed control pin
#define SpeedValue 50

/*encoder1's rmp1 setting*/
int rpm1 = 0;
int rpm1_HIGH = 0;
int rpm1_LOW = 0;
int count1 = 0 ;
/*encoder2's rmp2* setting*/
int rpm2 = 0;
int rpm2_HIGH = 0;
int rpm2_LOW = 0;
int count2 = 0 ;

unsigned long time = 0;
unsigned long old_time = 0;
unsigned long time_delay = 0;
byte CHECK_CW_OR_CCW; /*use for check the motor is CW or CCW*/
byte flag;
/*Global var setting*/
char val;
char acpt_cmd_temp[2]; /* parse the command from uart*/
char acpt_cmd ; 
void setup()
{
	/*MotorA and MotorB*/
	pinMode(INA,OUTPUT);
	pinMode(INB,OUTPUT);
	pinMode(PWMA,OUTPUT);
	pinMode(PWMB,OUTPUT);
	/*encoder_left and encoder_right */
	pinMode(PIN_2,INPUT);	
	pinMode(PIN_4,INPUT);	
	pinMode(PIN_3,INPUT);	
	pinMode(PIN_5,INPUT);	

	attachInterrupt(0 , Code1 , FALLING);
	attachInterrupt(1 , Code2 , FALLING);
	Serial.begin(9600);
}

void loop()
{
		if(Serial.available()> 0){
				acpt_cmd = Serial.read();
				PerformCommand();
		}


		time = millis();
		/*caclulator the encoder counter by 20msec once */
		if(abs(time - old_time) >= 1000) /*if the time over the 20 msec*/
		{
				detachInterrupt(interruptNumber_0); /*close external interrupt 0*/ 
				detachInterrupt(interruptNumber_1); /*close external interrupt 1*/ 
				old_time= millis();  /*record the time of every testing speed*/
				/*use the count total by 20msec to caclulator RPM */
				/*the encoder spec is  600 pulse/1 rev */
				rpm1 =(float)count1*60/600;
				rpm2 =(float)count2*60/600;
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
		analogWrite(PWMA,SpeedValue);
		//analogWrite(PWMB,SpeedValue);
}
void go_reverse()
{
		digitalWrite(INA,LOW);   
		//digitalWrite(INB,LOW);
		analogWrite(PWMA,SpeedValue);
		//analogWrite(PWMB,SpeedValue);

}
void go_left()
{
		digitalWrite(INA,LOW);   
		//digitalWrite(INB,HIGH);
		analogWrite(PWMA,SpeedValue);
		//analogWrite(PWMB,SpeedValue);
}
void go_right()
{
		digitalWrite(INA,HIGH);   
		//digitalWrite(INB,LOW);
		analogWrite(PWMA,SpeedValue);
		//analogWrite(PWMB,SpeedValue);
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
		//digitalWrite(INB,LOW);
		digitalWrite(PWMA,LOW);
		//digitalWrite(PWMB,LOW);
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
				count1 += 1; 
		time_delay=millis(); 
}
