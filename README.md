Parse and deserialize the output of the command `upower -i /org/freedesktop/UPower/devices/battery_BAT0' (of course assuming you have upower installed). 

Compile this tiny self-contained single file as you would, for example:

```bash
gcc upower_parsebat.c -o bat
```

By default, the program will output the battery percentage if given no arguments.

For instance, it accepts field names as an argument or short names that linearly matches with one of the field names but you will have to add an ` * ` at the end in case of using short abbreviations in place of the names, for example:

```
./a.out perc* # short for percentage 
```
