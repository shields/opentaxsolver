/***
   Run_taxsolve_GUI.c - This program merely invokes "bin/ots_gui2.exe".
        It is a convenience, because it is easily found at the top directory
        of the OTS installation.  So new users can make a shortcut to it.
	And it enables starting the application by double-clicking on the
	start-icon (or file), unlike the previously used command-script.
	(Some window managers would open the command-script for editing
	 instead of executing it, since being a script, it was a text file.
	 Other window managers would ask whether to run or display, which
	 this is an uneeded extra step, and it did also confuse some users.
	)

  Compile:
	cc -O Run_taxsolve_GUI.c -o ../Run_taxsolve_GUI
***/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MaxStr 8192

char path[MaxStr], cmd[MaxStr], bin_path[MaxStr];
int verbose=0;


void caps_cpy( char *wrd )
{
 int j=0;
 while (wrd[j] != '\0')
  {
   wrd[j] = toupper( wrd[j] );
   j++;
  }
}


void my_strcpy( char *dst, char *src, int maxlen )
{
 int j=0;
 while ((j < maxlen-1) && (src[j] != '\0'))
  { 
   dst[j] = src[j];
   j++;
  }
 dst[j] = '\0';
}


#ifndef PLATFORM_KIND
 #define Posix_Platform  0 
 #define Mingw_Platform  1
 #define MsVisC_Platform 2
 #ifdef __CYGWIN32__
  #ifndef __CYGWIN__
   #define __CYGWIN__ __CYGWIN32__
  #endif
 #endif
 #if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MSYS__)
  #define PLATFORM_KIND Mingw_Platform /* MinGW or like platform */
 #elif defined(__WIN32) || defined(WIN32)
  #define PLATFORM_KIND MsVisC_Platform /* microsoft visual C */
 #else
  #define PLATFORM_KIND Posix_Platform    /* Posix/Linux/Unix */
 #endif
#endif


int main( int argc, char *argv[] )
{
 char *substr;
 int k=1;

 while (k < argc)
  {
   if (strcmp( argv[k], "-v" ) == 0)
    {
     verbose = 1;
     printf("Verbose mode ON.\n");
    }
   else
    printf("Unknown option '%s'\n", argv[k] );
   k++;
  }

 strncpy( path, argv[0], MaxStr );
 if (verbose) printf("RunTaxSolve invoked as: '%s'\n", path );

 #if (PLATFORM_KIND != Posix_Platform)

  { /*mswin*/
   char drive_letter='c';
   caps_cpy( path );
   if (verbose) printf("\tPath: '%s'\n", path );
   if (path[1] == ':')
    { /*Absolute_reference*/
     drive_letter = path[0];
     if (verbose) printf("\tAbsolutePath: driveletter = %c\n", drive_letter );
     my_strcpy( bin_path, &(path[2]), MaxStr - 1 );
     if (verbose) printf("\t BinPath = '%s'\n", bin_path );
     substr = strstr(bin_path,"\\RUN_TAXSOLVE_GUI");
     if (substr != 0) 
      {
       substr[0] = '\0';
       strcat( bin_path, "\\bin" );
       sprintf(cmd,"set path=%c:%s;%%PATH%% & start /B %c:\"%s\\ots_gui2.exe\"", drive_letter, bin_path, drive_letter, bin_path );
      }
     else { printf("Unexpected Error\n");  exit(1); }
    } /*Absolute_reference*/
   else 
    { /*Local_reference*/
      //     printf("Start path must begin with drive-letter:\n");
      //     if ((path[0] != '/') && (path[0] != '\\')) exit(1);
      if (verbose) printf("\tLocalReference: (no colon in path)\n");
      my_strcpy( bin_path, path, MaxStr - 1 );
      if (verbose) printf("\t BinPath = '%s'\n", bin_path );
      substr = strstr(bin_path,"RUN_TAXSOLVE_GUI");
      if (substr != 0)
       {
        substr[0] = '\0';
        if (strlen( bin_path ) > 0)
	 strcat( bin_path, "bin" );
        else
	 strcpy( bin_path, "bin" );
	sprintf(cmd,"set path=%s;%%PATH%% & start /B %s\\ots_gui2.exe", bin_path, bin_path );
       }
      else { printf("Unexpected Error2\n");  exit(1); }
    } /*Local_reference*/
  } /*mswin*/

 #else

  { /*posix*/
   int j;
   substr = strstr( path, "Run_taxsolve_GUI" );
   if (substr == 0) { printf("Could not find path.\nSaw: '%s'\n", bin_path);  exit(1); }
   substr[0] = '\0';	/* The "path" variable now holds the path to OTS's top directory. */
   if (verbose) printf("\tOTS Path = '%s'\n", path );

   strcpy( bin_path, path );
   j = strlen( bin_path );
   if ((j > 0) && (bin_path[j-1] != '/')) strcat( bin_path, "/" );
   if (j == 0) strcpy( bin_path, "./" );
   strcat( bin_path, "bin/ots_gui2" );

   #ifdef __APPLE__
    /* Apple's Finder starts OTS with "PWD" set to user's home-dir, instead of OTS-dir. */
    /* So "cd" to OTS just before starting GUI. */
    strcpy( cmd, "cd \"" );
    strcat( cmd, path );
    strcat( cmd, "\" ; " );
    strcat( cmd, "\"" );
    strcat( cmd, bin_path );
    strcat( cmd, "\" &" );
   #else
    /* Linux, etc. */
    strcpy( cmd, "\"" );
    strcat( cmd, bin_path );
    strcat( cmd, "\" &" );
   #endif
  } /*posix*/

 #endif

 printf("Issuing: '%s'\n", cmd );
 system( cmd );

 return 0;
}

