// 자외선 램프 작동 프로그램
#include <math.h>

// incremental encoder program
#define CLK 3 	// 2번핀을 CLK에 연결 S1
#define DT 2 	// 3번핀을 DT에 연결 S2
#define SW 4	// incremental encoder의 누름 스위치. 누르면 LOW
#define RELAY 13
// 8 digits 7 segments LED drived by max7219
#define DIN7219 12	
#define CLK7219 11
#define CS7219	10
int counter = 90;   // 카운팅 저장용 변수
int currentStateCLK;     // 현재 CLK의 상태 저장용 변수
int lastStateCLK;         // 이전 CLK의 상태 저장용 변수
String currentDir ="";     // 현재 방향 출력용 문자열 변수


// 4 digits 7 segments LED display driver part
byte adr = 0x08;
byte num = 0x00;
int i = 0;

void shift(byte send_to_address, byte send_this_data)
{
  digitalWrite(CS7219, LOW);
  shiftOut(DIN7219, CLK7219, MSBFIRST, send_to_address);
  shiftOut(DIN7219, CLK7219, MSBFIRST, send_this_data);
  digitalWrite(CS7219, HIGH);
}

void SetZero(void){
	counter = 0; //약간 오지랍 같은 느낌의 처리이다.
	shift(8,0);
	shift(7,0);
	shift(6,10);
	shift(5,0);
	shift(4,0);
	shift(3,10);
	shift(2,0);
	shift(1,0);
}
// 7 segments LED에 출력하는 프로그램
//char* arg는 출력할 수를 표현한다. 특수 문자도 포함할 수 있다.
//int size_t는 arg[]의 수를 가진다. 0이면 Null, 한 자리라면 1
void PrintSeg(int counter){
	if (counter < 0) counter = 0;
	int digit;
	for(i=7; i >= 0; i--){
		digit = (int) (counter / pow(10, i));
		shift(i+1, digit);
		counter -= (int) digit * pow(10, i);
	}
}

// nCounter를 인수로 받아서 60진법 표시로 바꾸어 보낸다.
// mm-ss-ds 형태로 보낸다.
void PrintTimeSeg(double fCounter){
	//nCounter의 범위는 0~3599가 의미 있는 수 이고 이것 밖에 있는 것은 취급하지 않는 것으로 한다.
	/*if(fCounter > 3600 || fCounter < 0){
		SetZero();
		return;
	}
	*/
	unsigned long sMin, sSec, sMilli;
	sMin = (unsigned long) fCounter / 60;
	sSec = (unsigned long) fCounter % 60;
	sMilli = (fCounter - (unsigned long)fCounter) * 100;

	shift(8, sMin/10);
	shift(7, sMin%10);
	shift(6, 10);
	shift(5, sSec/10);
	shift(4, sSec%10);
	shift(3, 10);
	shift(2, sMilli/10);
	shift(1, sMilli%10);
}
// 이 함수가 호출되면 다이얼 counter을 0으로 reset
void resetCounter(){
	counter = 0;
	Serial.print("reset counter : "); Serial.println(counter);
	// [] 여기에 7세그먼트 표시를 위한 코드가 필요하다.
	SetZero();
}

// 인터럽트 발생시 실행되는 함수
void updateEncoder(){   

	// CLK의 현재 상태를 읽어서 
	currentStateCLK = digitalRead(CLK);

	// CLK핀의 신호가 바뀌었고(즉, 로터리엔코더의 회전이 발생했했고), 그 상태가 HIGH이면(최소 회전단위의 회전이 발생했다면) 
	if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
		// DT핀의 신호를 확인해서 엔코더의 회전 방향을 확인함.
		if (digitalRead(DT) != currentStateCLK) {    // 신호가 다르다면 시계방향 회전
			counter ++;                                // 카운팅 용 숫자 1 증가
			currentDir ="CW";
		} else {                                   // 신호가 같다면 반시계방향 회전
			counter --;                              // 카운팅 용 숫자 1 감소
			currentDir ="CCW";
		}
		Serial.print("revolution: ");
		Serial.println(currentDir);               // 회전방향 출력
		Serial.print("Counter: ");
		Serial.println(counter);              // 카운팅 출력

		//7seg에 출력
		PrintTimeSeg(counter);

	}

	// 마지막 상태 변수 저장
	lastStateCLK = currentStateCLK;
}

