#include <bits/stdc++.h>
using namespace std;

int IC, PTR, VA, RA, flag[30], PI, TI, SI;
char M[300][4], IR[4], R[4];
bool C, isError;
fstream fin, fout;
string line;

struct PCB{
    int job, TTL, TLL, TTC, TLC;
} pcb;

// Initialize the program environment
void init()
{
    for(int i = 0; i < 300; i++){
        for(int j = 0; j < 4; j++){
            M[i][j] = ' ';
        }
    }
    // Initialize block occupancy flags
    for(int i = 0; i < 30; i++){
        flag[i] = 0;
    }
    for(int i = 0; i < 4; i++){
        IR[i] = '-';
        R[i] = '-';
    }
    IC = 0;            
    C = false;
    isError = false;
    PTR = 0;           
    VA = 0;            
    RA = 0;            
    PI = 0;            
    TI = 0;            
    SI = 0;            
}

// Generate random number between 0 to 29 for storing the program in mm
int allocate()
{
    return (rand() % 30);
}

// Memory mapping function that maps the virtual address with the real address
int AddMap(int reg)
{
    int PTE = PTR * 10 + (reg / 10);   
    
    // If page table is NULL at this entry
    if(M[PTE][0] == '*')
    {
        return -1; // Page Fault
    }
    else{
        string temp = "";
        for(int i = 0; i < 4; i++){
            if(isdigit(M[PTE][i])){
                temp += M[PTE][i];
            }
        }
        if(temp == "") return -1;
        int data = (stoi(temp) * 10) + (reg % 10);
        return data;
    }
}

void status(){
    fout << "Job ID: " << pcb.job << "\tSI: " << SI << "\tTI: " << TI << "\tPI: " << PI 
         << "\tTTC : " << pcb.TTC << "\tLLC: " << pcb.TLC << "\tIR: ";
    for(int i = 0; i < 4; i++){
         fout << IR[i];
    }
    fout << "\n\n";
}

void endprogram()
{
    fout << "\n---PROGRAM TERMINATED ABNORMALLY---\n";
}

void MOS()
{
    if(SI == 1)
    {
        getline(fin, line);
        if(line[0] == '$' && line[1] == 'E' && line[2] == 'N' && line[3] == 'D'){
            endprogram();
            fout << "Error: Out of Data\n";
            status();
            isError = true;
            return;
        }
        int frame = allocate();
        while (flag[frame] != 0){
            frame = allocate();
        }
        flag[frame] = 1;
        
        int i = PTR * 10;
        while (M[i][0] != '*'){
            i++;
        }
        M[i][0] = ' ';
        M[i][1] = ' ';
        M[i][2] = (frame / 10) + 48;
        M[i][3] = (frame % 10) + 48;
        
        int l = 0;
        int f_ptr = frame * 10;
        for (int k = 0; k < line.length() && k < 40; k++){
            M[f_ptr][l++] = line[k];
            if (l == 4){
                l = 0;
                f_ptr += 1;
            }
        }
    }
    else if(SI == 2){
        pcb.TLC += 1;
        if (pcb.TLC > pcb.TLL){
            endprogram();
            fout << "Error: Line Limit Exceed\n";
            status();
            isError = true;
            return;
        }
        int add = ((IR[2] - 48) * 10) + (IR[3] - 48); 
        RA = AddMap(add);
        if (RA != -1){
            string out;
            for (int i = 0; i < 10; i++){
                for (int j = 0; j < 4; j++){
                    out += M[RA][j];
                }
                RA += 1;
            }
            fout << out << "\n";
        } else {
            endprogram();
            PI = 3;
            fout << "Error ( PI = 3 ): Invalid Page Fault\n";
            status();
            isError = true;
        }
    }
    else if(SI == 3){
        fout << "\nProgram Terminated Successfully\n";
        status();
        isError = true; 
    }
}

