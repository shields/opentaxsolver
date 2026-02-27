/************************************************************************/
/* TaxSolve_Form_6781.c - 2024                                          */
/*  User contributed.							*/
/*                                                                      */
/* GNU Public License - GPL:                                            */
/* This program is free software; you can redistribute it and/or        */
/* modify it under the terms of the GNU General Public License as       */
/* published by the Free Software Foundation; either version 2 of the   */
/* License, or (at your option) any later version.                      */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU     */
/* General Public License for more details.                             */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with this program; if not, write to the Free Software          */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA             */
/* 02111-1307 USA.                                                      */
/*                                                                      */
/************************************************************************/

float thisversion=1.00;

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "taxsolve_routines.c"

#define F6781_MAX_ENTRIES 100
#define F6781_MAX_LINE_LEN 4096

enum { No = 0, Yes = 1 };

int BoxA , BoxB , BoxC , BoxD ;

typedef struct
{
    char* account_id;
    double loss;
    double gain;

} PartI_L1_GainLoss;

typedef struct
{
  char*             description;
  double            gross_proceeds;
  struct date_rec   open_date;
  double            cost_basis;
  struct date_rec   close_date;
  double            offsetting_gain;

} PartII_L10_Loss;

typedef struct
{
  char*             description;
  double            gross_proceeds;
  struct date_rec   open_date;
  double            cost_basis;
  struct date_rec   close_date;

} PartII_L12_Gain;

typedef struct
{
  char*             description;
  struct date_rec   open_date;
  double            cost_basis;
  double            year_end_fmv;

} PartIII_L14_MarkMktGains;


static PartI_L1_GainLoss        L1_1256_contracts[ F6781_MAX_ENTRIES ];
static PartII_L10_Loss          L10_losses[ F6781_MAX_ENTRIES ];
static PartII_L12_Gain          L12_gains[ F6781_MAX_ENTRIES ];
static PartIII_L14_MarkMktGains L14_mrkmrkt_gains[ F6781_MAX_ENTRIES ];

static unsigned int num_L1_1256_contracts = 0;
static unsigned int num_L10_losses = 0;
static unsigned int num_L12_gains = 0;
static unsigned int num_L14_mrkmrkt_gains = 0;

static double L11a, L11b;
static double L13a, L13b;


static void decode_command_line_args( int argc, char* argv[] );
static void get_part1_L1( FILE* infile );
static void get_part2_L10( FILE* infile );
static void get_part2_L12( FILE* infile );
static void get_part3_L14( FILE* infile );
static void expect_no_semicolon( const char* const word, const unsigned int entry_idx );
static double checked_strtod( const char* const word, const unsigned int entry_idx, const char col );

static char* infname;
static char* outfname;

