1. 프로그램의 목적
    비닐 하우스 측창의 모터를 조정하는 프로그램

2. 목표
    MQTT를 이용하여 명령을 받는다.
    어떤 모드가 되면 MQTT 데이터를 이용하여 MCU가 스스로 측창을 조정한다.
    모터가 정상으로 작동되는지 전류 센서를 모니터하여, 작동, 정지, 이상, 리미트 이런 정도의 상태를 추정한다.

3. 프로그램 분리
    shutter_header.h
        .ino에 쓰일  lib 인클루드
        #define 정의
        여러 동을 제어하기 위한 define 설정
        class 정의
            Shutter
            RaiseTimeEventInLoop
        함수 정의
    shutter.h
        .ino에 쓰일 구체적 함수를 다룬다. 이 선언은 .ino 맨 하단에 정의한다.
    .ino
        핀 정의는 이곳에서 한다.
        class의 생성도 여기서 한다.
        
