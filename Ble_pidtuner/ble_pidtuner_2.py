# Quick BLE hack to receive sensor information
# on Laptop without using the USB serial.
# Since BLE has a really short range, it should just
# be used for testing and PID tuning.

# I. import dependencies
import time
import uuid
import Adafruit_BluefruitLE
import sys
import struct
import binascii
import curses

# II. subroutines
def initialize_terminal():
    stdscr = curses.initscr()
    curses.noecho()
    curses.cbreak()
    stdscr.keypad(True)
    curses.start_color()
    curses.init_pair(1, curses.COLOR_BLACK, curses.COLOR_WHITE)
    curses.init_pair(2, curses.COLOR_WHITE, curses.COLOR_GREEN)
    return stdscr

def characteristic_to_float(data):
    val = struct.unpack('f', data)
    return val

def float_to_characteristic(float_value):
    val = struct.pack('f', float_value)
    return val

# III. rv struct
class RemoteVariable(object):
    def __init__(self,name,uuid_string):
        #initializes the object
        self.name=name
        self.uuid=uuid.UUID(uuid_string)
        self.characteristic=None
    def write(self,value):
        self.characteristic.write_value(float_to_characteristic(value))
    def read(self):
        return characteristic_to_float(self.characteristic.read_value())

SERVICE_UUID=uuid.UUID('2E8C6277-2DDE-4D80-8C4B-629876703C70')
#rvs
rv1=RemoteVariable('ROLL_KP','2E8C6277-2DDE-4D80-8C4B-629876703C71')
rv2=RemoteVariable('ROLL_TI','2E8C6277-2DDE-4D80-8C4B-629876703C72')
rv3=RemoteVariable('ROLL_TD','2E8C6277-2DDE-4D80-8C4B-629876703C73')
rv4=RemoteVariable('ROLL_TDD','2E8C6277-2DDE-4D80-8C4B-629876703C74')
rv5=RemoteVariable('PITCH_KP','2E8C6277-2DDE-4D80-8C4B-629876703C75')
rv6=RemoteVariable('PITCH_TI','2E8C6277-2DDE-4D80-8C4B-629876703C76')
rv7=RemoteVariable('PITCH_TD','2E8C6277-2DDE-4D80-8C4B-629876703C77')
rv8=RemoteVariable('PITCH_TDD','2E8C6277-2DDE-4D80-8C4B-629876703C78')


characteristic_uuids=[rv1.uuid,rv2.uuid,rv3.uuid,rv4.uuid,rv5.uuid,rv6.uuid,rv7.uuid,rv8.uuid]

# IV. mainroutine
ble = Adafruit_BluefruitLE.get_provider()

def main():
    #Wiping the terminal
    stdscr=initialize_terminal()
    ble.clear_cached_data()

    # Get the first available BLE network adapter and make sure it's powered on.
    # Disconnect any currently connected UART devices.
    adapter = ble.get_default_adapter()
    adapter.power_on()
    ble.disconnect_devices([SERVICE_UUID])

    # Scan for UART devices.
    stdscr.addstr(0,0,'Searching for Quad101 device...                          ')
    stdscr.refresh()

    device=None
    while(device is None):
        adapter.start_scan()
        device = ble.find_device(service_uuids=[SERVICE_UUID])
        adapter.stop_scan()

    #Connecting quad 101
    stdscr.addstr(0,0,'Connecting to Quad101 device...                          ')
    stdscr.refresh()
    device.connect()

    if(device.is_connected):
        stdscr.addstr(0,0,'Discovering services...                              ')
        stdscr.refresh()
        device.discover([SERVICE_UUID], characteristic_uuids)

        # Find the UART service and its characteristics.
        quad101 = device.find_service(SERVICE_UUID)

        rv1.characteristic = quad101.find_characteristic(rv1.uuid)
        rv2.characteristic = quad101.find_characteristic(rv2.uuid)
        rv3.characteristic = quad101.find_characteristic(rv3.uuid)
        rv4.characteristic = quad101.find_characteristic(rv4.uuid)
        rv5.characteristic = quad101.find_characteristic(rv5.uuid)
        rv6.characteristic = quad101.find_characteristic(rv6.uuid)
        rv7.characteristic = quad101.find_characteristic(rv7.uuid)
        rv8.characteristic = quad101.find_characteristic(rv8.uuid)

        stdscr.clear()
        stdscr.addstr(0,0,'Connected',curses.color_pair(2))
        stdscr.refresh()

        #Define LOOP Functions
        def print_rvs():
            stdscr.addstr(4,0,'(1)'+rv1.name+':'+str(rv1.read()),curses.color_pair(1))
            stdscr.addstr(5,0,'(2)'+rv2.name+':'+str(rv2.read()),curses.color_pair(1))
            stdscr.addstr(6,0,'(3)'+rv3.name+':'+str(rv3.read()),curses.color_pair(1))
            stdscr.addstr(7,0,'(4)'+rv4.name+':'+str(rv4.read()),curses.color_pair(1))
            stdscr.addstr(8,0,'(5)'+rv5.name+':'+str(rv5.read()),curses.color_pair(1))
            stdscr.addstr(9,0,'(6)'+rv6.name+':'+str(rv6.read()),curses.color_pair(1))
            stdscr.addstr(10,0,'(7)'+rv7.name+':'+str(rv7.read()),curses.color_pair(1))
            stdscr.addstr(11,0,'(8)'+rv8.name+':'+str(rv8.read()),curses.color_pair(1))
            stdscr.refresh()

        def write_rv(rv):
            stdscr.clear()
            stdscr.addstr(4,0,'Enter'+rv.name+', use a point for decimal separation:',curses.color_pair(1))
            stdscr.refresh()
            curses.nocbreak()
            curses.echo()
            value=float(stdscr.getstr(5,0))
            rv.write(value)
            curses.cbreak()
            curses.noecho()
            stdscr.clear()

        #mainloop
        stdscr.clear()
        while True:
            stdscr.addstr(0,0,'Connected',curses.color_pair(2))
            stdscr.addstr(13,0,'Enter (1) to (8) to modify a value, enter (q) to quit.',curses.color_pair(1))
            stdscr.refresh()

            print_rvs()

            try:
                key = stdscr.getkey()
            except:
                key = None
            if key == 'q':
                #quit
                stdscr.addstr(0,0,'Disconnecting...                                         ')
                stdscr.refresh()
                device.disconnect()
                curses.nocbreak()
                stdscr.keypad(False)
                curses.echo()
                curses.endwin()
                break
            elif key == '1':
                write_rv(rv1)
            elif key == '2':
                write_rv(rv2)
            elif key == '3':
                write_rv(rv3)
            elif key == '4':
                write_rv(rv4)
            elif key == '5':
                write_rv(rv5)
            elif key == '6':
                write_rv(rv6)
            elif key == '7':
                write_rv(rv7)
            elif key == '8':
                write_rv(rv8)


# Initialize the BLE system.  MUST be called before other BLE calls!
ble.initialize()
# Start the mainloop to process BLE events, and run the provided function in
# a background thread.  When the provided main function stops running, returns
# an integer status code, or throws an error the program will exit.
ble.run_mainloop_with(main)
