#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<io.h>
#include<time.h>
#include<limits.h>
#include<stdint.h>

typedef struct _finddata_t FILE_SEARCH;

struct prior {
	int first, second;
};

struct text {
	char* name;
	prior prior;
};

#define INPUT_SIZE 100
#define ALPHABET_LEN 256
#define NOT_FOUND patlen
#define max(a, b) ((a < b) ? b : a)

void make_delta1(int *delta1, uint8_t *pat, int32_t patlen) {
	int i;
	for (i = 0; i < ALPHABET_LEN; i++) {
		delta1[i] = NOT_FOUND;
	}
	for (i = 0; i < patlen - 1; i++) {
		delta1[pat[i]] = patlen - 1 - i;
	}
}

int is_prefix(uint8_t *word, int wordlen, int pos) {
	int i;
	int suffixlen = wordlen - pos;
	for (i = 0; i < suffixlen; i++) {
		if (word[i] != word[pos + i]) {
			return 0;
		}
	}
	return 1;
}

int suffix_length(uint8_t *word, int wordlen, int pos) {
	int i;
	for (i = 0; (word[pos - i] == word[wordlen - 1 - i]) && (i < pos); i++);
	return i;
}

void make_delta2(int *delta2, uint8_t *pat, int32_t patlen) {
	int p;
	int last_prefix_index = patlen - 1;

	for (p = patlen - 1; p >= 0; p--) {
		if (is_prefix(pat, patlen, p + 1)) {
			last_prefix_index = p + 1;
		}
		delta2[p] = last_prefix_index + (patlen - 1 - p);
	}

	for (p = 0; p < patlen - 1; p++) {
		int slen = suffix_length(pat, patlen, p);
		if (pat[p - slen] != pat[patlen - 1 - slen]) {
			delta2[patlen - 1 - slen] = patlen - 1 - p + slen;
		}
	}
}

uint8_t* boyer_moore(uint8_t *string, uint32_t stringlen, uint8_t *pat, uint32_t patlen) {
	int i;
	int delta1[ALPHABET_LEN];
	int *delta2 = (int *)malloc(patlen * sizeof(int));
	make_delta1(delta1, pat, patlen);
	make_delta2(delta2, pat, patlen);

	i = patlen - 1;
	while (i < stringlen) {
		int j = patlen - 1;
		while (j >= 0 && (string[i] == pat[j])) {
			--i;
			--j;
		}
		if (j < 0) { // 검색 키워드를 찾은 경우
			free(delta2);
			return (string + i + 1);
		}

		i += max(delta1[string[i]], delta2[j]);
	}
	free(delta2);
	return NULL;
}

bool isAlphabet(char ch) {
	if (ch >= 'A' && ch <= 'Z') return true;
	else if (ch >= 'a' && ch <= 'z') return true;
	return false;
}


