  LWS F0 10(R1)
    ADDS F3 F0 F2
    BEQZ R3 TARGET
    SUBS F2 F5 F4
TARGET:    ADDS F4 F2 F3
    SUBS F2 F5 F4
    EOP