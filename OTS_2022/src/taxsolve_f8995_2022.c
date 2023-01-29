/************************************************************************/
/* taxsolve_f8995.c -                                                   */
/* Contributed by Rylan Luke, 1/2023					*/
/************************************************************************/

float thisversion=1.00;

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "taxsolve_routines.c"


/*----------------------------------------------------------------------------*/

int main( int argc, char *argv[] )
{
 int i, j, k;
 char word[6000], outfname[6000], *infname=0;
 time_t now;
 // Local line number variables for direct/indirect lines

 printf("Form 8995, 2022 - v%3.2f\n", thisversion);

 /* Decode any command-line arguments. */
 i = 1;  k=1;
 while (i < argc)
 {
  if (strcmp(argv[i],"-verbose")==0)  { verbose = 1; }
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
 /* Expect lines, something like:
        Title:  Form XXXX Return
        L2              {Returns and Allowances}
        . . .
 */


 /* Accept Form's "Title" line, and put out with date-stamp for your records. */
 read_line( infile, word );
 now = time(0);
 fprintf(outfile,"\n%s,  v%2.2f, %s\n", word, thisversion, ctime( &now ));
 check_form_version( word, "Title: 2022 Form 8995" );

 /* ----- Accept form data and process the numbers.         ------ */
 /* ----- Place all your form-specific code below here .... ------ */

 // Example:
 //  GetLineF( "L2", &L[2] );
 //  GetLineF( "L3", &L[3] );
 //  L[4] = L[2] - L[3];
 //  showline_wlabel( "L4", L[4] );

 GetTextLineF( "YourName:" );
 GetTextLineF( "YourSocSec#:" );


 char * L1_row_names[] = { "i", "ii", "iii", "iv", "v" };
 char * L1_col_names[] = { "a", "b", "c" };

 double L1[sizeof(L1_row_names)/sizeof(char *)][sizeof(L1_col_names)/sizeof(char *)];
 char   L1_name[sizeof(L1_row_names)/sizeof(char *)][sizeof(L1_col_names)/sizeof(char *)][100];

 // Set total income value to 0
 L[2] = 0.0;
 for (int row = 0; row < sizeof(L1_row_names)/sizeof(char *); ++row) {
    for (int col = 0; col < sizeof(L1_col_names)/sizeof(char *); ++col) {
        sprintf(L1_name[row][col], "L1_%s_%s%s", L1_row_names[row], L1_col_names[col], (col == 2) ? "" : ":");
        printf("L1_name: %s\n", L1_name[row][col]);
        if (col != 2)
            GetTextLineF(L1_name[row][col]) ;
        else
            GetLineFnz(L1_name[row][col], &L1[row][col] );
    }
    // Add to total income
    L[2] += L1[row][2];
 }

 GetLineFnz( "L3", &L[3] );
 GetLineFnz( "L6", &L[6] );
 GetLineFnz( "L7", &L[7] );
 GetLineFnz( "L11", &L[11] );
 GetLineFnz( "L12", &L[12] );

 double qbi_percentage = 0.20;

 // Total qualified business income
 L[4] = NotLessThanZero(L[2] + L[3]);
 // Qualified business income component; mult by 20%
 L[5] = L[4] * qbi_percentage;
 // Total qualified REIT dividends and PTP income
 L[8] = NotLessThanZero(L[6] + L[7]);
 // REIT and PTP component
 L[9] = L[8] * qbi_percentage;

 L[10] = L[5] + L[9];
 L[13] = NotLessThanZero( L[11] - L[12] );
 L[14] = L[13] * qbi_percentage;
 L[15] = SmallerOf( L[10], L[14] );

 L[16] = L[2] + L[3];
 if (L[16] > 0.0)
    L[16] = 0.0;

 L[17] = L[6] + L[7];
 if (L[17] > 0.0)
    L[17] = 0.0;

 showline( 2 );
 ShowLineNonZero( 3 );
 showline( 4 );
 showline( 5 );

 ShowLineNonZero( 6 );
 ShowLineNonZero( 7 );
 showline       ( 8 );
 showline       ( 9 );

 showline( 10 );
 showline( 11 );
 ShowLineNonZero( 12 );
 showline( 13 );
 showline( 14 );
 showline( 15 );
 ShowLineNonZero( 16 );
 ShowLineNonZero( 17 );



 /* ----- .... Until here.  ----- */


 /***
    Summary of useful functions:
	GetLine( "label", &variable )	- Looks for "label" in input file, and places the corresponding sum of 
					  values following that label (until ";") into variable.
	GetLineF( "label", &variable )	- Like GetLine() above, but also writes the result to the output file.
	GetLineFnz(( "label", &variable ) - Like GetLine(), but only writes non-zero values to the output file.
	GetLine1( "label", &variable )  - Like GetLine() above, but expects single value (no sum, no ";" in input file).

	c = SmallerOf( a, b );		- Selects smaller of two values.
	c = LargerOf( a, b );		- Selects larger of two values.
	c = NotLessThanZero( a );	- Selects positive value or zero. Prevents negative values.

	showline( j )			- Writes currency value of L[j] to output file with label in nice format.
	shownum( j )			- Writes integer value of L[j] to output file with label in nice format.
	showline_wmsg( j, "msg" )	- Like showline, but adds the provided message to the output line.
	ShowLineNonZero( j )		- Like showline, but only writes non-zero values.
	ShowLineNonZero_wMsg( j, "msg" ) - Like showline_wmsg, but only writes non-zero values.
	showline_wlabel( "label", value ) - For custom line names and variables not in the default L[] array.
	showline_wlabelnz( "label", value ) - Like showline_wlabel, but only writes non-zero values.
	showline_wlabelmsg( "label", value, "msg" ) - Like showline_wlabel,but adds the provided message to the output line.
	
  ***/

 fclose(infile);
 grab_any_pdf_markups( infname, outfile );
 fclose(outfile);

 printf("\nListing results from file: %s\n\n", outfname);
 Display_File( outfname );

 return 0;
}

