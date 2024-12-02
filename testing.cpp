#define DATA_POINTS 16000*4
short int waveIn[DATA_POINTS];
char cbNum[200];
char cbAlpha[200];
char cbCom[200];

long double CbNum[M][P];
long double CbAlpha[M][P];
long double CbCom[M][P];

int tempOB[T_MAX];

void PlayRecord()
{
 const int NUMPTS = 16000 * 4;   // 4 seconds
 int sampleRate = 16000;  
 // 'short int' is a 16-bit type; I request 16-bit samples below
    // for 8-bit capture, you'd    use 'unsigned char' or 'BYTE' 8-bit types
 HWAVEIN  hWaveIn;
 WAVEFORMATEX pFormat;
 pFormat.wFormatTag=WAVE_FORMAT_PCM;     // simple, uncompressed format
 pFormat.nChannels=1;                    //  1=mono, 2=stereo
 pFormat.nSamplesPerSec=sampleRate;      // 44100
 pFormat.nAvgBytesPerSec=sampleRate*2;   // = nSamplesPerSec * n.Channels * wBitsPerSample/8
 pFormat.nBlockAlign=2;                  // = n.Channels * wBitsPerSample/8
 pFormat.wBitsPerSample=16;              //  16 for high quality, 8 for telephone-grade
 pFormat.cbSize=0;
 // Specify recording parameters
 waveInOpen(&hWaveIn, WAVE_MAPPER,&pFormat, 0L, 0L, WAVE_FORMAT_DIRECT);
 WAVEHDR      WaveInHdr;
 // Set up and prepare header for input
 WaveInHdr.lpData = (LPSTR)waveIn;
 WaveInHdr.dwBufferLength = NUMPTS*2;
 WaveInHdr.dwBytesRecorded=0;
 WaveInHdr.dwUser = 0L;
 WaveInHdr.dwFlags = 0L;
 WaveInHdr.dwLoops = 0L;
 waveInPrepareHeader(hWaveIn, &WaveInHdr, sizeof(WAVEHDR));
 HWAVEOUT hWaveOut;
 printf("playing...\n");
 waveOutOpen(&hWaveOut, WAVE_MAPPER, &pFormat, 0, 0, WAVE_FORMAT_DIRECT);
 waveOutWrite(hWaveOut, &WaveInHdr, sizeof(WaveInHdr)); // Playing the data
 Sleep(4 * 1000); //Sleep for as long as there was recorded
 waveInClose(hWaveIn);
 waveOutClose(hWaveOut);
}

void StartRecord()
{
    const int NUMPTS = DATA_POINTS;
    int sampleRate = 16000;

    HWAVEIN hWaveIn;
    MMRESULT result;

    WAVEFORMATEX pFormat;
    pFormat.wFormatTag = WAVE_FORMAT_PCM;
    pFormat.nChannels = 1;
    pFormat.nSamplesPerSec = sampleRate;
    pFormat.nAvgBytesPerSec = sampleRate * 2;
    pFormat.nBlockAlign = 2;
    pFormat.wBitsPerSample = 16;
    pFormat.cbSize = 0;

    result = waveInOpen(&hWaveIn, WAVE_MAPPER, &pFormat, 0L, 0L, WAVE_FORMAT_DIRECT);

    WAVEHDR WaveInHdr;
    WaveInHdr.lpData = (LPSTR)waveIn;
    WaveInHdr.dwBufferLength = NUMPTS * 2;
    WaveInHdr.dwBytesRecorded = 0;
    WaveInHdr.dwUser = 0L;
    WaveInHdr.dwFlags = 0L;
    WaveInHdr.dwLoops = 0L;
    waveInPrepareHeader(hWaveIn, &WaveInHdr, sizeof(WAVEHDR));

    result = waveInAddBuffer(hWaveIn, &WaveInHdr, sizeof(WAVEHDR));

    result = waveInStart(hWaveIn);

    printf("Recording for 4 seconds...\n");
    Sleep(4 * 1000);

    waveInClose(hWaveIn);
	for(int i=0;i<16000*4;i++){
		signal[i] = waveIn[i];
	}
	samples = 16000*4;
    PlayRecord();
}
void readCodebook(char *fileName,long double CB[M][P]){
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		printf("File Not Open");
		exit(0);
	}
	int r1=0,c1=0;
	while(r1<MAX_SIZE && fscanf(file,"%Lf",&CB[r1][c1])!=EOF){
		c1++;
		if(c1==ORDER){
			c1=0;
			r1++;
		}
	}
	fclose(file);
}
void create_observation_sequence_for_mic(){
	sprintf(cbAlpha, "Dataset/%s/codebook.txt", "alphabets");
	sprintf(cbNum, "Dataset/%s/codebook.txt", "numbers");
	sprintf(cbCom, "Dataset/%s/codebook.txt", "commands");
	readCodebook(cbAlpha,CbAlpha);
	readCodebook(cbNum,CbNum);
	readCodebook(cbCom,CbCom);
	for(int i=0;i<200;i++){
		long double min = obsdist(CbCom,i,0);
		int idx = 0;
		for(int j=1;j<M;j++){
			long double dist = obsdist(CbCom,i,j);
			if(dist<min){
				min = dist;
				idx = j;
			}
		}
		tempOB[i] = idx;
	}
	int i=0;
	for(i=0;i<70;i++){
		long double min = obsdist(CbCom,i,0);
		int idx = 0;
		for(int j=1;j<M;j++){
			long double dist = obsdist(CbCom,i,j);
			if(dist<min){
				min = dist;
				idx = j;
			}
		}
		OB[i] = idx;
	}
	for(;i<95;i++){
		long double min = obsdist(CbAlpha,i,0);
		int idx = 0;
		for(int j=1;j<M;j++){
			long double dist = obsdist(CbAlpha,i,j);
			if(dist<min){
				min = dist;
				idx = j;
			}
		}
		OB[i] = idx;
	}
	for(;i<120;i++){
		long double min = obsdist(CbNum,i,0);
		int idx = 0;
		for(int j=1;j<M;j++){
			long double dist = obsdist(CbNum,i,j);
			if(dist<min){
				min = dist;
				idx = j;
			}
		}
		OB[i] = idx;
	}
	for(;i<150;i++){
		long double min = obsdist(CbCom,i,0);
		int idx = 0;
		for(int j=1;j<M;j++){
			long double dist = obsdist(CbCom,i,j);
			if(dist<min){
				min = dist;
				idx = j;
			}
		}
		OB[i] = idx;
	}
	for(;i<175;i++){
		long double min = obsdist(CbAlpha,i,0);
		int idx = 0;
		for(int j=1;j<M;j++){
			long double dist = obsdist(CbAlpha,i,j);
			if(dist<min){
				min = dist;
				idx = j;
			}
		}
		OB[i] = idx;
	}
	for(;i<200;i++){
		long double min = obsdist(CbNum,i,0);
		int idx = 0;
		for(int j=1;j<M;j++){
			long double dist = obsdist(CbNum,i,j);
			if(dist<min){
				min = dist;
				idx = j;
			}
		}
		OB[i] = idx;
	}
}
void framing_for_mic(){
	dc_shift();
	normalization();
	read_frames(frames,signal,&fr);
	frame_energy();
}
void calculateAiCi_for_mic(){
	apply_hamming_window();
	calculate_ri();
	calculate_ai();
	calculate_ci();
	apply_raised_sine_window();
}

