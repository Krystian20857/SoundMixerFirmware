import string
import secrets
import os

Import("env")

def print_error(message):
    print("\033[91m" + message + '\033[0m')

def print_info(message):
    print('\033[94m' + message + '\033[0m')

def gen_device_ID():
    bit_length = 6
    cformat = "{%s}"

    secret_hex = secrets.token_hex(bit_length)
    outformated = ""

    strlen = len(secret_hex)
    if strlen % 2 != 0 :
        print_error("Error while generating device ID.")
        exit()
    for n in range(0, strlen + 1):
        if n % 2 == 0 and n != 0:
            outformated += "0x"
            outformated += secret_hex[n - 2: n].upper()
            if n < strlen :
                outformated += ", "
    return cformat % (outformated)

def get_device_ID(useCacheFile):
    if useCacheFile == False:
        return gen_device_ID()

    fileName = "./.device.id"
    if not os.path.isfile(fileName):
        try:
            file = open(fileName, "w")
            deviceId = gen_device_ID()
            file.write(deviceId)
            return deviceId
        except:
            print_error("Error while writing device ID cache.")
        finally:
            file.close()

    try:
        file = open(fileName, "r")
        return file.read()
    except:
        print_error("Error while reading device ID cache file. Returning new device ID.")
    finally:
        file.close()   

    return gen_device_ID() 

device_id = get_device_ID(True)         # get device id using cache file
print_info(f"Device ID: {device_id}")

env.Append(CPPDEFINES=[
  ("DEVICE_ID_RAW", device_id)
])


