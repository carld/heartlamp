sdcc --verbose --model-small -o"./bin/" "./src/heartleds.c"
packihx "./bin/heartleds.ihx" > "./bin/heartleds.hex"