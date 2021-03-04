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

class IPMI_Ioctl(object):
    _IONONE = 0
    _IOWRITE = 1
    _IOREAD = 2

    IPMI_MAINTENANCE_MODE_AUTO = 0
    IPMI_MAINTENANCE_MODE_OFF  = 1
    IPMI_MAINTENANCE_MODE_ON   = 2

    IPMICTL_GET_MAINTENANCE_MODE_CMD = _IOREAD << 30 | sizeof(c_int) << 16 | \
        ord('i') << 8 | 30  # from ipmi.h
    IPMICTL_SET_MAINTENANCE_MODE_CMD = _IOWRITE << 30 | sizeof(c_int) << 16 | \
        ord('i') << 8 | 31  # from ipmi.h

    def __init__(self):
        self.ipmidev = None
        devnodes=["/dev/ipmi0", "/dev/ipmi/0", "/dev/ipmidev/0"]
        for dev in devnodes:
            try:
                self.ipmidev = open(dev, 'rw')
                break
            except Exception as e:
                print("open file {} failed, error: {}".format(dev, e))

    def __del__(self):
        if self.ipmidev is not None:
            self.ipmidev.close()

    def get_ipmi_maintenance_mode(self):
        input_buffer=pack('i',0)
        out_buffer=fcntl.ioctl(self.ipmidev, self.IPMICTL_GET_MAINTENANCE_MODE_CMD, input_buffer)
        maintanence_mode=unpack('i',out_buffer)[0]

        return maintanence_mode

    def set_ipmi_maintenance_mode(self, mode):
        fcntl.ioctl(self.ipmidev, self.IPMICTL_SET_MAINTENANCE_MODE_CMD, c_int(mode))

class OnlPlatform_x86_64_ufispace_s9600_64x_r0(OnlPlatformUfiSpace):
    PLATFORM='x86-64-ufispace-s9600-64x-r0'
    MODEL="S9600-64X"
    SYS_OBJECT_ID=".9600.64"
    PORT_COUNT=64
    PORT_CONFIG="64x100"

    def check_bmc_enable(self):
        return 1

    def baseconfig(self):

        bmc_enable = self.check_bmc_enable()
        msg("bmc enable : %r\n" % (True if bmc_enable else False))

        # record the result for onlp
        os.system("echo %d > /etc/onl/bmc_en" % bmc_enable)

        # Golden Finger to show CPLD
        os.system("i2cset -y 0 0x75 0x1")
        os.system("i2cget -y 0 0x30 0x2")
        os.system("i2cget -y 0 0x31 0x2")
        os.system("i2cset -y 0 0x75 0x0")

        #CPLD
        self.insmod("x86-64-ufispace-s9600-64x-lpc")

        ########### initialize I2C bus 0 ###########
        # init PCA9548
        self.new_i2c_devices(
            [
                ('pca9548', 0x75, 0),
                ('pca9548', 0x73, 0),
                ('pca9548', 0x72, 0),
                ('pca9546', 0x71, 17),
                ('pca9548', 0x76, 17),
                ('pca9548', 0x76, 18),
                ('pca9548', 0x76, 19),
                ('pca9548', 0x76, 20),
                ('pca9548', 0x76, 21),
                ('pca9548', 0x76, 22),
                ('pca9548', 0x76, 23),
                ('pca9548', 0x76, 24),
            ]
        )

        self.insmod("x86-64-ufispace-eeprom-mb")
        self.insmod("optoe")

        # init SYS EEPROM devices
        self.new_i2c_devices(
            [
                #  on cpu board
                ('mb_eeprom', 0x57, 0),
            ]
        )

        # init SFP EEPROM
        for port in range(25, 27):
            self.new_i2c_device('optoe2', 0x50, port)
        # init QSFP EEPROM
        for port in range(29, 93):
            self.new_i2c_device('sff8436', 0x50, port)

        # init Temperature
        self.new_i2c_devices(
            [
                # CPU Board Temp
                ('tmp75', 0x4F, 0),
            ]
        )

        # init GPIO sysfs
        #9555_BOARD_ID
        self.new_i2c_device('pca9535', 0x20, 3)
        #9539_CPU_I2C
        self.new_i2c_device('pca9535', 0x77, 0)
        #9539_VOL_MARGIN
        self.new_i2c_device('pca9535', 0x76, 6)

        # export GPIO
        for i in range(464, 512):
            os.system("echo {} > /sys/class/gpio/export".format(i))

        # init GPIO direction
        # 9555_BOARD_ID 0x20, 9539_VOL_MARGIN 0x76, 9539_CPU_I2C 0x77
        for i in range(464, 512):
            os.system("echo in > /sys/class/gpio/gpio{}/direction".format(i))

        #CPLD
        self.insmod("x86-64-ufispace-s9600-64x-cpld")
        for i, addr in enumerate((0x30, 0x31, 0x32, 0x33, 0x34)):
            self.new_i2c_device("s9600_64x_cpld" + str(i+1), addr, 1)

        #config mac rov

        cpld_addr=30
        cpld_bus=1
        rov_addr=0x76
        rov_reg=0x21
        rov_bus=[10, 11]
        mask=[0b00000111, 0b00111000]
        shift=[0, 3]

        # vid to mac vdd value mapping
        vdd_val_array=( 0.82,  0.82,  0.76,  0.78,  0.80,  0.84,  0.86,  0.88 )
        # vid to rov reg value mapping
        rov_reg_array=( 0x73, 0x73, 0x67, 0x6b, 0x6f, 0x77, 0x7b, 0x7f )

        for index, bus in enumerate(rov_bus):
            #get rov from cpld
            reg_val_str = subprocess.check_output("cat /sys/bus/i2c/devices/{}-00{}/cpld_mac_rov".format(cpld_bus, cpld_addr), shell=True)
            reg_val = int(reg_val_str, 16)
            vid = (reg_val & mask[index]) >> shift[index]
            mac_vdd_val = vdd_val_array[vid]
            rov_reg_val = rov_reg_array[vid]
            #set rov to mac
            msg("Setting mac vdd %1.2f with rov register value 0x%x\n" % (mac_vdd_val, rov_reg_val) )
            os.system("i2cset -y {} {} {} {}".format(rov_bus[index], rov_addr, rov_reg, rov_reg_val))

        # onie syseeprom
        self.insmod("x86-64-ufispace-s9600-64x-onie-syseeprom.ko")

        self.enable_ipmi_maintenance_mode()

        return True

    def enable_ipmi_maintenance_mode(self):
        ipmi_ioctl = IPMI_Ioctl()

        mode=ipmi_ioctl.get_ipmi_maintenance_mode()
        msg("Current IPMI_MAINTENANCE_MODE=%d\n" % (mode) )

        ipmi_ioctl.set_ipmi_maintenance_mode(IPMI_Ioctl.IPMI_MAINTENANCE_MODE_ON)

        mode=ipmi_ioctl.get_ipmi_maintenance_mode()
        msg("After IPMI_IOCTL IPMI_MAINTENANCE_MODE=%d\n" % (mode) )