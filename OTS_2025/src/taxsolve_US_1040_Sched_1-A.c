/************************************************************************/
/* TaxSolve_US_1040_Sched_1-A.c - 					*/
/* Copyright (C)  2026 - S.Jenkins					*/
/* 									*/
/* Documentation & Updates:						*/
/*        http://opentaxsolver.sourceforge.net/				*/
/*									*/
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
/* S.Jenkins 1-28-2026   						*/
/************************************************************************/

float thisversion=1.01;

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "taxsolve_routines.c"
#define Yes 1
#define No  0
char YourNames[4000]="", *your_socsec=0;
char Your1stName[2000]="", YourLastName[2000]="";
char Spouse1stName[2000]="", SpouseLastName[2000]="";

#define SINGLE                  1
#define MARRIED_FILING_JOINTLY  2
#define MARRIED_FILING_SEPARAT  3
#define HEAD_OF_HOUSEHOLD       4
#define WIDOW                   5

int status=0;   /* Value for filing status. */
int over65you=0, over65spouse=0;

struct FedReturnData
 {
  double fedline[MAX_LINES], fed_11b;
 } PrelimFedReturn;


/* From the Federal results, just need Name, SocSec, Status, Over65 (for you + spouse),
   and, amount from 1040, line 11b. 
*/


int ImportFederalReturnData( char *fedlogfile, struct FedReturnData *fed_data )
{
 FILE *infile;
 char fline[2000], word[2000];
 int linenum;

 for (linenum=0; linenum<MAX_LINES; linenum++) 
   fed_data->fedline[linenum] = 0.0;
 fed_data->fed_11b = 0.0;

 convert_slashes( fedlogfile );
 infile = fopen(fedlogfile, "r");
 if (infile==0)
  {
   printf("Error: Could not open Federal return '%s'\n", fedlogfile);
   fprintf(outfile,"Error: Could not open Federal return '%s'\n", fedlogfile);
   exit( 1 ); 
  }
 read_line(infile,fline);  linenum = 0;
 while (!feof(infile))
  {
   next_word(fline, word, " \t=");
   if ((word[0] == 'L') && (strstr(fline," = ") != 0))
    { /*L*/
     if (strcmp(word,"L11b") == 0)
      grab_line_value( word, fline, &(fed_data->fed_11b) );
    } /*L*/
   else
   if (strcmp(word, "Your1stName:" ) == 0)
    grab_line_string( fline, Your1stName );
   else
   if (strcmp(word, "YourLastName:" ) == 0)
    grab_line_string( fline, YourLastName );
   else
   if (strcmp(word, "Spouse1stName:" ) == 0)
    grab_line_string( fline, Spouse1stName );
   else
   if (strcmp(word, "SpouseLastName:" ) == 0)
    grab_line_string( fline, SpouseLastName );
   else
   if (strcmp(word, "YourSocSec#:" ) == 0)
    grab_line_alloc( fline, &your_socsec );
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
    else
    if (strcmp(word,"CkYouOver65") == 0)
     {
       next_word(fline, word, " \t=");
       if (strncasecmp(word,"X",1)==0) over65you = 1; else over65you = 0;
     }
    else
    if (strcmp(word,"CkSpouseOver65") == 0)
     {
       next_word(fline, word, " \t=");
       if (strncasecmp(word,"X",1)==0) over65spouse = 1; else over65spouse = 0;
     }
    read_line(infile,fline);
  }
 fclose(infile);
 return 1;
}



/*----------------------------------------------------------------------------*/

