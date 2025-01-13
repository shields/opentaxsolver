/***********************************************************************
 TaxSolve_AZ_140_2024.c - Arizina State Tax Program.
   Project:  http://opentaxsolver.sourceforge.com/
   Date:     1-2-2025

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
   02111-1307 USA

   Original Author:  E. Strnod 9/12/2024
 ***********************************************************************/

float thisversion = 3.00;

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "taxsolve_get_fed_return_data.c"

#define Yes 1
#define No  0

#define STANDARD_DEDUCTION  0
#define ITEMIZED_DEDUCTIONS 1
#define QUALIFYING_PARENTS_AND_GRANDPARENTS_EXEMPTION_AMOUNT 10000;
#define AGE_65_OR_OVER_EXEMPTION_AMOUNT 2100
#define BLIND_EXEMPTION_AMOUNT 1500
#define OTHER_EXEMPTION_AMOUNT 2300


/*----------------------------------------------------------------------------*/

double getAZStdDedAmt() 
{
	double azStdDedAmt[5][1]={				/* Updated for 2024. */
			{0.0},
			{ 14600.0 },  /* Single */
			{ 29200.0 },  /* Married, filing jointly. */
			{ 14600.0 },  /* Married, filing separate. */
			{ 21900.0 }   /* Head of Household. */
			     };
	return azStdDedAmt[status][0];
}

double getAZTaxAmt( double azTaxableIncome ) 
{
	double taxAmt = 0.0;
	taxAmt = 0.025 * azTaxableIncome;			/* Updated for 2024. */
	return taxAmt;
}

