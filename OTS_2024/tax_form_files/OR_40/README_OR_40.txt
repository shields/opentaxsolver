Open Tax Solver - Oregon State Tax Forms
----------------------------------------

These forms are supported:
Oregon state tax form OR-40, including Schedule OR-A, and Schedule OR-ASC.

The source file is ../src/taxsolve_OR_40_2024.c.

The file 'OR_40_2024_example.txt', and an example Form 1040 output file
'US_1040_example_out.txt', are included for testing.

The OR_40_2024_template.txt file is a blank starting form for entering
your tax data.

See the Makefile in the src directory for build information.

Run:
  ./taxsolve_OR_40_2024 <input_file>

The 'doc' subdirectory contains the following files which document the development process.

OR40_Tax_Table.xlsx -- Compares the values listed in the Oregon OR-40
instructions tax tables, which are quantized, with both an Excel model and the
OTS output, to validate the TaxLookup function. Debug code in the source
module, taxsolve_OR_40_2024.c. is enabled through the use of the TAX_TABLE_TEST
macro.

OR_40_Line_Coordinate_Calculations.xlsx -- Provides a method of defining the PDF
display coordinates for each text field. The contents of the or_40_meta.dat file
can be updated directly from the first columns of the spreadsheet. All
coordinates for each field are the lower left corner of the corresponding
alignment box.

or40_pdf_test.cmd -- Invokes universal_pdf_file_modifier.exe with the OR_40_test_data.txt file.
OR_40_test_data.txt -- Full set of test data for all fields on the form; used
from the 'or40_pdf_test.cmd' file.

Place these two files in the top level OpenTaxSolver directory to run the test
and generate a fully populated PDF output test file.

=== Notes on Source Code and Requirements of the OR-40, OR-A, and OR-ASC Forms ===
The Oregon PDF forms require careful alignment of the text fields with the
preformatted boxes, and include a full space for the comma between sets of 3
digits. Also, the ".00" is preprinted on every line, so it can't be included in
the output values.  uPDFModifier doesn't allow the insertion of commas when the
character spacing option is used, so commas have to be inserted in the taxsolver
code. All numeric values in the output file have commas inserted, and no decimal
point or cents.

A group of functions which support this output formatting are
defined in taxsolve_OR_40_*.c:

showline_wlabel_or_40
showline_wlabel_or_40_nz
shownum_or_40
shownum_or_40_nz

These behave the same way as the similarly named shared functions.

A single table named status_cfg[] is used to control the configuration of every
value or function which depends on filing status. This is an array of structs,
typedefed as FILING_STATUS_CFG.

Oregon uses a tax table with ranges of values for AGI amounts below $50,000.
This quantizes a range of values into a single, midpoint value. In order to
exactly match the values in the tax table in the OR-40 instructions, it is
necessary to first round to the nearest cent, and then the nearest dollar, in 2
steps. For example, the value $123.499 would be rounded down to $123.00 if
rounded directly to dollars, but would be rounded as 
$123.499 -> $123.50 -> $124.00 
if rounded in 2 steps. This double rounding is the method used by the Oregon tax
table.

Two types of additional structs are defined, TAX_TABLE and LIMIT_TABLE, to
support tax table and tax limit functions. These allow complete separation of
the algorithm used to calculate taxes, and the breakpoints and rates. Different
versions of these tables are selected through the primary status_cfg[] array,
based on filing status.

Here is an example of the tax table definition for single filers, for 2024.

// Tax breakpoint table for single or married filing separately.
TAX_TABLE or_40_single_tax_table[] = {
    { 0.00,      0.0475 },
    { 4300.00,   0.0675 },
    { 10750.00,  0.0875 },
    { 125000.00, 0.0990 },
    { -1.00,     0.00 },
};

And here is an example of a limit table, used to implement a maximum level of federal tax
subtraction based on AGI:

// Used for all other cases except single, and married filing separately:
// married/joint, head of household, and widow
LIMIT_TABLE or_40_other_fed_sub_table[] = {
    { 0,      8250 },
    { 250000, 6600 },
    { 260000, 4950 },
    { 270000, 3300 },
    { 280000, 1650 },
    { 290000, 0    },
    { -1,     0    },
};

For annual updates, search for the comment string 'TAXYEAR_DEPENDENT'. This is
placed before each table or value which is likely to change in future tax years.

