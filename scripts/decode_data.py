import sys
import string

def chars(string):
    return [c for c in string]


KEYCODE_LOOKUP =    ['0x00', '0x01', '0x02', '0x03'] + \
                    chars(string.ascii_lowercase + string.digits) + \
                    ['<ENTER>', '<ESC>', '<BS>', '<TAB>', ' '] +\
                    chars('-=[]') + \
                    ['\\\\'] + \
                    chars('#;"`,./') + \
                    ['<CAPS>'] + \
                    [f'F{i}' for i in range(1, 13)] + \
                    ['<PSCR>', '<SCRLK>', '<PAUSE>', '<INS>', '<HOME>', '<PGUP>', '<DEL>', '<END>', '<PGDN>', '<RARR>', '<LARR>', '<DARR>', '<UARR>', '<NUMLK>'] + \
                    chars('/*-+') + \
                    ['<ENTER>'] + \
                    chars(string.digits + '.') + \
                    ['\\\\', '<MENU>', '<POWER>', '='] + \
                    ['<RCTRL>', '<RSHFT>', '<RART>', '<RGUI>', '<RCTRL>', '<RSHFT>', '<RALT>', '<RGUI>']


if __name__ == '__main__':

    data = sys.stdin.readline()

    data = data[:-1]    # Remove trailing newline

    hexcodes = [data[i:i+2] for i in range(0, len(data), 2)]

    for hc in hexcodes:
        byte = int(hc, 16)
        is_pressed = (byte & 0x80) >> 7
        keycode = byte & 0x7F

        if is_pressed:
            print('pressed ', end='')
        else:
            print('released ', end='')

        
        print(KEYCODE_LOOKUP[keycode])