int main( int argc, char *argv[] )
{
  int i;
  char word[4000];
  time_t now;

  /* line entry variables L[n] are declared in taxsolve_routines.c */

  printf("Form 6781, 2025 - v%3.2f\n", thisversion);

  /* Decode any command-line arguments. */
  decode_command_line_args( argc, argv );

  /* Pre-initialize all lines to zeros. */
  for (i=0; i<MAX_LINES; i++)
  {
    L[i] = 0.0;
  }
  L11a = L11b = 0.0;
  L13a = L13b = 0.0;
  memset( L1_1256_contracts, '\0', F6781_MAX_ENTRIES * sizeof( PartI_L1_GainLoss ) );
  memset( L10_losses, '\0', F6781_MAX_ENTRIES * sizeof( PartII_L10_Loss ) );
  memset( L12_gains, '\0', F6781_MAX_ENTRIES * sizeof( PartII_L12_Gain ) );
  memset( L14_mrkmrkt_gains, '\0', F6781_MAX_ENTRIES * sizeof( PartIII_L14_MarkMktGains ) );
  BoxA = BoxB = BoxC = BoxD = 0;

  /* Accept Form's "Title" line, and put out with date-stamp for your records. */
  read_line( infile, word );
  now = time(0);
  fprintf(outfile,"\n%s,  v%2.2f, %s\n", word, thisversion, ctime( &now ));
  check_form_version( word, "Title: Form 6781 - Gains and Losses From Section 1256 Contracts and Straddles" );

  fprintf(outfile,"\n--- THIS IS PRELIMINARY USER-CONTRIBUTED FORM ---\n");
  //X MarkupPDF( 1, 240, 40, 17, 1.0, 0, 0 ) NotReady "This program is NOT updated for 2025."
  add_pdf_markup( "NotReady", 1, 240, 40, 17, 1, 1.0, 0, 0, "\"This program is NOT ready for 2025.\"" );

  /* ----- Accept form data and process the numbers.         ------ */
  /* ----- Place all your form-specific code below here .... ------ */

  // Example:
  //  GetLineF( "L2", &L[2] );
  //  GetLineF( "L3", &L[3] );
  //  L[4] = L[2] - L[3];
  //  showline_wlabel( "L4", L[4] );

  GetTextLineF( "YourName:" );
  GetTextLineF( "YourSocSec#:" );

  GetYesNo1( "BoxA", &BoxA );
  GetYesNo1( "BoxB", &BoxB );
  GetYesNo1( "BoxC", &BoxC );
  GetYesNo1( "BoxD", &BoxD );
  Show_String_wLabel( "BoxA:", BoxA ? "X" : "" );
  Show_String_wLabel( "BoxB:", BoxB ? "X" : "" );
  Show_String_wLabel( "BoxC:", BoxC ? "X" : "" );
  Show_String_wLabel( "BoxD:", BoxD ? "X" : "" );

  double L2b, L2c;
  L2b = L2c = 0.0;
  get_part1_L1( infile );
  for ( int i = 0; i < num_L1_1256_contracts; i++ )
  {
    /* The form only holds three entries. */
    if ( i >= 3 ) break;

    fprintf( outfile, "L1row%d_a: %s\n", i+1, L1_1256_contracts[ i ].account_id );
    fprintf( outfile, "L1row%d_b  %0.2f\n", i+1, L1_1256_contracts[ i ].loss );
    fprintf( outfile, "L1row%d_c  %0.2f\n", i+1, L1_1256_contracts[ i ].gain );
    L2b += L1_1256_contracts[ i ].loss;
    L2c += L1_1256_contracts[ i ].gain;
  }
  if ( L2b >= 0.0 )
  {
    fprintf( stderr, "Values in Line 1, column (b) should be negative. Their sum is positive.\n" );
    exit( 1 );
  }
  showline_wlabelnz( "L2b", L2b );
  showline_wlabelnz( "L2c", L2c );
  L[3] = L2b + L2c;
  ShowLineNonZero( 3 );

  GetLineF( "L4", &L[4] );
  L[5] = L[3] + L[4];
  ShowLineNonZero( 5 );

  GetLineF( "L6", &L[6] );
  if ( ! BoxD ) { L[6] = 0.0; }
  L[7] = L[5] + L[6];
  ShowLineNonZero( 7 );

  L[8] = L[7] * 0.40;
  ShowLineNonZero_wMsg( 8, "Include on line 4 of Schedule D or on Form 8949. See instructions." );
  L[9] = L[7] * 0.60;
  ShowLineNonZero_wMsg( 9, "Include on line 11 of Schedule D or on Form 8949. See instructions." );

  get_part2_L10( infile );
  for ( int i = 0; i < num_L10_losses; i++ )
  {
    /* The form only holds two entries. */
    if ( i >= 2 ) break;

    char date[ 32 ];
    char label[ 32 ];
    double loss_col_f;
    double loss_col_h;
    fprintf( outfile, "L10row%d_a: %s\n", i+1, L10_losses[ i ].description );
    date_rec_to_str( date, &L10_losses[ i ].open_date );
    fprintf( outfile, "L10row%d_b: %s\n", i+1, date );
    date_rec_to_str( date, &L10_losses[ i ].close_date );
    fprintf( outfile, "L10row%d_c: %s\n", i+1, date );
    sprintf( label, "L10row%d_d", i+1 );
    showline_wlabel( label, L10_losses[ i ].gross_proceeds );
    sprintf( label, "L10row%d_e", i+1 );
    showline_wlabel( label, L10_losses[ i ].cost_basis );
    sprintf( label, "L10row%d_f", i+1 );
    loss_col_f = fmax( 0.0, L10_losses[ i ].cost_basis - L10_losses[ i ].gross_proceeds );
    showline_wlabel( label, loss_col_f );
    sprintf( label, "L10row%d_g", i+1 );
    showline_wlabel( label, L10_losses[ i ].offsetting_gain );
    sprintf( label, "L10row%d_h", i+1 );
    loss_col_h = fmax( 0.0, loss_col_f - L10_losses[ i ].offsetting_gain );
    showline_wlabel( label, loss_col_h );
  }
  GetLineF( "L11a", &L11a );
  GetLineF( "L11b", &L11b );

  get_part2_L12( infile );
  for ( int i = 0; i < num_L12_gains; i++ )
  {
    /* The form only holds two entries. */
    if ( i >= 2 ) break;

    char date[ 32 ];
    char label[ 32 ];
    double gain_col_f;
    fprintf( outfile, "L12row%d_a: %s\n", i+1, L12_gains[ i ].description );
    date_rec_to_str( date, &L12_gains[ i ].open_date );
    fprintf( outfile, "L12row%d_b: %s\n", i+1, date );
    date_rec_to_str( date, &L12_gains[ i ].close_date );
    fprintf( outfile, "L12row%d_c: %s\n", i+1, date );
    sprintf( label, "L12row%d_d", i+1 );
    showline_wlabel( label, L12_gains[ i ].gross_proceeds );
    sprintf( label, "L12row%d_e", i+1 );
    showline_wlabel( label, L12_gains[ i ].cost_basis );
    sprintf( label, "L12row%d_f", i+1 );
    gain_col_f = fmax( 0.0, L12_gains[ i ].gross_proceeds - L12_gains[ i ].cost_basis );
    showline_wlabel( label, gain_col_f );
  }
  GetLineF( "L13a", &L13a );
  GetLineF( "L13b", &L13b );

  get_part3_L14( infile );
  for ( int i = 0; i < num_L14_mrkmrkt_gains; i++ )
  {
    /* The form only holds three entries. */
    if ( i >= 3 ) break;

    char date[ 32 ];
    char label[ 32 ];
    double gain_col_e;
    fprintf( outfile, "L14row%d_a: %s\n", i+1, L14_mrkmrkt_gains[ i ].description );
    date_rec_to_str( date, &L14_mrkmrkt_gains[ i ].open_date );
    fprintf( outfile, "L14row%d_b: %s\n", i+1, date );

    sprintf( label, "L14row%d_c", i+1 );
    showline_wlabel( label, L14_mrkmrkt_gains[ i ].year_end_fmv );
    sprintf( label, "L14row%d_d", i+1 );
    showline_wlabel( label, L14_mrkmrkt_gains[ i ].cost_basis );
    sprintf( label, "L14row%d_e", i+1 );
    gain_col_e = fmax( 0.0, L14_mrkmrkt_gains[ i ].year_end_fmv - L14_mrkmrkt_gains[ i ].cost_basis );
    showline_wlabel( label, gain_col_e );
  }

  ///////////////////////////////////////////////////////////////////////////////////////

  grab_any_pdf_markups( infname, outfile );

  fclose(infile);
  fclose(outfile);

  printf("\nListing results from file: %s\n\n", outfname);
  Display_File( outfname );

  return 0;
}

