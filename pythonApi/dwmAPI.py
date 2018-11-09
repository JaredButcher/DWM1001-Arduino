import serial
import struct

conn = serial.Serial("COM10", 115200, timeout=.1)

def send(data):
    conn.write(bytearray(data))
    
def receive(length):
    return conn.read(length)


def dwm_pos_get():
    tx = [0x02, 0x00]
    send(tx)
    rxData = receive(18)
    
    '''
    data_cnt = 5
    x = rx_data[data_cnt] + (rx_data[data_cnt+1]<<8) + (rx_data[data_cnt+2]<<16) + (rx_data[data_cnt+3]<<24)
    data_cnt += 4
    y = rx_data[data_cnt] + (rx_data[data_cnt+1]<<8) + (rx_data[data_cnt+2]<<16) + (rx_data[data_cnt+3]<<24)
    data_cnt += 4
    z = rx_data[data_cnt] + (rx_data[data_cnt+1]<<8) + (rx_data[data_cnt+2]<<16) + (rx_data[data_cnt+3]<<24)
    data_cnt += 4
    qf = rx_data[data_cnt]'''
    x = struct.unpack('i', rxData[5:9])
    y = struct.unpack('i', rxData[9:13])
    z = struct.unpack('i', rxData[13:17])
    qf = rxData[17]
    return rxData

while True:
    input()
    print(dwm_pos_get())

#define LOC_DATA_POS_OFFSET   3
#define LOC_DATA_DIST_OFFSET  LOC_DATA_POS_OFFSET+15
#returns dwm_loc_data_t
dwm_loc_get():
   txData = [0x12, 0]
   uint16_t rx_len;
   uint8_t data_cnt, i, j;
   send(txData)

   if(LMH_WaitForRx(rx_data, &rx_len, DWM1001_TLV_MAX_SIZE) == RV_OK)
   {
      if(rx_len<3+15+3)// ok + pos + distance/range
      {
         return RV_ERR;
      }
      
      if(rx_data[LOC_DATA_POS_OFFSET]==DWM1001_TLV_TYPE_POS_XYZ)//0x41
      {
         // node self position.
         data_cnt = LOC_DATA_POS_OFFSET+2;// jump Type and Length, goto data
         loc->p_pos->x = rx_data[data_cnt] 
                      + (rx_data[data_cnt+1]<<8) 
                      + (rx_data[data_cnt+2]<<16) 
                      + (rx_data[data_cnt+3]<<24); 
         data_cnt += 4;
         loc->p_pos->y = rx_data[data_cnt] 
                      + (rx_data[data_cnt+1]<<8) 
                      + (rx_data[data_cnt+2]<<16) 
                      + (rx_data[data_cnt+3]<<24); 
         data_cnt += 4;
         loc->p_pos->z = rx_data[data_cnt] 
                      + (rx_data[data_cnt+1]<<8) 
                      + (rx_data[data_cnt+2]<<16) 
                      + (rx_data[data_cnt+3]<<24); 
         data_cnt += 4;
         loc->p_pos->qf = rx_data[data_cnt++];
      }
      
      if(rx_data[LOC_DATA_DIST_OFFSET]==DWM1001_TLV_TYPE_DIST)//0x48
      {
         // node is Anchor, recording Tag ID, distances and qf
         loc->anchors.dist.cnt = rx_data[LOC_DATA_DIST_OFFSET+2];
         loc->anchors.an_pos.cnt = 0;
         data_cnt = LOC_DATA_DIST_OFFSET + 3; // jump Type, Length and cnt, goto data
         for (i = 0; i < loc->anchors.dist.cnt; i++)
         {
            // Tag ID
            loc->anchors.dist.addr[i] = 0;
            for (j = 0; j < 8; j++)
            {
               loc->anchors.dist.addr[i] += rx_data[data_cnt++]<<(j*8);
            }
            // Tag distance
            loc->anchors.dist.dist[i] = 0;
            for (j = 0; j < 4; j++)
            {
               loc->anchors.dist.dist[i] += rx_data[data_cnt++]<<(j*8);
            }
            // Tag qf
            loc->anchors.dist.qf[i] = rx_data[data_cnt++];
         }
      }
      else if (rx_data[LOC_DATA_DIST_OFFSET]==DWM1001_TLV_TYPE_RNG_AN_POS_DIST)//0x49
      {
         // node is Tag, recording Anchor ID, distances, qf and positions
         loc->anchors.dist.cnt = rx_data[LOC_DATA_DIST_OFFSET+2];
         loc->anchors.an_pos.cnt = rx_data[LOC_DATA_DIST_OFFSET+2];
         data_cnt = LOC_DATA_DIST_OFFSET + 3; // jump Type, Length and cnt, goto data
         for (i = 0; i < loc->anchors.dist.cnt; i++)
         {
            // anchor ID
            loc->anchors.dist.addr[i] = 0;
            for (j = 0; j < 2; j++)
            {
               loc->anchors.dist.addr[i] += ((uint64_t)rx_data[data_cnt++])<<(j*8);
            }
            // anchor distance
            loc->anchors.dist.dist[i] = 0;
            for (j = 0; j < 4; j++)
            {
               loc->anchors.dist.dist[i] += ((uint32_t)rx_data[data_cnt++])<<(j*8);
            }
            // anchor qf
            loc->anchors.dist.qf[i] = rx_data[data_cnt++];
            // anchor position
            loc->anchors.an_pos.pos[i].x  = rx_data[data_cnt] 
                                         + (rx_data[data_cnt+1]<<8) 
                                         + (rx_data[data_cnt+2]<<16) 
                                         + (rx_data[data_cnt+3]<<24); 
            data_cnt += 4;
            loc->anchors.an_pos.pos[i].y = rx_data[data_cnt] 
                                         + (rx_data[data_cnt+1]<<8) 
                                         + (rx_data[data_cnt+2]<<16) 
                                         + (rx_data[data_cnt+3]<<24); 
            data_cnt += 4;
            loc->anchors.an_pos.pos[i].z = rx_data[data_cnt] 
                                         + (rx_data[data_cnt+1]<<8) 
                                         + (rx_data[data_cnt+2]<<16) 
                                         + (rx_data[data_cnt+3]<<24); 
            data_cnt += 4;
            loc->anchors.an_pos.pos[i].qf = rx_data[data_cnt++];
         }
      }
      else
      {
         return RV_ERR;   
      }
   }
   else
   {
      return RV_ERR;   
   }
   return RV_OK;
}