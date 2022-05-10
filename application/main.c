#include <stdio.h>

#include "ssd1306.h"
#include "common.h"

extern volatile _Bool flag;

int main(int argc, char **argv)
{
    reg_timer();

    send_command("0x00", sizeof("0x00"));
    send_cursor_pos(5);
    send_line_num(5);
    
    /*
    printf("cursor_pos = %d; line_num = %d\n",
            read_cursor_pos(), read_line_num()
        );
    */

    send_message("HELLO", sizeof("HELLO"));
    

    while(1){
        if(flag == 1){
            send_cursor_pos(5);
            send_line_num(5);
            send_message("k0v4", sizeof("k0v4"));
            read_cursor_pos();
            read_line_num();

            printf("cursor_pos = %d; line_num = %d\n",
                ssd1306_data.cursor_pos, ssd1306_data.line_num);
            printf("read_cursor_pos = %d; read_line_num = %d\n",
                ssd1306_data.read_cursor_pos, ssd1306_data.read_line_num);

            flag = 0;
        }
    }
    
    return 0;
}
