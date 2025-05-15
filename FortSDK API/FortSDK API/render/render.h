#pragma once
#include "../util/util.h"
#include "render.h"
#include <Windows.h>
#include <iostream>
#include "../cheat/esp/esp.h"
#include "font/fortnitefont.h"

//Cracky

extern IDirect3D9Ex* p_Object;
extern IDirect3DDevice9Ex* p_Device;
extern D3DPRESENT_PARAMETERS p_Params;

IDirect3D9Ex* p_Object = NULL;
IDirect3DDevice9Ex* p_Device = NULL;
D3DPRESENT_PARAMETERS p_Params = { NULL };

HWND GameHwnd = NULL;
RECT GameRect = { NULL };
HWND MyHwnd = NULL;
MSG Message = { NULL };

ImFont* FortniteFont;


HRESULT DirectXInit() {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(3);

	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = MyHwnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = Width;
	p_Params.BackBufferHeight = Height;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;
	p_Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, MyHwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device))) {
		p_Object->Release();
		exit(4);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(MyHwnd);
	ImGui_ImplDX9_Init(p_Device);
	ImGui::StyleColorsClassic();
	ImGuiStyle* style = &ImGui::GetStyle();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	FortniteFont = io.Fonts->AddFontFromMemoryTTF(FnFont, sizeof(FnFont), 15.f);

	ImGui::StyleColorsClassic();
	style->WindowTitleAlign.x = 0.50f;;

	p_Object->Release();
	return S_OK;
}

void CreateOverlay() {
	WNDCLASSEXA wcex = {
		sizeof(WNDCLASSEXA),
		0,
		DefWindowProcA,
		0,
		0,
		nullptr,
		LoadIcon(nullptr, IDI_APPLICATION),
		LoadCursor(nullptr, IDC_ARROW),
		nullptr,
		nullptr,
		("LemarJackson"),
		LoadIcon(nullptr, IDI_APPLICATION)
	};

	RECT Rect;
	GetWindowRect(GetDesktopWindow(), &Rect);

	RegisterClassExA(&wcex);

	MyHwnd = CreateWindowExA(NULL, ("LemarJackson"), ("Anal"), WS_POPUP, Rect.left, Rect.top, Rect.right, Rect.bottom, NULL, NULL, wcex.hInstance, NULL);

	SetWindowLong(MyHwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(MyHwnd, &margin);
	ShowWindow(MyHwnd, SW_SHOW);
	SetWindowPos(MyHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetLayeredWindowAttributes(MyHwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	UpdateWindow(MyHwnd);
}

void CleanuoD3D() {
	if (p_Device != NULL) {
		p_Device->EndScene();
		p_Device->Release();
	}

	if (p_Object != NULL) {
		p_Object->Release();
	}
}


bool bMenu = true;
ImVec4 headerColor = ImVec4(0.1f, 0.1f, 0.3f, 1.0f);
ImVec4 accentColor = ImVec4(0.0f, 0.8f, 1.0f, 1.0f);

void Menu() {
	ImGuiStyle* style = &ImGui::GetStyle();

	if (aimbot.showFOV) {
		ImGui::GetOverlayDrawList()->AddCircle(ImVec2(Width / 2, Height / 2), aimbot.fovvalue, ImColor(255, 255, 255), 64, 1);
	}

	if (GetAsyncKeyState(VK_INSERT) & 1) bMenu = !bMenu;

	if (bMenu) {

		ImGui::PushFont(FortniteFont);

		style->Colors[ImGuiCol_WindowBg] = headerColor;
		style->Colors[ImGuiCol_Border] = accentColor;
		style->Colors[ImGuiCol_CheckMark] = accentColor;
		style->Colors[ImGuiCol_FrameBg] = headerColor;
		style->Colors[ImGuiCol_Button] = accentColor;
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
		style->Colors[ImGuiCol_ButtonActive] = accentColor;

		ImGui::SetNextWindowSize({ 620.f, 350.f });
		ImGui::Begin("CRACKY", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar);

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
		ImGui::TextColored(accentColor, "CRACKY");
		ImGui::PopFont();

		// Tab bar
		if (ImGui::BeginTabBar("Tabs")) {
			if (ImGui::BeginTabItem("Aimbot")) {
				ImGui::Checkbox("Enable Aimbot", &aimbot.enable);
				ImGui::Checkbox("Draw FOV Circle", &aimbot.showFOV);
				ImGui::SliderFloat("FOV Size", &aimbot.fovvalue, 10.0f, 360.0f, "%.1f");
				ImGui::SliderFloat("Aim Smooth", &aimbot.smoothness, 1.0f, 20.0f, "%.1f");
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Visuals")) {
				ImGui::Checkbox("Box", &visual.FullBox);
				ImGui::SameLine();
				ImGui::Checkbox("Filled Box", &visual.FilledBox);
				ImGui::Checkbox("UserDistance", &visual.userDistance);
				ImGui::Checkbox("Weapon", &visual.WeaponName);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Misc")) {
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Settings")) {
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("World")) {
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}


WPARAM MainLoop() {
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT) {
		if (PeekMessage(&Message, MyHwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();
		if (GetAsyncKeyState(0x23) & 1)
			exit(8);

		if (hwnd_active == GameHwnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(MyHwnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(GameHwnd, &rc);
		ClientToScreen(GameHwnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = GameHwnd;
		io.DeltaTime = 1.0f / 1200.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;
		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom) {

			old_rc = rc;

			Width = rc.right;
			Height = rc.bottom;

			p_Params.BackBufferWidth = Width;
			p_Params.BackBufferHeight = Height;
			SetWindowPos(MyHwnd, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
			p_Device->Reset(&p_Params);
		}
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ActorLoop();
		Menu();

		ImGui::EndFrame();
		p_Device->SetRenderState(D3DRS_ZENABLE, false);
		p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
		if (p_Device->BeginScene() >= 0) {
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			p_Device->EndScene();
		}
		HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);

		if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_Device->Reset(&p_Params);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanuoD3D();
	DestroyWindow(MyHwnd);
	return Message.wParam;
}