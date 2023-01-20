unsigned int getPpmPWM(int _PIN){
  unsigned long th, tl;
  unsigned int ppm=0;
  
  th = (pulseIn(_PIN, HIGH,2500000)/1000); 
  ppm = 2000*(th-2)/1000;
  return ppm;
}
