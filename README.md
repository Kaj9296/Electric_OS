# Electric_OS  
  
A simple operating system inspired by whetever will work.  

# About  
Boot Loader: GNU-EFI  
Arch: x86_64  
   
Main Goal:   
To create a simple and completely useless operating system, because i can.  
  
## Build instructions   

Use the ```make all``` command to compile the OS.  

You can also use the ```make run``` command or the "run.bat" file to run the OS in qemu.  
If your insane enough to try and run on it real hardware simply flash the .img file in the bin directory to a USB.

## Next steps 

Make heap part of STL.
Implement grandfather system for processes.
Implement startchild and startsibling commands.
Fix shutdown general protection fault.  
Write more comments.  
Create own stack in the kernel.  
