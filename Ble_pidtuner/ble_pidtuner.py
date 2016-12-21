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

def characteristic_to_char(data):
    val = struct.unpack('B',data)
    return val

def char_to_characteristic(char_value):
    val = struct.pack('B',char_value)
    return val

# III. quad ble service uuids
SERVICE_UUID=uuid.UUID('2E8C6277-2DDE-4D80-8C4B-629876703C70')
#ROLL
ROLL_KP_UUID=uuid.UUID('2E8C6277-2DDE-4D80-8C4B-629876703C71')
ROLL_TI_UUID=uuid.UUID('2E8C6277-2DDE-4D80-8C4B-629876703C72')
ROLL_TD_UUID=uuid.UUID('2E8C6277-2DDE-4D80-8C4B-629876703C73')
ROLL_TDD_UUID=uuid.UUID('2E8C6277-2DDE-4D80-8C4B-629876703C74')
#PITCH
PITCH_KP_UUID=uuid.UUID('2E8C6277-2DDE-4D80-8C4B-629876703C75')
PITCH_TI_UUID=uuid.UUID('2E8C6277-2DDE-4D80-8C4B-629876703C76')
PITCH_TD_UUID=uuid.UUID('2E8C6277-2DDE-4D80-8C4B-629876703C77')
PITCH_TDD_UUID=uuid.UUID('2E8C6277-2DDE-4D80-8C4B-629876703C78')
#startstop
START_STOP_UUID=uuid.UUID('2E8C6277-2DDE-4D80-8C4B-629876703C79')