int main()
{
	long h_file;
	char search_path[200];
	FILE** file;
	char** file_main;
	text* file_text;
	char* result;
	long fileNum = 0;
	FILE_SEARCH file_search;
	int i, j, index = 0;
	char ch;
	char input[INPUT_SIZE + 1];
	bool isUpperString, isLowerString;

	clock_t start, end;
	float time;

	// calculate number of files in directory
	sprintf_s(search_path, "./dataset/*.txt");
	if ((h_file = _findfirst(search_path, &file_search)) == -1L) {
		printf("No files in current directory!\n");
		system("pause");
		return 0;
	}
	else {
		do {
			fileNum++;
		} while (_findnext(h_file, &file_search) == 0);

		_findclose(h_file);
	}

	// dynamic memory allocation && Initialization
	file = (FILE**)malloc(fileNum * sizeof(FILE*));
	file_main = (char**)malloc(fileNum * sizeof(char*));
	file_text = (text*)malloc(fileNum * sizeof(text));
	for (i = 0; i < fileNum; i++) {
		file_text[i].prior.first = 0;
		file_text[i].prior.second = 0;
	}

	// input search keyword
	printf("20125757 노요한 SW프로그래밍(2)\n");
	printf("String Search Project\n");
	printf("---------------------------------------------------\n");
	while (index == 0 || index > INPUT_SIZE) {
		printf("검색할 문자열을 입력하세요.(공백 포함 100자 이내) ");
		index = 0;
		while ((ch = getchar()) != '\n')
			input[index++] = ch;
		input[index] = '\0';
		if (index > INPUT_SIZE) {
			printf("입력이 100자를 넘었습니다.\n다시 입력해 주세요.\n");
		}
	}

	// isLowerString : true - if search keyword don't have Upper Alphabet
	isLowerString = true;
	for (i = 0; i < index; i++) {
		if (isAlphabet(input[i])) {
			if (!(input[i] >= 'a')) {
				isLowerString = false;
				break;
			}
		}
	}
	// isUpperString : true - if search keyword don't have Lower Alphabet
	isUpperString = true;
	for (i = 0; i < index; i++) {
		if (isAlphabet(input[i])) {
			if (!(input[i] <= 'Z')) {
				isUpperString = false;
				break;
			}
		}
	}

	start = clock();
	// access files in directory
	h_file = _findfirst(search_path, &file_search);
		i = 0;
		do {
			sprintf_s(search_path, "./dataset/%s", file_search.name);
			file_text[i].name = (char*)malloc(strlen(file_search.name) * sizeof(char));
			strcpy(file_text[i].name, file_search.name);
						
			file[i] = fopen(search_path, "r");
			index = 0;
			while ((ch = fgetc(file[i])) != EOF) {
				//printf("%c", ch);
				index++;
			}
			file_main[i] = (char*)malloc((index + 1) * sizeof(char));
			fclose(file[i]);

			file[i] = fopen(search_path, "r");
			index = 0;
			while ((ch = fgetc(file[i])) != EOF) {
				file_main[i][index++] = ch;
			}
			file_main[i][index] = '\0';

			result = NULL;
			result = (char*)boyer_moore((uint8_t*)file_main[i], strlen(file_main[i]), (uint8_t*)input, strlen(input));
			if (result != NULL) {
				file_text[i].prior.first++;
				while ((result = (char*)boyer_moore((uint8_t*)(result + strlen(input)), strlen(result), (uint8_t*)input, strlen(input))) != NULL) {
					file_text[i].prior.first++;
				}
			}

			if (isUpperString) {
				result = (char*)boyer_moore((uint8_t*)strupr(file_main[i]), strlen(file_main[i]), (uint8_t*)input, strlen(input));
				if (result != NULL) {
					file_text[i].prior.second++;
					while ((result = (char*)boyer_moore((uint8_t*)strupr(result + strlen(input)), strlen(result), (uint8_t*)input, strlen(input))) != NULL) {
						file_text[i].prior.second++;
					}
					file_text[i].prior.second -= file_text[i].prior.first;
				}
			}
			else if (isLowerString) {
				result = (char*)boyer_moore((uint8_t*)strlwr(file_main[i]), strlen(file_main[i]), (uint8_t*)input, strlen(input));
				if (result != NULL) {
					file_text[i].prior.second++;
					while ((result = (char*)boyer_moore((uint8_t*)strlwr(result + strlen(input)), strlen(result), (uint8_t*)input, strlen(input))) != NULL) {
						file_text[i].prior.second++;
					}
					file_text[i].prior.second -= file_text[i].prior.first;
				}
			}
			else {
				result = (char*)boyer_moore((uint8_t*)strlwr(file_main[i]), strlen(file_main[i]), (uint8_t*)strlwr(input), strlen(input));
				if (result != NULL) {
					file_text[i].prior.second++;
					while ((result = (char*)boyer_moore((uint8_t*)strlwr(result + strlen(input)), strlen(result), (uint8_t*)strlwr(input), strlen(input))) != NULL) {
						file_text[i].prior.second++;
					}
					file_text[i].prior.second -= file_text[i].prior.first;
				}
			}
			
			fclose(file[i]);
			i++;
		} while (_findnext(h_file, &file_search) == 0);
		_findclose(h_file);

	text temp;
	for(i = 0; i < fileNum; i++){
		for (j = i + 1; j < fileNum; j++) {
			if ((file_text[i].prior.first < file_text[j].prior.first) || ((file_text[i].prior.first == file_text[j].prior.first) && (file_text[i].prior.second < file_text[j].prior.second))) {
				temp = file_text[i];
				file_text[i] = file_text[j];
				file_text[j] = temp;
			}
		}
	}
	for (i = 0; i < fileNum; i++) {
		if (file_text[i].prior.first < 1 && file_text[i].prior.second < 1) break;
		printf("%s : %d - %d\n", file_text[i].name, file_text[i].prior.first, file_text[i].prior.second);
	}

	end = clock();
	time = (float)(end - start) / CLOCKS_PER_SEC;
	printf("소요 시간 : %.3f\n", time);

	return 0;
}