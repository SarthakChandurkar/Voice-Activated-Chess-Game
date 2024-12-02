void READ_matrix(long double p[][M],int r,int c,const char* fileName){
	FILE* file = fopen(fileName,"r");
	for(int i=0;i<r;i++){
		for(int j=0;j<c;j++){
			fscanf(file,"%Lf",&p[i][j]);
		}
	}
	fclose(file);
}


void PRINT_matrix(long double p[][M],int r,int c){
	printf("\n");
	for(int i=0;i<r;i++){
		for(int j=0;j<c;j++){
			printf("%e ",p[i][j]);
		}
		printf("\n\n");
	}
}

void PRINT_matrix_to_file(long double p[][M],int r,int c,const char* fileName){
	FILE* file = fopen(fileName,"w");
	for(int i=0;i<r;i++){
		for(int j=0;j<c;j++){
			fprintf(file,"%e   ",p[i][j]);
		}
		fprintf(file,"\n\n");
	}
	fclose(file);
}

void read_matrix(long double p[][N],int r,int c,const char* fileName){
	FILE* file = fopen(fileName,"r");
	for(int i=0;i<r;i++){
		for(int j=0;j<c;j++){
			fscanf(file,"%Lf",&p[i][j]);
		}
	}
	fclose(file);
}

void print_matrix(long double p[][N],int r,int c){
	printf("\n");
	for(int i=0;i<r;i++){
		for(int j=0;j<c;j++){
			printf("%e   ",p[i][j]);
		}
		printf("\n");
	}
}

void print_matrix_to_file(long double p[][N],int r,int c,const char* fileName){
	FILE* file = fopen(fileName,"w");
	for(int i=0;i<r;i++){
		for(int j=0;j<c;j++){
			fprintf(file,"%e ",p[i][j]);
		}
		fprintf(file,"\n");
	}
	fclose(file);
}

void read_signal(long double signal[],char* fileName,int *z){
	FILE *file = fopen(fileName,"r");
	// printf("%s",file.
	if(file==NULL){
		printf("Cannot Open a file\n");
		exit(1);
	}
	int cnt = 0;
	while(cnt<50000 && fscanf(file,"%Lf",&signal[cnt])!=EOF){
		cnt++;
	}
	*z = cnt;
	fclose(file);
}


void read_frames(long double frames[MAX_SIZE][FRAME_SIZE],long double signal[],int *z){
	int t = 0;
	int r = 0,cnt = 0;
	while(t<samples){
		frames[r][cnt] = signal[t++];
		if(cnt==FRAME_SIZE-1){ cnt=0;r++;}
		else cnt++;
		if(r==MAX_SIZE) break;
	}
	*z = r;
}

void print_sequence(int p[],int z,int diff=0){
	for(int i=0;i<z;i++){
		printf("%d ",p[i]+diff);
	}
	printf("\n");
}

void create_folder_if_not_exists(const char *folder_path) {
    // Check if the folder exists by attempting to create it
    if (_mkdir(folder_path) == 0) {
        printf("Folder created at: %s\n", folder_path);
    } else if (errno == EEXIST) {
        printf("Folder already exists at: %s\n", folder_path);
    } else {
        perror("Error creating folder");
    }
}

void save_ci(){
	create_folder_if_not_exists(outputFolder);
	FILE* file = fopen(ciFile,"w");
	for(int f=0;f<fr;f++){
		for(int i=0;i<ORDER;i++){
			fprintf(file,"%e ",Ci[f][i]);
		}
		fprintf(file,"\n");
	}
	fclose(file);
}