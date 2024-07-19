//
// Created by Yotam Levit on 19/07/2024.
//

#include "../include/utils.h"

FILE* open_file(char* file)
{
    FILE* fp = fopen(file, "r");
    if (fp == NULL)
    {
        printf("ERROR!! File not found or file with extension.\n");
        return NULL;
    }
    return fp;
}