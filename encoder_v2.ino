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
const int interruptNumber_1 =  1; /*interrupt 1 on the number pin2*/
const int PIN_2 = 2 ; /*encoder_left trigger signal*/ 
const int PIN_3 = 3 ; /*encoder_right trigger signal*/

#define INA  8 //motor A reversing control pin
#define PWMA  11 //motor A speed control pin
#define INB  12 //motor B reversing control pin
#define PWMB  10 //motor B speed control pin
#define SpeedValue 50

int rpm = 0 ;
int rpm_HIGH = 0 ;
int rpm_LOW = 0 ;
int count1 = 0 ;
int count2 = 0 ;
unsigned long time = 0 ;
unsigned long old_time = 0 ;
unsigned long time_delay = 0 ;
byte flag;     /*use for check the motor is CW or CCW*/

/*Global var setting*/
char val;
char acpt_cmd_temp[2]; 

void setup()
{
	/*MotorA and MotorB*/
	pinMode(INA,OUTPUT);
	pinMode(INB,OUTPUT);
	pinMode(PWMA,OUTPUT);
	pinMode(PWMB,OUTPUT);
	pinMode(PIN_2,INPUT);	
	pinMode(PIN_3,INPUT);	

	attachInterrupt(0 , Code , FALLING);
	Serial.begin(9600);
}

void loop()
{
		while(Serial.available()>0){
				acpt_cmd_temp[1]=acpt_cmd_temp[0];
				acpt_cmd_temp[0]=Serial.read();
		}
		PerformCommand();

		time = millis();
		/*caclulator the encoder counter by 20msec once */
		if(abs(time - old_time) >= 20) /*if the time over the 20 msec*/
		{
				detachInterrupt(interruptNumber_0); /*close external interrupt 0*/ 
				old_time= millis();  // 记录每次测速时的时间节点
				//把每一秒钟编码器码盘计得的脉冲数，换算为当前转速值
				//此编码器码盘为600个齿。
				rpm =(float)count1*60/600;
				rpm_HIGH=rpm/256;//把转速值分解为高字节和低字节
				rpm_LOW=rpm%256;  
				Serial.println(flag);    //向上位计算机上传电机转向标志字节
				Serial.println(rpm_HIGH);//向上位计算机上传电机当前转速的高字节     
				Serial.println(rpm_LOW); //向上位计算机上传电机当前转速的低字节 
				Serial.println(count1); 
				count1 = 0;   //把脉冲计数值清零，以便计算下一秒的脉冲计数        
				attachInterrupt(interruptNumber_0, Code,FALLING); // 重新开放外部中断0
		}
}

/*determine the command from Serail port*/
void PerformCommand()
{
		if(acpt_cmd_temp[0] == 'f'){
				//if(acpt_cmd_temp[1] == 'r') go_forward_R();
				//else if(acpt_cmd_temp[1] == 'l') go_forward_L();
				//else go_forward();
				go_forward();
		}
		else if(acpt_cmd_temp[0] == 's'){
				stop_motion();
		}
		else if(acpt_cmd_temp[0] == 'b'){
				//if(acpt_cmd_temp[1] == 'r') go_reverse_R();
				//else if(acpt_cmd_temp[1] == 'l') go_reverse_L();
				//else go_reverse();
				go_reverse();
		}
		else if(acpt_cmd_temp[0] == 'l'){
				go_left();
		}
		else if(acpt_cmd_temp[0] == 'r'){
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
		count1 = 0;
		old_time = millis();
}
void go_reverse()
{
		digitalWrite(INA,LOW);   
		//digitalWrite(INB,LOW);
		analogWrite(PWMA,SpeedValue);
		//analogWrite(PWMB,SpeedValue);
		count1 = 0;
		old_time = millis();

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
void Code()
{
		/*because our encoder is A leading B 90 angle*/
		if(digitalRead(PIN3)==HIGH)
		{
				flag=0x7F;/* flag is 127 , means of CW*/
		}
		else
		{
				flag=0xFF; /* flag is 255 , means of CCW*/
		}  
		//if((millis()-time_delay) > 5)
				count1 += 1; 
		time_delay=millis(); 
}
