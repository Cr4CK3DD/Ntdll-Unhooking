# About
This repo contains some projects I ended up making during my learning journey on unhooking.

# Hooking
- A malicious user or programs wants to spawn “malware.exe”. To do this, CreateProcessW is called to create the new process and its primary thread.
- The “process create” callback function is executed, and sends a pre notification to the EDR driver stating that a new process is going to be created.
- The EDR driver instructs the EDR application (EDR_Process.exe) to inject and hook NTDLL in the memory space of the application (malware.exe) to redirect execution flow to itself.

# Unhooking

First thing we need to do is to check if our Ntdll is Hooked.

## Detecting Hooks
We can do that by simply checking if the first 4 bytes of syscall stub is correct { 4C, 8B, D1, B0, ??, ??, ??, ??}.

This is how unhooked syscall stub should look like:

![image](https://github.com/user-attachments/assets/31be2ba5-5e3f-4e3c-9c97-0bf37d3a2f9c)

Keep in mind that this method will generate some false positives:

![Pasted image 20240810194458](https://github.com/user-attachments/assets/8e48e100-11f6-4966-9532-77f53295a6de)

## Removing Hooks
If we confirm that our Ntdll is hooked, we can get a fresh copy of Ntdll and replace the hooked section with the clean one.

![](https://www.ired.team/~gitbook/image?url=https%3A%2F%2F386337598-files.gitbook.io%2F%7E%2Ffiles%2Fv0%2Fb%2Fgitbook-legacy-files%2Fo%2Fassets%252F-LFEMnER3fywgFHoroYn%252F-M9b77N-cstE2jRzZI4M%252F-M9b7TdH_eS5azVO-Vxe%252Fimage.png%3Falt%3Dmedia%26token%3D3f4c233b-0388-4c0c-9801-ab20b472ec38&width=768&dpr=4&quality=100&sign=d8226aad&sv=1)

# Resources:
https://www.ired.team/offensive-security/defense-evasion/how-to-unhook-a-dll-using-c++

https://synzack.github.io/Blinding-EDR-On-Windows/
