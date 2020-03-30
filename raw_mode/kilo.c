
/*** Includes ***/
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

/*** data ***/
struct termios orig_termios;

/*** terminal ***/
void die(const char *s){
    perror(s);
    exit(1);

}

void disableRawMode(){
   if( tcsetattr(STDIN_FILENO,TCSAFLUSH, &orig_termios)== -1){
        die("tcsetattr"); 
   }
}
void enableRawMode(){
    
    if(tcgetattr(STDIN_FILENO, &orig_termios)==-1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = orig_termios;
   
        
    tcgetattr(STDIN_FILENO, &raw);
   
    //trun off canonical mode.... 
    //canonical mode waits for user to hit enter before reading input
    //ISIG turns off ctrl-c and ctrl-z signals
    //IEXTEN disables ctrl-V

    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    //turn off ctrl-Q and ctrl-S
    //ICRNL fixes ctrl-M output
    raw.c_lflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);

    raw.c_cflag |= (CS8);

    //turn off all output processing
    raw.c_oflag &= ~(OPOST);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if(tcsetattr(STDIN_FILENO,TCSAFLUSH, &raw)==-1) die("tcsetattr");
}


/*** init program ***/
int main(){
    enableRawMode();

    char c;
    while( 1){
        char c = '\0';

        if(read(STDIN_FILENO, &c, 1)==-1 && errno != EAGAIN) die("read");
    
        if(iscntrl(c)){
            printf("%d\r\n",c);
        }else{
            printf("%d ('%c')\r\n",c,c);
        }
        if(c == 'q'){
            break;
        }

    }

    return 0;
}