int apply_forward_alpha(){
	int idx = -1;
	long double p_max = 0;
	for(int digit=0;digit<=7;digit++){
		sprintf(Ainput, "Dataset/%s/%c/A.txt", "alphabets", 'A'+digit);
		sprintf(Binput, "Dataset/%s/%c/B.txt", "alphabets", 'A'+digit);
		sprintf(PIinput, "Dataset/%s/%c/PI.txt", "alphabets", 'A'+digit);
		read_matrix(A,N,N,Ainput);
		READ_matrix(B,N,M,Binput);
		read_matrix(PI,1,N,PIinput);
		long double p = forward_procedure();
		// printf("\nP with Model %d:%e ",digit,p);
		if(p>p_max){
			p_max = p;
			idx = digit;
		}
	}
	return idx;
}

int apply_forward_num(){
	int idx = -1;
	long double p_max = 0;
	for(int digit=0;digit<=9;digit++){
		sprintf(Ainput, "Dataset/%s/%d/A.txt", "numbers", digit);
		sprintf(Binput, "Dataset/%s/%d/B.txt", "numbers", digit);
		sprintf(PIinput, "Dataset/%s/%d/PI.txt", "numbers", digit);
		read_matrix(A,N,N,Ainput);
		READ_matrix(B,N,M,Binput);
		read_matrix(PI,1,N,PIinput);
		long double p = forward_procedure();
		// printf("P with Model %d:%e\n",digit,p);
		if(p>p_max){
			p_max = p;
			idx = digit;
		}
	}
	return idx;
}

int apply_forward_command(){
	int idx = -1;
	long double p_max = 0;
	for(int digit=0;digit<=2;digit++){
		sprintf(Ainput, "Dataset/%s/%s/A.txt", "commands", cmds[digit]);
		sprintf(Binput, "Dataset/%s/%s/B.txt", "commands", cmds[digit]);
		sprintf(PIinput, "Dataset/%s/%s/PI.txt", "commands", cmds[digit]);
		read_matrix(A,N,N,Ainput);
		READ_matrix(B,N,M,Binput);
		read_matrix(PI,1,N,PIinput);
		long double p = forward_procedure();
		// printf("P with Model %d:%e\n",digit,p);
		if(p>p_max){
			p_max = p;
			idx = digit;
		}
	}
	return idx;
}


void test_by_mic(int *a,int *b,int *c,int *d){
	StartRecord();
	framing_for_mic();
	calculateAiCi_for_mic();
	create_observation_sequence_for_mic();

	int i=0,j=0,ans = -1;
	for(i=0,j=0;i<75;i++,j++){
		OB[j] = tempOB[i];
	}
	T = 75;
	ans = apply_forward_command();
	if(ans!=-1) printf("%s ",cmds[ans]);

	for(j=0;i<100;i++,j++){
		OB[j] = tempOB[i];
	}
	T = 25;
	ans = apply_forward_alpha();
	if(ans!=-1){printf("%c",'A'+ans);
	*a = ans;}
	for(j=0;i<125;i++,j++){
		OB[j] = tempOB[i];
	}
	T = 25;
	
	ans = apply_forward_num();
	if(ans!=-1){printf("%d ",ans);
	*b = ans;}

	for(j=0;i<160;i++,j++){
		OB[j] = tempOB[i];
	}
	T = 30;
	ans = apply_forward_command();
	if(ans!=-1) printf("%s ",cmds[ans]);

	for(j=0;i<180;i++,j++){
		OB[j] = tempOB[i];
	}
	T = 25;
	ans = apply_forward_alpha();
	if(ans!=-1){printf("%c",'A'+ans);
	*c = ans;}
	for(j=0;i<200;i++,j++){
		OB[j] = tempOB[i];
	}
	T = 25;
	ans = apply_forward_num();
	if(ans!=-1){printf("%d ",ans);
	*d = ans;}
}
