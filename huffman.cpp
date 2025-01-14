#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <bits/stdc++.h>

using namespace std;

struct Forest {
    int weight;
    int root;
};

struct Tree {
    int left;
    int right;
    int parent;
    int symbol;
};

struct Tcoding {
    int len;
    vector<int> vec;
};

Forest forest[256];
Tree tree[520];
Tcoding code[256];

void min_min(Forest f[], int fsize, int& pos1, int& pos2) {
    int min1 = INT_MAX, min2 = INT_MAX;
    pos1 = -1; pos2 = -1;

    for (int i = 0; i < fsize; i++) {
        if (f[i].weight < min1) {
            min2 = min1;
            pos2 = pos1;
            min1 = f[i].weight;
            pos1 = i;
        }
        else if (f[i].weight < min2 && pos1 != i) {
            min2 = f[i].weight;
            pos2 = i;
        }
    }
}

void encode(const char* inputFile, const char* outputFile) {
    for (int i = 0; i < 256; i++) {
        forest[i] = { 0, -1 };
        code[i].len = 0;
    }

    FILE* f = fopen(inputFile, "rb");
    FILE* g = fopen(outputFile, "wb");

    int freq[256] = { 0 };
    unsigned char ch;
    long long kolsymbols = 0;

    while (fscanf(f, "%c", &ch) != EOF) {
        kolsymbols++;
        freq[ch]++;
    }

    //fprintf(g, "%d\n", kolsymbols); 
    fwrite(&kolsymbols, sizeof(long long), 1, g);

    int n = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] != 0) {
            forest[n].weight = freq[i];
            forest[n].root = n;
            tree[n].symbol = i;
            tree[n].left = -1;
            tree[n].right = -1;
            tree[n].parent = -1;
            n++;
        }
    }

    int forest_size = n;
    int tree_size = n;
    int p1, p2;

    while (forest_size > 1) {
        min_min(forest, forest_size, p1, p2);

        tree[tree_size].left = forest[p1].root;
        tree[tree_size].right = forest[p2].root;
        tree[tree_size].parent = -1;

        tree[tree[tree_size].left].parent = tree_size;
        tree[tree[tree_size].right].parent = tree_size;

        forest[p1].weight += forest[p2].weight;
        forest[p1].root = tree_size;
        forest[p2] = forest[forest_size - 1];
        forest_size--;
        tree_size++;
    }

    fprintf(g, "%d\n", tree_size);
    for (int i = 0; i < tree_size; i++) {
        fprintf(g, "%d %d %d %d\n", tree[i].left, tree[i].right, tree[i].parent, tree[i].symbol);
    }

    for (int i = 0; i < n; i++) {
        int t = i;
        while (tree[t].parent != -1) {
            if (tree[tree[t].parent].left == t) {
                code[tree[i].symbol].vec.push_back(1);
            }
            else {
                code[tree[i].symbol].vec.push_back(0);
            }
            t = tree[t].parent;
        }
        reverse(code[tree[i].symbol].vec.begin(), code[tree[i].symbol].vec.end());
        code[tree[i].symbol].len = code[tree[i].symbol].vec.size();
    }

    unsigned char byte = 0;
    int kol = 0;
    fseek(f, 0, SEEK_SET);
    while (fscanf(f, "%c", &ch) != EOF) {
        Tcoding& t = code[ch];
        for (int i = 0; i < t.len; i++) {
            byte = (byte << 1) | t.vec[i];
            kol++;
            if (kol == 8) {
                fwrite(&byte, sizeof(unsigned char), 1, g);
                byte = 0;
                kol = 0;
            }
        }
    }

    if (kol > 0) {
        byte <<= (8 - kol);
        fwrite(&byte, sizeof(unsigned char), 1, g);
    }

    fclose(f);
    fclose(g);
}

void decode(const char* inputFile, const char* outputFile) {
    FILE* g = fopen(inputFile, "rb");
    FILE* k = fopen(outputFile, "wb");

    long long kolsymbols;
    fread(&kolsymbols, sizeof(long long), 1, g);
    int tree_size;
    fscanf(g, "%d\n", &tree_size);
    for (int i = 0; i < tree_size; i++) {
        fscanf(g, "%d %d %d %d\n", &tree[i].left, &tree[i].right, &tree[i].parent, &tree[i].symbol);
    }

    unsigned char ch;
    int currentNode = tree_size - 1;
    int counter = 0;

    while (fread(&ch, sizeof(unsigned char), 1, g) == 1) {
        int mask = 128;
        for (int i = 0; i < 8; i++) {
            if ((ch & mask) == mask) {
                currentNode = tree[currentNode].left;
            }
            else {
                currentNode = tree[currentNode].right;
            }

            mask >>= 1;

            if (currentNode >= 0 && tree[currentNode].left == -1 && tree[currentNode].right == -1 && tree[currentNode].parent != -1 && counter < kolsymbols) {
                fwrite(&tree[currentNode].symbol, sizeof(unsigned char), 1, k);
                counter++;
                currentNode = tree_size - 1;
            }
        }
    }

    fclose(g);
    fclose(k);
}

int32_t main(int argc, char* argv[]) {

    if (!strcmp("encode", argv[1])) {
        encode(argv[3], argv[2]);
    }
    else {
        decode(argv[2], argv[3]);
    }

    return 0;
}

