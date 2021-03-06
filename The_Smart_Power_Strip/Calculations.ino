//Methods for power calculations


//Samples the analog inputs <sampleNumber> times -----------------
void sample() 
{ 
  int analogReadState[2][sampleNumber]; // Stores the sampled vectors of the analog pins
  memset(activePower,0,analogLayoutLength-1);

  //long start,stopp;
  for(int x = 0; x <= 3; x++) { 
    for(int i = 0; i <= sampleNumber-1 ; i++)
    {
      //      if(i==0)
      //      start=micros();
      // Read the analog pins and store them in analogReadstate
      analogReadState[0][i] = analogRead(analogLayout[4]);
      analogReadState[1][i] = analogRead(analogLayout[x]);

      delayMicroseconds(107+333);
      //      if(i==0)
      //      stopp=micros();
    }
    //Serial << "Time:" << stopp-start << endl;
    //if(x==3)
    delay(10);
    //    for(int m=0;m<sampleNumber;m++){
    //      Serial << analogReadState[1][m] << " ";
    //    }
    //    Serial << endl;
    //    for(int m=0;m<sampleNumber;m++){
    //      Serial << analogReadState[1][m] << " ";
    //    }
    //    Serial << endl;
    //Serial << freeMemory();

    //    int medel=0;
    //    for(int k=0;k<sampleNumber;k++){
    //      medel=medel+analogReadState[1][k];
    //    }
    //    Serial << "Medel:" << medel/sampleNumber << endl;
    powerCalc(analogReadState, x);
    //    medel=0;
    //    for(int k=0;k<sampleNumber;k++){
    //      medel=medel+analogReadState[1][k];
    //    }
    //    Serial << "Medel:" << medel/sampleNumber << endl;
  }

}

void powerCalc (int analogReadState[][60], int socket) {

  double RMS[2]={
    0,0        };
  double squaredSum[2]={
    0,0        };
  float phaseDiff;
  double powertemp, costemp;
  long offset=0;
  for(int k=0;k<sampleNumber;k++){
    offset=offset+analogReadState[1][k];
  }
  offset=offset/sampleNumber;
  //Serial << freeMemory();
  for (int i=0; i<2;i++){
    for(int j=0; j<sampleNumber; j++){
      if(i==1){
        analogReadState[i][j]=analogReadState[i][j]-offset;

      }
      squaredSum[i]=squaredSum[i] + pow((analogReadState[i][j]),2);
    }
    if(i!=0){
      RMS[i]=sqrt(squaredSum[i]/sampleNumber)*4.88/0.185/1000;
      if(RMS[i]>=0.55)
        RMS[i]=RMS[i]*1.048;
      else
        RMS[i]=RMS[i]*(0.2696*RMS[i]+0.894);
    }
    else
      RMS[i]=sqrt(squaredSum[i]/sampleNumber)*4.88*0.221494;
  }
  phaseDiff = phaseDifference(analogReadState);
  //Serial << phaseDiff << endl;
  //Serial << "Phase_DFT: " << DFT_phase(analogReadState) << endl;
  costemp=cos(phaseDiff);
  powertemp=RMS[0]*RMS[1];//*costemp;
  //Serial << "Socket: " << socket << " Current: " << RMS[1] << " Voltage: " << RMS[0] << " Power factor: " << cos(phaseDiff) << endl;
  //Serial << "DFT_phase_diff: " << cos(DFT_phase(analogReadState)) << endl;
  //Serial << "Phase Philgorithm: " << cos(phaseDiff) << endl;
  if(RMS[1]>=0.055)// && costemp>=0.7)
    activePower[socket]=powertemp;
  else
    activePower[socket]=0;
  //Serial << "Mem:" << freeMemory() << endl;











  // The DFT phase differance algorithm---------------------------------------------------------------------------------------------------------
}

