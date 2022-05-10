#include <stdio.h>

#include "ssd1306.h"

int main(int argc, char **argv)
{
    send_command("0x00", sizeof("0x00"));
    send_cursor_pos(25);
    send_line_num(5);
    
    /*
    printf("cursor_pos = %d; line_num = %d\n",
            read_cursor_pos(), read_line_num()
        );
    */

    send_message("k0v4", sizeof("k0v4"));
    

    while(1){

    }
    
    return 0;
}
