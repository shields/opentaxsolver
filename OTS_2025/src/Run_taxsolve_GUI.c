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

  GNU Public License - GPL:
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.

  Compile:
	cc -O Run_taxsolve_GUI.c -o ../Run_taxsolve_GUI
***/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MaxStr 8192

char path[MaxStr], cmd[MaxStr], cmd2[MaxStr], bin_path[MaxStr];
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


void safe_strcpy( char *dst, char *src, int maxlen )
{
 int j=0;
 while ((j < maxlen-1) && (src[j] != '\0'))
  { 
   dst[j] = src[j];
   j++;
  }
 dst[j] = '\0';
}

void safe_strcat( char *dst, char *src, int maxlen )
{
 int j=0, k;
 k = strlen( dst );
 while ((k < maxlen-1) && (src[j] != '\0'))
  { 
   dst[k++] = src[j++];
  }
 dst[k] = '\0';
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
 int k=1, gui_version_to_invoke=2, gui_size=0, darkmode=0;

 while (k < argc)
  {
   if (strcmp( argv[k], "-gui3" ) == 0)
    {
     gui_version_to_invoke = 3;
     printf("Will use 'ots_gui%d'.\n", gui_version_to_invoke );
    }
   else
   if (strcmp( argv[k], "-large" ) == 0)
    {
     gui_size = 1;
     gui_version_to_invoke = 3;
     printf("Will use Large-mode with 'ots_gui%d'.\n", gui_version_to_invoke );
    }
   else
   if (strcmp( argv[k], "-darkmode" ) == 0)
    {
     darkmode = 1;
     gui_version_to_invoke = 3;
     printf("Will use 'dark-mode' with 'ots_gui%d'.\n", gui_version_to_invoke );
    }
   else
   if (strcmp( argv[k], "-v" ) == 0)
    {
     verbose = 1;
     printf("Verbose mode ON.\n");
    }
   else
   if (strcmp( argv[k], "-help" ) == 0)
    {
     printf("RunTaxSolve Help:\n");
     printf(" This program is a convenient script for starting the OTS GUI\n");
     printf(" in a way that is unified and portable to all platforms.\n");
     printf("\n");
     printf(" Options:\n");
     printf("  -v         - Verbose mode,\n");
     printf("  -gui3      - Use the newer (Gtk-3) version of the OTS GUI.\n");
     printf("  -large     - Large GUI mode for High-DPI screens.\n");
     printf("  -darkmode  - Setss GUI to use 'dark-mode'.\n");
     printf("\n");
     exit(0);
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
     safe_strcpy( bin_path, &(path[2]), MaxStr );
     if (verbose) printf("\t BinPath = '%s'\n", bin_path );
     substr = strstr(bin_path,"\\RUN_TAXSOLVE_GUI");
     if (substr != 0) 
      {
       substr[0] = '\0';
       safe_strcat( bin_path, "\\bin", MaxStr );
       sprintf(cmd,"set path=%c:%s;%%PATH%% & ", drive_letter, bin_path );

       if (gui_size)
        safe_strcat( cmd, "set GDK_SCALE=2 & ", MaxStr );
       if (darkmode)
        safe_strcat( cmd, "set GTK_THEME=Adwaita:dark & ", MaxStr );

       sprintf(cmd2,"start /B %c:\"%s\\ots_gui%d.exe\"", 
		drive_letter, bin_path, gui_version_to_invoke );
       safe_strcat( cmd, cmd2, MaxStr );
      }
     else { printf("Unexpected Error\n");  exit(1); }
    } /*Absolute_reference*/
   else 
    { /*Local_reference*/
      //     printf("Start path must begin with drive-letter:\n");
      //     if ((path[0] != '/') && (path[0] != '\\')) exit(1);
      if (verbose) printf("\tLocalReference: (no colon in path)\n");
      safe_strcpy( bin_path, path, MaxStr );
      if (verbose) printf("\t BinPath = '%s'\n", bin_path );
      substr = strstr(bin_path,"RUN_TAXSOLVE_GUI");
      if (substr != 0)
       {
        substr[0] = '\0';
        if (strlen( bin_path ) > 0)
	 safe_strcat( bin_path, "bin", MaxStr );
        else
	 safe_strcpy( bin_path, "bin", MaxStr );
	sprintf(cmd,"set path=%s;%%PATH%% & ", bin_path );

        if (gui_size)
         safe_strcat( cmd, "set GDK_SCALE=2 & ", MaxStr );
        if (darkmode)
         safe_strcat( cmd, "set GTK_THEME=Adwaita:dark & ", MaxStr );

	sprintf(cmd2,"start /B %s\\ots_gui%d.exe", bin_path, gui_version_to_invoke );
        safe_strcat( cmd, cmd2, MaxStr );
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

   safe_strcpy( bin_path, path, MaxStr );
   j = strlen( bin_path );
   if ((j > 0) && (bin_path[j-1] != '/')) safe_strcat( bin_path, "/", MaxStr );
   if (j == 0) safe_strcpy( bin_path, "./", MaxStr );
   sprintf(cmd,"bin/ots_gui%d", gui_version_to_invoke );
   safe_strcat( bin_path, cmd, MaxStr );

   #ifdef __APPLE__
    /* Apple's Finder starts OTS with "PWD" set to user's home-dir, instead of OTS-dir. */
    /* So "cd" to OTS just before starting GUI. */
    safe_strcpy( cmd, "cd \"", MaxStr );
    safe_strcat( cmd, path, MaxStr );
    safe_strcat( cmd, "\" ; ", MaxStr );
    if (gui_size)
     safe_strcat( cmd, "export GDK_SCALE=2; ", MaxStr );
    if (darkmode)
     safe_strcat( cmd, "export GTK_THEME=Adwaita:dark; ", MaxStr );
    safe_strcat( cmd, "\"", MaxStr );
    safe_strcat( cmd, bin_path, MaxStr );
    safe_strcat( cmd, "\" &", MaxStr );
   #else
    /* Linux, etc. */
    safe_strcpy( cmd, "", MaxStr );
    if (gui_size)
     safe_strcat( cmd, "export GDK_SCALE=2; ", MaxStr );
    if (darkmode)
     safe_strcat( cmd, "export GTK_THEME=Adwaita:dark; ", MaxStr );
    safe_strcat( cmd, "\"", MaxStr );
    safe_strcat( cmd, bin_path, MaxStr );
    safe_strcat( cmd, "\" &", MaxStr );
   #endif
  } /*posix*/

 #endif

 printf("Issuing: '%s'\n", cmd );
 system( cmd );

 return 0;
}