static void decode_command_line_args( int argc, char* argv[] )
{
  const char suffix[] = "_out.txt";
  const unsigned int max_filename_len = F6781_MAX_LINE_LEN - strlen( suffix );
  int i = 1, k = 1;
  infname = argv[i];

  while (i < argc)
  {
    if (strcmp(argv[i],"-verbose")==0)  { verbose = 1; }
    else
    {
      if (k++==1)
      {
        infile = fopen(infname,"r");
        if (infile==0)
        {
          printf("ERROR: Parameter file '%s' could not be opened.\n", infname );
          exit(1);
        }
        /* Base name of output file on input file. */
        if ( strnlen( infname, max_filename_len ) >= max_filename_len )
        {
          fprintf( stderr, "Program argument %d file name is too long. Must be < %d bytes\n", i, max_filename_len );
          exit( 1 );
        }
        // outfname = strndup( infname, max_filename_len );
        const int infname_len = strnlen( infname, max_filename_len );
        if ( infname_len > max_filename_len - strlen( suffix ) )
        {
          fprintf( stderr, "Input file name is too long.\n" );
          exit( 1 );
        }
        outfname = malloc( infname_len + strlen( suffix ) );
        strcpy( outfname, infname );
        char* dot = strrchr( outfname, '.' );
        /* If '.' was not found, append suffix to end of file name. */
        dot = ( dot == NULL ) ? outfname + strnlen( outfname, max_filename_len ) : dot;
        strncpy( dot, suffix, strlen( suffix ) ); 
        outfile = fopen(outfname,"w");
        if (outfile==0)
        {
          printf("ERROR: Output file '%s' could not be opened.\n", outfname);
          exit(1);
        }
        printf("Writing results to file:  %s\n", outfname);
      }
      else
      {
        printf("Unknown command-line parameter '%s'\n", argv[i]);
        exit(1);
      }
    }
    i = i + 1;
  }
  if (infile==0) {printf("Error: No input file on command line.\n"); exit(1);}
}


