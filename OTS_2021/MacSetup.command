echo -e "\n\nAllow permission to run OTS.\n"

export cmdpath=$0
export otsdirpath=${cmdpath%?????????????????}

echo -e "  Enter your password to allow permision:   (Your typing will NOT display.)"

sudo xattr -rd  com.apple.quarantine   $otsdirpath

echo -e "\nDone.  You can now start OTS by double-clicking 'Run_taxsolve_GUI'\n\n."