int main( int argc, char *argv[] )
{
 int i, j, k;
 char word[4000], outfname[4000], *infname=0, prelim_1040_outfilename[5000];
 time_t now;
 double L2a=0.0, L2b=0.0, L2c=0.0, L2d=0.0, L4a=0.0, L4b=0.0;
 double L14a=0.0, L14b=0.0, L22aii=0.0, L22aiii=0.0, L22bii=0.0, L22biii=0.0;
 double L36a=0.0, L36b=0.0;

 printf("Fed-1040 Schedule 1-A, 2025 - v%3.2f\n", thisversion);

 /* Decode any command-line arguments. */
 i = 1;  k=1;
 while (i < argc)
 {
  if (strcmp(argv[i],"-verbose")==0)  { verbose = 1; }
  else
  if (strcmp(argv[i],"-round_to_whole_dollars")==0)  { round_to_whole_dollars = 1; }
  else
  if (k==1)
   {
    infname = strdup(argv[i]);
    infile = fopen(infname,"r");
    if (infile==0) {printf("ERROR: Parameter file '%s' could not be opened.\n", infname ); exit(1);}
    k = 2;
    /* Base name of output file on input file. */
    strcpy(outfname,infname);
    j = strlen(outfname)-1;
    while ((j>=0) && (outfname[j]!='.')) j--;
    if (j<0) strcat(outfname,"_out.txt"); else strcpy(&(outfname[j]),"_out.txt");
    outfile = fopen(outfname,"w");
    if (outfile==0) {printf("ERROR: Output file '%s' could not be opened.\n", outfname); exit(1);}
    printf("Writing results to file:  %s\n", outfname);
   }
  else
   {printf("Unknown command-line parameter '%s'\n", argv[i]); exit(1);}
  i = i + 1;
 }

 if (infile==0) {printf("Error: No input file on command line.\n"); exit(1);}

 /* Pre-initialize all lines to zeros. */
 for (i=0; i<MAX_LINES; i++) { L[i] = 0.0; }

 /* Accept parameters from input file. */
 /* Expect  Sched C lines, something like:
	Title:  Sched C 1999 Return
	L1		{Gross Receipts}
	L2		{Returns and Allowances}
	. . .
 */

 /* Accept Form's "Title" line, and put out with date-stamp for your records. */
 read_line( infile, word );
 now = time(0);
 fprintf(outfile,"\n%s,	 v%2.2f, %s\n", word, thisversion, ctime( &now ));
 check_form_version( word, "Title:  Fed-1040 Schedule 1-A" );

 get_parameter( infile, 's', word, "FileName1040:" );      /* Preliminary Fed Return Output File-name. */
 get_word(infile, prelim_1040_outfilename );
 ImportFederalReturnData( prelim_1040_outfilename, &PrelimFedReturn );

 if (strlen( YourLastName ) > 0)
  {
   consume_leading_trailing_whitespace( YourLastName );
   strcpy( YourNames, YourLastName );
   strcat( YourNames, ", " );
   consume_leading_trailing_whitespace( Your1stName );
   strcat( YourNames, Your1stName );
   consume_leading_trailing_whitespace( Spouse1stName );
   if (Spouse1stName[0] != '\0')
    {
     strcat( YourNames, ", " );
     consume_leading_trailing_whitespace( SpouseLastName );
     if ((SpouseLastName[0] != '\0') && (strcmp( YourLastName, SpouseLastName ) != 0))
      {
       strcat( YourNames, SpouseLastName );
       strcat( YourNames, ", " );
      }
     strcat( YourNames, Spouse1stName );
    }
   fprintf(outfile,"YourNames: %s\n", YourNames );
  }

 if (your_socsec)
  fprintf(outfile, "YourSocSec#: %s\n", your_socsec );

 /* Part I */
 L[1] = PrelimFedReturn.fed_11b;
 showline( 1 );
 GetLineF( "L2a", &L2a );	
 GetLineF( "L2b", &L2b );	
 GetLineF( "L2c", &L2c );	
 GetLineF( "L2d", &L2d );	
 L[2] = L2a + L2b + L2c + L2d;
 showline_wlabelnz( "L2e", L[2] );
 L[3] = L[1] + L[2];
 showline( 3 );

 /* Part II */
 GetLineF( "L4a", &L4a );
 GetLineF( "L4b", &L4b );
 L[4] = LargerOf( L4a, L4b );
 showline_wlabelnz( "L4c", L[4] );
 GetLineF( "L5", &L[5] );
 L[6] = L[4] + L[5];
 showline( 6 );
 L[7] = SmallerOf( L[6], 25000.0 );
 showline( 7 );
 L[8] = L[3];
 showline( 8 );
 if (status == MARRIED_FILING_JOINTLY)
  L[9] = 300000.0;
 else
  L[9] = 150000.0;
 showline( 9 );
 L[10] = NotLessThanZero( L[8] - L[9] );
 showline( 10 );
 j = L[10] / 1000.0;
 L[11] = j;
 shownum( 11 );
 L[12] = 100.0 * L[11];
 showline( 12 );
 L[13] = NotLessThanZero( L[7] - L[12] );
 showline( 13 );

 /* Part III */
 GetLineF( "L14a", &L14a );
 GetLineF( "L14b", &L14b );
 L[14] = L14a + L14b;
 showline_wlabelnz( "L14c", L[14] );
 if (status == MARRIED_FILING_JOINTLY)
  L[15] = SmallerOf( L[14], 25000.0 );
 else
  L[15] = SmallerOf( L[14], 12500.0 );
 showline( 15 );
 L[16] = L[3];
 showline( 16 );
 if (status == MARRIED_FILING_JOINTLY)
  L[17] = 300000.0;
 else
  L[17] = 150000.0;
 showline( 17 );
 L[18] = NotLessThanZero( L[16] - L[17] );
 showline( 18 );
 j = L[18] / 1000.0;
 L[19] = j;
 shownum( 19 );
 L[20] = 100.0 * L[19];
 showline( 20 );
 L[21] = NotLessThanZero( L[15] - L[20] );
 showline( 21 );

 /* Part IV */
 GetTextLineF( "L22a" );
 GetLineF( "L22aii", &L22aii );
 GetLineF( "L22aiii", &L22aiii );
 GetTextLineF( "L22b" );
 GetLineF( "L22bii", &L22bii );
 GetLineF( "L22biii", &L22biii );
 L[23] = L22aiii + L22biii;
 showline( 23 );
 L[24] = SmallerOf( L[23], 10000.0 );
 showline( 24 );
 L[25] = L[3];
 showline( 25 );
 if (status == MARRIED_FILING_JOINTLY)
  L[26] = 200000.0;
 else
  L[26] = 100000.0;
 showline( 26 );
 L[27] = NotLessThanZero( L[25] - L[26] );
 showline( 27 );
 j = L[27] / 1000.0;
 L[28] = j;
 shownum( 28 );
 L[29] = 300.0 * L[28];
 showline( 20 );
 L[30] = NotLessThanZero( L[24] - L[29] );
 showline( 30 );

 /* Part V */
 L[31] = L[3];
 showline( 31 );
 if (status == MARRIED_FILING_JOINTLY)
  L[32] = 150000.0;
 else
  L[32] = 75000.0;
 showline( 32 );
 L[33] = NotLessThanZero( L[31] - L[32] );
 showline( 33 );
 L[34] = 0.06 * L[33];
 showline( 34 );
 L[35] = NotLessThanZero( 6000.0 - L[34] );
 showline( 35 );
 if (over65you)
  {
   L36a = L[35];
   showline_wlabel( "L36a", L36a );
  }
 if ((status == MARRIED_FILING_JOINTLY) && (over65spouse))
  {
   L36b = L[35];
   showline_wlabel( "L36b", L36b );
  }
 L[37] = L36a + L36b;
 showline( 37 );

 /* Part VI */
 L[38] = L[13] + L[21] + L[30] + L[37];
 showline( 38 );

 fclose(infile);
 grab_any_pdf_markups( infname, outfile );
 fclose(outfile);

 printf("\nListing results from file: %s\n\n", outfname);
 Display_File( outfname );

 return 0;
}