static void get_part1_L1( FILE* infile )
{
  char word[ F6781_MAX_LINE_LEN ];

  enum { col_b, col_c, col_a, num_cols } column = col_b;

  /* Index into L1_1256_contracts */
  unsigned int entry_idx = 0;

  get_parameter( infile, 'l', word, "L1" );
  if ( 0 != strcmp( "L1", word ) )
  {
    fprintf( stderr, "get_part1_L1: Expected label \"L1\"\n" );
    exit(1);
  }
  word[ 0 ] = '\0';
  while ( 1 )
  {
    if ( feof( infile ) )
    {
      fprintf( stderr, "Unexpected EOF while reading Line 1 data.\n" );
      exit( 1 );
    }
    switch ( column )
    {
      case col_b:
        get_word( infile, word );
        if (  word[ 0 ] == ';' ) { goto L_done_reading_L1; }
        errno = 0;
        L1_1256_contracts[ entry_idx ].loss = checked_strtod( word, entry_idx, 'b' );
        column = col_c;
        break;
      case col_c:
        get_word( infile, word );
        expect_no_semicolon( word, entry_idx );
        errno = 0;
        L1_1256_contracts[ entry_idx ].gain = checked_strtod( word, entry_idx, 'c' );
        column = col_a;
        break;
      case col_a:
        get_comment( infile, word );
        L1_1256_contracts[ entry_idx ].account_id = strndup( word, strnlen( word, F6781_MAX_LINE_LEN ) );
        column = col_b;
        entry_idx++;
          break;
      default:
        fprintf( stderr, "Internal error scanning L1.\n" );
        exit( 1 );
    }
  }
L_done_reading_L1:
  num_L1_1256_contracts = entry_idx;
  return;
}


