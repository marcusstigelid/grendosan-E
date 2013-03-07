//Methods for power calculations


//Samples the analog inputs <sampleNumber> times -----------------
void sample() 
{
  for(int i = 0; i <= sampleNumber-1 ; i++)
  {
    for(int x = 0; x <= analogLayoutLength-1; x++)
    { // Read the analog pins and store them in analogReadstate
      analogReadState[x][i] = analogRead(analogLayout[x]);
    }

    delay(1);
  }

}

void powerCalc () {

  int squaredSum[sampleNumber]={0,0,0,0,0};
  int RMS[sampleNumber-1]={0,0,0,0};
  int activePower[sampleNumber-1]={0,0,0,0};
  for (int i=0; i<=analogLayoutLength-1;i++){
    for(int j=0; j<=sampleNumber-1; j++){
      squaredSum[i]=squaredSum[i] + pow(analogReadState[i][j],2);
    }
    RMS[i]=sqrt(squaredSum[i]/sampleNumber);
  }
  //phaseDifference();
  
  for (int i=1; i<=analogLayoutLength-1;i++){
    activePower[i-1]=RMS[0]*RMS[i]*cos(phaseDiff[i-1]);
  }
}


// Fuction for finding zero in an array --------------------------- (Part of the Phase Differance calculations)
int findZero(int init, int array[]){
  int lastValue = array[init];
  for (int i=init; i<sampleNumber; i++){
    if(((lastValue > 0) && (array[i] < 0)) || ((lastValue < 0) && (array[i] > 0)) || array[i] == 0){
      return i;
    }
  }
}
// Fuction for finding zero in an array --------------------------- (Part of the Phase Differance calculations)


//Function to find highest (maximum) value in array --------------- (Part of the Phase Differance calculations)
int maximumValue(int start, int stop_, int array[])
{    
     int max = array[start];       // start with max = first element
     for(int i = start; i<stop_; i++)
     {
          if(array[i] > max){
                max = array[i];
          }
     }
     return max;                // return highest value in array
}

//Function to find highest (maximum) value in array --------------- (Part of the Phase Differance calculations)

// Phase Differnece -----------------------------------------------
void phaseDifference(){
  int highestVoltageSampleNumber; //Tells on which sample the highest value of the voltage is located
  int firstzeroCrossing, secondzeroCrossing;
  int local_vector[sampleNumber]; //Local vector used for calculations
  for(int y=0; y<analogLayoutLength; y++){
    for(int x=0; x<sampleNumber; x++){
      local_vector[x] = analogReadState[y][x]; // Store in the local variable
    }
    if(y=0){
      firstzeroCrossing = findZero(50, local_vector);//Find first zero crossing in the voltage vecor with guidance from the given sample index (50 in this case )
      secondzeroCrossing = findZero((firstzeroCrossing + 40), local_vector); // the findzeroCrossing(argument 1) should be close to the end of 0----->T as guidance so 40 is probably wrong... 
      highestVoltageSampleNumber = maximumValue(firstzeroCrossing, secondzeroCrossing, local_vector); // Load the highest voltage sample
    }
    if(y!=0){
       int sampleDiff =  maximumValue(firstzeroCrossing, secondzeroCrossing, local_vector) - highestVoltageSampleNumber; //(May be positive or negative)
       phaseDiff[y] = (((sampleDiff)*sampleRate)/0.02)*360;  //Algorithm for converting samples to degrees
    }
  }
}
// Phase Difference -----------------------------------------------