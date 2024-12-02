#define N 5
#define T_MAX 300

char OBinput[200];
char Ainput[200];
char Binput[200];
char PIinput[200];

int T = 0;
long double A[N][N],B[N][M],PI[1][N],DELTA[T_MAX][N],ALPHA[T_MAX][N],BETA[T_MAX][N],XI[T_MAX][N][N],GAMMA[T_MAX][N];
int q_t_star[T_MAX],PSI[T_MAX][N];
int OB[T_MAX];

void normal_printing_function(){
	FILE* file = fopen(Ainput,"w");
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

void initialization(){
	FILE* file = fopen("Unbiased_Model/A.txt","r");
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			fscanf(file,"%Lf",&A[i][j]);
		}
	}
	fclose(file);
	file = fopen("Unbiased_Model/B.txt","r");
	for(int i=0;i<N;i++){
		for(int j=0;j<M;j++){
			fscanf(file,"%Lf",&B[i][j]);
		}
	}
	fclose(file);
	file = fopen("Unbiased_Model/PI.txt","r");
	for(int i=0;i<1;i++){
		for(int j=0;j<N;j++){
			fscanf(file,"%Lf",&PI[i][j]);
		}
	}
	fclose(file);
	for(int digit=0;digit<=9;digit++){
		for(int utterance=1;utterance<=50;utterance++){
			sprintf(Ainput, "Dataset/%s/%d/%d_%d/A.txt", "numbers", digit, digit, utterance);
			sprintf(Binput, "Dataset/%s/%d/%d_%d/B.txt", "numbers", digit, digit, utterance);
			sprintf(PIinput, "Dataset/%s/%d/%d_%d/PI.txt", "numbers", digit, digit, utterance);
			normal_printing_function();
		}
	}
	for(int digit=0;digit<=7;digit++){
		for(int utterance=1;utterance<=50;utterance++){
			sprintf(Ainput, "Dataset/%s/%c/%c_%d/A.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
			sprintf(Binput, "Dataset/%s/%c/%c_%d/B.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
			sprintf(PIinput, "Dataset/%s/%c/%c_%d/PI.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
			normal_printing_function();
		}
	}
	for(int digit=0;digit<=2;digit++){
		for(int utterance=1;utterance<=50;utterance++){
			sprintf(Ainput, "Dataset/%s/%s/%s_%d/A.txt", "commands", cmds[digit], cmds[digit], utterance);
			sprintf(Binput, "Dataset/%s/%s/%s_%d/B.txt", "commands", cmds[digit], cmds[digit], utterance);
			sprintf(PIinput, "Dataset/%s/%s/%s_%d/PI.txt", "commands", cmds[digit], cmds[digit], utterance);
			normal_printing_function();
		}
	}
}


long double forward_procedure(){
	// Initialization
	for(int i=0;i<N;i++){
		ALPHA[0][i] = PI[0][i]*B[i][OB[0]];
	}
	// Induction Step
	for(int t=1;t<T;t++){
		for(int j=0;j<N;j++){
			ALPHA[t][j]= 0;
			for(int i=0;i<N;i++){
				ALPHA[t][j] += ALPHA[t-1][i]*A[i][j]*B[j][OB[t]];
				if(ALPHA[t][j]<1e-250){
					ALPHA[t][j] = 1e-250;
				}
			}
		}
	}
	long double p = 0;
	for(int i=0;i<N;i++){
		p+=ALPHA[T-1][i];
	}
	return p;
}

void backward_procedure(){
	// Initialization
	for(int i=0;i<N;i++) BETA[T-1][i] = 1;
	// Induction step
	for(int t=T-2;t>=0;t--){
		for(int i=0;i<N;i++){
			BETA[t][i] = 0;
			for(int j=0;j<N;j++){
				BETA[t][i]+= A[i][j]*B[j][OB[t+1]]*BETA[t+1][j];
				if(BETA[t][i]<1e-250){
					BETA[t][i] = 1e-250;
				}
			}
		}
	}
}

long double problem1(){
	long double p = forward_procedure();
	backward_procedure();
	return p;
}

long double viterbi(){
	// Initialization
	for(int i=0;i<N;i++){
		DELTA[0][i] = PI[0][i]*B[i][OB[0]];
	}
	for(int i=0;i<N;i++){
		PSI[0][i] = -1;
	}
	// Induction Step
	for(int t=1;t<T;t++){
		for(int j=0;j<N;j++){
			int idx = 0;
			long double max = DELTA[t-1][0]*A[0][j];
			for(int i=1;i<N;i++){
				long double val =  DELTA[t-1][i]*A[i][j];
				if(val>max){
					max = val;
					idx = i;
				}
			}
			DELTA[t][j] = max*B[j][OB[t]];
			PSI[t][j] = idx;
		}
	}
	long double p_star = DELTA[T-1][0];	
	int idx = 0;
	for(int i=1;i<N;i++){
		if(p_star<DELTA[T-1][i]){
			p_star = DELTA[T-1][i];
			idx = i;
		}
	}
	q_t_star[T-1] = idx;
	for(int t=T-2;t>=0;t--){
		q_t_star[t] = PSI[t+1][q_t_star[t+1]];
	}
	return p_star;
}

long double problem2(){
	long double p_star = viterbi();
	return p_star;
}

void calculate_gamma(){
	for(int t=0;t<T;t++){
		long double sum = 0;
		for(int i=0;i<N;i++){
			sum+=(ALPHA[t][i]*BETA[t][i]);
		}
		for(int i=0;i<N;i++){
			GAMMA[t][i] = (ALPHA[t][i]*BETA[t][i])/sum;
		}
	}
}

void calculate_xi(){
	for(int t=0;t<T-1;t++){
		long double sum = 0;
		for(int i=0;i<N;i++){
			for(int j=0;j<N;j++){
				sum+=(ALPHA[t][i]*A[i][j]*B[j][OB[t+1]]*BETA[t+1][j]);
			}
		}
		for(int i=0;i<N;i++){
			for(int j=0;j<N;j++){
				XI[t][i][j] = (ALPHA[t][i]*A[i][j]*B[j][OB[t+1]]*BETA[t+1][j])/sum;
			}
		}
	}
}

void reestimate_pi(){
	for(int i=0;i<N;i++){
		PI[0][i] = GAMMA[0][i];
	}
}

void reestimate_a(){
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			long double xiSum = 0;
			long double gSum = 0;
			for(int t=0;t<T-1;t++){
				xiSum+=XI[t][i][j];
				gSum+=GAMMA[t][i];
			}
			A[i][j] = xiSum/gSum;
		}
	}
}

