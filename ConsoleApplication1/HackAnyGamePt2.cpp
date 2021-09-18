// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"

int main()
{
	HANDLE hProcess = 0;

	uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0;
	bool bHealth = false, bAmmo = false, bRecoil = false;

	const int newValue = 1337;

	DWORD procId = GetProcId(L"ac_client.exe");
	
	if (procId)
	{

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

		moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

		localPlayerPtr = moduleBase + 0x10F4F4;

		healthAddr = FindDMAAddy(hProcess, localPlayerPtr, { 0xf8 });
	}

	else
	{
		std::cout << "Process not found, press enter to exit\n";
		getchar();
		return 0;
	}

	DWORD dwExit = 0;

	while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
	{
		if (GetAsyncKeyState(VK_NUMPAD1))
		{
			bHealth = !bHealth;
		}

		if (GetAsyncKeyState(VK_NUMPAD2))
		{
			bAmmo = !bAmmo;

			if (bAmmo)
			{
				//ff 06 = inc [esi];
				mem::PatchEx((BYTE*)(moduleBase + 0x10F4F4), (BYTE*)"\xFF\X6", 2, hProcess);
			}
			else {
				//ff 0E = dec [esi];
				mem::PatchEx((BYTE*)(moduleBase + 0x10F4F4), (BYTE*)"\xFF\x0E", 2, hProcess);
			}
		}		
		
		if (GetAsyncKeyState(VK_NUMPAD3))
		{
			bRecoil = !bRecoil;

			if (bRecoil)
			{
				mem::NopEx((BYTE*)(moduleBase + 0x63786), 10, hProcess);
			}
			else {
				mem::PatchEx((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10, hProcess);
			}
		}	
		
		if (GetAsyncKeyState(VK_INSERT))
		{
			return 0;
		}

		if (bHealth)
		{
			mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
		}

		Sleep(10);
	}

	std::cout << "Process not found, press enter to exit\x";
	getchar();
	return 0;
}
