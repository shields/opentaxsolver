Open Tax Solver - Michigan State MI-1040 Personal Income Tax Return
-----------------------------------------------------------------------

Included here is a program, template, and example for
Michigan State MI-1040 personal income tax form.
Intended for use with the MI-1040 Instructions Booklet.

The MI_1040_2024_example.txt files is included for testing.
The MI_1040_2024_template.txt file is a blank starting form for
entering your tax data.  For each filer, copy template to a new
name, such as "MI_1040_2024.txt" or "MI_1040_2024_aunt_sally.txt,
and fill-in the lines.

The program consists of two files:
  taxsolve_MI_1040_2024.c - main, customized for MI-1040.
  taxsolve_routines.c - general purpose base routines.

Compile:
  cc taxsolve_MI_1040_2024.c -o taxsolve_MI_1040_2024

Run:
 First, run your Federal 1040 taxes and note the output AGI value.
 Then, complete your MI-1040 form input file and run.
  ../../bin/taxsolve_MI_1040_2024  MI_1040_2024.txt


For updates and further information, see:
        http://sourceforge.net/projects/opentaxsolver/
Documentation:
        http://opentaxsolver.sourceforge.net/


Contributed by C. Kindman.
