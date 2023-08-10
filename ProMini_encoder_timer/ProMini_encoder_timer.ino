#define CLK 3    // 2번핀을 CLK에 연결 S1
#define DT 2     // 3번핀을 DT에 연결 S2
#define SW 4
#define BUILTIN_LED 13
int counter = 0;   // 카운팅 저장용 변수
int currentStateCLK;     // 현재 CLK의 상태 저장용 변수
int lastStateCLK;         // 이전 CLK의 상태 저장용 변수
String currentDir ="";     // 현재 방향 출력용 문자열 변수

void resetCounter(){
	counter = 0;
	Serial.print("reset counter : "); Serial.println(counter);
}

void setup() {
	
	// 엔코더 핀을 입력으로 설정
	pinMode(CLK, INPUT);
	pinMode(DT, INPUT);
	pinMode(SW, INPUT);

	// 시리얼 출력 개시
	Serial.begin(115200);

	// CLK핀의 최초 상태 저장
	lastStateCLK = digitalRead(CLK);
	
	//외부 인터럽트 등록, 핀의 상태가 변할 때(HIGH에서 LOW 또는 LOW에서 HIGH) 마다 updateEncoder함수가 실행됨. 
	// 인터럽트 0번은 2번핀과 연결되어 있고 1번은 3번 핀과 연결되어 있음
	attachInterrupt(0, updateEncoder, CHANGE);
	attachInterrupt(1, updateEncoder, CHANGE);
}

void loop() {
	//버튼이 눌리면 LED를 켜고 다시 누르면 LED가 꺼지고
	static int switchStat, prevSwitchStat=1;
	static bool LEDstat = false;
	static unsigned long tPressed, tReleased;
	switchStat = digitalRead(SW);
	if(switchStat == LOW && prevSwitchStat == HIGH){ // PRESSED
		LEDstat = !LEDstat;
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
		digitalWrite(BUILTIN_LED, HIGH);
	} else {
		digitalWrite(BUILTIN_LED, LOW);
	}

	if ((tReleased - tPressed) > (unsigned long) 1000){
		Serial.print("tReleased - tPressed = "); Serial.println((tReleased - tPressed));
		resetCounter();
		LEDstat = false;
		tPressed = millis();
		tReleased = millis();
	}

	prevSwitchStat = switchStat;

}

void updateEncoder(){   // 인터럽트 발생시 실행되는 함수
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
	}

	// 마지막 상태 변수 저장
	lastStateCLK = currentStateCLK;
}