// 244101069_digitrecognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <Windows.h>
#include <stdio.h>
#include <direct.h> // For _mkdir
#include <errno.h>

#include<iostream>
using namespace std;

#define Cr 10000000

#pragma comment(lib, "winmm.lib")
char obFile[200];

#define MAX_SIZE 300
#define ORDER 12
long double Ci[MAX_SIZE][ORDER];

// Signal Variables & functions
char rollNo[] = "244101069";
char inputFile[200];
char outputFolder[200];
char ciFile[200];
long double signal[100000];
int samples = 0;
int fr = 0;
#define FRAME_SIZE 320
long double frames[MAX_SIZE][FRAME_SIZE];
long double energy[MAX_SIZE];
int mark[MAX_SIZE];

#define N 5
#define M 32
#include "printscan.c"

void dc_shift(){
	long double sum  = 0;
	for(int i=0;i<samples;i++){
		sum+=signal[i];
	}
	long double shift = sum/samples;
	for(int i=0;i<samples;i++){
		signal[i]-=shift;
	}
}

void normalization(){
	long double min = 0;
	long double max = 0;
	for(int i=0;i<samples;i++){
		if(signal[i]>max) max = signal[i];
		if(signal[i]<min) min = signal[i];
	}
	for(int i=0;i<samples;i++){
		signal[i] = -5000 + (((signal[i]-min)/(max-min))*10000);
	}
}


void frame_energy(){
	for(int i=0;i<fr;i++){
		energy[i] = 0;
		for(int j=0;j<FRAME_SIZE;j++){
			energy[i]+=frames[i][j]*frames[i][j];
		}
	}
	int start = energy[0];
	int cnt = 1;
	for(int i=1;i<fr;i++){
		mark[i] = cnt;
		if(energy[i]<=1.3*start && energy[i-1]>1.5*start){
			cnt++;
		}
		// cout<<mark[i]<<endl;
	}
}

void find_steady_frames(){
	for(int i=0;i<fr;i++){
		long double en = 0;
		for(int j=0;j<FRAME_SIZE;j++){
			en+=(frames[i][j]*frames[i][j]);
		}
		energy[i] = en;
	}
	long double max = 0;
	for(int i=0;i<fr;i++){
		if(energy[i]>max){
			max = energy[i];
		}
	}
	int f = 0, l = fr;
	int i;
	for(i=0;i<fr;i++){
		if(energy[i]>max*0.1){
			f = i-1;
			break;
		}
	}
	i+=20;
	for(;i<fr;i++){
		if(energy[i]<max*0.1){
			l = i;
			break;
		}
	}
	int cnt = 0,t=0;
	for(t=f;t<=l;t++){
		for(int j=0;j<FRAME_SIZE;j++){
			frames[cnt][j] = frames[t][j];
		}
		cnt++;
	}
	for(t=cnt;t<MAX_SIZE;t++){
		for(int j=0;j<FRAME_SIZE;j++){
			frames[t][j] = 0;
		}
	}
	fr = cnt;
	for(int i=0;i<MAX_SIZE;i++){
		if(i>=fr) {energy[i] = 0;continue;}
		long double er = 0;
		for(int j=0;j<FRAME_SIZE;j++){
			er+=(frames[i][j]*frames[i][j]);
		}
		energy[i] = er;
	}
}

void digit_framing(int digit=0,int utterance=0){
	sprintf(inputFile, "Dataset/%s/%d/%d_%d/%d_%d.txt", "numbers", digit, digit, utterance, digit, utterance);
    sprintf(outputFolder, "Dataset/%s/%d/%d_%d", "numbers", digit, digit, utterance);
    sprintf(ciFile, "Dataset/%s/%d/%d_%d/ci.txt", "numbers", digit, digit, utterance);
	read_signal(signal,inputFile,&samples);
	dc_shift();
	normalization();
	read_frames(frames,signal,&fr);
	// find_steady_frames();
}
void letter_framing(char alpha='A',int utterance=0){
	sprintf(inputFile, "Dataset/%s/%c/%c_%d/%c_%d.txt", "alphabets", alpha, alpha, utterance, alpha, utterance);
    sprintf(outputFolder, "Dataset/%s/%c/%c_%d", "alphabets", alpha, alpha, utterance);
    sprintf(ciFile, "Dataset/%s/%c/%c_%d/ci.txt", "alphabets", alpha, alpha, utterance);
	read_signal(signal,inputFile,&samples);
	dc_shift();
	normalization();
	read_frames(frames,signal,&fr);
	// find_steady_frames();
}
void command_framing(const char *command,int utterance=0){
	sprintf(inputFile, "Dataset/%s/%s/%s_%d/%s_%d.txt", "commands", command, command, utterance, command, utterance);
    sprintf(outputFolder, "Dataset/%s/%s/%s_%d", "commands", command, command, utterance);
    sprintf(ciFile, "Dataset/%s/%s/%s_%d/ci.txt", "commands", command, command, utterance);
	read_signal(signal,inputFile,&samples);
	dc_shift();
	normalization();
	read_frames(frames,signal,&fr);
	// find_steady_frames();
}



