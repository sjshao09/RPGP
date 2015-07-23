#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <cstdlib>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

using std::cout;
using std::endl;

// Population and Problem Size
const size_t LowLevelPopulation = 64;
const size_t UpLevelPopulation = 64;
const size_t LowLevelSize = 5;
const size_t UpLevelSize = 5;

// Number of Cycles
const size_t N = 1024 * 1024 * 31;

void LowLevelSelectTestbench(const std::string &OutFileName) {

	// Allocating Memory
	float * LOW_IN = new float[N * LowLevelSize];
	float * UP_IN = new float[N * UpLevelSize];
	short * Addr_IN = new short[N];
	short * isNew_IN = new short[N];
	
	float * LOW_OUT = new float[N * LowLevelSize];
	float * Can_OUT = new float[N * LowLevelSize];
	float * Fitness_OUT = new float[N];
	
	int BRAMOffset=-1;
	
	// Creating Input
	for (size_t i=0; i<N*LowLevelSize; i++) {
		LOW_IN[i] = rand() % 100;  // [0, 99]
		UP_IN[i] = 0;
	}
	for (size_t i=0; i<N; i++) {
		Addr_IN[i] = (i<LowLevelPopulation) ? i : rand() % LowLevelPopulation;
		isNew_IN[i] = i<64; // isNew = 1 for Cycle #0 ~ Cycle #63 
	}
	
	// Run DFE

	cout<<endl<<"----------------------------------------------------"<<endl;
	cout<<"Running DFE..."<<endl;
	
	//PGP(N, &BRAMOffset, Addr_IN, LOW_IN, UP_IN, isNew_IN, Can_OUT, Fitness_OUT, LOW_OUT);
	
  	cout<<endl<<"----------------------------------------------------"<<endl;

	// Open Output File
	std::fstream OutFile;
	OutFile.open (OutFileName.c_str(), std::fstream::out);
	
	OutFile<<"--------------------------RESULTS--------------------------"<<endl;
	OutFile<<"BRAM Offset = "<<BRAMOffset<<endl;
	//OutFile<<"Val To BRAM Offet = "<<ValToBRAMOffset<<endl;
	OutFile<<"----------------------------------------------------"<<endl;
	
	float sum = 0;
	for (size_t i=0; i<N; i++) {
		float cur = LOW_IN[i*5+0]+LOW_IN[i*5+1]+LOW_IN[i*5+2]+LOW_IN[i*5+3]+LOW_IN[i*5+4];
		sum = (cur>sum) ? cur : sum;
		OutFile<<"Tick = "<<std::setw(3)<<i;
		OutFile<<" | IN = "<<std::setw(3)<<(int)LOW_IN[i*5+0]<<" "<<(int)LOW_IN[i*5+1]<<" "<<(int)LOW_IN[i*5+2]<<" "<<(int)LOW_IN[i*5+3]<<" "<<(int)LOW_IN[i*5+4];
		OutFile<<" ==> "<<std::setw(3)<<(int)(cur);
		OutFile<<" | OUT = "<<std::setw(3)<<(int)Can_OUT[i*5+0]<<" "<<(int)Can_OUT[i*5+1]<<" "<<(int)Can_OUT[i*5+2]<<" "<<(int)Can_OUT[i*5+3]<<" "<<(int)Can_OUT[i*5+4];
		OutFile<<" | Fit = "<<std::setw(3)<<(int)Fitness_OUT[i]<<endl;
	}
	
	OutFile<<"--------------------------CHECK--------------------------"<<endl;
	OutFile<<"Last 'Best Fitness Output'  = "<<(int)Fitness_OUT[N-1]<<endl;
	OutFile<<"Actual Best Fitness = "<<(int)sum<<endl;
	OutFile<<"----------------------------------------------------"<<endl;
	
	OutFile.close();
	
	// Cleaning up
	delete LOW_IN; delete UP_IN; delete Addr_IN; delete isNew_IN; 
	delete LOW_OUT; delete Can_OUT; delete Fitness_OUT;

}


void LowLevelHubTestbench() {

	// Allocating Memory
	float * RAND_IN = new float[N * LowLevelSize];
	float * UP_IN = new float[N * UpLevelSize];
	short * ADDR1_IN = new short[N];
	short * ADDR2_IN = new short[N];
	short * ADDR3_IN = new short[N];
	short * ADDR4_IN = new short[N];
	short * TICK_OUT = new short[N];
	
	
	// Creating Input
	for (size_t i=0; i<N*LowLevelSize; i++) {
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX)  / 10;
		float sgn = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		RAND_IN[i] = (sgn > 0.5) ? r : -r; // [-1,1]
		UP_IN[i] = 1;
	}
	for (size_t i=0; i<N; i++) {
		ADDR1_IN[i] = rand() % LowLevelPopulation;
		ADDR2_IN[i] = rand() % LowLevelPopulation;
		ADDR3_IN[i] = rand() % LowLevelPopulation;
		ADDR4_IN[i] = rand() % LowLevelPopulation;
	}
	
	// Run DFE

	cout<<endl<<"----------------------------------------------------"<<endl;
	cout<<"Running DFE..."<<endl;
	
	//PGP(N, ADDR1_IN, ADDR2_IN, ADDR3_IN, ADDR4_IN, RAND_IN, UP_IN, TICK_OUT);
	
	cout<<endl<<"----------------------------------------------------"<<endl;

	
	// Cleaning up
	delete RAND_IN; delete UP_IN; delete TICK_OUT;
	delete ADDR1_IN; delete ADDR2_IN; delete ADDR3_IN; delete ADDR4_IN; 

}


void RecursivePGP() {

	float * FIT_OUT = new float[N];

	float LowEps = 0.005;
	float UpEps = 0.01;
	float Sigma = 0.4;

	// Run DFE
	cout<<endl<<"---- Running DFE ----"<<endl;
	
	struct timeval tv1, tv2;
	gettimeofday(&tv1, NULL);
	PGP(LowEps, N, Sigma, UpEps, FIT_OUT);
	gettimeofday(&tv2, NULL);
	double RunTime = ((tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec)/(double)1E6);


/*	
	cout<<"Loading DFE..."<<endl;
	max_file_t *MaxFile = PGP_init();
	max_engine_t *MaxDFE = max_load(MaxFile, "*");
	
	cout<<"Running DFE..."<<endl;
	max_actions_t *Action = max_actions_init(MaxFile, NULL);
	max_set_ticks(Action, "H1", N);	
	max_run(MaxDFE, Action);
	max_actions_free(Action);

	cout<<"Unloading DFE..."<<endl;	
	max_unload(MaxDFE);
	max_file_free(MaxFile);
*/	
	cout<<endl<<"---- Check Result ----"<<endl;

	for (size_t i=1024; i<N; i++) {
		if (FIT_OUT[i]<UpEps&&FIT_OUT[i]>0) {cout<<"[Cycle "<<i<<"] FIT_OUT = "<<FIT_OUT[i]<<endl;break;}
	}

	cout<<endl<<"---- Run Time = "<<RunTime<<"s ----"<<endl;
	
	delete FIT_OUT;

}


int main() {

	srand (static_cast <unsigned> (time(0)));

	RecursivePGP();

	return 0;
}

