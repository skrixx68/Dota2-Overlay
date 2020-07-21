// Dota2MemReader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "include.h"
#include "proc.h"


using std::cout;
using std::cin;
using std::endl;

//Prototype
std::vector<unsigned int> getOffsetFromText();
void exitOverlay();
void writeFile(bool vBe);
void MainHack();


// TODO: Add more features like patternscan , cameradistance etc...
int main()
{
    MainHack();
}


void MainHack()
{
    //Get Handle to Process
    HANDLE hProcess = 0;

    //Get ProcId of the target process
    DWORD procId = GetProcId(L"dota2.exe");
    if (!procId == 0)
    {
        cout << "Process id found = " << procId << endl;

        //Getmodulebaseaddress
        uintptr_t moduleBase = GetModuleBaseAddress(procId, L"engine2.dll");
        //std::cout << std::hex << "Module Base Address: 0x" << moduleBase << std::endl;

        hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, NULL, procId);

        if (!hProcess == 0)
        {
            cout << "Handle successfully attached..." << endl;
            //Load offsets from file
            std::vector<unsigned int> offsets = getOffsetFromText();

            //Resolve base address of the pointer chain
            uintptr_t dynamicPtrBaseAddr = moduleBase + offsets[0];

            //Resolve our pointer chain
            uintptr_t vbEAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, offsets);
            cout << "Waiting ingame activation..." << endl;
            while (vbEAddr == 0)
            {
                vbEAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, offsets);
            }

            bool visible = false;
            cout << endl << "Overlay success" << endl;
            writeFile(visible);
            while (true) // Loop = 50ms update
            {
                if (GetKeyState(VK_END))
                {
                    break;

                }

                int vbEVal = 0; //buffer
                //Read Value from address
                ReadProcessMemory(hProcess, (BYTE*)vbEAddr, &vbEVal, sizeof(vbEVal), nullptr);

                //Test Value
                if (vbEVal == 14) // Visible by enemy
                {
                    if (visible == false)
                    {
                        visible = true;
                        writeFile(visible);

                    }

                }
                else if (vbEVal >= 6 && vbEVal <= 10) // Not visible by enemy
                {
                    if (visible == true)
                    {
                        visible = false;
                        writeFile(visible);

                    }

                }
                else // Address not found or value not initiazized ingame
                {
                    cout << " Address not found." << endl;
                    break;

                }

                Sleep(50);

            }
            CloseHandle(hProcess);

        }
        else
        {
            cout << "Failed to attach process. Try running with administrative previlages";

        }
    }
    else
    {
        cout << "Process not found." << endl;

    }
    exitOverlay();
    cout << ".........." << endl;
    Sleep(3000);
}

//Write value of vbe status to text.
//This will be read by the Overlay
void writeFile(bool vBe)
{
    std::ofstream myfile;
    myfile.open("vBe.txt");
    myfile << vBe;
    myfile.close();
}

//close overlay process
void exitOverlay() {
    DWORD parentProcId = GetProcId(L"Dota2Overlay.exe");
    if (!parentProcId == 0)
    {
        HANDLE parentProc = 0;
        parentProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, parentProcId);
        TerminateProcess(parentProc, 0);
        CloseHandle(parentProc);

    }
}


//Not Visible = 06(radiant team) , 10(dire team) ; Visible = 14 
//Update address
//AOB = 06 00 00 00 ?? ?? ?? ?? F? 7F 00 00 ?? ?? ?? 0? 00 00 00 00 0? 00 00 00 00 00 00 00 ?0 0? ?? ?? = 32bytes (50results)
//AOB sig  = 06 00 00 00 ?0 ?? ?? ?? F? 7F 00 00 ?? ?? ?? 0? 00 00 00 00 0? 00 00 00 00 00 00 00 ?0 0? ?? ?? ?? 0? 00 00 0? 00 00 00 00 00 00 00 0? 00 00 00 00 00 00 00 ?0 ?? ?? ?? ?? 0? 00 00 ?? 00 00 00 00 00 00 00 0? FF FF FF = 72bytes (1result)
//BaseAddr = engine2.dll + 00??????
//Offset usually ends with  = 170 0 1E4


//Read offsets from config file.
std::vector<unsigned int> getOffsetFromText() {

    std::fstream file;
    std::string word;
    std::vector<std::string> offsets;
    std::vector<unsigned int> offsetsInt;

    //Open config file
    file.open("offs.conf", std::ios::out | std::ios::in);
    if (file.fail()) {
        cout << "Failed to load config file...";
    }

    int i = 0;
    while (file >> word)
    {
        if (i >= 8) break;

        offsets.push_back(word);
        i++;
    }
    for (size_t i = 0; i < offsets.size(); i++)
    {
        std::istringstream buffer(offsets[i]);
        unsigned long long value;
        buffer >> std::hex >> value;
        offsetsInt.push_back(value);
    }
    return offsetsInt;
}