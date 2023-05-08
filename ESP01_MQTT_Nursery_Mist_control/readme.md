mqtt topic
    Argument/NR/mist/dura
    Argument/NR/mist/freq
    Argument/NR/mist/motor
    Argument/NR/mist/auto

<schematic process>

callback에서
    isAuto가 1이 되면 lastwatering = millis()

루프 중에서
    만약 자동인가?
    예:
        지금시각 = now-lastwatering
        만약 지금시각이 관수주기(peri * 60000) 안에 있나?
        예:
            만약 지금시각이 1회관수시간(dura * 1000) 안에 있나?
            예:
                모터on
            아니오:
                모터off
        아니오: (관수주기 밖에 있다!
                 새롭게 관수 주기를 리셋해야 한다.)
            lastwatering = millis() //이것이 의미가 있으려면 루프밖에서 
                                    //isAuto가 참이 될 때 세트헤야 한다.
    아니오: (수동이다)
        만약 motor가 on 인가?
        예:
            모터on
        아니오:
            모터off
루프 중이다.

