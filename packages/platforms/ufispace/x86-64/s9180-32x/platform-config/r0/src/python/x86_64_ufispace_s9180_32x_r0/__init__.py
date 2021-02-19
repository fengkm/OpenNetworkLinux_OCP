from onl.platform.base import *
from onl.platform.ufispace import *
from struct import *
from ctypes import c_int, sizeof
import os
import sys
import subprocess
import time
import fcntl

def msg(s, fatal=False):
    sys.stderr.write(s)
    sys.stderr.flush()
    if fatal:
        sys.exit(1)

class OnlPlatform_x86_64_ufispace_s9180_32x_r0(OnlPlatformUfiSpace):
    PLATFORM='x86-64-ufispace-s9180-32x-r0'
    MODEL="S9180_32X"
    SYS_OBJECT_ID=".9180.32"
    PORT_COUNT=32
    PORT_CONFIG="32x100"
     
    def baseconfig(self):

        return True

