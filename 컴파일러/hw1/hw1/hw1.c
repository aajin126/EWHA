#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 

#define FILE_NAME "testdata2.txt"
#define STsize 1000 // string table size
#define HTsize 100  // hash table size


typedef struct HTentry* HTpointer;
typedef struct HTentry {
    int index;    // Index of identifier in ST
    HTpointer next;  // Pointer to next identifier
} HTentry;

HTpointer HT[HTsize];
char ST[STsize];

FILE* fp; // Pointer to FILE

char separators[] = " .,;:?!\t\n"; // Separators

int nextFreeIndex = 0; // ST에서 다음으로 사용할 인덱스

void initialize() {
    fp = fopen(FILE_NAME, "r");
}

void PrintHeading()
{
    printf("\n");
    printf(" -----------       ---------- \n");
    printf(" Index in ST       identifier \n");
    printf(" -----------       ---------- \n");
}


// 해시 코드 계산 함수
int hash(const char* identifier) {
    int sum = 0; // identifier의 각 문자에 대한 ASCII 값의 합을 저장하는데 사용된다. 
    for (int i = 0; identifier[i] != '\0'; i++) { // null에 도달할 때 까지 identifier의 각 문자 순회  
        sum += (unsigned char)identifier[i]; // 식별자의 ASCII 값을 더함
    }
    return sum % HTsize; // sum값을 HTsize로 나눈 나머지 반환 
}

// 식별자를 HT에 추가하는 함수
void insertIdentifier(const char* identifier, int startIndex) {
    int h = hash(identifier); // hash table에서 identifier가 저장될 위치를 결정할 수 있도록 hash 함수로 계산 
    HTpointer current = HT[h]; // 해시 버킷

    // 리스트를 순회하면서 식별자가 이미 존재하는지 확인(strcmp로 비교)
    while (current != NULL) {
        if (strcmp(&ST[current->index], identifier) == 0) {
            printf("%d\t\t%10s\t\t(already existed)\n", current->index, identifier);
            return;
        }
        current = current->next;
    }

    // 없다면 새 식별자 추가
    HTpointer newNode = (HTpointer)malloc(sizeof(HTentry));
    newNode->index = startIndex;
    newNode->next = HT[h];
    HT[h] = newNode;
    printf("%d\t\t%10s\t\t(entered)\n", startIndex, identifier);
}

// 식별자를 처리하는 함수
void processIdentifier() {
    char ch;
    char identifier[100]; // 임시 식별자 버퍼
    int idLen = 0; // 식별자 길이

    char deleter[100]; // 식별자가 아닌 문자 저장
    int idDelLen = 0; // deleter 길이

    while ((ch = fgetc(fp)) != EOF) {
        if (isalpha(ch) || ch == '_') { // 알파벳, 밑줄(_), 따옴표(')로 시작하는 경우
            identifier[idLen++] = ch; // identifier에 문자 추가
            while (1)
            {
                ch = fgetc(fp);

                if (ch < 0) {
                    break;
                }
                if (isalnum(ch) || ch == '_') // 알파벳, 숫자, 밑줄(_)인 경우 identifier에 추가 
                {
                    if (idLen < sizeof(identifier) - 1) {
                        identifier[idLen++] = ch; // identifier에 계속 추가
                    }
                }
                else if (strchr(separators, ch))
                {
                    ungetc(ch, fp); // 구분자를 다시 파일 스트림에 넣음
                    identifier[idLen] = '\0'; // 식별자 종료 (문자열의 마지막은 null이여야 하기 때문)

                    if (nextFreeIndex + idLen < STsize) {
                        if (strlen(identifier) < 13)
                        {
                            strcpy(&ST[nextFreeIndex], identifier); // ST에 identifier 붙여넣기 
                            insertIdentifier(identifier, nextFreeIndex); // 해시 테이블에 추가
                            nextFreeIndex += idLen + 1; // 다음 빈 인덱스 업데이트
                        }
                        else
                        {
                            printf("***Error***\t%s\tOVER 12 CHARACTERS\n", identifier);
                        }

                    }
                    else {
                        printf("***Error***\t\tOVERFLOW\n");
                        return;
                    }

                    break; // 식별자 처리를 마침     
                }
                else // 알파벳, 숫자, 밑줄(_), separators 가 아닌 경우 에러처리
                {
                    identifier[idLen] = '\0'; // 식별자 종료 (문자열의 마지막은 null이여야 하기 때문)

                    if (nextFreeIndex + idLen < STsize) {
                        if (strlen(identifier) < 13)
                        {
                            strcpy(&ST[nextFreeIndex], identifier); // ST에 identifier 붙여넣기 
                            insertIdentifier(identifier, nextFreeIndex); // 해시 테이블에 추가
                            nextFreeIndex += idLen + 1; // 다음 빈 인덱스 업데이트
                        }
                        else
                        {
                            printf("***Error***\t\t%s\t\tOVER 12 CHARACTERS\n", identifier);
                        }
                    }
                    else {
                        printf("***Error***\t\tOVERFLOW\n");
                        return;
                    }

                    printf("***Error***\t\t %c\t\tInvalid character\n", ch);
                    break; // 현재 식별자 처리 중단
                }
            }
            idLen = 0; // 식별자 길이 초기화
        }
        else if (!strchr(separators, ch)) { // separators 가 아닌 경우 

            if (isdigit(ch)) {
                deleter[idDelLen++] = ch; // deleter에 문자 추가
                while (isalnum(ch = fgetc(fp)) || strchr(separators, ch) == NULL) { // seperator가 아니고 문자 또는 숫자인 경우만 계속 deleter에 추가한다. 
                    if (idDelLen < sizeof(deleter) - 1) {
                        deleter[idDelLen++] = ch; // deleter에 계속 추가
                    }
                }

                deleter[idDelLen] = '\0';
                printf("***Error***\t%10s\t\tstart with digit\n", deleter);
                char* deleter = NULL;
                idDelLen = 0;
            }
            else {
                printf("***Error***\t\t %c\t\tInvalid character\n", ch);
            }
        }
    }


}


// 해시 테이블 출력 함수
void printHashTable() {
    printf("\n\n[[ HASH TABLE ]]\n\n");
    for (int i = 0; i < HTsize; i++) {
        HTpointer entry = HT[i];
        if (entry != NULL) {
            printf("Hash Code %d : ", i);
            while (entry != NULL) {
                printf("%s ", &ST[entry->index]);
                entry = entry->next;
            }
            printf("\n");
        }
    }
    printf("\n<%d characters are used in the string table>\n", nextFreeIndex);
}

// 메모리 해제 함수
void freeMemory() {
    for (int i = 0; i < HTsize; ++i) {
        HTentry* current = HT[i];
        while (current != NULL) {
            HTentry* temp = current;
            current = current->next;
            free(temp);
        }
    }
}

int main() {
    printf("Compiler(01) Team09 Assignment1 - Symbol Table\n");
    printf("2076052 Sunwoo Kim\n");
    printf("2071059 Gahyeon Kwon\n");
    printf("2176308 Hahjin Lee\n");
    printf("2276185 Dana Yang\n\n");

    memset(HT, 0, sizeof(HT)); // Initialize hash table
    PrintHeading();
    initialize();
    processIdentifier();
    printHashTable();
    // 메모리 해제 및 파일 닫기
    freeMemory();

    return 0;
}