int main( int argc, char *argv[] )
{
	int i, j, k, deduction = 0, includePage4 = 0;
	char word[4000], outfname[4000], prelim_1040_outfilename[5000], *infname = 0;
	time_t now;
	double L10a = 0.0, L10b = 0.0, L11a = 0.0;
	double L29a = 0.0, L29b = 0.0;
	double L34a = 0.0, L34b = 0.0;
	double L40e = 0.0;
	double L54a = 0.0, L54b = 0.0;
	double SD1C = 0.0, SD2C = 0.0, SD3C = 0.0, SD4C = 0.0, SD5C = 0.0, SD6C = 0.0;
	double OA_A = 0.0, OA_B = 0.0, OA_C = 0.0, OA_D = 0.0, OA_E = 0.0, OA_Fa = 0.0, OA_Fb = 0.0;
	double OA_Ga = 0.0, OA_Gb = 0.0, OA_Ha = 0.0, OA_Hb = 0.0, OA_Hc = 0.0, OA_I = 0.0;
	double OA_J = 0.0, OA_K = 0.0, OA_L = 0.0, OA_M = 0.0, OA_N = 0.0, OA_O = 0.0, OA_P = 0.0,
		   OA_Q = 0.0, OA_R = 0.0, OA_S = 0.0, OA_T = 0.0;
	double OS_A = 0.0, OS_B = 0.0, OS_C = 0.0, OS_D = 0.0, OS_E = 0.0, OS_F = 0.0, OS_G = 0.0,
			OS_H = 0.0, OS_I = 0.0, OS_J = 0.0, OS_K = 0.0, OS_L = 0.0, OS_M = 0.0, OS_N = 0.0,
			OS_O = 0.0, OS_P = 0.0, OS_Q = 0.0, OS_R = 0.0, OS_S = 0.0, OS_T = 0.0,
			OS_U = 0.0, OS_V = 0.0, OS_W = 0.0;

	printf("Arizona Form 140, 2023 - v%3.2f\n", thisversion);

	#if (0)
		add_pdf_markup( "NotReady", 1, 240, 40, 17, 1, 1.0, 0, 0, "\"This program is NOT ready for 2024.\"" );
		#ifdef microsoft
		 system( "start bin\\notify_popup -delay 3 -expire 10 \"Warning: This program is NOT ready for 2024.\"" );
		#else
		 system( "bin/notify_popup -delay 3 -expire 10 \"Warning: This program is NOT ready for 2024.\" &" );
		#endif
	#endif

	/* Decode any command-line arguments. */
	i = 1;
	k = 1;
	while (i < argc) {
		if (strcmp(argv[i], "-verbose") == 0) {
			verbose = 1;
		} else if (k == 1) {
			infname = strdup(argv[i]);
			infile = fopen(infname, "r");
			if (infile == 0) {
				printf("ERROR: Parameter file '%s' could not be opened.\n",
						infname);
				exit(1);
			}
			k = 2;
			/* Base name of output file on input file. */
			strcpy(outfname, infname);
			j = strlen(outfname) - 1;
			while ((j >= 0) && (outfname[j] != '.'))
				j--;
			if (j < 0)
				strcat(outfname, "_out.txt");
			else
				strcpy(&(outfname[j]), "_out.txt");
			outfile = fopen(outfname, "w");
			if (outfile == 0) {
				printf("ERROR: Output file '%s' could not be opened.\n",
						outfname);
				exit(1);
			}
			printf("Writing results to file:  %s\n", outfname);
		} else {
			printf("Unknown command-line parameter '%s'\n", argv[i]);
			exit(1);
		}
		i = i + 1;
	}
	if (infile == 0) {
		printf("Error: No input file on command line.\n");
		exit(1);
	}

	/* Pre-initialize all lines to zeros. */
	for (i = 0; i < MAX_LINES; i++) {
		L[i] = 0.0;
	}

	/* Accept parameters from input file. */
	/* Expect lines, something like:
	 Title:  Form XXXX Return
	 L2              {Returns and Allowances}
	 . . .
	 */

	/* Accept Form's "Title" line, and put out with date-stamp for your records. */
	read_line(infile, word);
	now = time(0);
	fprintf(outfile, "\n%s,  v%2.2f, %s\n", word, thisversion, ctime(&now));

	/* ----- Accept form data and process the numbers.         ------ */
	/* ----- Place all your form-specific code below here .... ------ */

	// Example:
	//  GetLineF( "L2", &L[2] );
	//  GetLineF( "L3", &L[3] );
	//  L[4] = L[2] - L[3];
	//  showline_wlabel( "L4", L[4] );

	 get_parameter( infile, 's', word, "FileName1040:" );      /* Preliminary Fed Return Output File-name. */
	 get_word(infile, prelim_1040_outfilename );
	 ImportFederalReturnData( prelim_1040_outfilename, &PrelimFedReturn );

	 // transfer heading info to AZ form 1040
	 fprintf(outfile, "Your1stName: %s\n", Your1stName );
	 fprintf(outfile, "YourLastName: %s\n", YourLastName );
	 fprintf(outfile, "YourSocSec#: %s\n", your_socsec );
	 fprintf(outfile, "Spouse1stName: %s\n", Spouse1stName );
	 fprintf(outfile, "SpouseLastName: %s\n", SpouseLastName );
	 fprintf(outfile, "SpouseSocSec#: %s\n", spouse_socsec );
	 fprintf(outfile, "Number&Street: %s\n", street_address );
	 fprintf(outfile, "Apt#: %s\n", apartment );

	 // get daytime phone
	 GetTextLineF("DaytimePhone#:" );

	 fprintf(outfile, "Town/City: %s\n", town );
     fprintf(outfile, "State: %s\n", state );
     fprintf(outfile, "ZipCode: %s\n", zipcode );

	 // get prior last names
	 GetTextLineF("PriorLastNames:" );

	 get_parameter(infile, 's', word, "L4a_InjuredSpouse:");
	 get_parameter(infile, 'b', &j, "L4a_InjuredSpouse:");

	 // filing status checkboxes
	 if (status == MARRIED_FILING_JOINTLY) {
	 	fprintf(outfile, "L4Chk X\n");
		if (j != 0)
			fprintf(outfile, "L4a_InjuredSpouseChk X\n");
	 } else if (status == HEAD_OF_HOUSEHOLD) {
		 fprintf(outfile, "L5Chk X\n");
		 // TODO L5 includes space to name qualifying child or dependent;
		 // this is also in 1040, but not collected by OTS.  Ideally, it
		 // could be captured there and made use of here.
	 } else if (status == MARRIED_FILING_SEPARAT) {
		 fprintf(outfile, "L6Chk X\n");
	 } else if (status == SINGLE) {
		 fprintf(outfile, "L7Chk X\n");
	 }

	GetLineF("L8", &L[8]); // age 65+ filers
	GetLineF("L9", &L[9]); // blind filers
	GetLineF("L10a", &L10a); // dependents under 17
	GetLineF("L10b", &L10b); // dependents 17+
	GetLineF("L11a", &L11a); // qualifying parents / grandparents

	// dependents (if more than 3, optional page 4 will be activated below.)
	for (j = 1; j <= 3; j++) // why is this 1-based?!
		if (strlen(PrelimFedReturn.Dep1stName[j]) > 0) {
			fprintf(outfile, "Dep%d_FirstName: %s\n", j, PrelimFedReturn.Dep1stName[j]);
			fprintf(outfile, "Dep%d_LastName: %s\n", j,	PrelimFedReturn.DepLastName[j]);
			fprintf(outfile, "Dep%d_SocSec#: %s\n", j, PrelimFedReturn.DepSocSec[j]);
			fprintf(outfile, "Dep%d_Relation: %s\n", j, PrelimFedReturn.DepRelation[j]);
		}
	if (strlen(PrelimFedReturn.Dep1stName[4]) > 0) {
		fprintf(outfile, "L10_NeedMoreSpaceChk X\n");
		includePage4 = 1;
	}

	get_parameter(infile, 's', word, "L11_NeedMoreSpace:");
	get_parameter(infile, 'b', &j, "L11_NeedMoreSpace:");
	if (j != 0) {
		fprintf(outfile, "L11_NeedMoreSpaceChk X\n");
		includePage4 = 1;
	}
	GetTextLineF("L11b_PG1_FirstName:");
	GetTextLineF("L11b_PG1_LastName:");
	GetTextLineF("L11b_PG1_SocSec#:");
	GetTextLineF("L11b_PG1_Relation:");
	GetTextLineF("L11b_PG1_Months:");

	get_parameter(infile, 's', word, "L11b_PG1_65Over:");
	get_parameter(infile, 'b', &j, "L11b_PG1_65Over:");
	if (j != 0)
		fprintf(outfile, "L11b_PG1_65OverChk X\n");

	get_parameter(infile, 's', word, "L11b_PG1_DiedTaxYr:");
	get_parameter(infile, 'b', &j, "L11b_PG1_DiedTaxYr:");
	if (j != 0)
		fprintf(outfile, "L11b_PG1_DiedTaxYrChk X\n");

	GetTextLineF("L11c_PG2_FirstName:");
	GetTextLineF("L11c_PG2_LastName:");
	GetTextLineF("L11c_PG2_SocSec#:");
	GetTextLineF("L11c_PG2_Relation:");
	GetTextLineF("L11c_PG2_Months:");

	get_parameter(infile, 's', word, "L11c_PG2_65Over:");
	get_parameter(infile, 'b', &j, "L11c_PG2_65Over:");
	if (j != 0)
		fprintf(outfile, "L11c_PG2_65OverChk X\n");

	get_parameter(infile, 's', word, "L11c_PG2_DiedTaxYr:");
	get_parameter(infile, 'b', &j, "L11c_PG2_DiedTaxYr:");
	if (j != 0)
		fprintf(outfile, "L11c_PG2_DiedTaxYrChk X\n");

	//  L12 Federal adjusted gross income here
	L[12] = PrelimFedReturn.fedline[11];	/* Fed Wages (Fed 1040 line 11). */
	showline(12);


	get_parameter(infile, 's', word, "L13_Filing140SBI:");
	get_parameter(infile, 'b', &j, "L13_Filing140SBI:");
	if (j != 0)
		fprintf(outfile, "L13_Filing140SBIChk X\n");

	// Additions
	GetLineF("L13", &L[13]);
	GetLineF("L15", &L[15]);
	GetLineF("L16", &L[16]);
	GetLineF("L17", &L[17]);

	// Other Additions to Arizona Gross Income (Schedule)
	GetLine("OA_A", &OA_A);
	GetLine("OA_B", &OA_B);
	GetLine("OA_C", &OA_C);
	GetLine("OA_D", &OA_D);
	GetLine("OA_E", &OA_E);
	GetLine("OA_Fa", &OA_Fa);
	GetLine("OA_Fb", &OA_Fb);
	GetLine("OA_Ga", &OA_Ga);
	GetLine("OA_Gb", &OA_Gb);
	GetLine("OA_Ha", &OA_Ha);
	GetLine("OA_Hb", &OA_Hb);
	GetLine("OA_Hc", &OA_Hc);
	GetLine("OA_I", &OA_I);
	GetLine("OA_J", &OA_J);
	GetLine("OA_K", &OA_K);
	GetLine("OA_L", &OA_L);
	GetLine("OA_M", &OA_M);
	GetLine("OA_N", &OA_N);
	GetLine("OA_O", &OA_O);
	GetLine("OA_P", &OA_P);
	GetLine("OA_Q", &OA_Q);
	GetLine("OA_R", &OA_R);
	GetLine("OA_S", &OA_S);

	// Add lines OA_A thru OA_R for OA_S and L18
	OA_T = OA_A + OA_B + OA_C + OA_D + OA_E + OA_Fa + OA_Fb + OA_Ga + OA_Gb + OA_Ha + OA_Hb + OA_Hc + OA_I
			+ OA_J + OA_K + OA_L + OA_M + OA_N + OA_O + OA_P + OA_Q + OA_R + OA_T;
	L[18] = OA_T;

	showline(18);

	// modified federal adjusted gross income
	L[14] = L[12] - L[13];

	showline(14);

	// subtotal
	// do AFTER L18 is calculated!!
	L[19] = L[14] + L[15] + L[16] + L[17] + L[18];
	showline(19);

	// Subtractions

	// TODO lines 20-23 (Net Capital Gain/Loss) are (I think) handled by US_1040 but are not in struct FedReturnData.
	// Since this appears to be an optional form in OTS and I am not sure yet how this is done, punting and
	// having the user enter these values again here if they apply.
	GetLineF("L20", &L[20]);
	GetLineF("L21", &L[21]);
	GetLineF("L22", &L[22]);
	GetLineF("L23", &L[23]);

	L[24] = L[23] * 0.25;					/* Updated for 2024. */
	showline(24);

	GetLineF("L25", &L[25]);
	GetLineF("L26", &L[26]);
	GetLineF("L27", &L[27]);
	GetLineF("L28", &L[28]);
	GetLineF("L29a", &L29a);

	// validate 29a - no greater than $2500 per taxpayer
	if (L29a > 2500.0) {
         printf("Error: Line 29a entry '%9.2f' (Exclusion for U.S. Gov't, AZ State or Local Gov't Pensions) may not exceed $2500. Exiting.\n", L29a);
         fprintf(outfile,"Error: Line 29a entry '%9.2f' (Exclusion for U.S. Gov't, AZ State or Local Gov't Pensions) may not exceed $2500. Exiting.\n", L29a);
         exit(1);
	}

	GetLineF("L29b", &L29b);
	GetLineF("L30", &L[30]);
	GetLineF("L31", &L[31]);
	GetLineF("L32", &L[32]);
	GetLineF("L33", &L[33]);
	GetLineF("L34a", &L34a);
	GetLineF("L34b", &L34b);

	L[34] = L34a + L34b;
	showline(34);

	// Subtract lines 24 thru 34c from L19 and place in L35
	L[35] = L[19] - (L[24] + L[25] + L[26] + L[27] + L[28] + L29a + L29b + L[30] + L[31] + L[32] + L[33] + L[34]);
	showline(35);

	// Exemptions


	// Other Subtractions from Arizona Gross Income (Schedule)
	GetLine("OS_A", &OS_A);
	GetLine("OS_B", &OS_B);
	GetLine("OS_C", &OS_C);
	GetLine("OS_D", &OS_D);
	GetLine("OS_E", &OS_E);
	GetLine("OS_F", &OS_F);
	GetLine("OS_G", &OS_G);
	GetLine("OS_H", &OS_H);
	GetLine("OS_I", &OS_I);
	GetLine("OS_J", &OS_J);
	GetLine("OS_K", &OS_K);
	GetLine("OS_L", &OS_L);
	GetLine("OS_M", &OS_M);
	GetLine("OS_N", &OS_N);
	GetLine("OS_O", &OS_O);
	GetLine("OS_P", &OS_P);
	GetLine("OS_Q", &OS_Q);
	GetLine("OS_R", &OS_R);
	GetLine("OS_S", &OS_S);
	GetLine("OS_T", &OS_T);
	GetLine("OS_U", &OS_U);
	GetLine("OS_V", &OS_V);

	// Add lines OS_A thru OS_S for OS_T and L36
	OS_W = OS_A + OS_B + OS_C + OS_D + OS_E + OS_F + OS_G + OS_H + OS_I + OS_J + OS_K + OS_L + OS_M + OS_N
			+ OS_O + OS_P + OS_Q + OS_R + OS_S + OS_T + OS_U + OS_V;
	L[36] = OS_W;
	showline(36);

	// whole name for subsequent page
    fprintf(outfile,"YourName: %s, %s\n", YourLastName, Your1stName);


	// do AFTER L36 is calculated!!
	L[37] = L[35] - L[36];
	showline(37);

	// multiply L8 (number of filers over 65) by 2100 (make it a constant)
	// and place in L38.
	// validate this against 1040
	if (PrelimFedReturn.Over65 + PrelimFedReturn.SpouseOver65 != L[8]) {
	    printf("Warning: Number of filers over 65 does not match Fed return Over65/SpouseOver65 responses.\n");
	    fprintf(outfile, "Warning: Number of filers over 65 does not match Fed return Over65/SpouseOver65 responses.\n");
	}
	L[38] = L[8] * AGE_65_OR_OVER_EXEMPTION_AMOUNT;
	showline(38);

	// multiply L9 (number of filers blind) by 1500 (make it a constant)
	// and place in L39.
	// validate this against 1040
	if (PrelimFedReturn.Blind + PrelimFedReturn.SpouseBlind != L[9]) {
	    printf("Warning: Number of filers blind does not match Fed return YouBlind/SpouseBlind responses.\n");
	    fprintf(outfile, "Warning: Number of filers blind does not match Fed return YouBlind/SpouseBlind responses.\n");
	}
	L[39] = L[9] * BLIND_EXEMPTION_AMOUNT;
	showline(39);

	// Other Exemptions - collect total number in 40e, multiply by 2300 (make it a constant) and place in L40
	// (part 3 list collection not handled here.)
	GetLineF("L40e", &L40e);
	L[40] = L40e * OTHER_EXEMPTION_AMOUNT;
	showline(40);
	if (L40e > 0)
		includePage4 = 1;


	// multiply number in box 11a (qualifying parents & grandparents) by 10,000 and place in L41
	L[41] = L11a * QUALIFYING_PARENTS_AND_GRANDPARENTS_EXEMPTION_AMOUNT;
	showline(41);

	// subtract lines 38 through 41 from L37 and place greater of 0 or result in L42
	L[42] = NotLessThanZero( L[37] - L[38] - L[39] - L[40] - L[41] );
	showline(42);

	// Balance of Tax
	get_parameter( infile, 's', word, "L43_Deductions:" );	/* Itemized, Standard */
	get_parameter( infile, 'l', word, "L43_Deductions:");
	GetLine("L43", &L[43]);
	if (strncasecmp(word,"Itemized",8)==0) {
		deduction = ITEMIZED_DEDUCTIONS;
		fprintf(outfile, "L43_DeductionsItemizedChk X\n");

	} else if (strncasecmp(word,"Standard",8)==0) {
		deduction = STANDARD_DEDUCTION;
		fprintf(outfile, "L43_DeductionsStandardChk X\n");
		// calculate the standard deduction amount (use constants) and place in L43.
		L[43] = getAZStdDedAmt();
	} else {
		printf("Error: unrecognized deduction type '%s'. Exiting.\n", word);
		fprintf(outfile,"Error: unrecognized deduction '%s'. Exiting.\n", word);
		exit(1);
	}
	showline(43);

/*	// this is checked automatically below, after the worksheet is done, if amount is > 0.
    get_parameter( infile, 's', word, "L44C_Charitable:" );	 Yes, No
	get_parameter( infile, 'b', &j, "L44C_Charitable:");
	if (j != 0) {
		fprintf(outfile, "L44C_CharitableChk X\n");
		stdWithCharContrib = j;
	}*/
	// Standard Deductions Increase for Charitable Contributions (Worksheet)
	GetLine("SD1C", &SD1C);
	GetLine("SD2C", &SD2C);
	GetLine("SD3C", &SD3C);

	// Add lines 1C thru 3C for 4C
	SD4C = SD1C + SD2C + SD3C;

	GetLine("SD5C", &SD5C);

	// subtract 5C from 4C for 6C (minimum 0)
	SD6C = (SD5C > SD4C ? 0 : SD4C - SD5C);
	if (SD6C > 0) {
		//  validate that 43I is not checked, if worksheet was filled out
		if (deduction == ITEMIZED_DEDUCTIONS) {
			printf("Error: Charitable contributions entered in Standard Deduction worksheet with Itemized deduction selection. Exiting.\n");
			fprintf(outfile,"Error: Charitable contributions entered Standard Deduction worksheet with Itemized deduction selection. Exiting.\n");
			exit(1);
		}
		fprintf(outfile, "L44_CharitableChk X\n");
	}


	// multiply 6C by 33% for 7C and place result in L44
	L[44] = SD6C * 0.33;							/* Updated for 2024. */
	showline(44);

	// subtract L43 and L44 from L42 and place greater of 0 or result in L45 (taxable income)
	L[45] = NotLessThanZero( L[42] - L[44] - L[43] );
	showline(45);

	// calculate tax amount and place in L46
	L[46] = getAZTaxAmt( L[45] );
	showline(46);

	GetLineF( "L47", &L[47] );

	// Add L46 and L47 and place total in L48 (tax subtotal)
	L[48] = L[46] + L[47];
	showline(48);

	// TODO hairball dependent tax credit calculation for L49 :-(  Punting and collecting the final result for now:
	GetLineF("L49", &L[49]);

	// TODO hairball family income tax credit calculation for L50 :-(  Punting and collecting the final result for now:
	GetLineF("L50", &L[50]);

	GetLineF("L51", &L[51]);

	// subtract L49, L50 & L51 from L48 and place result in L52; if SUM of L49, L50 & L51 is > L48, zero L52.
	int credits = L[49] + L[50] + L[51];
	L[52] = (credits > L[48] ? 0 : L[48] - credits);
	showline(52);

	GetLineF("L53", &L[53]);
	GetLineF("L54a", &L54a);
	GetLineF("L54b", &L54b);
	L[54] = L54a + L54b;
	showline(54);
	GetLineF("L55", &L[55]);
	GetLineF("L56", &L[56]);
	GetLineF("L57", &L[57]);

	get_parameter(infile, 's', word, "L58_Filing308I:");
	get_parameter(infile, 'b', &j, "L58_Filing308I:");
	if (j != 0)
		fprintf(outfile, "L58_Filing308IChk X\n");

	get_parameter(infile, 's', word, "L58_Filing334:");
	get_parameter(infile, 'b', &j, "L58_Filing334:");
	if (j != 0)
		fprintf(outfile, "L58_Filing334Chk X\n");

	get_parameter(infile, 's', word, "L58_Filing349:");
	get_parameter(infile, 'b', &j, "Filing349:");
	if (j != 0)
		fprintf(outfile, "L58_Filing349Chk X\n");

	GetLineF("L58", &L[58]);

	// Add lines 53 through 58 and place result in L59
	L[59] = L[53] + L[54] + L[55] + L[56] + L[57] + L[58];
	showline(59);

	// Tax Due or Overpayment
	GetLine("L62", &L[62]);

	// If L52 > L59, subtract L59 from L52, place result in L60, and skip lines 61, 62 and 63
	if (L[52] > L[59]) {
		L[60] = L[52] - L[59];
		showline(60);

		if (L[62] > 0) {
	        printf("Error: Line 62 entry '%9.2f' (amount of overpayment to be applied to next tax year) does not apply, as there is an amount due. Exiting.\n", L[62]);
	        fprintf(outfile,"Error: Line 62 entry '%9.2f' (amount of overpayment to be applied to next tax year) does not apply, as there is an amount due. Exiting.\n", L[62]);
	        exit(1);
		}
	} else {
		// (Line 61) If L59 > L52, subtract L52 from L59 and place result in L61
		if (L[59] > L[52]) {
			L[61] = L[59] - L[52];
			showline(61);
		}


		if (L[62] > L[61]) {
	        printf("Error: Line 62 entry '%9.2f' (amount of overpayment to be applied to next tax year) exceeds overpayment '%9.2f'. Exiting.\n", L[62], L[61]);
	        fprintf(outfile,"Error: Line 62 entry '%9.2f' (amount of overpayment to be applied to next tax year) exceeds overpayment '%9.2f'. Exiting.\n", L[62], L[61]);
	        exit(1);
		}

		showline(62);

		// (line 63) Subtract L62 from L61 and place result in L63
		L[63] = L[61] - L[62];
		showline(63);
	}

	// Voluntary Gifts
	GetLineF("L64", &L[64]);
	GetLineF("L65", &L[65]);
	GetLineF("L66", &L[66]);
	GetLineF("L67", &L[67]);
	GetLineF("L68", &L[68]);
	GetLineF("L69", &L[69]);
	GetLineF("L70", &L[70]);
	GetLineF("L71", &L[71]);
	GetLineF("L72", &L[72]);
	GetLineF("L73", &L[73]);
	GetLineF("L74", &L[74]);

	get_parameter( infile, 's', word, "L75_PoliticalParty" );	/* none, Democratic, Libertarian, Republican */
	get_parameter( infile, 'l', word, "L75_PoliticalParty");

	j = strncasecmp(word,"none",4);
	if (j==0 && L[68] == 0.0) {
		// no political gift amount entered and 'none' political party selected, so we can stop checking party selection
	} else if (j==0 && L[68] != 0.0) {
		// political gift amount entered but no party selected
		printf("Error: political party '%s' selected but gift amount entered. Exiting.\n", word);
		fprintf(outfile,"Error: political party '%s' selected but gift amount entered. Exiting.\n", word);
		exit(1);
	} else if (j!=0 && L[68] == 0.0) {
		// no political gift amount entered but something other than "none" entered as party
		printf("Error: political party '%s' selected but no gift amount entered. Exiting.\n", word);
		fprintf(outfile,"Error: political party '%s' selected but no gift amount entered. Exiting.\n", word);
		exit(1);
	} else if (strncasecmp(word,"Democratic",10)==0) {
		fprintf(outfile, "L75_DemocraticChk X\n");
	} else if (strncasecmp(word,"Libertarian",11)==0) {
		fprintf(outfile, "L75_LibertarianChk X\n");
	} else if (strncasecmp(word,"Republican",10)==0) {
			fprintf(outfile, "L75_RepublicanChk X\n");
	} else {
		printf("Error: unrecognized political party '%s'. Exiting.\n", word);
		fprintf(outfile,"Error: unrecognized political party '%s'. Exiting.\n", word);
		exit(1);
	}

	// Penalty
	GetLineF("L76", &L[76]);

	get_parameter(infile, 's', word, "L77_1AnnualizedOther:");
	get_parameter(infile, 'b', &j, "L77_1AnnualizedOther:");
	if (j != 0)
		fprintf(outfile, "L77_1AnnualizedOtherChk X\n");

	get_parameter(infile, 's', word, "L77_2FarmerFisherman:");
	get_parameter(infile, 'b', &j, "L77_2FarmerFisherman:");
	if (j != 0)
		fprintf(outfile, "L77_2FarmerFishermanChk X\n");

	get_parameter(infile, 's', word, "L77_Filing221:");
	get_parameter(infile, 'b', &j, "L77_Filing221:");
	if (j != 0)
		fprintf(outfile, "L77_Filing221Chk X\n");

	// add lines 64 through 74 and 76 and enter total in L78
	L[78] = L[64] + L[65] + L[66] + L[67] + L[68] + L[69] + L[70] + L[71] + L[72] + L[73] + L[74] + L[76];
	showline(78);

	// Refund or Amount Owed
	// subtract L78 from L63 and place abs value in L79 if greater than zero or if less than or equal to 0, place abs value in L80
	double balance = L[63] - L[78];
	if (balance <= 0) {
		// Add lines 60 and 78; enter the amount you owe on line 80
		L[80] = L[60] + L[78];
		// TODO these next instructions (under line 80) are a little confusing (slightly contradictory with the prior):
		// "If you are making voluntary gifts on lines 64 - 74 in excess of your overpayment,
		// enter the difference on line 80."  (Not modifying the prior calculation for this, as it seems to be handled.)
		showline(80); // amount owed
	} else {
		L[79] =  balance;
		showline(79); // refund
	}

	get_parameter(infile, 's', word, "L79_ForeignAcct:");
	get_parameter(infile, 'b', &j, "L79_ForeignAcct:");
	if (j != 0)
		fprintf(outfile, "L79_ForeignAcctChk X\n");

	// Standard Deductions worksheet (optional)
	if (SD6C > 0) {
		// include the optional worksheet page
		// TODO (no documentation found on format of this command in outfile; copying from US 1040)
		fprintf(outfile, "PDFpage: 3 3\n"); /* Optional PDF Page. */
		showline_wlabel("SD1C", SD1C);
		showline_wlabel("SD2C", SD2C);
		showline_wlabel("SD3C", SD3C);
		showline_wlabel("SD4C", SD4C);
		showline_wlabel("SD5C", SD5C);
		showline_wlabel("SD6C", SD6C);
		showline_wlabel("SD7C", L[44]);
		showline(44);
		fprintf(outfile, "EndPDFpage.\n");
	}

	if (includePage4 > 0) {
		// include the optional page to list additional dependents
		// TODO (no documentation found on format of this command in outfile; copying US 1040)
		fprintf(outfile,"PDFpage: 4 4\nEndPDFpage.\n");	/* Optional PDF Page. */
		// This page will not be filled out; it is being provided for the user to fill out manually.
	}

	if (L[18] > 0.0) {
		// include optional page to list other additions
		// TODO (no documentation found on format of this command in outfile; copying US 1040)
		fprintf(outfile,"PDFpage: 5 5\n");	/* Optional PDF Page. */
		// Other Additions to Arizona Gross Income (Schedule)
		showline_wlabel("OA_A", OA_A);
		showline_wlabel("OA_B", OA_B);
		showline_wlabel("OA_C", OA_C);
		showline_wlabel("OA_D", OA_D);
		showline_wlabel("OA_E", OA_E);
		showline_wlabel("OA_Fa", OA_Fa);
		showline_wlabel("OA_Fb", OA_Fb);
		showline_wlabel("OA_Ga", OA_Ga);
		showline_wlabel("OA_Gb", OA_Gb);
		showline_wlabel("OA_Ha", OA_Ha);
		showline_wlabel("OA_Hb", OA_Hb);
		showline_wlabel("OA_Hc", OA_Hc);
		showline_wlabel("OA_I", OA_I);
		showline_wlabel("OA_J", OA_J);
		showline_wlabel("OA_K", OA_K);
		showline_wlabel("OA_L", OA_L);
		showline_wlabel("OA_M", OA_M);
		showline_wlabel("OA_N", OA_N);
		showline_wlabel("OA_O", OA_O);
		showline_wlabel("OA_P", OA_P);
		showline_wlabel("OA_Q", OA_Q);
		showline_wlabel("OA_R", OA_R);
		showline_wlabel("OA_S", OA_S);
		showline_wlabel("OA_T", OA_T);
		showline(18);
		fprintf(outfile,"EndPDFpage.\n");	/* Optional PDF Page. */
	}

	if (L[36] > 0.0) {
		// include optional page to list other subtractions
		fprintf(outfile,"PDFpage: 6 6\n");	/* Optional PDF Page. */
		// TODO (no documentation found on format of this command in outfile; copying US 1040)
		// Other Subtractions from Arizona Gross Income (Schedule)
		showline_wlabel("OS_A", OS_A);
		showline_wlabel("OS_B", OS_B);
		showline_wlabel("OS_C", OS_C);
		showline_wlabel("OS_D", OS_D);
		showline_wlabel("OS_E", OS_E);
		showline_wlabel("OS_F", OS_F);
		showline_wlabel("OS_G", OS_G);
		showline_wlabel("OS_H", OS_H);
		showline_wlabel("OS_I", OS_I);
		showline_wlabel("OS_J", OS_J);
		showline_wlabel("OS_K", OS_K);
		showline_wlabel("OS_L", OS_L);
		showline_wlabel("OS_M", OS_M);
		showline_wlabel("OS_N", OS_N);
		showline_wlabel("OS_O", OS_O);
		showline_wlabel("OS_P", OS_P);
		showline_wlabel("OS_Q", OS_Q);
		showline_wlabel("OS_R", OS_R);
		showline_wlabel("OS_S", OS_S);
		showline_wlabel("OS_T", OS_T);
		showline_wlabel("OS_U", OS_U);
		showline_wlabel("OS_V", OS_V);
		showline_wlabel("OS_W", OS_W);
		showline(36);
		fprintf(outfile,"EndPDFpage.\n");	/* Optional PDF Page. */

	}


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
	grab_any_pdf_markups(infname, outfile);
	fclose(outfile);

	printf("\nListing results from file: %s\n\n", outfname);
	Display_File(outfname);

	return 0;
}
