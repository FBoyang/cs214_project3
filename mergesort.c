#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>
#include "csv.h"
#include "mergesort.h"

int compare(char *a, char *b);
int lexcmp(char *a, int alen, char *b, int blen);
int charcmp(char a, char b);
int strbegin(char *str);
int strend(char *str);

void mergesort(struct csv *table)
{
    int *end;
    int field_index, num_rows, low, middle, high, i, j;
    char ***a, ***b, ***tmp;
    struct file_node *ptr, *nxt;
    int ind;
    num_rows = table->total_rows;
    field_index = table->field_index;
    a = malloc(num_rows * sizeof(*a));
    for (ind = 0, ptr = table->front; ptr; ind += ptr->num_rows, ptr = nxt) {
        memcpy(a + ind, ptr->matrix, ptr->num_rows * sizeof(*a));
        nxt = ptr->next;
        free(ptr->matrix);
        free(ptr);
    }
    end = malloc(num_rows * sizeof(*end));
    j = 0;
    for (i = 1; i < num_rows; i++) {
        if (compare(a[i - 1][field_index], a[i][field_index]) > 0) {
            end[j] = i;
            j = i;
        }
    }
    end[j] = num_rows;
    b = malloc(num_rows * sizeof(*b));
    ind = 0;
    while (end[0] != num_rows) {
        low = ind;
        middle = end[ind];
        high = end[middle];
        i = low;
        j = middle;
        while (i < middle && j < high) {
            if (compare(a[i][field_index], a[j][field_index]) <= 0) {
                b[ind] = a[i];
                ind++;
                i++;
            } else {
                b[ind] = a[j];
                ind++;
                j++;
            }
        }
        while (i < middle) {
            b[ind] = a[i];
            ind++;
            i++;
        }
        while (j < high) {
            b[ind] = a[j];
            ind++;
            j++;
        }
        end[low] = high;
        if (high == num_rows || end[high] == num_rows) {
            for (; ind < num_rows; ind++)
                b[ind] = a[ind];
            ind = 0;
            tmp = a;
            a = b;
            b = tmp;
        }
    }
    ptr = malloc(sizeof(*ptr));
    ptr->matrix = a;
    ptr->num_rows = num_rows;
    ptr->next = NULL;
    table->front = ptr;
}

int compare(char *a, char *b)
{
    int ret;
    double ad, bd;
    bool ac, bc;
    int ab, bb;
    int ae, be;
    int alen, blen;
    bool aempty, bempty;
    char *endptr;
    ab = strbegin(a);
    ae = strend(a);
    bb = strbegin(b);
    be = strend(b);
    alen = ae - ab + 1;
    blen = be - bb + 1;
    aempty = (alen <= 0);
    bempty = (blen <= 0);
    if (aempty && bempty) {
        ret = 0;
    } else if (aempty) {
        ret = -1;
    } else if (bempty) {
        ret = 1;
    } else {
        ac = false;
        bc = false;
        ad = strtod(a + ab, &endptr);
        if (endptr == a + ae + 1)
            ac = true;
        bd = strtod(b + bb, &endptr);
        if (endptr == b + be + 1)
            bc = true;
        if (ac && bc) {
            if (fabs(ad - bd) < 0.0001)
                ret = 0;
            else if (ad < bd)
                ret = -1;
            else
                ret = 1;
        } else if (ac) {
            ret = -1;
        } else if (bc) {
            ret = 1;
        } else {
            ret = lexcmp(a + ab, alen, b+ bb, blen);
        }
    }
    return ret;
}

int lexcmp(char *a, int alen, char *b, int blen)
{
    int ret;
    int i, j;
    int cmp;
    i = 0;
    j = 0;
    ret = 2;
    while (ret == 2 && i < alen && j < blen) {
        while (a[i] < '\0' || isspace(a[i]))
            i++;
        while (b[j] < '\0' || isspace(b[j]))
            j++;
        if (i == alen && j == blen)
            ret = 0;
        else if (i == alen)
            ret = -1;
        else if (j == blen)
            ret = 1;
        else if ((cmp = charcmp(a[i], b[j])))
            ret = cmp;
        i++;
        j++;
    }
    if (ret == 2) {
        if (i >= alen && j >= blen)
            ret = 0;
        else if (i >= alen)
            ret = -1;
        else
            ret = 1;
    }
    return ret;
}

int charcmp(char a, char b)
{
    int ret;
    if (isalpha(a) && isalpha(b)) {
        if (toupper(a) == toupper(b)) {
            if (isupper(a) && isupper(b))
                ret = 0;
            else if (isupper(a))
                ret = -1;
            else if (isupper(b))
                ret = 1;
            else
                ret = 0;
        } else if (toupper(a) < toupper(b)) {
            ret = -1;
        } else {
            ret = 1;
        }
    } else if (toupper(a) == toupper(b)) {
        ret = 0;
    } else if (toupper(a) < toupper(b)) {
        ret = -1;
    } else {
        ret = 1;
    }
    return ret;
}

int strbegin(char *str)
{
    int i, len, begin;
    if (str)
        len = strlen(str);
    else
        len = 0;
    begin = len;
    i = 0;
    for (i = 0; i < len; i++) {
        if (str[i] > '\0' && !isspace(str[i])) {
            begin = i;
            break;
        }
    }
    return begin;
}

int strend(char *str)
{
    int i, len, end;
    if (str)
        len = strlen(str);
    else
        len = 0;
    end = -1;
    for (i = len - 1; i >= 0; i--) {
        if (str[i] > '\0' && !isspace(str[i])) {
            end = i;
            break;
        }
    }
    return end;
}

void insertionsort(char ***matrix, int field_index, int low, int high)
{
    int i, j;
    char **tmp;
    for (i = low + 1; i < high; i++) {
        tmp = matrix[i];
        for (j = i; j > 0 && compare(matrix[j - 1][field_index], tmp[field_index]) > 0; j--)
            matrix[j] = matrix[j - 1];
        matrix[j] = tmp;
    }
}
