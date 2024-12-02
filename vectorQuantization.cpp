#define MAX_UN_SIZE 20000
#define P 12
#define e 0.03

char unFile[200];
char cbFile[200];
long double tokuraWeights[] = {1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0};

int rows = 0;
long double Universe[MAX_UN_SIZE][P];
long double Codebook[M][P];
long double NewCodebook[M][P];

char cmds[3][5] = {"to","move","undo"};

int k = 1;

typedef struct {
    long double centroid[P];
	long double vectors[MAX_UN_SIZE][P];
    int t;
    long double distortion;
} REGION;


REGION region[M];
REGION subRegions[2];

void load_universe(char *type) {
	int r = 0,c = 0;
	for(int digit=0;digit<=(strcmp(type,"alphabet")?(strcmp(type,"number")?2:9):7);digit++){
		for(int utterance=1;utterance<=50;utterance++){
			if(!strcmp(type,"alphabet")) sprintf(ciFile, "Dataset/%s/%c/%c_%d/ci.txt", "alphabets", 'A'+digit, 'A'+digit, utterance);
			if(!(strcmp(type,"number"))) sprintf(ciFile, "Dataset/%s/%d/%d_%d/ci.txt", "numbers", digit, digit, utterance);
			if(!(strcmp(type,"command"))) sprintf(ciFile, "Dataset/%s/%s/%s_%d/ci.txt", "commands", cmds[digit], cmds[digit], utterance);
			FILE* file = fopen(ciFile, "r");
			if (file == NULL) {
				printf("File Not Open");
				exit(0);
			}
			while(r<MAX_UN_SIZE && fscanf(file,"%Lf",&Universe[r][c])!=EOF){
				c++;
				if(c==P){
					c=0;
					r++;
				}
			}
			printf("Done %d %d\n",digit,utterance);
			fclose(file);
		}
	}
	int x = r;
	rows = r;
	while(x<MAX_UN_SIZE){
	for(int i=0;i<P;i++){
			Universe[x][c] = 0;
		}
		x++;
	}
	if(!strcmp(type,"alphabet")) sprintf(unFile, "Dataset/%s/universe.txt", "alphabets");
	if(!(strcmp(type,"number"))) sprintf(unFile, "Dataset/%s/universe.txt", "numbers");
	if(!(strcmp(type,"command"))) sprintf(unFile, "Dataset/%s/universe.txt", "commands");
	FILE* wfile = fopen(unFile, "w");
	for(int i = 0; i < r; i++) {
		for(int j = 0; j < P; j++) {
			fprintf(wfile, "%e\t", Universe[i][j]);
		}
		fprintf(wfile, "%c", '\n');
	}
	fclose(wfile);
}

void printCodebook(char* type) {
	if(!strcmp(type,"alphabet")) sprintf(cbFile, "Dataset/%s/codebook.txt", "alphabets");
	if(!(strcmp(type,"number"))) sprintf(cbFile, "Dataset/%s/codebook.txt", "numbers");
	if(!(strcmp(type,"command"))) sprintf(cbFile, "Dataset/%s/codebook.txt", "commands");
	FILE* file = fopen(cbFile, "w");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < P; j++) {
			fprintf(file, "%e\t", Codebook[i][j]);
        }
		fprintf(file, "%c", '\n');
    }
	fclose(file);
}

void initialize_codebook(char *type) {
	if(!strcmp(type,"alphabet")) sprintf(unFile, "Dataset/%s/universe.txt", "alphabets");
	if(!(strcmp(type,"number"))) sprintf(unFile, "Dataset/%s/universe.txt", "numbers");
	if(!(strcmp(type,"command"))) sprintf(unFile, "Dataset/%s/universe.txt", "commands");
	FILE *file = fopen(unFile,"r");
	int r = 0,c = 0;
	while(r<MAX_UN_SIZE && fscanf(file,"%Lf",&Universe[r][c])!=EOF){
		c++;
		if(c==P){
			c=0;
			r++;
		}
	}
	rows = r;
	fclose(file);
	long double initialVector[P] = {0.0};
	for (int i = 0; i < P; i++) {
		initialVector[i] = 0;
		for (int j = 0; j < rows; j++) {
			initialVector[i] += Universe[j][i];
		}
		initialVector[i] /= rows;
	}
	for (int i = 0; i < k; i++) {
		for (int j = 0; j < P; j++) {
			Codebook[i][j] = initialVector[j];
		}
	}
}