static void get_part2_L10( FILE* infile )
{
  char word[ F6781_MAX_LINE_LEN ];
  char errmsg[ F6781_MAX_LINE_LEN ];

  enum { col_b, col_c, col_d, col_e, col_g, col_a, num_cols } column = col_a;

  /* Index into L10_losses */
  unsigned int entry_idx = 0;

  get_parameter( infile, 'l', word, "L10" );
  if ( 0 != strcmp( "L10", word ) )
  {
    fprintf( stderr, "get_part2_L10: Expected label \"L10\"\n" );
    exit(1);
  }
  word[ 0 ] = '\0';
  while ( 1 )
  {
    if ( feof( infile ) )
    {
      fprintf( stderr, "Unexpected EOF while reading Line 10 data.\n" );
      exit( 1 );
    }
    switch ( column )
    {
      case col_a:
        get_comment( infile, word );
        /* If there isn't a comment found, check for a ';', which terminates the list of entries.
         * Otherwise there is an input error. */
        if ( word[ 0 ] == '\0' )
        {
          get_word( infile, word );
          if ( word[ 0 ] == ';' ) { goto L_done_reading_L10; }
          else
          {
            fprintf( stderr, "Expected comment, L10 entry#%d, column (a)\n", entry_idx +1 );
            exit( 1 );
          }
        }
        L10_losses[ entry_idx ].description = strndup( word, strnlen( word, F6781_MAX_LINE_LEN ) );
        column = col_b;
        break;
      case col_b:
        snprintf( errmsg, F6781_MAX_LINE_LEN, "L10 entry#%d, column (b)", entry_idx +1 );
        get_word( infile, word );
        expect_no_semicolon( word, entry_idx );
        gen_date_rec( word, errmsg, &L10_losses[ entry_idx ].open_date );
        column = col_c;
        break;
      case col_c:
        snprintf( errmsg, F6781_MAX_LINE_LEN, "L10 entry#%d, column (c)", entry_idx +1 );
        get_word( infile, word );
        expect_no_semicolon( word, entry_idx );
        gen_date_rec( word, errmsg, &L10_losses[ entry_idx ].close_date );
        column = col_d;
        break;
      case col_d:
        get_word( infile, word );
        expect_no_semicolon( word, entry_idx );
        L10_losses[ entry_idx ].gross_proceeds = checked_strtod( word, entry_idx, 'd' );
        column = col_e;
        break;
      case col_e:
        get_word( infile, word );
        expect_no_semicolon( word, entry_idx );
        L10_losses[ entry_idx ].cost_basis = checked_strtod( word, entry_idx, 'e' );
        column = col_g;
        break;
      case col_g:
        get_word( infile, word );
        expect_no_semicolon( word, entry_idx );
        L10_losses[ entry_idx ].offsetting_gain = checked_strtod( word, entry_idx, 'g' );
        column = col_a;
        entry_idx++;
        break;
      default:
        fprintf( stderr, "Internal error scanning L10.\n" );
        exit( 1 );
    }
  }
L_done_reading_L10:
  num_L10_losses = entry_idx;
  return;
}


static void get_part2_L12( FILE* infile )
{
  char word[ F6781_MAX_LINE_LEN ];
  char errmsg[ F6781_MAX_LINE_LEN ];

  enum { col_b, col_c, col_d, col_e, col_a, num_cols } column = col_a;

  /* Index into L12_gains */
  unsigned int entry_idx = 0;

  get_parameter( infile, 'l', word, "L12" );
  if ( 0 != strcmp( "L12", word ) )
  {
    fprintf( stderr, "get_part2_L12: Expected label \"L12\"\n" );
    exit(1);
  }
  word[ 0 ] = '\0';
  while ( 1 )
  {
    if ( feof( infile ) )
    {
      fprintf( stderr, "Unexpected EOF while reading Line 12 data.\n" );
      exit( 1 );
    }
    switch ( column )
    {
      case col_a:
        get_comment( infile, word );
        /* If there isn't a comment found, check for a ';', which terminates the list of entries.
         * Otherwise there is an input error. */
        if ( word[ 0 ] == '\0' )
        {
          get_word( infile, word );
          if ( word[ 0 ] == ';' ) { goto L_done_reading_L12; }
          else
          {
            fprintf( stderr, "Expected comment, L12 entry#%d, column (a)", entry_idx +1 );
            exit( 1 );
          }
        }
        L12_gains[ entry_idx ].description = strndup( word, strnlen( word, F6781_MAX_LINE_LEN ) );
        column = col_b;
        break;
      case col_b:
        snprintf( errmsg, F6781_MAX_LINE_LEN, "L12 entry#%d, column (b)", entry_idx +1 );
        get_word( infile, word );
        if ( word[ 0 ] == ';' ) { goto L_done_reading_L12; }
        gen_date_rec( word, errmsg, &L12_gains[ entry_idx ].open_date );
        column = col_c;
        break;
      case col_c:
        snprintf( errmsg, F6781_MAX_LINE_LEN, "L12 entry#%d, column (c)", entry_idx +1 );
        get_word( infile, word );
        expect_no_semicolon( word, entry_idx );
        gen_date_rec( word, errmsg, &L12_gains[ entry_idx ].close_date );
        column = col_d;
        break;
      case col_d:
        get_word( infile, word );
        expect_no_semicolon( word, entry_idx );
        L12_gains[ entry_idx ].gross_proceeds = checked_strtod( word, entry_idx, 'd' );
        column = col_e;
        break;
      case col_e:
        get_word( infile, word );
        expect_no_semicolon( word, entry_idx );
        L12_gains[ entry_idx ].cost_basis = checked_strtod( word, entry_idx, 'e' );
        column = col_a;
        entry_idx++;
        break;
      default:
        fprintf( stderr, "Internal error scanning L12.\n" );
        exit( 1 );
    }
  }
L_done_reading_L12:
  num_L12_gains = entry_idx;
  return;
}


