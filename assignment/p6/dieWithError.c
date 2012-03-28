/*
 *  Created by Praseem and Maheshwar on 05/11/11.
 *
 */
#include <stdio.h>  
#include <stdlib.h> 

void dieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