void executeProgram()
{
    while(1){
        if (PI != 0 || TI != 0 ){
            endprogram();
            fout << "JOBID: " << pcb.job << "\tIC: " << IC << "\tToggle : " << C 
                 << "\tTLC: " << pcb.TLC << "\tTTC: " << pcb.TTC << "\tTTL: " << pcb.TTL 
                 << "\tTLL: " << pcb.TLL << "\tIR: ";
            for (int i = 0; i < 4; i++){
                fout << IR[i];
            }
            fout << "\n\n";
            break;
        }
        
        RA = AddMap(IC);
        
        if (RA == -1) { 
            endprogram();
            PI = 3;
            fout << "Error ( PI = 3 ): Invalid Page Fault\n";
            status();
            break;
        }

        if(M[RA][0] != 'H' && (!isdigit(M[RA][2]) || !isdigit(M[RA][3]))){
            endprogram();
            PI = 2;
            fout << "Error ( PI = 2 ): Operand Error\n";
            for (int i = 0; i < 4; i++) IR[i] = M[RA][i];
            status();
            break; 
        }

        for (int i = 0; i < 4; i++){
            IR[i] = M[RA][i];
        }
        IC++;
        
        int add = ((IR[2] - 48) * 10) + (IR[3] - 48);
        
        // Time validation
        if((IR[0] == 'G' && IR[1] == 'D') || (IR[0] == 'S' && IR[1] == 'R')){
            pcb.TTC += 2;
        }
        else{
            pcb.TTC += 1;
        }
        if(pcb.TTC > pcb.TTL){
            TI = 2;
            endprogram();
            fout << "Error ( TI = 2 ): Time Limit Exceed\n";
            status();
            break;
        }
        
        // Execution
        if(IR[0] == 'L' && IR[1] == 'R'){
            int mapRA = AddMap(add);
            if(mapRA == -1){
                endprogram();
                PI = 3;
                fout << "Error ( PI = 3 ): Invalid Page Fault\n";
                status();
                break;
            }
            else{
                for(int i = 0; i < 4; i++){
                    R[i] = M[mapRA][i];
                }
            }
        }
        else if(IR[0] == 'S' && IR[1] == 'R'){
            int mapRA = AddMap(add);
            if(mapRA == -1){
                int frame = allocate();
                while (flag[frame] != 0){
                    frame = allocate();
                }
                flag[frame] = 1;
                int i = PTR * 10;
                while (M[i][0] != '*'){
                    i++;
                }
                M[i][0] = ' ';
                M[i][1] = ' ';
                M[i][2] = (frame/10) + 48;
                M[i][3] = frame % 10 + 48;
                frame = frame * 10;
                for (int k = 0; k < 4; k++){
                    M[frame][k] = R[k];
                }
            }
            else{
                for(int i = 0; i < 4; i++){
                    M[mapRA][i] = R[i];
                }
            }
        }
        else if(IR[0] == 'C' && IR[1] == 'R'){
            int b = 0;
            int mapRA = AddMap(add);
            if(mapRA == -1){
                endprogram();
                PI = 3;
                fout << "Error ( PI = 3 ): Invalid Page Fault\n";
                status();
                break;
            }
            else{
                for(int i = 0; i < 4; i++){
                    if(R[i] != M[mapRA][i]){
                        b = 1;
                    }
                }
                if(b == 1){
                    C = false;
                }
                else{
                    C = true;
                }
            }
        }
        else if(IR[0] == 'B' && IR[1] == 'T'){
            if(C == true){
                IC = add;
            }
        }
        else if(IR[0] == 'G' && IR[1] == 'D'){
            SI = 1;
            MOS();
            if(isError) break;
        }
        else if(IR[0] == 'P' && IR[1] == 'D'){
            SI = 2;
            MOS();
            if(isError) break;
        }
        else if(IR[0] == 'H'){
            SI = 3;
            MOS();
            break;
        }
        else{
            endprogram();
            PI = 1;
            fout << "Error ( PI = 1 ): Opcode Error\n";
            status();
            break;
        }
    }
}

// Load in memory
void load()
{
    while(getline(fin, line))
    {
        if(line[0] == '$' && line[1] == 'A' && line[2] == 'M' && line[3] == 'J')
        {
            init();
            PTR = allocate(); 
            
            for(int i = PTR * 10; i < PTR * 10 + 10; i++){
                for(int j = 0; j < 4 ; j++){
                    M[i][j] = '*';
                }
            }
            flag[PTR] = 1;
            
            pcb.TTC = 0;
            pcb.TLC = 0;
            string job_id = "", ttl = "", tll = "";
            
            for(int i = 4; i < 8; i++){
                job_id += line[i];
            }
            for(int i = 8; i < 12; i++){
                ttl += line[i];
            }
            for(int i = 12; i < 16; i++){
                tll += line[i];
            }
            
            pcb.job = stoi(job_id);
            pcb.TTL = stoi(ttl);
            pcb.TLL = stoi(tll);
        }
        else if(line[0] == '$' && line[1] == 'D' && line[2] == 'T' && line[3] == 'A')
        {
            IC = 0;
            executeProgram();
        }
        else if(line[0] == '$' && line[1] == 'E' && line[2] == 'N' && line[3] == 'D')
        {
            // Reset for safety if needed, end of job block
        }
        else{ 
            // Program card
            int frame = allocate();
            while(flag[frame] != 0){
                frame = allocate();
            }
            flag[frame] = 1;
            
            int a = PTR * 10; 
            while(M[a][0] != '*'){
                a++;
            }
            M[a][0] = ' ';
            M[a][1] = ' ';
            M[a][2] = (frame / 10) + 48; 
            M[a][3] = (frame % 10) + 48;
            
            int len = 0;
            for(int i = frame * 10; i < frame * 10 + 10 && len < line.length(); i++){
                for(int j = 0; j < 4; j++){
                    if(line[len] == 'H'){
                        M[i][j] = line[len++];
                        break;
                    }
                    else{
                        M[i][j] = line[len++];
                    }
                }
            }
        }
    }
}

int main()
{
    // Updated filenames
    fin.open("input.txt", ios::in);
    fout.open("output.txt", ios::out);
    
    if(!fin.is_open()) {
        cout << "Failed to open input.txt\n";
        return 1;
    }
    
    load();
    
    fin.close();
    fout.close();
    return 0;
}