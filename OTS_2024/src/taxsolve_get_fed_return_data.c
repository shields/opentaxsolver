/************************************************************************/
/* taxsolve_get_fed_return_data.c					*/
/* Copyright (C) 2023 - Aston Roberts					*/
/*									*/
/* (Abstracted from taxsolve_CA_540_2023.c by E. Strnod for 		*/
/* use in AZ and potentially other State tax solvers.			*/
/* 									*/
/* GNU Public License - GPL:						*/
/* This program is free software; you can redistribute it and/or	*/
/* modify it under the terms of the GNU General Public License as	*/
/* published by the Free Software Foundation; either version 2 of the	*/
/* License, or (at your option) any later version.			*/
/* 									*/
/* This program is distributed in the hope that it will be useful,	*/
/* but WITHOUT ANY WARRANTY; without even the implied warranty of	*/
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU	*/
/* General Public License for more details.				*/
/* 									*/
/* You should have received a copy of the GNU General Public License	*/
/* along with this program; if not, write to the Free Software		*/
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA		*/
/* 02111-1307 USA							*/
/* 									*/
/* Aston Roberts 1-2-2024	aston_roberts@yahoo.com			*/
/* Ellen Strnod  9/14/2024						*/
/************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>

#include "taxsolve_routines.c"

#define SINGLE 		        1
#define MARRIED_FILING_JOINTLY  2
#define MARRIED_FILING_SEPARAT  3
#define HEAD_OF_HOUSEHOLD       4
#define WIDOW		        5

int status=0;	/* Value for filing status. */
char 	*Your1stName="", *YourLastName="", *your_socsec="",
	*Spouse1stName="", *SpouseLastName="", *spouse_socsec="",
	*street_address="", *apartment="", *town="", *state="", *zipcode="";

