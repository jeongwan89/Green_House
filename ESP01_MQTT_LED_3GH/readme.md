# 주요 개요

3동에 설치된 LED를 켜는 프로그림이다.
MQTT프로토콜을 적용한다.
esp01-relay module을 사용한다. 
    주의 : gipo0가 릴레이 작동 출력신호인데 부신호로 작동한다.
            하지만 연결은 NO으로 한다. 작동이 안될 때, 전원이 이상할 때, 켜지면 안되기 때문이다. 대신 GPIO의 LOW HIGH의 반전으로 작동을 제어한다.
MQTT 토픽
    Actuator/GH3/LED/Lightall : 여기를 바로 직접 모니터하여,
                                Lightall가 1이면 작동(LOW), 0이면 정지(HIGH)