characteristic_uuids=[ROLL_KP_UUID,ROLL_TI_UUID,ROLL_TD_UUID,ROLL_TDD_UUID,PITCH_KP_UUID,PITCH_TI_UUID,PITCH_TD_UUID,PITCH_TDD_UUID,START_STOP_UUID]

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

        roll_kp = quad101.find_characteristic(ROLL_KP_UUID)
        roll_ti = quad101.find_characteristic(ROLL_TI_UUID)
        roll_td = quad101.find_characteristic(ROLL_TD_UUID)
        roll_tdd = quad101.find_characteristic(ROLL_TDD_UUID)

        pitch_kp = quad101.find_characteristic(PITCH_KP_UUID)
        pitch_ti = quad101.find_characteristic(PITCH_TI_UUID)
        pitch_td = quad101.find_characteristic(PITCH_TD_UUID)
        pitch_tdd = quad101.find_characteristic(PITCH_TDD_UUID)

        start_stop = quad101.find_characteristic(START_STOP_UUID)


        stdscr.clear()
        stdscr.addstr(0,0,'Connected',curses.color_pair(2))
        stdscr.refresh()

        #Define LOOP Functions
        def print_constants():
            stdscr.addstr(3,0,'ROLLCONTROL')
            stdscr.addstr(4,0,'KP'+'    '+str(characteristic_to_float(roll_kp.read_value())),curses.color_pair(1))
            stdscr.addstr(5,0,'TI'+'    '+str(characteristic_to_float(roll_ti.read_value())),curses.color_pair(1))
            stdscr.addstr(6,0,'TD'+'    '+str(characteristic_to_float(roll_td.read_value())),curses.color_pair(1))
            stdscr.addstr(7,0,'TDD'+'    '+str(characteristic_to_float(roll_tdd.read_value())),curses.color_pair(1))

            stdscr.addstr(3,25,'PITCHCONTROL')
            stdscr.addstr(4,25,'KP'+'    '+str(characteristic_to_float(pitch_kp.read_value())),curses.color_pair(1))
            stdscr.addstr(5,25,'TI'+'    '+str(characteristic_to_float(pitch_ti.read_value())),curses.color_pair(1))
            stdscr.addstr(6,25,'TD'+'    '+str(characteristic_to_float(pitch_td.read_value())),curses.color_pair(1))
            stdscr.addstr(7,25,'TDD'+'    '+str(characteristic_to_float(pitch_tdd.read_value())),curses.color_pair(1))
            stdscr.refresh()

        def write_roll_con():
            stdscr.addstr(10,0,'ROLL-MODE: Enter (p),(i),(d) or (a).',curses.color_pair(1))
            stdscr.refresh()

            key = stdscr.getkey()
            if key == 'p':
                stdscr.clear()
                stdscr.addstr(10,0,'Enter KP, use a point for decimal separation:',curses.color_pair(1))
                stdscr.refresh()
                curses.nocbreak()
                curses.echo()
                val=float(stdscr.getstr(11,0))
                roll_kp.write_value(float_to_characteristic(val))
                curses.cbreak()
                curses.noecho()
                stdscr.clear()
            elif key == 'i':
                stdscr.clear()
                stdscr.addstr(10,0,'Enter TI, use a point for decimal separation:',curses.color_pair(1))
                stdscr.refresh()
                curses.nocbreak()
                curses.echo()
                val=float(stdscr.getstr(11,0))
                roll_ti.write_value(float_to_characteristic(val))
                curses.cbreak()
                curses.noecho()
                stdscr.clear()
            elif key == 'd':
                stdscr.clear()
                stdscr.addstr(10,0,'Enter TD, use a point for decimal separation:',curses.color_pair(1))
                stdscr.refresh()
                curses.nocbreak()
                curses.echo()
                val=float(stdscr.getstr(11,0))
                roll_td.write_value(float_to_characteristic(val))
                curses.cbreak()
                curses.noecho()
                stdscr.clear()
            elif key == 'a':
                stdscr.clear()
                stdscr.addstr(10,0,'Enter TDD, use a point for decimal separation:',curses.color_pair(1))
                stdscr.refresh()
                curses.nocbreak()
                curses.echo()
                val=float(stdscr.getstr(11,0))
                roll_tdd.write_value(float_to_characteristic(val))
                curses.cbreak()
                curses.noecho()
                stdscr.clear()
            else:
                stdscr.addstr(11,0,'Invalid choice, returning to update loop.',curses.color_pair(1))
                stdscr.refresh()

        def write_pitch_con():
            stdscr.addstr(10,0,'PITCH-MODE: Enter (p),(i),(d),(a)',curses.color_pair(1))
            stdscr.refresh()

            key = stdscr.getkey()
            if key == 'p':
                stdscr.clear()
                stdscr.addstr(10,0,'Enter KP, use a point for decimal separation:',curses.color_pair(1))
                stdscr.refresh()
                curses.nocbreak()
                curses.echo()
                val=float(stdscr.getstr(11,0))
                pitch_kp.write_value(float_to_characteristic(val))
                curses.cbreak()
                curses.noecho()
                stdscr.clear()
            elif key == 'i':
                stdscr.clear()
                stdscr.addstr(10,0,'Enter TI, use a point for decimal separation:',curses.color_pair(1))
                stdscr.refresh()
                curses.nocbreak()
                curses.echo()
                val=float(stdscr.getstr(11,0))
                pitch_ti.write_value(float_to_characteristic(val))
                curses.cbreak()
                curses.noecho()
                stdscr.clear()
            elif key == 'd':
                stdscr.clear()
                stdscr.addstr(10,0,'Enter TD, use a point for decimal separation:',curses.color_pair(1))
                stdscr.refresh()
                curses.nocbreak()
                curses.echo()
                val=float(stdscr.getstr(11,0))
                pitch_td.write_value(float_to_characteristic(val))
                curses.cbreak()
                curses.noecho()
                stdscr.clear()
            elif key == 'a':
                stdscr.clear()
                stdscr.addstr(10,0,'Enter TDD, use a point for decimal separation:',curses.color_pair(1))
                stdscr.refresh()
                curses.nocbreak()
                curses.echo()
                val=float(stdscr.getstr(11,0))
                pitch_tdd.write_value(float_to_characteristic(val))
                curses.cbreak()
                curses.noecho()
                stdscr.clear()
            else:
                stdscr.addstr(11,0,'Invalid choice, returning to update loop.',curses.color_pair(1))
                stdscr.refresh()
                stdscr.clear()

        #mainloop
        stop_flag=1
        stdscr.clear()
        while True:
            stdscr.addstr(0,0,'Connected',curses.color_pair(2))
            stdscr.addstr(9,0,'Enter (q) to quit, (i) to insert a value or (s) to start/stop.',curses.color_pair(1))
            if stop_flag==0:
                stdscr.addstr(0,25,'ROTORS ON ',curses.color_pair(2))
            else:
                stdscr.addstr(0,25,'ROTORS OFF')
            stdscr.refresh()
            print_constants()

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

            elif key == 's':
                #start stop
                if stop_flag:
                    start_stop.write_value(char_to_characteristic(0))
                    stop_flag=0
                else:
                    start_stop.write_value(char_to_characteristic(1))
                    stop_flag=1

            elif key == 'i':
                #Write controller
                stdscr.clear()
                stdscr.addstr(0,0,'Connected',curses.color_pair(2))
                stdscr.addstr(9,0,'(1) to change roll-con, (2) to change pitch-con',curses.color_pair(1))
                stdscr.refresh()

                key = stdscr.getkey()
                if key == '1':
                    write_roll_con()
                elif key == '2':
                    write_pitch_con()
                else:
                    stdscr.addstr(10,0,'Invalid choice, returning to update loop.',curses.color_pair(1))
                    stdscr.refresh()
                    time.sleep(1)
                    stdscr.clear()




# Initialize the BLE system.  MUST be called before other BLE calls!
ble.initialize()
# Start the mainloop to process BLE events, and run the provided function in
# a background thread.  When the provided main function stops running, returns
# an integer status code, or throws an error the program will exit.
ble.run_mainloop_with(main)
