import serial

conn = serial.Serial("COM11", 115200)

def send(data):
    conn.write(bytearray(data))
    
def receive(length):
    return conn.read(length)


def dwm_pos_get():
    tx = [0x02, 0x00]
    send(tx)
    rx_data = receive(18)
    data_cnt = 5
    x = rx_data[data_cnt] + (rx_data[data_cnt+1]<<8) + (rx_data[data_cnt+2]<<16) + (rx_data[data_cnt+3]<<24)
    data_cnt += 4
    y = rx_data[data_cnt] + (rx_data[data_cnt+1]<<8) + (rx_data[data_cnt+2]<<16) + (rx_data[data_cnt+3]<<24)
    data_cnt += 4
    z = rx_data[data_cnt] + (rx_data[data_cnt+1]<<8) + (rx_data[data_cnt+2]<<16) + (rx_data[data_cnt+3]<<24)
    data_cnt += 4
    qf = rx_data[data_cnt]

    print("X: " + str(x))
    print("Y: " + str(y))
    print("Z: " + str(z))
    print("QF: " + str(qf))

input()
dwm_pos_get()