#include <REGX52.H>
#include <INTRINS.H>
#include "IRED_controller.h"
#include "LCD1602_func.h"

u8 Power = 0;   // only can be operated when power = 1;

u8 INPUT[10] = {' ', ' ', ' ', ' ',' ',' ',' ',' ',' ',' '};

// set if cursor need to display (0 disp, 1 not)
void Cursor_Disp_Set(void){
    struct LCD1602_Cursor cs;
    cs = LCD1602_GetCursor();
    if (cs.x < 4 || cs.x > 13) {
        LCD1602_SetDisp(0x0c); // cursor not display -> also not display when loc = 13;
    }else{
        LCD1602_SetDisp(0x0f); // cursor display and blink
    }
}

void WriteData(u8 ch){
    struct LCD1602_Cursor cs;
    cs = LCD1602_GetCursor(); 
    if (cs.x < 4 || cs.x > 13) return;
    LCD1602_ShowChar(cs.x, cs.y, ch);
    INPUT[cs.x - 4] = ch;
}


// restart as screen 
void PasswordLock_Restart(){
    if (Power) {
        LCD1602_Init(0x38, 0x0f, 0x06);
        LCD1602_ShowStr(0,0, "Password:");
        LCD1602_ShowStr(0,1, "    ");
    }
    else LCD1602_SetDisp(0x08);  // not
}

// compare password, 0 failed , 1 success 
void CmpPassword(){
    u8 cor , ans, *cmd = 0x00, i;
    u8 password[10] = {'2','0','0','4','0','6','0','9',' ',' '};
    
    cor = 1;
    for (i= 0; i < 10; i++){
        if (INPUT[i]!= password[i]){
            cor = 0; break;
        }
    }

    switch (cor)
    {
    case 0: LCD1602_Init(0x38, 0x0c, 0x06); LCD1602_ShowStr(1, 0, "Unlock Failed!"); break;
    case 1: LCD1602_Init(0x38, 0x0c, 0x06); LCD1602_ShowStr(1, 0, "Unlock Succeed!"); break;
    default: break;
    }
    LCD1602_ShowStr(0, 1, "use any btn exit");
    
    while (1){
        ans = IRED_Get_Command(cmd);
        if (ans) {
            PasswordLock_Restart();
            break;
        }
    }
    for (i = 0; i < 10; i++){ // reset
        INPUT[i] = ' '; 
    }
}


void PasswordLock_Input(u8 control_code){
    struct LCD1602_Cursor cs;
    if (!Power && control_code!=0) return;
    // check location after every time 
    cs = LCD1602_GetCursor();

    switch (control_code)
    {
    case 0: Power = !Power; PasswordLock_Restart(); break;
    case 4: if (cs.x > 4) { LCD1602_MoveCursor(0);} break; 
    case 5: if (cs.x < 13){ LCD1602_MoveCursor(1);} break;
    case 9: WriteData('0'); break;
    case 10: PasswordLock_Restart(); break; // reset
    case 11: CmpPassword();  break; // confirm and check the password 
    case 12: WriteData('1'); break; 
    case 13: WriteData('2'); break;
    case 14: WriteData('3'); break;
    case 15: WriteData('4'); break;
    case 16: WriteData('5'); break; 
    case 17: WriteData('6'); break;
    case 18: WriteData('7'); break;
    case 19: WriteData('8'); break;
    case 20: WriteData('9'); break;
    default: break; 
    }
    if (control_code!=0) Cursor_Disp_Set(); //-> when not 0 set cursor display state;
}

void main(){
    u8 ans = 0, *cmd = 0x00, codes = 0x00;
    IRED_INIT();

    LCD1602_Init(0x38, 0x08, 0x06);
    LCD1602_ShowStr(0,0, "Password:");
    LCD1602_ShowStr(0,1, "    "); // first location 0x44;

    while (1)
    {
        ans = IRED_Get_Command(cmd);
        if (ans) {
            codes = IRED_CMD_Mapping(cmd, 1); // if use simulation, change it to 1
            PasswordLock_Input(codes);
        }
    }
}
