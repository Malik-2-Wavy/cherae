#include <Windows.h>
#include <iostream>
#include "render/render.h"
#include "cheat/esp/esp.h"

//Cracky

auto main() -> int {

	if (!requests::FDriver()) {
		std::cout << "Driver not Loaded";
	}

	requests::process_id = requests::FProcess(L"FortniteClient-Win64-Shipping.exe");
    virtualaddy = requests::FImage();
	cr3 = requests::FCr3();

	CreateOverlay();
	DirectXInit();
	MainLoop();

}