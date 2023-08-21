# 자외선 노광기

자외선 노광기를 위한 타이머 구성
    [v] 로터리 인코더 {CLK:2, DT:3}. interupt 회로에 사용
    [v]  4 digits 7 segments display
        (x) 아두이노에서 나오는 핀과 드라이버 PCB는 분리해서 회로 구성한다.
    [] 릴레이 구성으로 램프를 구동하기 위한 회로가 필요하다.

개발 일정 2023/8/10부터
8/10    [] SW 스위치 모니터링 loop()에 넣고 BUILT_IN_LED사용
        [] 1초 이상 눌렀을 때, resetCount() 실행하기
        [] 프로그램 주석작업
8/18    [] Maxim7219칩을 가진 8자리 7세그먼트 부품 도착
8/19    [] 7 세그먼트 LED 프로그램 삽입
            └─ 프로그램 시현 정도만 채택하였음

PrintSeg(int arg)
PrintTimeSeg(float fCounter)
SetZero(void) // counter를 0으로 만드는 것이고
SetInit(void) // 초기화된 counter(90)를 표시하는 것
LightOn(void)
LightOff(void)
type definition이 잘못되어서 34초에서 로직에러를 자꾸만 내었음. 해결은 자리수를 늘리는 수밖에 없었고,  type casting을 엄밀하게 해야 되었음.
8/20    [] 7 세그먼트 LED 프로그램 안착