void setup() {
	// 엔코더 핀을 입력으로 설정
	pinMode(CLK, INPUT);
	pinMode(DT, INPUT);
	pinMode(SW, INPUT);

	//8 digits 7 segments LED of max7219
	pinMode(DIN7219, OUTPUT);
	pinMode(CS7219, OUTPUT);
	pinMode(CLK7219, OUTPUT);
	digitalWrite(CLK7219, LOW);
	delay(200);

	//Setup of MAX7219 chip
	shift(0x0f, 0x00); //display test register - test mode off
	shift(0x0c, 0x01); //shutdown register - normal operation
	shift(0x0b, 0x07); //scan limit register - display digits 0 thru 7
	shift(0x0a, 0x0f); //intensity register - max brightness
  	shift(0x09, 0xff); //decode mode register - CodeB decode all digits
	
	// 시리얼 출력 개시
	Serial.begin(115200);

	// CLK핀의 최초 상태 저장
	lastStateCLK = digitalRead(CLK);
	
	//외부 인터럽트 등록, 핀의 상태가 변할 때(HIGH에서 LOW 또는 LOW에서 HIGH) 마다 updateEncoder함수가 실행됨. 
	// 인터럽트 0번은 2번핀과 연결되어 있고 1번은 3번 핀과 연결되어 있음
	attachInterrupt(0, updateEncoder, CHANGE);
	attachInterrupt(1, updateEncoder, CHANGE);
	
	//7segment 초기화
	PrintTimeSeg(counter);
}

void loop() {
	static int switchStat, prevSwitchStat=1;	// loop()안에서 두 변수의 상태가 할당된다. prevSwitchStat는 loop()떠나기 직전에 switchStat의 값으로 할당된다.
	static bool LEDstat = false; 				// LEDstat는 바로 점등상태로 연결된다
	static unsigned long tPressed, tReleased;	// 다이얼 버튼이 눌러진 시간, 헤제된 시간 : 이를 결정하기 위해 조건이 필요하다
	static unsigned long tPrevPressed = millis();			// doubleclick하기 위한 code
	static unsigned long tStart; //timer 시작 시간
	static double fTimer; //PrintTimeSeg에 인자를 넘겨주기 위한 변수

	switchStat = digitalRead(SW);
	if(switchStat == LOW && prevSwitchStat == HIGH){ // PRESSED
		LEDstat = !LEDstat;
		if(LEDstat == true){
			tStart = millis();
		}
		tPrevPressed = tPressed;
		tPressed = millis();
		tReleased = millis();
		Serial.print("tPressed = "); Serial.println(tPressed);
		delay(10);
		/* debug
		Serial.print("LEDstat: "); Serial.println(LEDstat);
		Serial.print("switchStat: "); Serial.println(switchStat);
		*/
	}
	// elimate debouncing after release
	if(prevSwitchStat == LOW && switchStat == HIGH) { // RELEASED
		tReleased = millis();
		Serial.print("tReleased = "); Serial.println(tReleased); 
		delay(10);
	}

	if(LEDstat == true) {
		// count down 처리해야한다. *********** 카 운 트 다 운 ****************
		digitalWrite(RELAY, HIGH);
		if(millis()-tStart < (unsigned long) counter * 1000) {
			// Relay가 켜졌고, 눌러졌을 때의 시각이 저장된 카운터보다 작으면, 카운터 작동을 한다
			fTimer = (double)( (unsigned long) counter * 1000 - (millis() - tStart) ) / 1000;
			PrintTimeSeg(fTimer);
		}
		else{
			// 릴레이가 켜졌고, 카운터가 다 되었을 때의 작동법
			PrintTimeSeg(0);
			counter = 0;
			LEDstat = false;
			digitalWrite(RELAY, LOW);
		}
		// count down 처리해야한다. *********** 카 운 트 다 운 ****************
	} else {
		digitalWrite(RELAY, LOW);
	}

	if ((tReleased - tPressed) > (unsigned long) 1000){
		// 누른 시간이 1초 초과이면 리셋
		Serial.print("tReleased - tPressed = "); Serial.println((tReleased - tPressed));
		resetCounter();
		LEDstat = false;
		tPressed = millis();
		tReleased = millis();
	} 

	// doubleClick되었을 때 counter를 다시 90으로 하고 표시를 해준다.
	if((tPressed - tPrevPressed) <= (unsigned long) 700){
		counter = 90;
		PrintTimeSeg(counter);
		tPrevPressed = millis();
	}
	prevSwitchStat = switchStat;

	// 8 digits 7 segments LED of max7219
	//Data transfer
}
