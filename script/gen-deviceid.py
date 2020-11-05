import string
import secrets

bit_length = 6
cformat = "const uint8_t DEVICE_ID[6] PROGMEM = {%s};"

secret_hex = secrets.token_hex(bit_length)

outformated = ""

strlen = len(secret_hex)
if strlen % 2 != 0 :
    print("Error while generating device ID.")
    exit()
for n in range(0, strlen + 1):
    if n % 2 == 0 and n != 0:
        outformated += "0x"
        outformated += secret_hex[n - 2: n].upper()
        if n < strlen :
            outformated += ", "

print(cformat % (outformated))



