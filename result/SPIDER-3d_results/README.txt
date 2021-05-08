IDS data summarized from SPIDER3D
Flow Bytes/s -  Goodput*125
BWD Packet Length Std - [(Goodput/(PacketsReceived+ACKsReceived+PacketsLost))*ACKsReceived] Std by time (e.g., time 20's BWD Packet Length Std, x_bar is average from time 1-20)
Destination Port - Always 80
Subflow Fwd Bytes - (Goodput/(PacketsReceived+ACKsReceived+PacketsLost))*PacketsReceived
Total Length of Fwd Packets - (Goodput/(PacketsReceived+ACKsReceived+PacketsLost))*PacketsReceived
Init_Win_bytes_forward - Goodput/(PacketsReceived+ACKsReceived+PacketsLost) (Trick: assume each packet same size within 1 second include init)
act_data_pkt_fwd - PacketsReceived
Fwd IAT Min - min((1/PacketsReceived) of each time )
Bwd Packets/s - ACKsReceived
Average Packet Size - Goodput/(PacketsReceived+ACKsReceived+PacketsLost)

All the SPIDER-3D data are without any attack.