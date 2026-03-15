Open Tax Solver - UNOFFICIAL Pro Forma 2026 Fork
--------------------------------------------------

March 2026 v0.01 - Pro Forma Tax-Year 2026 (UNOFFICIAL)

*** IMPORTANT: See 0_DISCLAIMER.txt before using this software. ***

This is an unofficial fork of OpenTaxSolver for 2026 tax year
scenario modeling. It is NOT endorsed by or affiliated with the
OpenTaxSolver project or Aston Roberts. It is NOT tax advice and WILL
NOT accurately reflect final 2026 tax obligations.

This package supports ONLY:
	- US-1040 Federal return (with Schedules A, B, D, 1-A, and
	  forms 8949, 8812, 8829, 8959, 8960, 8995, 8606, 8889,
	  2210, 6781, Schedule C, Schedule SE)
	- California State 540 (with Form 5805)

The following are NOT supported in this fork:
	- PDF form filling (no 2026 PDF forms exist)
	- Non-CA state returns (OH, NJ, VA, PA, MA, NC, AZ, MI, NY, OR)

Known 2026 law changes incorporated:
	- Federal tax brackets from IRS Rev. Proc. 2025-32
	- Updated standard deduction amounts
	- Capital gains thresholds (inflation-adjusted)
	- AMT exemption and phase-out thresholds
	- SALT deduction cap indexed per OBBB ($40,400/$20,200)
	- Schedule 1-A (No-Tax on Tips/Overtime/SocSec)
	- 65+ age check updated to January 2, 1962
	- CA tax brackets (estimated ~2.971% CPI adjustment)
	- CA dependent exemption credit: $700 (SB 1144)
	- HSA contribution limits: $4,400/$8,750

Many values are estimates; see CHANGES_FROM_2025.txt for details.

Usage:
 Run the GUI (if available) or run solvers from the command line:
   bin/taxsolve_US_1040_2026  your_fed_return.txt
   bin/taxsolve_CA_540_2026   your_ca_return.txt

Example tax-data files and blank templates are in tax_form_files/.

Re-compiling:
 Unix/Linux/Mac:
     cd OTS_2026/src && make

Based on OpenTaxSolver by Aston Roberts and contributors.
See http://opentaxsolver.sourceforge.net/ for the official project.