double DFT_phase(int analogReadState[][60]){
  //Serial << "K: " << k << endl;
  double N = 60; // Number of samples
  double twopi = 6.2831853;
  double phase_v, phase_c, C, S;
  double R = 0;
  double I = 0;
  double DFT_sample_rate = 1500; // Sample rate
  double r_n = 0;
  double i_n = 0;
  double k = 50*N/DFT_sample_rate; 
  double t = 0;
  double r[60];
  double ans = 0;
  
  for (int n = 0; n < N; n++) {
    t =  n/DFT_sample_rate;
    r[n] = 450*cos(twopi*50*t);
    //Serial << r[n] << " ";
  }  

  for(int n=0; n < N; n++){
    //r_n = analogReadState[0][n];
    i_n = analogReadState[0][n];
    //Serial << endl << "Spänning: " << i_n;
    C = cos(twopi*n*k/N);
    S = sin(twopi*n*k/N);

    R = R + r[n]*C + i_n*S;
    I = I + i_n*C - r[n]*S;

  }
  
  phase_v = atan2(I, R);
  
  R = 0;
  I = 0;
  
  
  for(int n=0; n < N; n++){
    //r_n = analogReadState[0][n];
    i_n = analogReadState[1][n];
    C = cos(twopi*n*k/N); 
    S = sin(twopi*n*k/N);

    R = R + r[n]*C + i_n*S;
    I = I + i_n*C - r[n]*S;

  }
  phase_c = atan2(I, R);
  //Serial << "Phase Differance phase_v - phase_c: " << (phase_v - phase_c) << endl;
  return (phase_v-phase_c);
}
//}

// The DFT phase differance algorithm---------------------------------------------------------------------------------------------------------









// Phase Differnece -----------------------------------------------
float phaseDifference(int analogReadState[][60]){//int analogReadState[][]){
  int highestVoltageSampleNumber; //Tells on which sample the highest value of the voltage is located
  int firstzeroCrossing, secondzeroCrossing;
  float sampleDiff;
  float phaseDiff;
  for(int y=0; y<2; y++){
    if(y == 0){
      firstzeroCrossing = findZero(5, analogReadState[y]);//Find first zero crossing in the voltage vecor with guidance from the given sample index (50 in this case )
      secondzeroCrossing = findZero((firstzeroCrossing + 15), analogReadState[y]); // the findzeroCrossing(argument 1) should be close to the end of 0----->T as guidance so 40 is probably wrong... 
      highestVoltageSampleNumber = maximumValue(firstzeroCrossing, secondzeroCrossing, analogReadState[y]); // Load the highest voltage sample
    }
    if(y!=0){
      sampleDiff =  maximumValue(firstzeroCrossing, secondzeroCrossing, analogReadState[y]) - highestVoltageSampleNumber; //(May be positive or negative)
      // Serial << "Diff: " << sampleDiff << endl;
      phaseDiff=3.1415*(sampleDiff/30);  //Algorithm for converting samples to degrees
      //Serial << phaseDiff << endl;
    }
  }
  //Serial << "Mem:Phase:" << freeMemory() << endl;
  return phaseDiff;
}
// Phase Difference -----------------------------------------------


// Fuction for finding zero in an array --------------------------- (Part of the Phase Differance calculations)
int findZero(int init, int array[]){
  int lastValue = array[init];
  for (int i=init; i<sampleNumber; i++){
    if(((lastValue > 0) && (array[i] < 0)) || ((lastValue < 0) && (array[i] > 0)) || array[i] == 0){
      //Serial << "I: " << i << endl;
      return i;
    }
  }
}
// Fuction for finding zero in an array --------------------------- (Part of the Phase Differance calculations)


//Function to find highest (maximum) value in array --------------- (Part of the Phase Differance calculations)
int maximumValue(int start, int stop_, int array[])
{ 
  int maxindex;  
  int max = array[start];       // start with max = first element
  for(int i = start; i<stop_; i++)
  {
    if(array[i] > max){
      max = array[i];
      maxindex=i;
    }
  }
  //Serial << "Max: " << maxindex << endl;
  return maxindex;                // return highest value in array
}

//Function to find highest (maximum) value in array --------------- (Part of the Phase Differance calculations)