struct FedReturnData
 {
  double fedline[MAX_LINES], schedA[MAX_LINES],
	fed_L1a, fed_L1b, fed_L1c, fed_L1d, fed_L1e, fed_L1f, fed_L1g, fed_L1h, fed_L1z,
	fed_L2a, fed_L3a,
	fed_L4a, fed_L4b, fed_L5a, fed_L5b, fed_L6a, fed_L6b,
	schedA5a, schedA5b, schedA5c, schedA5,
	schedA8a, schedA8b, schedA8c, schedA8d,
	sched1[MAX_LINES], s1_8[30], s1_24[30], s2_17[30], s3_6[30], s3_13[30],
	fedl8b, fedl9b, fedl15a, fedl16a, fedl20a;
  int Exception, Itemized, Over65, SpouseOver65, Blind, SpouseBlind;
  char AlimRecipSSN[512], *AlimRecipName, OtherIncomeType[512],
	OtherAdjustmentsType[512], OtherTaxesType[512],
	Dep1stName[10][512], DepLastName[10][512],
	DepSocSec[10][512], DepRelation[10][512];
 } PrelimFedReturn;

 void convert_slashes( char *fname )
 { /* Convert slashes in file name based on machine type. */
   char *ptr;
  #ifdef __MINGW32__
   char slash_sreach='/', slash_replace='\\';
  #else
   char slash_sreach='\\', slash_replace='/';
  #endif
   ptr = strchr( fname, slash_sreach );
   while (ptr)
    {
     ptr[0] = slash_replace;
     ptr = strchr( fname, slash_sreach );
    }
 }

 void grab_line_value( char *label, char *fline, double *value )
 {
  char twrd[2048];
  next_word(fline, twrd, " \t=;");
  if ((twrd[0] != '\0') && (sscanf(twrd,"%lf", value) != 1))
   {
    printf("Error: Reading Fed %s '%s%s'\n", label, twrd, fline);
    fprintf(outfile,"Error: Reading Fed %s '%s%s'\n", label, twrd, fline);
   }
 }

 void grab_line_string( char *fline, char *strng )
 { /* Grab a string and copy it into pre-allocated character array. */
  char twrd[2048];
  strng[0] = '\0';
  do
   {
    next_word(fline, twrd, " \t=" );
    if (twrd[0] != ';')
     { strcat( strng, twrd );  strcat( strng, " " ); }
   }
  while ((fline[0] != '\0') && (strstr( twrd, ";" ) == 0));
 }


 void grab_line_alloc( char *fline, char **strng )
 { /* Grab a string and allocate space for it. */
  char twrd[4096];
  grab_line_string( fline, twrd );
  if (twrd[0] != '\0')
   *strng = strdup( twrd );
 }

 int ImportFederalReturnData( char *fedlogfile, struct FedReturnData *fed_data )
 {
  FILE *infile;
  char fline[2000], word[2000], tword[2000];
  int linenum, j;

  for (linenum=0; linenum<MAX_LINES; linenum++)
   {
    fed_data->fedline[linenum] = 0.0;
    fed_data->schedA[linenum] = 0.0;
    fed_data->sched1[linenum] = 0.0;
   }
  fed_data->fed_L1a = 0;
  fed_data->fed_L1b = 0;
  fed_data->fed_L1c = 0;
  fed_data->fed_L1d = 0;
  fed_data->fed_L1e = 0;
  fed_data->fed_L1f = 0;
  fed_data->fed_L1g = 0;
  fed_data->fed_L1h = 0;
  fed_data->fed_L1z = 0;
  fed_data->fed_L2a = 0;
  fed_data->fed_L3a = 0;
  fed_data->fed_L4a = 0;
  fed_data->fed_L4b = 0;
  fed_data->fed_L5a = 0;
  fed_data->fed_L5b = 0;
  fed_data->fed_L6a = 0;
  fed_data->fed_L6b = 0;
  fed_data->schedA5a = 0.0;
  fed_data->schedA5b = 0.0;
  fed_data->schedA5c = 0.0;
  fed_data->schedA8a = 0.0;
  fed_data->schedA8b = 0.0;
  fed_data->schedA8c = 0.0;
  fed_data->schedA8d = 0.0;

  fed_data->fedl8b = 0.0;
  fed_data->fedl9b = 0.0;
  fed_data->fedl15a = 0.0;
  fed_data->fedl16a = 0.0;
  fed_data->fedl20a = 0.0;
  for (j=0; j < 30; j++)
   {
    fed_data->s1_8[j] = 0.0;
    fed_data->s1_24[j] = 0.0;
    fed_data->s2_17[j] = 0.0;
    fed_data->s3_6[j] = 0.0;
    fed_data->s3_13[j] = 0.0;
   }
  strcpy( fed_data->AlimRecipSSN, "" );
  fed_data->AlimRecipName = strdup( "" );
  strcpy( fed_data->OtherIncomeType, "" );
  strcpy( fed_data->OtherAdjustmentsType, "" );
  strcpy( fed_data->OtherTaxesType, "" );
  for (j=0; j < 5; j++)
   {
    strcpy( fed_data->Dep1stName[j], "" );
    strcpy( fed_data->DepLastName[j], "" );
    strcpy( fed_data->DepSocSec[j], "" );
    strcpy( fed_data->DepRelation[j], "" );
   }
  convert_slashes( fedlogfile );
  infile = fopen(fedlogfile, "r");
  if (infile==0)
   {
    printf("Error: Could not open Federal return '%s'\n", fedlogfile);
    fprintf(outfile,"Error: Could not open Federal return '%s'\n", fedlogfile);
    exit( 1 );
   }
  fed_data->Itemized = 1; /* Set initial default values. */
  read_line(infile,fline);  linenum = 0;
  while (!feof(infile))
   {
	if (strstr(fline, "CkYouOver65")!=0) fed_data->Over65 = 1;
	if (strstr(fline, "CkSpouseOver65")!=0) fed_data->SpouseOver65 = 1;
	if (strstr(fline, "CkYouBlind")!=0) fed_data->Blind = 1;
	if (strstr(fline, "CkSpouseBlind")!=0) fed_data->SpouseBlind = 1;
    if (strstr(fline,"Use standard deduction.")!=0) fed_data->Itemized = 0;
    next_word(fline, word, " \t=");
    if ((word[0] == 'L') && (strstr(fline," = ")!=0))
     { /*L*/
      if (strcmp(word,"L1a") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L1a) );
      else
      if (strcmp(word,"L1b") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L1b) );
      else
      if (strcmp(word,"L1c") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L1c) );
      else
      if (strcmp(word,"L1d") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L1d) );
      else
      if (strcmp(word,"L1e") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L1e) );
      else
      if (strcmp(word,"L1f") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L1f) );
      else
      if (strcmp(word,"L1g") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L1g) );
      else
      if (strcmp(word,"L1h") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L1h) );
      else
      if (strcmp(word,"L1z") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L1z) );
      else
      if (strcmp(word,"L2a") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L2a) );
      else
      if (strcmp(word,"L3a") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L3a) );
      else
      if (strcmp(word,"L4a") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L4a) );
      else
      if (strcmp(word,"L4b") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L4b) );
      else
      if (strcmp(word,"L5a") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L5a) );
      else
      if (strcmp(word,"L5b") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L5b) );
      else
      if (strcmp(word,"L6a") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L6a) );
      else
      if (strcmp(word,"L6b") == 0)
       grab_line_value( word, fline, &(fed_data->fed_L6b) );
      else
      if (strcmp(word,"L8a") == 0)
       grab_line_value( word, fline, &(fed_data->fedline[8]) );
      else
      if (strcmp(word,"L8b") == 0)
       grab_line_value( word, fline, &(fed_data->fedl8b) );
      else
      if (strcmp(word,"L9a") == 0)
       grab_line_value( word, fline, &(fed_data->fedline[9]) );
      else
      if (strcmp(word,"L9b") == 0)
       grab_line_value( word, fline, &(fed_data->fedl9b) );
      else
      if (strcmp(word,"L15a") == 0)
       grab_line_value( word, fline, &(fed_data->fedl15a) );
      else
      if (strcmp(word,"L15b") == 0)
       grab_line_value( word, fline, &(fed_data->fedline[15]) );
      else
      if (strcmp(word,"L16a") == 0)
       grab_line_value( word, fline, &(fed_data->fedl16a) );
      else
      if (strcmp(word,"L16b") == 0)
       grab_line_value( word, fline, &(fed_data->fedline[16]) );
      else
      if (strcmp(word,"L20a") == 0)
       grab_line_value( word, fline, &(fed_data->fedl20a) );
      else
      if (strcmp(word,"L20b") == 0)
       grab_line_value( word, fline, &(fed_data->fedline[20]) );
      else
       {
        if (sscanf(&word[1],"%d",&linenum)!=1)
 	{
 	 printf("Error: Reading Fed line number '%s%s'\n",word,fline);
 	 fprintf(outfile,"Error: Reading Fed line number '%s%s'\n",word,fline);
 	}
        next_word(fline, word, " \t=");
        if (sscanf(word,"%lf", &fed_data->fedline[linenum])!=1)
 	{
 	 printf("Error: Reading Fed line %d '%s%s'\n",linenum,word,fline);
 	 fprintf(outfile,"Error: Reading Fed line %d '%s%s'\n",linenum,word,fline);
 	}
        if (verbose) printf("FedLin[%d] = %2.2f\n", linenum, fed_data->fedline[linenum]);
       }
     } /*L*/
    else

    if (strcmp(word, "Your1stName:" ) == 0)
     grab_line_alloc( fline, &Your1stName );
    else
    if (strcmp(word, "YourLastName:" ) == 0)
     grab_line_alloc( fline, &YourLastName );
    else
    if (strcmp(word, "YourSocSec#:" ) == 0)
     grab_line_alloc( fline, &your_socsec );
    else
    if (strcmp(word, "Spouse1stName:" ) == 0)
     grab_line_alloc( fline, &Spouse1stName );
    else
    if (strcmp(word, "SpouseLastName:" ) == 0)
     grab_line_alloc( fline, &SpouseLastName );
    else
    if (strcmp(word, "SpouseSocSec#:" ) == 0)
     grab_line_alloc( fline, &spouse_socsec );
    else
    if (strcmp(word, "Number&Street:" ) == 0)
     grab_line_alloc( fline, &street_address );
    else
    if (strcmp(word, "Apt#:" ) == 0)
     grab_line_alloc( fline, &apartment );
    else
    if (strcmp(word, "Town/City:" ) == 0)
     grab_line_alloc( fline, &town );
    else
	if (strcmp(word, "State:" ) == 0)
	 grab_line_alloc( fline, &state );
	else
    if (strcmp(word, "ZipCode:" ) == 0)
     grab_line_alloc( fline, &zipcode );
    else

    if (strncmp(word, "AlimRecipSSN", 12) == 0)
     grab_line_string( fline, fed_data->AlimRecipSSN );
    else
    if (strcmp( word, "S1_8z_Type:" ) == 0)
     grab_line_string( fline, fed_data->OtherIncomeType );
    else
    if (strcmp( word, "S1_24z_Type:" ) == 0)
     grab_line_string( fline, fed_data->OtherAdjustmentsType );
    else
    if (strcmp( word, "S2_17z_Type:" ) == 0)
     grab_line_string( fline, fed_data->OtherTaxesType );
    else

    if (strcmp(word, "Dep1_FirstName:") == 0)
     grab_line_string( fline, fed_data->Dep1stName[1] );
    else
    if (strcmp(word, "Dep1_LastName:") == 0)
     grab_line_string( fline, fed_data->DepLastName[1] );
    else
    if (strcmp(word, "Dep1_SocSec#:") == 0)
     grab_line_string( fline, fed_data->DepSocSec[1] );
    else
    if (strcmp(word, "Dep1_Relation:") == 0)
     grab_line_string( fline, fed_data->DepRelation[1] );
    else

    if (strcmp(word, "Dep2_FirstName:") == 0)
     grab_line_string( fline, fed_data->Dep1stName[2] );
    else
    if (strcmp(word, "Dep2_LastName:") == 0)
     grab_line_string( fline, fed_data->DepLastName[2] );
    else
    if (strcmp(word, "Dep2_SocSec#:") == 0)
     grab_line_string( fline, fed_data->DepSocSec[2] );
    else
    if (strcmp(word, "Dep2_Relation:") == 0)
     grab_line_string( fline, fed_data->DepRelation[2] );
    else

    if (strcmp(word, "Dep3_FirstName:") == 0)
     grab_line_string( fline, fed_data->Dep1stName[3] );
    else
    if (strcmp(word, "Dep3_LastName:") == 0)
     grab_line_string( fline, fed_data->DepLastName[3] );
    else
    if (strcmp(word, "Dep3_SocSec#:") == 0)
     grab_line_string( fline, fed_data->DepSocSec[3] );
    else
    if (strcmp(word, "Dep3_Relation:") == 0)
     grab_line_string( fline, fed_data->DepRelation[3] );
    else

    if ((word[0] == 'A') && (strstr(word,"AMT")!=word) && (strstr(fline," = ")!=0))
     {
      if (strcmp(word,"A5a") == 0)
       grab_line_value( word, fline, &(fed_data->schedA5a) );
      else
      if (strcmp(word,"A5b") == 0)
       grab_line_value( word, fline, &(fed_data->schedA5b) );
      else
      if (strcmp(word,"A5c") == 0)
       grab_line_value( word, fline, &(fed_data->schedA5c) );
      else
      if (strcmp(word,"A8a") == 0)
       grab_line_value( word, fline, &(fed_data->schedA8a) );
      else
      if (strcmp(word,"A8b") == 0)
       grab_line_value( word, fline, &(fed_data->schedA8b) );
      else
      if (strcmp(word,"A8c") == 0)
       grab_line_value( word, fline, &(fed_data->schedA8c) );
      else
      if (strcmp(word,"A8d") == 0)
       grab_line_value( word, fline, &(fed_data->schedA8d) );
      else
       {
        if (sscanf(&word[1],"%d",&linenum)!=1)
         {
 	 printf("Error: Reading Fed line number '%s%s'\n",word,fline);
 	 fprintf(outfile,"Error: Reading Fed line number '%s%s'\n",word,fline);
         }
        next_word(fline, word, " \t=");
        if (sscanf(word,"%lf", &fed_data->schedA[linenum])!=1)
         {
  	 printf("Error: Reading Fed schedA %d '%s%s'\n",linenum,word,fline);
 	 fprintf(outfile, "Error: Reading Fed schedA %d '%s%s'\n",linenum,word,fline);
         }
        if (verbose) printf("FedLin.A[%d] = %2.2f\n", linenum, fed_data->schedA[linenum]);
       }
     }
    else
    if ((strncmp( word, "S1_", 3 ) == 0) && (strstr(fline," = ")!=0))
     {
        next_word( &(word[3]), tword, " \t: =" );
        if ((tword[0] == '8') && (tword[1] >= 'a') && (tword[1] <= 'z'))
 	{ int j;
 	  if ((tword[1] >= 'a') && (tword[1] <= 'z'))
 	   {
 	    j = tword[1] - 'a';
 	    next_word(fline, word, " \t=");
 	    if (sscanf( word, "%lf", &fed_data->s1_8[j] ) != 1)
 	     {
 	      printf("Error: Reading Fed s1_8%c '%s%s'\n", 'a' + j, word, fline);
 	      fprintf(outfile, "Error: Reading Fed s1_8%c '%s%s'\n", 'a' + j, word, fline);
 	     }
 	    if (verbose) printf("FedLin.S1_8%c] = %2.2f\n", 'a' + j, fed_data->s1_8[j] );
 	   }
 	  else
 	   printf("Error: Unexpected line '%s'\n", word );
 	}
        else
        if ((strncmp( tword, "24", 2 ) == 0) && (tword[2] >= 'a') && (tword[2] <= 'z'))
 	{ int j;
 	  if ((tword[2] >= 'a') && (tword[2] <= 'z'))
 	   {
 	    j = tword[2] - 'a';
 	    next_word(fline, word, " \t=");
 	    if (sscanf( word, "%lf", &fed_data->s1_24[j] ) != 1)
 	     {
 	      printf("Error: Reading Fed s1_24%c '%s%s'\n", 'a' + j, word, fline);
 	      fprintf(outfile, "Error: Reading Fed s1_24%c '%s%s'\n", 'a' + j, word, fline);
 	     }
 	    if (verbose) printf("FedLin.S1_24%c = %2.2f\n", 'a' + j, fed_data->s1_24[j] );
 	   }
 	  else
 	   printf("Error: Unexpected line '%s'\n", word );
 	}
        else
 	{
          if (sscanf( tword, "%d", &linenum ) != 1)
           {
   	   printf("Error: Reading Fed line number 'S1_%s %s'\n", tword, fline);
 	   fprintf(outfile,"Error: Reading Fed line number 'S1_%s %s'\n", tword, fline);
            }
          next_word(fline, word, " \t=");
          if (sscanf(word,"%lf", &fed_data->sched1[linenum])!=1)
           {
     	   printf("Error: Reading Fed sched1 %d '%s%s'\n", linenum, word, fline);
 	   fprintf(outfile, "Error: Reading Fed sched1 %d '%s%s'\n", linenum, word, fline);
           }
          if (verbose) printf("FedLin.S1[%d] = %2.2f\n", linenum, fed_data->sched1[linenum]);
         }
     }
    else

    if ((strncmp( word, "S2_", 3 ) == 0) && (strstr(fline," = ")!=0))
     {
        next_word( &(word[3]), tword, " \t: =" );
        if ((strncmp( tword, "17", 2 ) == 0) && (tword[2] >= 'a') && (tword[2] <= 'z'))
 	{ int j;
 	  if ((tword[2] >= 'a') && (tword[2] <= 'z'))
 	   {
 	    j = tword[2] - 'a';
 	    next_word(fline, word, " \t=");
 	    if (sscanf( word, "%lf", &fed_data->s2_17[j] ) != 1)
 	     {
 	      printf("Error: Reading Fed s2_17%c '%s%s'\n", 'a' + j, word, fline);
 	      fprintf(outfile, "Error: Reading Fed s2_17%c '%s%s'\n", 'a' + j, word, fline);
 	     }
 	    if (verbose) printf("FedLin.S2_17%c = %2.2f\n", 'a' + j, fed_data->s2_17[j] );
 	   }
 	  else
 	   printf("Error: Unexpected line '%s'\n", word );
 	}
     }
    else

    if ((strncmp( word, "S3_", 3 ) == 0) && (strstr(fline," = ")!=0))
     {
        next_word( &(word[3]), tword, " \t: =" );
        if ((strncmp( tword, "6", 1 ) == 0) && (tword[1] >= 'a') && (tword[1] <= 'z'))
 	{ int j;
 	  if ((tword[1] >= 'a') && (tword[1] <= 'z'))
 	   {
 	    j = tword[1] - 'a';
 	    next_word(fline, word, " \t=");
 	    if (sscanf( word, "%lf", &fed_data->s3_6[j] ) != 1)
 	     {
 	      printf("Error: Reading Fed s3_6%c '%s%s'\n", 'a' + j, word, fline);
 	      fprintf(outfile, "Error: Reading Fed s3_6%c '%s%s'\n", 'a' + j, word, fline);
 	     }
 	    if (verbose) printf("FedLin.S3_6%c = %2.2f\n", 'a' + j, fed_data->s3_6[j] );
 	   }
 	  else
 	   printf("Error: Unexpected line '%s'\n", word );
 	}
        else
        if ((strncmp( tword, "13", 2 ) == 0) && (tword[2] >= 'a') && (tword[2] <= 'z'))
 	{ int j;
 	  if ((tword[2] >= 'a') && (tword[2] <= 'z'))
 	   {
 	    j = tword[2] - 'a';
 	    next_word(fline, word, " \t=");
 	    if (sscanf( word, "%lf", &fed_data->s3_13[j] ) != 1)
 	     {
 	      printf("Error: Reading Fed s3_13%c '%s%s'\n", 'a' + j, word, fline);
 	      fprintf(outfile, "Error: Reading Fed s3_13%c '%s%s'\n", 'a' + j, word, fline);
 	     }
 	    if (verbose) printf("FedLin.S3_13%c = %2.2f\n", 'a' + j, fed_data->s3_13[j] );
 	   }
 	  else
 	   printf("Error: Unexpected line '%s'\n", word );
 	}
     }

    else
    if (strcmp(word,"Status") == 0)
     {
      next_word(fline, word, " \t=");
      if (strncasecmp(word,"Single",4)==0) status = SINGLE; else
      if (strncasecmp(word,"Married/Joint",13)==0) status = MARRIED_FILING_JOINTLY; else
      if (strncasecmp(word,"Married/Sep",11)==0) status = MARRIED_FILING_SEPARAT; else
      if (strncasecmp(word,"Head_of_House",4)==0) status = HEAD_OF_HOUSEHOLD; else
      if (strncasecmp(word,"Widow",4)==0) status = WIDOW;
      else
       {
        printf("Error: unrecognized status '%s'. Exiting.\n", word);
        fprintf(outfile,"Error: unrecognized status '%s'. Exiting.\n", word);
        return 0;
       }
     }
    read_line(infile,fline);
   }
  fclose(infile);
  return 1;
 }
