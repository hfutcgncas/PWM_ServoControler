SET PATH=D:\Program Files (x86)\Keil\ARM\ARMCC\bin\;D:\Program Files\Daheng Imavision\MER-Series\GenICam\bin\Win64_x64;D:\Program Files\Daheng Imavision\MER-Series\APIDll\Win64;D:\Program Files\Daheng Imavision\MER-Series\GenICam\bin\Win32_i86;D:\Program Files\Daheng Imavision\MER-Series\APIDll\Win32;C:\ProgramData\Oracle\Java\javapath;C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v6.5\bin;C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v6.5\libnvvp;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;D:\Program Files\opencv\build\x64\vc12\bin;D:\Program Files\opencv\build\x86\vc12\bin;D:\Program Files (x86)\CTEX\UserData\miktex\bin;D:\Program Files (x86)\CTEX\MiKTeX\miktex\bin;D:\Program Files (x86)\CTEX\CTeX\ctex\bin;D:\Program Files (x86)\CTEX\CTeX\cct\bin;D:\Program Files (x86)\CTEX\CTeX\ty\bin;D:\Program Files (x86)\CTEX\Ghostscript\gs9.05\bin;D:\Program Files (x86)\CTEX\GSview\gsview;D:\Program Files (x86)\CTEX\WinEdt;C:\Program Files\Intel\WiFi\bin\;C:\Program Files\Common Files\Intel\WirelessCommon\;C:\Windows\System32;C:\Program Files\Microsoft SQL Server\110\Tools\Binn\;C:\Program Files\Microsoft SQL Server\120\Tools\Binn\;C:\Program Files (x86)\Windows Kits\8.1\Windows Performance Toolkit\;C:\Program Files (x86)\Microsoft SDKs\TypeScript\1.0\;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;D:\Anaconda2;D:\Anaconda2\Scripts;D:\Anaconda2\Library\bin;C:\Program Files\Intel\WiFi\bin\;C:\Program Files\Common Files\Intel\WirelessCommon\
SET CPU_TYPE=STM32F103C8
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=Target 1
SET CPU_CLOCK=0x044AA200
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmAsm" --Via "..\obj\startup_stm32f10x_md._ia"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\minibalance.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\delay.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\sys.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\usart.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\led.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\key.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\oled.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\adc.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\timer.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\motor.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\encoder.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\ioi2c.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\24l01.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\spi.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\usart2.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\control.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\datascope_dp.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\inv_mpu.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\inv_mpu_dmp_motion_driver.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\filter.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\mpu6050.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmCC" --Via "..\obj\show.__i"
"D:\Program Files (x86)\Keil\ARM\ARMCC\bin\ArmLink" --Via "..\OBJ\Minibalance.lnp"
D:\Program Files (x86)\Keil\ARM\ARMCC\bin\fromelf.exe "..\OBJ\Minibalance.axf" --i32combined --output "..\OBJ\Minibalance.hex"
fromelf.exe --bin -o ./Minibalance.bin ../OBJ/Minibalance.axf
