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
//      if(i==59)
//      stopp=micros();
    }
    //Serial << "Time:" << stopp-start << endl;
    //if(x==3)
    delay(10);
//    for(int m=0;m<sampleNumber;m++){
//      Serial << analogReadState[0][m] << " ";
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
    0,0  };
  double squaredSum[2]={
    0,0  };
  double phaseDiff;
  int offset;
  for(int k=0;k<sampleNumber;k++){
    offset=offset+analogReadState[1][k];
  }
  offset=offset/sampleNumber;
  //Serial << freeMemory();
  for (int i=0; i<2;i++){

    for(int j=0; j<sampleNumber; j++){
      if(i==1)
      analogReadState[i][j]=analogReadState[i][j]-offset;
      
      squaredSum[i]=squaredSum[i] + pow((analogReadState[i][j]),2);
    }
    if(i!=0)
      RMS[i]=sqrt(squaredSum[i]/sampleNumber)*4.88/0.185/1000;
    else
      RMS[i]=sqrt(squaredSum[i]/sampleNumber)*4.88/1000*195.2;
  }
  phaseDiff = phaseDifference(analogReadState);
  Serial << phaseDiff << endl;
  activePower[socket]=RMS[0]*RMS[1];//*cos(phaseDiff);
  //Serial << "Mem:" << freeMemory() << endl;
}

// Phase Differnece -----------------------------------------------
double phaseDifference(int analogReadState[][60]){//int analogReadState[][]){
  int highestVoltageSampleNumber; //Tells on which sample the highest value of the voltage is located
  int firstzeroCrossing, secondzeroCrossing, sampleDiff;
  double phaseDiff;
  for(int y=0; y<2; y++){
    if(y == 0){
      firstzeroCrossing = findZero(5, analogReadState[y]);//Find first zero crossing in the voltage vecor with guidance from the given sample index (50 in this case )
      secondzeroCrossing = findZero((firstzeroCrossing + 15), analogReadState[y]); // the findzeroCrossing(argument 1) should be close to the end of 0----->T as guidance so 40 is probably wrong... 
      highestVoltageSampleNumber = maximumValue(firstzeroCrossing, secondzeroCrossing, analogReadState[y]); // Load the highest voltage sample
    }
    if(y!=0){
      sampleDiff =  maximumValue(firstzeroCrossing, secondzeroCrossing, analogReadState[y]) - highestVoltageSampleNumber; //(May be positive or negative)
      Serial << "Diff: " << sampleDiff << endl;
      phaseDiff = sampleDiff/sampleRate/0.02*360;  //Algorithm for converting samples to degrees
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
      Serial << "I: " << i << endl;
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
  Serial << "Max: " << maxindex << endl;
  return maxindex;                // return highest value in array
}

//Function to find highest (maximum) value in array --------------- (Part of the Phase Differance calculations)