void reestimate_b(){
	for(int i=0;i<N;i++){
		for(int j=0;j<M;j++){
			long double num = 0;
			long double den = 0;
			for(int t=0;t<T;t++){
				if(OB[t]==j){
					num+=GAMMA[t][i];
				}
				den+=GAMMA[t][i];
			}
			B[i][j] = num/den;
			if(B[i][j]==0){
				B[i][j] = 1e-30;
			}
		}
		long double sum = 0;
		for(int j=0;j<M;j++){
			sum+=B[i][j];
		}
		for(int j=0;j<M;j++){
			B[i][j]/=sum;
		}
	}
}

void problem3(){
	calculate_gamma();
	calculate_xi();
	reestimate_pi();
	reestimate_a();
	reestimate_b();
}

void HMM(char* type,int digit,int utterance)
{
	if(!strcmp(type,"alphabet")) sprintf(OBinput, "Dataset/%s/%c/%c_%d/OB.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
	if(!(strcmp(type,"number"))) sprintf(OBinput, "Dataset/%s/%d/%d_%d/OB.txt", "numbers", digit, digit, utterance);
	if(!(strcmp(type,"command"))) sprintf(OBinput, "Dataset/%s/%s/%s_%d/OB.txt", "commands", cmds[digit], cmds[digit], utterance);
	FILE* file = fopen(OBinput,"r");
	T=0;
	while(fscanf(file,"%d",&OB[T])!=EOF && T<150){
		T++;
	}
	fclose(file);
	if(!strcmp(type,"alphabet")){
		sprintf(Ainput, "Dataset/%s/%c/%c_%d/A.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
		sprintf(Binput, "Dataset/%s/%c/%c_%d/B.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
		sprintf(PIinput, "Dataset/%s/%c/%c_%d/PI.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
	}
	if(!strcmp(type,"number")){
		sprintf(Ainput, "Dataset/%s/%d/%d_%d/A.txt", "numbers", digit, digit, utterance);
		sprintf(Binput, "Dataset/%s/%d/%d_%d/B.txt", "numbers", digit, digit, utterance);
		sprintf(PIinput, "Dataset/%s/%d/%d_%d/PI.txt", "numbers", digit, digit, utterance);
	}
	if(!strcmp(type,"command")){
		sprintf(Ainput, "Dataset/%s/%s/%s_%d/A.txt", "commands", cmds[digit], cmds[digit], utterance);
		sprintf(Binput, "Dataset/%s/%s/%s_%d/B.txt", "commands", cmds[digit], cmds[digit], utterance);
		sprintf(PIinput, "Dataset/%s/%s/%s_%d/PI.txt", "commands", cmds[digit], cmds[digit], utterance);
	}
	read_matrix(A,N,N,Ainput);
	READ_matrix(B,N,M,Binput);
	read_matrix(PI,1,N,PIinput);

	// long double p_prev = 1;
	
	for(int i=0;i<1000;i++){
		long double p_curr = problem1();
		long double p_star_curr = problem2();
		printf("Iteration: %d\n",i+1);
		// printf("P: %e\n",p_curr);
		printf("P_STAR_PROBABILITY: %e\n",p_star_curr);
		problem3();
	}

	/*for(int i=0;i<T;i++){
		printf("%d->",q_t_star[i]+1);
	}*/

	print_matrix_to_file(A,N,N,Ainput);
	PRINT_matrix_to_file(B,N,M,Binput);
	print_matrix_to_file(PI,1,N,PIinput);
}

void average_models(char* type,int digit){
	for(int utterance=1;utterance<=50;utterance++){
		if(!strcmp(type,"alphabet")){
			sprintf(Ainput, "Dataset/%s/%c/%c_%d/A.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
			sprintf(Binput, "Dataset/%s/%c/%c_%d/B.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
			sprintf(PIinput, "Dataset/%s/%c/%c_%d/PI.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
		}
		if(!strcmp(type,"number")){
			sprintf(Ainput, "Dataset/%s/%d/%d_%d/A.txt", "numbers", digit, digit, utterance);
			sprintf(Binput, "Dataset/%s/%d/%d_%d/B.txt", "numbers", digit, digit, utterance);
			sprintf(PIinput, "Dataset/%s/%d/%d_%d/PI.txt", "numbers", digit, digit, utterance);
		}
		if(!strcmp(type,"command")){
			sprintf(Ainput, "Dataset/%s/%s/%s_%d/A.txt", "commands", cmds[digit], cmds[digit], utterance);
			sprintf(Binput, "Dataset/%s/%s/%s_%d/B.txt", "commands", cmds[digit], cmds[digit], utterance);
			sprintf(PIinput, "Dataset/%s/%s/%s_%d/PI.txt", "commands", cmds[digit], cmds[digit], utterance);
		}
		FILE *file = fopen(Ainput,"r");
		for(int i=0;i<N;i++){
			for(int j=0;j<N;j++){
				long double val = 0;
				fscanf(file,"%Lf",&val);
				A[i][j]+=val;
			}
		}
		fclose(file);
		file = fopen(Binput,"r");
		for(int i=0;i<N;i++){
			for(int j=0;j<M;j++){
				long double val = 0;
				fscanf(file,"%Lf",&val);
				B[i][j]+=val;
			}
		}
		fclose(file);
		file = fopen(PIinput,"r");
		for(int i=0;i<1;i++){
			for(int j=0;j<N;j++){
				long double val = 0;
				fscanf(file,"%Lf",&val);
				PI[i][j]+=val;
			}
		}
		fclose(file);
	}
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			A[i][j]/=50;
		}
	}
	for(int i=0;i<N;i++){
		for(int j=0;j<M;j++){
			B[i][j]/=50;
		}
	}
	for(int i=0;i<1;i++){
		for(int j=0;j<N;j++){
			PI[i][j]/=50;
		}
	}
	if(!strcmp(type,"alphabet")){
		sprintf(Ainput, "Dataset/%s/%c/A.txt", "alphabets", 'A'+digit);
		sprintf(Binput, "Dataset/%s/%c/B.txt", "alphabets", 'A'+digit);
		sprintf(PIinput, "Dataset/%s/%c/PI.txt", "alphabets", 'A'+digit);
	}
	if(!strcmp(type,"number")){
		sprintf(Ainput, "Dataset/%s/%d/A.txt", "numbers", digit);
		sprintf(Binput, "Dataset/%s/%d/B.txt", "numbers", digit);
		sprintf(PIinput, "Dataset/%s/%d/PI.txt", "numbers", digit);
	}
	if(!strcmp(type,"command")){
		sprintf(Ainput, "Dataset/%s/%s/A.txt", "commands", cmds[digit]);
		sprintf(Binput, "Dataset/%s/%s/B.txt", "commands", cmds[digit]);
		sprintf(PIinput, "Dataset/%s/%s/PI.txt", "commands", cmds[digit]);
	}
	print_matrix_to_file(A,N,N,Ainput);
	print_matrix_to_file(PI,1,N,PIinput);
	PRINT_matrix_to_file(B,N,M,Binput);
}