// Finding Cofficients
long double Ri[MAX_SIZE][ORDER+1];
long double Ai[MAX_SIZE][ORDER];
#define PI_VAL 3.142857142857

void calculate_ri(){
	for(int f=0;f<fr;f++){
		for(int i=0;i<=ORDER;i++){
			long double r = 0;
			for(int j=0;j<FRAME_SIZE-i;j++){
				r+=(frames[f][j]*frames[f][j+i]);
			}
			Ri[f][i] = r;
		}
	}
}

void calculate_ai() {
	for(int f=0;f<fr;f++){
		long double E[ORDER+1];
		long double Alpha[ORDER+1][ORDER+1];
		long double k[ORDER+1];

		E[0] = Ri[f][0];
		for(int i=1;i<=ORDER;i++) {
			long double sum = 0.0;

			for(int j=0;j<=i-1;j++) {
				sum += Alpha[i-1][j] * Ri[f][i-j];
			}

			k[i] = (Ri[f][i] - sum) / E[i-1];

			Alpha[i][i] =  k[i];

			for(int j=1;j<=i-1;j++) {
				Alpha[i][j] = Alpha[i-1][j] - k[i] * Alpha[i-1][i-j];
			}

			E[i] = (1 - k[i] * k[i]) * E[i-1];
		}
		int cnt = 0;
		for(int i=1;i<=ORDER;i++) {
			Ai[f][cnt] = Alpha[ORDER][i];
			cnt++;
		}
	}
}

void apply_hamming_window(){
	for(int f=0;f<fr;f++){
		for (int n=0;n<FRAME_SIZE;n++) {
			frames[f][n]*=(0.54-0.46*cos(2*PI_VAL*n/(FRAME_SIZE-1)));
		}
	}
}

void calculate_ci(){
	for(int f=0;f<fr;f++){
		for(int m=1;m<=ORDER;m++) {
			Ci[f][m-1] = Ai[f][m-1];
			long double sum = 0;
			for (int k=1;k<m;k++) {
				sum += k*Ai[f][m-1-k]*Ci[f][k];
			}
			Ci[f][m-1] += sum / m;
		}
	}	
}

void apply_raised_sine_window(){
	for(int f=0;f<fr;f++){
		for(int i=0;i<ORDER;i++){
			Ci[f][i]*=(1+(ORDER/2)*sin(PI_VAL*(i+1)/ORDER));
		}
	}
}

void calculateAiCi(){
	apply_hamming_window();
	calculate_ri();
	calculate_ai();
	calculate_ci();
	apply_raised_sine_window();
	save_ci();
}

void framing_ci(){
	for(int i=0;i<=9;i++){
		for(int j=1;j<=50;j++){
			digit_framing(i,j);
			calculateAiCi();
		}
	}
	for(char i='A';i<='H';i++){
		for(int j=1;j<=50;j++){
			letter_framing(i,j);
			calculateAiCi();
		}
	}
	for(int j=1;j<=50;j++){
		command_framing("to",j);
		calculateAiCi();
		command_framing("move",j);
		calculateAiCi();
		command_framing("undo",j);
		calculateAiCi();
	}
}

#include "vectorQuantization.cpp"

void vector_quantization(char *type){
	load_universe(type);
    initialize_codebook(type);
	while (k < M) {
		int cnt = 0;
		prev_avg_distortion = 0;
		for (int i = 0; i < k && cnt < M; i++) {
			for (int j = 0; j < P; j++) {
				NewCodebook[cnt][j] = Codebook[i][j] * (1 + e);
			}
			cnt++;
			for (int j = 0; j < P; j++) {
				NewCodebook[cnt][j] = Codebook[i][j] * (1 - e);
			}
			cnt++;
		}
		for (int i = 0; i < cnt; i++) {
			for (int j = 0; j < P; j++) {
				Codebook[i][j] = NewCodebook[i][j];
			}
		}
		k = cnt;
		k_means_algorithm(k);
	}
	printf("Rows: %d\n",rows);
	printf("Total No of Iterations: %d\n",m);
	printf("Average Distortion %e\n",prev_avg_distortion);
    printCodebook(type);
}

