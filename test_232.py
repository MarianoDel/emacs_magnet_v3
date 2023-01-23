import serial


speed = 115200
# port = '/dev/ttyUSB0'    # para el Prolific
port = '/dev/ttyACM0'    # para el Microchip
data0 = bytes.fromhex('5a a5 07 82 10 02 33 31 33 32')    #'3132' numero de serie
data1 = bytes.fromhex('5a a5 07 82 10 12 68 6f 6c 61')    #'hola' localidad
data2 = bytes.fromhex('5a a5 07 82 00 84 5a 01 00 01')    #cambio de pantalla
# data = bytes.fromhex('5a a5 07 82 10 02 33 31 33 32')
# data = bytes(10)    #10 ceros hexa
# data = bytes(b'121 121 121 121')

try:
    ser = serial.Serial(port, speed)
    if (ser != None):
        print ("Port Open\n")
        ser.write(data1)


except:
    print ("Port Not Open\n")
            

