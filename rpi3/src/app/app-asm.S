.globl sendInterrupt
sendInterrupt:
    svc #0x0A


.globl _read_char_from_register
_read_char_from_register:
    ret x13