long double dist(int i, int j) {
    long double dst = 0;
    for (int x = 0; x < P; x++) {
        dst += tokuraWeights[x] * (Universe[i][x] - Codebook[j][x]) * (Universe[i][x] - Codebook[j][x]);
    }
    return dst;
}

long double obsdist(long double CB[M][P],int i, int j) {
    long double dst = 0;
    for (int x = 0; x < P; x++) {
        dst += tokuraWeights[x] * (Ci[i][x] - CB[j][x]) * (Ci[i][x] - CB[j][x]);
    }
    return dst;
}

long double obsregiondist(int i,int y,int j) {
    long double dst = 0;
    for (int x = 0; x < P; x++) {
		dst += tokuraWeights[x] * (region[i].vectors[y][x] - subRegions[j].centroid[x]) * (region[i].vectors[y][x] - subRegions[j].centroid[x]);
    }
    return dst;
}

long double prev_avg_distortion = 0;
int m = 0;
void k_means_algorithm(int k) {
    int max_m = 1000;  // To avoid infinite loops, add a max m limit
    while (m < max_m) {
        for (int i = 0; i < k; i++) {
            region[i].t = 0;

            for (int j = 0; j < P; j++) {
                region[i].centroid[j] = Codebook[i][j];
            }
        }

        // Assign vectors to regions based on nearest neighbor
        for (int i = 0; i < rows; i++) {
            long double min_dist = dist(i, 0);
            int idx = 0;
            for (int j = 1; j < k; j++) {
                long double curr_dist = dist(i, j);
                if (curr_dist < min_dist) {
                    min_dist = curr_dist;
                    idx = j;
                }
            }
            for (int x = 0; x < P; x++) {
                region[idx].vectors[region[idx].t][x] = Universe[i][x];
            }
            region[idx].t++;
        }
		
		while(1){
			int max = region[0].t;
			int min = region[0].t;
			int last = 0;
			int first = 0;
			for(int i=1;i<k;i++){
				if(region[i].t>max){
					max = region[i].t;
					first = i;
				}
				if(region[i].t<min){
					min = region[i].t;
					last = i;
				}
			}
			if(min>50) break;
			for (int y = 0; y < P; y++) {
                long double sum = 0;
                for (int j = 0; j < region[first].t; j++) {
                    sum += region[first].vectors[j][y];
                }
                if (region[first].t != 0) {
					region[first].centroid[y] = sum / region[first].t;
                }
            }
			subRegions[0].t = 0; subRegions[1].t = 0;
			for (int j = 0; j < P; j++) {
				subRegions[0].centroid[j] = region[first].centroid[j] * (1 + e);
			}
			for (int j = 0; j < P; j++) {
				subRegions[1].centroid[j] = region[first].centroid[j] * (1 - e);
			}
			for (int y = 0; y < region[first].t; y++) {
				long double min_dist = obsregiondist(first,y,0);
				int idx = 0;
				for (int j = 1; j < 2; j++) {
					long double curr_dist = obsregiondist(first,y,1);
					if (curr_dist < min_dist) {
						min_dist = curr_dist;
						idx = j;
					}
				}
				for (int x = 0; x < P; x++) {
					subRegions[idx].vectors[subRegions[idx].t][x] = region[first].vectors[y][x];
				}
				subRegions[idx].t++;
			}
			region[first] = subRegions[0];
			region[last] = subRegions[1];
		}

        // Update codebook centroids and compute distortions
        for (int i = 0; i < k; i++) {
            long double distortion = 0;
            for (int j = 0; j < region[i].t; j++) {
                long double indDist = 0;
                for (int x = 0; x < P; x++) {
                    indDist += tokuraWeights[x] * (region[i].centroid[x] - region[i].vectors[j][x])*(region[i].centroid[x] - region[i].vectors[j][x]);
                }
                distortion += indDist;
            }
			if(region[i].t!=0) region[i].distortion = distortion / region[i].t;
        }

        // Calculate average distortion
        long double avg_distortion = 0;
        for (int i = 0; i < k; i++) {
            avg_distortion += region[i].distortion;
        }
        avg_distortion /= k;

        // Update centroids
        for (int i = 0; i < k; i++) {
            for (int y = 0; y < P; y++) {
                long double sum = 0;
                for (int j = 0; j < region[i].t; j++) {
                    sum += region[i].vectors[j][y];
                }
                if (region[i].t != 0) {
                    Codebook[i][y] = sum / region[i].t;
                }
            }
        }

        if (prev_avg_distortion!=0 && (prev_avg_distortion - avg_distortion < 0.00001)) {
            break;
        }

        prev_avg_distortion = avg_distortion;
        m++;
    }
}


