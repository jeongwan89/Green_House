# 자외선 노광기

version 2는 PCB 때문에 CS 와 CLK이 바꾸었다. 이 두 핀만 바뀌면 Through hole이 없어도 되기 때문이다. 그런데 기억의 한편 속에 이 핀을 전에 이미 바꾸었던 것이 기억이 난다. CW CCW가 뒤집혀서 CLK와 DT을 바꿔서 이를 해결하였는데, 해결한 것이 ver1의 내용이었다. 그래서 CLK와 DT의 번호매김이 이상하게 생겼었던 것이다. 이렇게 PCB 때문에 일부로 바꾼 것이 되려 처음의 시도로 돌아갔다. 반드시 PCB만들고 나서 Knob 카운터가 내가 원하는 대로 움직이닌지를 확인하고 실장해야할 것이다. -> 또한 이 프로그램은 시간을 쓰고 까먹는 예를 들면 물 받는 모터 작동과 간은 프로그램에 바로 응용할 수 있을 것이다.ㄴ

자외선 노광기를 위한 타이머 구성
    [v] 로터리 인코더 {CLK:2, DT:3}. interupt 회로에 사용
    [v]  4 digits 7 segments display
        (x) 아두이노에서 나오는 핀과 드라이버 PCB는 분리해서 회로 구성한다.
    [v] 릴레이 구성으로 램프를 구동하기 위한 회로가 필요하다.

개발 일정 2023/8/10부터
8/10    [v] SW 스위치 모니터링 loop()에 넣고 BUILT_IN_LED사용
        [v] 1초 이상 눌렀을 때, resetCount() 실행하기
        [v] 프로그램 주석작업
8/18    [v] Maxim7219칩을 가진 8자리 7세그먼트 부품 도착
8/19    [v] 7 세그먼트 LED 프로그램 삽입
            └─ 프로그램 시현 정도만 채택하였음

PrintSeg(int arg)
PrintTimeSeg(float fCounter)
SetZero(void) // counter를 0으로 만드는 것이고
SetInit(void) // 초기화된 counter(90)를 표시하는 것
LightOn(void)
LightOff(void)
type definition이 잘못되어서 34초에서 로직에러를 자꾸만 내었음. 해결은 자리수를 늘리는 수밖에 없었고,  type casting을 엄밀하게 해야 되었음.
8/20    [v] 7 세그먼트 LED 프로그램 안착
