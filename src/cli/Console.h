#pragma once

#include <string>
#include "core/CPUCore.h"

inline const std::string logo = R"(                                                   
                                   ___                
                      ,----.     ,--.'|_              
   ,---.             /   /  \-.  |  | :,'             
  '   ,'\   .--.--. |   :    :|  :  : ' :  .--.--.    
 /   /   | /  /    '|   | .\  ..;__,'  /  /  /    '   
.   ; ,. :|  :  /`./.   ; |:  ||  |   |  |  :  /`./   
'   | |: :|  :  ;_  '   .  \  |:__,'| :  |  :  ;_     
'   | .; : \  \    `.\   `.   |  '  : |__ \  \    `.  
|   :    |  `----.   \`--'""| |  |  | '.'| `----.   \ 
 \   \  /  /  /`--'  /  |   | |  ;  :    ;/  /`--'  / 
  `----'  '--'.     /   |   | :  |  ,   /'--'.     /  
            `--'---'    `---'.|   ---`-'   `--'---'   
                          `---                                                                                     
)";

const std::string motd = "0.0.0";

void printProcessList(Scheduler& sched);
void console(char* argv[]);

