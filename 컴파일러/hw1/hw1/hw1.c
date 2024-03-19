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

int nextFreeIndex = 0; // ST���� �������� ����� �ε���

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


// �ؽ� �ڵ� ��� �Լ�
int hash(const char* identifier) {
    int sum = 0; // identifier�� �� ���ڿ� ���� ASCII ���� ���� �����ϴµ� ���ȴ�. 
    for (int i = 0; identifier[i] != '\0'; i++) { // null�� ������ �� ���� identifier�� �� ���� ��ȸ  
        sum += (unsigned char)identifier[i]; // �ĺ����� ASCII ���� ����
    }
    return sum % HTsize; // sum���� HTsize�� ���� ������ ��ȯ 
}

// �ĺ��ڸ� HT�� �߰��ϴ� �Լ�
void insertIdentifier(const char* identifier, int startIndex) {
    int h = hash(identifier); // hash table���� identifier�� ����� ��ġ�� ������ �� �ֵ��� hash �Լ��� ��� 
    HTpointer current = HT[h]; // �ؽ� ��Ŷ

    // ����Ʈ�� ��ȸ�ϸ鼭 �ĺ��ڰ� �̹� �����ϴ��� Ȯ��(strcmp�� ��)
    while (current != NULL) {
        if (strcmp(&ST[current->index], identifier) == 0) {
            printf("%d\t\t%10s\t\t(already existed)\n", current->index, identifier);
            return;
        }
        current = current->next;
    }

    // ���ٸ� �� �ĺ��� �߰�
    HTpointer newNode = (HTpointer)malloc(sizeof(HTentry));
    newNode->index = startIndex;
    newNode->next = HT[h];
    HT[h] = newNode;
    printf("%d\t\t%10s\t\t(entered)\n", startIndex, identifier);
}

// �ĺ��ڸ� ó���ϴ� �Լ�
void processIdentifier() {
    char ch;
    char identifier[100]; // �ӽ� �ĺ��� ����
    int idLen = 0; // �ĺ��� ����

    char deleter[100]; // �ĺ��ڰ� �ƴ� ���� ����
    int idDelLen = 0; // deleter ����

    while ((ch = fgetc(fp)) != EOF) {
        if (isalpha(ch) || ch == '_') { // ���ĺ�, ����(_), ����ǥ(')�� �����ϴ� ���
            identifier[idLen++] = ch; // identifier�� ���� �߰�
            while (1)
            {
                ch = fgetc(fp);

                if (ch < 0) {
                    break;
                }
                if (isalnum(ch) || ch == '_') // ���ĺ�, ����, ����(_)�� ��� identifier�� �߰� 
                {
                    if (idLen < sizeof(identifier) - 1) {
                        identifier[idLen++] = ch; // identifier�� ��� �߰�
                    }
                }
                else if (strchr(separators, ch))
                {
                    ungetc(ch, fp); // �����ڸ� �ٽ� ���� ��Ʈ���� ����
                    identifier[idLen] = '\0'; // �ĺ��� ���� (���ڿ��� �������� null�̿��� �ϱ� ����)

                    if (nextFreeIndex + idLen < STsize) {
                        if (strlen(identifier) < 13)
                        {
                            strcpy(&ST[nextFreeIndex], identifier); // ST�� identifier �ٿ��ֱ� 
                            insertIdentifier(identifier, nextFreeIndex); // �ؽ� ���̺� �߰�
                            nextFreeIndex += idLen + 1; // ���� �� �ε��� ������Ʈ
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

                    break; // �ĺ��� ó���� ��ħ     
                }
                else // ���ĺ�, ����, ����(_), separators �� �ƴ� ��� ����ó��
                {
                    identifier[idLen] = '\0'; // �ĺ��� ���� (���ڿ��� �������� null�̿��� �ϱ� ����)

                    if (nextFreeIndex + idLen < STsize) {
                        if (strlen(identifier) < 13)
                        {
                            strcpy(&ST[nextFreeIndex], identifier); // ST�� identifier �ٿ��ֱ� 
                            insertIdentifier(identifier, nextFreeIndex); // �ؽ� ���̺� �߰�
                            nextFreeIndex += idLen + 1; // ���� �� �ε��� ������Ʈ
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
                    break; // ���� �ĺ��� ó�� �ߴ�
                }
            }
            idLen = 0; // �ĺ��� ���� �ʱ�ȭ
        }
        else if (!strchr(separators, ch)) { // separators �� �ƴ� ��� 

            if (isdigit(ch)) {
                deleter[idDelLen++] = ch; // deleter�� ���� �߰�
                while (isalnum(ch = fgetc(fp)) || strchr(separators, ch) == NULL) { // seperator�� �ƴϰ� ���� �Ǵ� ������ ��츸 ��� deleter�� �߰��Ѵ�. 
                    if (idDelLen < sizeof(deleter) - 1) {
                        deleter[idDelLen++] = ch; // deleter�� ��� �߰�
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


// �ؽ� ���̺� ��� �Լ�
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

// �޸� ���� �Լ�
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
    // �޸� ���� �� ���� �ݱ�
    freeMemory();

    return 0;
}
