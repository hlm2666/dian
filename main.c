// Huffman编码实现
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_TREE_HT 256
#define ASCII_SIZE 256

typedef struct MinHeapNode {
    char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
} MinHeapNode;

typedef struct {
    unsigned size;
    unsigned capacity;
    MinHeapNode** array;
} MinHeap;

// 工具函数声明
MinHeapNode* newNode(char data, unsigned freq);
MinHeap* createMinHeap(unsigned capacity);
void swapNode(MinHeapNode** a, MinHeapNode** b);
void minHeapify(MinHeap* minHeap, int idx);
int isSizeOne(MinHeap* minHeap);
MinHeapNode* extractMin(MinHeap* minHeap);
void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode);
void buildMinHeap(MinHeap* minHeap);
void printArr(int arr[], int n);

// Huffman树构建
MinHeapNode* buildHuffmanTree(char data[], int freq[], int size);
void generateCodes(MinHeapNode* root, int arr[], int top, char** codes);
char** HuffmanCodes(char data[], int freq[], int size);

// 编码解码函数
char* encodeText(char* text, char** codes);
char* decodeText(MinHeapNode* root, char* encoded);

// 内存管理
void freeHuffmanTree(MinHeapNode* root);
void freeCodes(char** codes);

// 主要实现
MinHeapNode* newNode(char data, unsigned freq) {
    MinHeapNode* temp = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**)malloc(minHeap->capacity * sizeof(MinHeapNode*));
    return minHeap;
}

void swapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && 
        minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && 
        minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

int isSizeOne(MinHeap* minHeap) {
    return (minHeap->size == 1);
}

MinHeapNode* extractMin(MinHeap* minHeap) {
    MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1)/2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1)/2];
        i = (i - 1)/2;
    }
    minHeap->array[i] = minHeapNode;
}

void buildMinHeap(MinHeap* minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    MinHeapNode *left, *right, *top;
    MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);

    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

void generateCodes(MinHeapNode* root, int arr[], int top, char** codes) {
    if (root->left) {
        arr[top] = 0;
        generateCodes(root->left, arr, top + 1, codes);
    }

    if (root->right) {
        arr[top] = 1;
        generateCodes(root->right, arr, top + 1, codes);
    }

    if (!root->left && !root->right) {
        char* code = (char*)malloc(top+1);
        for (int i = 0; i < top; ++i)
            code[i] = arr[i] + '0';
        code[top] = '\0';
        codes[(unsigned char)root->data] = code;
    }
}

char** HuffmanCodes(char data[], int freq[], int size) {
    MinHeapNode* root = buildHuffmanTree(data, freq, size);
    char** codes = (char**)calloc(ASCII_SIZE, sizeof(char*));
    int arr[MAX_TREE_HT], top = 0;
    generateCodes(root, arr, top, codes);
    return codes;
}

char* encodeText(char* text, char** codes) {
    size_t total_len = 0;
    for (char* c = text; *c; c++) {
        char* code = codes[(unsigned char)*c];
        if (!code) {
            fprintf(stderr, "Missing code for character: %c\n", *c);
            exit(EXIT_FAILURE);
        }
        total_len += strlen(code);
    }

    char* encoded = (char*)malloc(total_len + 1);
    encoded[0] = '\0';

    for (char* c = text; *c; c++) {
        strcat(encoded, codes[(unsigned char)*c]);
    }
    return encoded;
}

char* decodeText(MinHeapNode* root, char* encoded) {
    if (root == NULL) return strdup("");

    // 处理单节点树的情况
    if (!root->left && !root->right) {
        size_t len = strlen(encoded);
        char* decoded = (char*)malloc(len + 1);
        memset(decoded, root->data, len);
        decoded[len] = '\0';
        return decoded;
    }

    char* result = (char*)malloc(strlen(encoded) + 1);
    MinHeapNode* current = root;
    int pos = 0;

    for (char* c = encoded; *c; c++) {
        current = (*c == '0') ? current->left : current->right;

        if (!current->left && !current->right) {
            result[pos++] = current->data;
            current = root;
        }
    }
    result[pos] = '\0';
    return result;
}

// 内存释放函数
void freeHuffmanTree(MinHeapNode* root) {
    if (root) {
        freeHuffmanTree(root->left);
        freeHuffmanTree(root->right);
        free(root);
    }
}

void freeCodes(char** codes) {
    for (int i = 0; i < ASCII_SIZE; i++) {
        if (codes[i]) free(codes[i]);
    }
    free(codes);
}

// 测试用例
void testHuffman(char* text) {
    printf("\nTesting text: %s\n", text);
    
    // 统计频率
    int freq[ASCII_SIZE] = {0};
    for (char* c = text; *c; c++) 
        freq[(unsigned char)*c]++;
    
    // 收集非零频率字符
    char chars[ASCII_SIZE];
    int freqs[ASCII_SIZE];
    int size = 0;
    for (int i = 0; i < ASCII_SIZE; i++) {
        if (freq[i] > 0) {
            chars[size] = (char)i;
            freqs[size] = freq[i];
            size++;
        }
    }

    // 构建Huffman编码
    char** codes = HuffmanCodes(chars, freqs, size);
    MinHeapNode* root = buildHuffmanTree(chars, freqs, size);
    
    // 编码
    char* encoded = encodeText(text, codes);
    printf("Encoded: %s\n", encoded);
    
    // 解码
    char* decoded = decodeText(root, encoded);
    printf("Decoded: %s\n", decoded);
    
    // 验证
    if (strcmp(text, decoded) == 0) {
        printf("Test PASSED\n");
    } else {
        printf("Test FAILED\n");
    }

    // 清理内存
    free(encoded);
    free(decoded);
    freeHuffmanTree(root);
    freeCodes(codes);
}

int main() {
    testHuffman("hello world");

    return 0;
}