void VQ(){
	prev_avg_distortion = 0;m=0;k=1;
	vector_quantization("alphabet");
	prev_avg_distortion = 0;m=0;k=1;
	vector_quantization("number");
	prev_avg_distortion = 0;m=0;k=1;
	vector_quantization("command");
}
void create_observation_sequence(char *type,int digit=0,int utterance=0){
	if(!strcmp(type,"alphabet")){
		sprintf(cbFile, "Dataset/%s/codebook.txt", "alphabets");
		sprintf(ciFile, "Dataset/%s/%c/%c_%d/ci.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
		sprintf(obFile, "Dataset/%s/%c/%c_%d/OB.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
	}
	if(!strcmp(type,"number")){
		sprintf(cbFile, "Dataset/%s/codebook.txt", "numbers");
		sprintf(ciFile, "Dataset/%s/%d/%d_%d/ci.txt", "numbers", digit, digit, utterance);
		sprintf(obFile, "Dataset/%s/%d/%d_%d/OB.txt", "numbers", digit, digit, utterance);
	}
	if(!strcmp(type,"command")){
		sprintf(cbFile, "Dataset/%s/codebook.txt", "commands");
		sprintf(ciFile, "Dataset/%s/%s/%s_%d/ci.txt", "commands", cmds[digit], cmds[digit], utterance);
		sprintf(obFile, "Dataset/%s/%s/%s_%d/OB.txt", "commands", cmds[digit], cmds[digit], utterance);
	}
	FILE* file = fopen(ciFile, "r");
	if (file == NULL) {
		printf("File Not Open");
		exit(0);
	}
	int r=0,c=0;
	while(r<MAX_SIZE && fscanf(file,"%Lf",&Ci[r][c])!=EOF){
		c++;
		if(c==ORDER){
			c=0;
			r++;
		}
	}
	fclose(file);
	file = fopen(cbFile, "r");
	if (file == NULL) {
		printf("File Not Open");
		exit(0);
	}
	int r1=0,c1=0;
	while(r<MAX_SIZE && fscanf(file,"%Lf",&Codebook[r1][c1])!=EOF){
		c1++;
		if(c1==ORDER){
			c1=0;
			r1++;
		}
	}
	fclose(file);
	file = fopen(obFile, "w");
	if (file == NULL) {
		printf("File Not Open");
		exit(0);
	}
	for(int i=0;i<r;i++){
		long double min = obsdist(Codebook,i,0);
		int idx = 0;
		for(int j=1;j<r1;j++){
			long double dist = obsdist(Codebook,i,j);
			if(dist<min){
				min = dist;
				idx = j;
			}
		}
		fprintf(file,"%d ",idx);
	}
	fclose(file);
	printf("Created for Digit %d utterance %d\n",digit,utterance);
}

void create_OB(){
	for(int digit=0;digit<=9;digit++){
		for(int utterance=1;utterance<=50;utterance++){
			create_observation_sequence("number",digit,utterance);
		}
	}
	for(int digit=0;digit<=7;digit++){
		for(int utterance=1;utterance<=50;utterance++){
			create_observation_sequence("alphabet",digit,utterance);
		}
	}
	for(int digit=0;digit<=2;digit++){
		for(int utterance=1;utterance<=50;utterance++){
			create_observation_sequence("command",digit,utterance);
		}
	}
}

#include "HMM.cpp"

void HMM_main(){
	for(int digit=0;digit<=9;digit++){
		for(int utterance=1;utterance<=50;utterance++){
			printf("Digit: %d, Utterance: %d\n\n",digit,utterance);
			HMM("number",digit,utterance);
		}
	}
	for(int digit=0;digit<=9;digit++){
		for(int i=0;i<N;i++){
			for(int j=0;j<N;j++){
				A[i][j] = 0;
			}
		}
		for(int i=0;i<N;i++){
			for(int j=0;j<M;j++){
				B[i][j] = 0;
			}
		}
		for(int i=0;i<1;i++){
			for(int j=0;j<N;j++){
				PI[i][j] = 0;
			}
		}
		average_models("number",digit);
		printf("Averaged for digit %d\n",digit);
	}
	for(int digit=0;digit<=7;digit++){
		for(int utterance=1;utterance<=50;utterance++){
			printf("Digit: %d, Utterance: %d\n\n",digit,utterance);
			HMM("alphabet",digit,utterance);
		}
	}
	for(int digit=0;digit<=7;digit++){
		for(int i=0;i<N;i++){
			for(int j=0;j<N;j++){
				A[i][j] = 0;
			}
		}
		for(int i=0;i<N;i++){
			for(int j=0;j<M;j++){
				B[i][j] = 0;
			}
		}
		for(int i=0;i<1;i++){
			for(int j=0;j<N;j++){
				PI[i][j] = 0;
			}
		}
		average_models("alphabet",digit);
		printf("Averaged for digit %d\n",digit);
	}
	for(int digit=0;digit<=2;digit++){
		for(int utterance=1;utterance<=50;utterance++){
			printf("Digit: %d, Utterance: %d\n\n",digit,utterance);
			HMM("command",digit,utterance);
		}
	}
	for(int digit=0;digit<=2;digit++){
		for(int i=0;i<N;i++){
			for(int j=0;j<N;j++){
				A[i][j] = 0;
			}
		}
		for(int i=0;i<N;i++){
			for(int j=0;j<M;j++){
				B[i][j] = 0;
			}
		}
		for(int i=0;i<1;i++){
			for(int j=0;j<N;j++){
				PI[i][j] = 0;
			}
		}
		average_models("command",digit);
		printf("Averaged for digit %d\n",digit);
	}
}

void looping(){
	for(int digit=0;digit<=9;digit++){
		sprintf(Ainput, "Dataset/%s/%d/A.txt", "numbers", digit);
		sprintf(Binput, "Dataset/%s/%d/B.txt", "numbers", digit);
		sprintf(PIinput, "Dataset/%s/%d/PI.txt", "numbers", digit);
		FILE* file = fopen(Ainput,"r");
		for(int i=0;i<N;i++){
			for(int j=0;j<N;j++){
				fscanf(file,"%Lf",&A[i][j]);
			}
		}
		fclose(file);
		file = fopen(Binput,"r");
		for(int i=0;i<N;i++){
			for(int j=0;j<M;j++){
				fscanf(file,"%Lf",&B[i][j]);
			}
		}
		fclose(file);
		file = fopen(PIinput,"r");
		for(int i=0;i<1;i++){
			for(int j=0;j<N;j++){
				fscanf(file,"%Lf",&PI[i][j]);
			}
		}
		fclose(file);
		for(int utterance=1;utterance<=50;utterance++){
			sprintf(Ainput, "Dataset/%s/%d/%d_%d/A.txt", "numbers", digit, digit, utterance);
			sprintf(Binput, "Dataset/%s/%d/%d_%d/B.txt", "numbers", digit, digit, utterance);
			sprintf(PIinput, "Dataset/%s/%d/%d_%d/PI.txt", "numbers", digit, digit, utterance);
			file = fopen(Ainput,"w");
			for(int i=0;i<N;i++){
				for(int j=0;j<N;j++){
					fprintf(file,"%e ",A[i][j]);
				}
				fprintf(file,"\n");
			}
			fclose(file);
			file = fopen(Binput,"w");
			for(int i=0;i<N;i++){
				for(int j=0;j<M;j++){
					fprintf(file,"%e ",B[i][j]);
				}
				fprintf(file,"\n");
			}
			fclose(file);
			file = fopen(PIinput,"w");
			for(int i=0;i<1;i++){
				for(int j=0;j<N;j++){
					fprintf(file,"%e ",PI[i][j]);
				}
				fprintf(file,"\n");
			}
			fclose(file);
		}
	}


	for(int digit=0;digit<=7;digit++){
		sprintf(Ainput, "Dataset/%s/%c/A.txt", "alphabets", 'A'+digit);
		sprintf(Binput, "Dataset/%s/%c/B.txt", "alphabets", 'A'+digit);
		sprintf(PIinput, "Dataset/%s/%c/PI.txt", "alphabets", 'A'+digit);
		FILE* file = fopen(Ainput,"r");
		for(int i=0;i<N;i++){
			for(int j=0;j<N;j++){
				fscanf(file,"%Lf",&A[i][j]);
			}
		}
		fclose(file);
		file = fopen(Binput,"r");
		for(int i=0;i<N;i++){
			for(int j=0;j<M;j++){
				fscanf(file,"%Lf",&B[i][j]);
			}
		}
		fclose(file);
		file = fopen(PIinput,"r");
		for(int i=0;i<1;i++){
			for(int j=0;j<N;j++){
				fscanf(file,"%Lf",&PI[i][j]);
			}
		}
		fclose(file);
		for(int utterance=1;utterance<=50;utterance++){
			sprintf(Ainput, "Dataset/%s/%c/%c_%d/A.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
			sprintf(Binput, "Dataset/%s/%c/%c_%d/B.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
			sprintf(PIinput, "Dataset/%s/%c/%c_%d/PI.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
			file = fopen(Ainput,"w");
			for(int i=0;i<N;i++){
				for(int j=0;j<N;j++){
					fprintf(file,"%e ",A[i][j]);
				}
				fprintf(file,"\n");
			}
			fclose(file);
			file = fopen(Binput,"w");
			for(int i=0;i<N;i++){
				for(int j=0;j<M;j++){
					fprintf(file,"%e ",B[i][j]);
				}
				fprintf(file,"\n");
			}
			fclose(file);
			file = fopen(PIinput,"w");
			for(int i=0;i<1;i++){
				for(int j=0;j<N;j++){
					fprintf(file,"%e ",PI[i][j]);
				}
				fprintf(file,"\n");
			}
			fclose(file);
		}
	}

	for(int digit=0;digit<=2;digit++){
		sprintf(Ainput, "Dataset/%s/%s/A.txt", "commands", cmds[digit]);
		sprintf(Binput, "Dataset/%s/%s/B.txt", "commands", cmds[digit]);
		sprintf(PIinput, "Dataset/%s/%s/PI.txt", "commands", cmds[digit]);
		FILE* file = fopen(Ainput,"r");
		for(int i=0;i<N;i++){
			for(int j=0;j<N;j++){
				fscanf(file,"%Lf",&A[i][j]);
			}
		}
		fclose(file);
		file = fopen(Binput,"r");
		for(int i=0;i<N;i++){
			for(int j=0;j<M;j++){
				fscanf(file,"%Lf",&B[i][j]);
			}
		}
		fclose(file);
		file = fopen(PIinput,"r");
		for(int i=0;i<1;i++){
			for(int j=0;j<N;j++){
				fscanf(file,"%Lf",&PI[i][j]);
			}
		}
		fclose(file);
		for(int utterance=1;utterance<=50;utterance++){
			sprintf(Ainput, "Dataset/%s/%s/%s_%d/A.txt", "commands", cmds[digit], cmds[digit], utterance);
			sprintf(Binput, "Dataset/%s/%s/%s_%d/B.txt", "commands", cmds[digit], cmds[digit], utterance);
			sprintf(PIinput, "Dataset/%s/%s/%s_%d/PI.txt", "commands", cmds[digit], cmds[digit], utterance);
			file = fopen(Ainput,"w");
			for(int i=0;i<N;i++){
				for(int j=0;j<N;j++){
					fprintf(file,"%e ",A[i][j]);
				}
				fprintf(file,"\n");
			}
			fclose(file);
			file = fopen(Binput,"w");
			for(int i=0;i<N;i++){
				for(int j=0;j<M;j++){
					fprintf(file,"%e ",B[i][j]);
				}
				fprintf(file,"\n");
			}
			fclose(file);
			file = fopen(PIinput,"w");
			for(int i=0;i<1;i++){
				for(int j=0;j<N;j++){
					fprintf(file,"%e ",PI[i][j]);
				}
				fprintf(file,"\n");
			}
			fclose(file);
		}
	}
	
}
 #include "testing.cpp"
/*
int _tmain(int argc, _TCHAR* argv[])
{
	for(int digit=0;digit<=9;digit++){
		for(int utterance=1;utterance<=30;utterance++){
			framing(digit,utterance);
			calculateAiCi();
		}
	}
	k=1;
	vector_quantization();
	for(int digit=0;digit<=9;digit++){
		for(int utterance=1;utterance<=30;utterance++){
			create_observation_sequence(digit,utterance);
		}
	}
	for(int digit=0;digit<=9;digit++){
		for(int utterance=1;utterance<=30;utterance++){
			printf("Digit: %d, Utterance: %d\n\n",digit,utterance);
			HMM(digit,utterance);
		}
	}
	for(int digit=0;digit<=9;digit++){
		for(int i=0;i<N;i++){
			for(int j=0;j<N;j++){
				A[i][j] = 0;
			}
		}
		for(int i=0;i<N;i++){
			for(int j=0;j<M;j++){
				B[i][j] = 0;
			}
		}
		for(int i=0;i<1;i++){
			for(int j=0;j<N;j++){
				PI[i][j] = 0;
			}
		}
		average_models(digit);
		printf("Averaged for digit %d\n",digit);
	}
	int method = 0;
	printf("Enter Test Method: \n");
	printf("1.Test by Dataset\n");
	printf("2.Test by Mic\n");
	scanf("%d",&method);
	if(method==1){
		testing();
	}
	else if(method==2){
		test_by_mic();
	}
	else{
		printf("Invalid Input");
	}
	return 0;
}
*/
	