static void get_part3_L14( FILE* infile )
{
  char word[ F6781_MAX_LINE_LEN ];
  char errmsg[ F6781_MAX_LINE_LEN ];

  enum { col_a, col_b, col_c, col_d, num_cols } column = col_a;

  /* Index into L14_mrkmrkt_gains */
  unsigned int entry_idx = 0;

  get_parameter( infile, 'l', word, "L14" );
  if ( 0 != strcmp( "L14", word ) )
  {
    fprintf( stderr, "get_part3_L14: Expected label \"L14\"\n" );
    exit(1);
  }
  word[ 0 ] = '\0';
  while ( 1 )
  {
    if ( feof( infile ) )
    {
      fprintf( stderr, "Unexpected EOF while reading Line 12 data.\n" );
      exit( 1 );
    }
    switch ( column )
    {
      case col_a:
        get_comment( infile, word );
        /* If there isn't a comment found, check for a ';', which terminates the list of entries.
         * Otherwise there is an input error. */
        if ( word[ 0 ] == '\0' )
        {
          get_word( infile, word );
          if ( word[ 0 ] == ';' ) { goto L_done_reading_L14; }
          else
          {
            fprintf( stderr, "Expected comment, L14 entry#%d, column (a)", entry_idx +1 );
            exit( 1 );
          }
        }
        L14_mrkmrkt_gains[ entry_idx ].description = strndup( word, strnlen( word, F6781_MAX_LINE_LEN ) );
        column = col_b;
        break;
      case col_b:
        snprintf( errmsg, F6781_MAX_LINE_LEN, "L14 entry#%d, column (b)", entry_idx +1 );
        get_word( infile, word );
        if ( word[ 0 ] == ';' ) { goto L_done_reading_L14; }
        gen_date_rec( word, errmsg, &L14_mrkmrkt_gains[ entry_idx ].open_date );
        column = col_c;
        break;
      case col_c:
        snprintf( errmsg, F6781_MAX_LINE_LEN, "L14 entry#%d, column (c)", entry_idx +1 );
        get_word( infile, word );
        expect_no_semicolon( word, entry_idx );
        L14_mrkmrkt_gains[ entry_idx ].year_end_fmv = checked_strtod( word, entry_idx, 'c' );
        column = col_d;
        break;
      case col_d:
        get_word( infile, word );
        expect_no_semicolon( word, entry_idx );
        L14_mrkmrkt_gains[ entry_idx ].cost_basis = checked_strtod( word, entry_idx, 'd' );
        column = col_a;
        entry_idx++;
        break;
      default:
        fprintf( stderr, "Internal error scanning L14.\n" );
        exit( 1 );
    }
  }
L_done_reading_L14:
  num_L14_mrkmrkt_gains = entry_idx;
  return;
}


static void expect_no_semicolon( const char* const word, const unsigned int entry_idx )
{
  if (  word[ 0 ] == ';' )
  {
      fprintf( stderr, "Wrong number of columns in Line 1 entry%d.\n", entry_idx +1 );
      exit( 1 );
  }
}

static double checked_strtod( const char* const word, const unsigned int entry_idx, const char col )
{
  char* endptr;
  double d = strtod( word, &endptr );
  errno = 0;
  if ( errno != 0 || word == endptr )
  {
    fprintf( stderr, "Number conversion error for Line 1 entry #%d, column (%c)\n", entry_idx +1, col );
    exit( 1 );
  }
  return d;
}
