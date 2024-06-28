//TITLE: emulator																					
//Declaration of Authorship -----
											
//AUTHOR :  PRAKHAR SHUKLA
//ROLL NUMBER : 2201CS54

//This cpp file, emu.cpp, is part of the MiniProject Phase - I of CS210 at the 
//department of Computer Science and Engg, IIT Patna.




#include <bits/stdc++.h>
using namespace std;

#define pii pair<int, int>
#define loop(i,a,b) for(int i = a; i < b; i++)
#define nl std::cout<<endl;
#define INT_MAX_MEMORY 16000000

string fName;
string command_to_be_executed;

vector<int> macCodes;	 // stores machine code
int memorySpace[INT_MAX_MEMORY];		 // stores memory


struct mnemonic
{
    // Opcode in Hexadecimal

    string opc; 



    /*  Type:
        1 - No Operands
        2 - Argument is Given (one operand)
        3 - Offset is Given (one operand)
    */

    int mnemonic_number;
    
};


int A, B, PC, SP; // registers

int parsed_lines = 0;
pii RW;




void ldc(int value);
void adc(int value);
void ldl(int offset);
void stl(int offset);
void ldnl(int offset);
void stnl(int offset);
void add(int value);
void sub(int value);
void shl(int value);
void shr(int value);
void adj(int value);
void a2sp(int value);
void sp2a(int value);
void call(int offset);
void ret(int value);
void brz(int offset);
void brlz(int offset);
void br(int offset);
void halt(int value);



void (*func[])(int value) = {ldc, adc, ldl, stl, ldnl, stnl, add, sub, shl, shr, adj, a2sp, sp2a, call, ret, brz, brlz, br, halt};


map<int, string> mnemo_mapper;
map<string, mnemonic> opcode;
map<string, int> command_mapping;



void opct();
void menoInit();
void comm_init();
string intToHexString(int num);
void Trace(ofstream &outputFile);
void read_command();
void write_command();
void before_execution();
void after_execution();
void wipe_registers();
void ISA();

bool executeInstruction(int currentLine, int flag, ofstream &outputFile);
void parseLines(int flag, ofstream &outputFile);
void executeCommand(string command, ofstream &outputFile);
void takeInput(string filename);
void generateTraceAndExecuteCommand();


void errorMsg();
void errorMsg2();

// Main function for the emulator program, takes command-line arguments
int main(int argc, char **argv)
{
    // Check if the correct number of command-line arguments is provided
    if (argc <= 2)
    {
        errorMsg(); // Display an error message if arguments are insufficient
        return 0;
    }

    // Extract command and filename from command-line arguments
    command_to_be_executed = argv[1];
    fName = argv[2];

    // Execute the specified command and generate trace
    generateTraceAndExecuteCommand();

    return 0;
}


// Function to display an error message with the expected format and available commands
void errorMsg()
{
    // Display entry message
    std::cout << "Invalid format."; nl
    std::cout << "Expected format: ./emu [command] filename.o"; nl

    // Display implemented commands
    std::cout << "Following commands are implemented:"; nl
    std::cout << "-isa    display the ISA list"; nl
    std::cout << "-trace  show instruction trace"; nl
    std::cout << "-read   show reading of memory"; nl
    std::cout << "-write  show writing of memory"; nl
    std::cout << "-before show memory dump BEFORE execution"; nl
    std::cout << "-after  show memory dump AFTER execution"; nl
    std::cout << "-wipe   WIPE written flags before execution"; nl
}


// Function to display an error message for an invalid command, listing available commands
void errorMsg2()
{
    // Display entry message
    std::cout << "Invalid command detected, please choose one of the following: "; nl

    // Display available commands from the command_mapping
    for (auto it : command_mapping)
    {
        std::cout << it.first; nl
    }
}





// Function to generate instruction trace and execute the specified command
void generateTraceAndExecuteCommand()
{
    string fileNameWithoutExt;

    // Extract filename without extension for use in trace file creation
    size_t pos1 = fName.find(".");
    if (pos1 != string::npos) fileNameWithoutExt = fName.substr(0, pos1);

    string nameTraceFile = fileNameWithoutExt + ".trace";

    // Open file for writing trace
    ofstream outputFile(nameTraceFile);

    // Initialize opcode table, mnemonic mapper, and command map
    opct();
    menoInit();
    comm_init();

    // Load machine code from the object file
    takeInput(fName);

    // Execute the specified command and generate the trace
    executeCommand(command_to_be_executed, outputFile);
}



// All operations 
void ldc(int value)
{
	B = A;
	A = value;
}

void adc(int value)
{
	A = A + value;
}

void ldl(int offset)
{
	B = A;
	A = memorySpace[SP + offset];
	RW = {SP + offset, 0};
}

void stl(int offset)
{
	RW = {SP + offset, memorySpace[SP + offset]};
	memorySpace[SP + offset] = A;
	A = B;
}

void ldnl(int offset)
{
	A = memorySpace[A + offset];
	RW = {SP + offset, 0};
}

void stnl(int offset)
{
	RW = {SP + offset, memorySpace[SP + offset]};
	memorySpace[A + offset] = B;
}

void add(int value)
{
	A = A + B;
}

void sub(int value)
{
	A = B - A;
}

void shl(int value)
{
	A = B << A;
}

void shr(int value)
{
	A = B >> A;
}

void adj(int value)
{
	SP = SP + value;
}

void a2sp(int value)
{
	SP = A; 
	A = B;
}

void sp2a(int value)
{
	B = A;
	A = SP;
}

void call(int offset)
{
	B = A;
	A = PC;
	PC = PC + offset;
}

void ret(int value)
{
	PC = A;
	A = B;
}
void brz(int offset)
{
	if (A == 0)
	{
		PC = PC + offset;
	}
}

void brlz(int offset)
{
	if (A < 0)
	{
		PC = PC + offset;
	}
}

void br(int offset)
{
	PC = PC + offset;
}

void halt(int value)
{
	return;
}

void opct()
{
    //OPCode table for mnemonics
    // No OPCODE for data and SET
    opcode["data"]={"",2},opcode["ldc"]={"00",2},opcode["adc"]={"01",2},opcode["ldl"]={"02",3},opcode["stl"]={"03",3},opcode["ldnl"]={"04",3};
    opcode["stnl"]={"05",3},opcode["add"]={"06",1},opcode["sub"]={"07",1};
    opcode["shl"]={"08",1},opcode["shr"]={"09",1},opcode["adj"]={"0a",2};
    opcode["a2sp"]={"0b",1},opcode["sp2a"]={"0c",1},opcode["call"]={"0d",3};
    opcode["return"]={"0e",1},opcode["brz"]={"0f",3},opcode["brlz"]={"10",3};
    opcode["br"]={"11",3},opcode["HALT"]={"12",1},opcode["SET"]={"",2};
}

void menoInit()
{
	//initialize mnemonics
	
	mnemo_mapper.insert({0, "ldc"});
	mnemo_mapper.insert({1, "adc"});
	mnemo_mapper.insert({2, "ldl"});
	mnemo_mapper.insert({3, "stl"});
	mnemo_mapper.insert({4, "ldnl"});
	mnemo_mapper.insert({5, "stnl"});
	mnemo_mapper.insert({6, "add"});
	mnemo_mapper.insert({7, "sub"});
	mnemo_mapper.insert({8, "shl"});
	mnemo_mapper.insert({9, "shr"});
	mnemo_mapper.insert({10, "adj"});
	mnemo_mapper.insert({11, "a2sp"});
	mnemo_mapper.insert({12, "sp2a"});
	mnemo_mapper.insert({13, "call"});
	mnemo_mapper.insert({14, "return"});
	mnemo_mapper.insert({15, "brz"});
	mnemo_mapper.insert({16, "brlz"});
	mnemo_mapper.insert({17, "br"});
	mnemo_mapper.insert({18, "HALT"});
}

void comm_init()
{
	//intialize commans mapper
	command_mapping["-trace"]=1;
	command_mapping["-read"]=2;
	command_mapping["-write"]=3;
	command_mapping["-before"]=4;
	command_mapping["-after"]=5;
	command_mapping["-wipe"]=6;
	command_mapping["-isa"]=7;
}

string intToHexString(int num) 
{
	//convert int to hexadecimal string
    stringstream sss;
    sss << hex << setw(8) << setfill('0') << num;
    return sss.str();
}


string formatValue(const string& label, int value) 
{
	//function to print to trace
    stringstream sss;
    sss << label << " = " << setfill('0') << setw(8) << std::hex << value << ", ";
    return sss.str();
}
string formatValue1(const string& label, int value) 
{
	//function to print to trace
    stringstream sss;
    sss << label << " = " << setfill('0') << setw(8) << std::hex << value <<endl;
    return sss.str();
}

// prints the registers
void Trace(ofstream &outputFile)
{

	//print into trace file and also onto terminal
	outputFile << formatValue("PC", PC) << formatValue("SP", SP) << formatValue("A", A) << formatValue1("B", B);

	printf("PC = %08X, SP = %08X, A = %08X, B = %08X ", PC, SP, A, B);
}

void read_command()
{
	// -read 
	std::cout << "Reading memory[" << intToHexString(RW.first) << "] finds " << intToHexString(A); nl
}

void write_command()
{
	// -write
	std::cout << "Writing memory[" << intToHexString(RW.first) << "] was " << intToHexString(RW.second) << " now " << intToHexString(memorySpace[RW.first]); nl
}


void before_execution()
{
	// -before
	int n_size = (int)macCodes.size();
	std::cout << "Memory dump prior to execution : "; nl
	for (int i = 0; i < n_size; i = i + 4)
	{
		std::cout << intToHexString(i) << " ";
		loop(j, i, min(n_size, i + 4))
		{
			std::cout << intToHexString(macCodes[j]) << " ";
		}
		nl
	}
}


void after_execution()
{
	// -after
	int n_size = (int)macCodes.size();
	std::cout << "Memory dump post execution"; nl
	for (int i = 0; i < n_size; i = i + 4)
	{
		std::cout << intToHexString(i) << " ";

		loop(j, i, min(n_size, i + 4))
		{
			std::cout << intToHexString(memorySpace[j]) << " ";
		}

		nl
	}
}

//resetting A, B, SP, PC
void wipe_registers()
{
	A = B = SP = PC = 0;
}

// Instruction set
void ISA()
{
	std::cout<<"Opcode Mnemonic Operand"; 
	nl
	std::cout<<"0      ldc      value ";
	nl
	std::cout<<"1      adc      value ";
	nl
	std::cout<<"2      ldl      value ";
	nl
	std::cout<<"3      stl      value ";
	nl
	std::cout<<"4      ldnl     value ";
	nl
	std::cout<<"5      stnl     value ";
	nl
	std::cout<<"6      add            ";
	nl
	std::cout<<"7      sub            ";
	nl
	std::cout<<"9      shr            ";
	nl
	std::cout<<"10     adj      value ";
	nl
	std::cout<<"11     a2sp           ";
	nl
	std::cout<<"12     sp2a           ";
	nl
	std::cout<<"13     call     offset";
	nl
	std::cout<<"14     return         ";
	nl
	std::cout<<"15     brz      offset";
	nl
	std::cout<<"16     brlz     offset";
	nl
	std::cout<<"17     br       offset";
	nl
	std::cout<<"18     HALT           ";
	nl
	std::cout<<"       SET      value ";
	nl
}


// Execute each line after decoding
bool executeInstruction(int currentLine, int flag, ofstream &outputFile)
{
	int n_size = (int)macCodes.size();
	int opcode_line = (currentLine & 0xFF);
	int value = (currentLine - opcode_line);
	value >>= 8;

	parsed_lines++;



	(func[opcode_line])(value);
	if ((PC < 0) || (PC > n_size) || (parsed_lines > (INT_MAX_MEMORY)))
	{
		//std::cout<<PC; nl
		std::cout<<"Segmentation fault."; nl
		std::cout<<"Program overflowing/too large."; nl
		std::cout<<"(possible infinite loop)"; nl
		return true;
	}


	if (flag == 0)
	{
		Trace(outputFile);
		std::cout << mnemo_mapper[opcode_line] << " ";
		if (opcode[mnemo_mapper[opcode_line]].mnemonic_number > 1)
		{
			std::cout << intToHexString(value);
		}
		nl
	}
	else if ((flag == 1) && ((opcode_line == 2) || (opcode_line == 4)))
	{
		read_command();
	}


	else if ((flag == 2) && ((opcode_line == 3) || (opcode_line == 5)))
	{
		write_command();
	}


	if (opcode_line >= 18)
	{
		return true;
	}



	return false;
}

// call for each line
void parseLines(int flag, ofstream &outputFile)
{
	int n_size = (int)macCodes.size();
	while (PC < n_size)
	{
		int currentLine = macCodes[PC++];
		bool quit_or_no = executeInstruction(currentLine, flag, outputFile);
		if (quit_or_no == true)
		{
			break;
		}
	}
}

// check and execute command given in input 
void executeCommand(string command, ofstream &outputFile)
{

	if(command_mapping.find(command) == command_mapping.end())
	{
		errorMsg2();
		exit(0);
	}
	
	else
	{

		if (command_mapping[command] == 7)
		{
			ISA();
		}

		else if (command_mapping[command] == 6)
		{			
			wipe_registers();
		}

		else if (command_mapping[command] == 5)
		{
			parseLines(3, outputFile);
			after_execution();
		}


		else if (command_mapping[command] == 4)
		{			
			parseLines(3, outputFile);
			before_execution();
		}


		else if (command_mapping[command] == 3)
		{
			parseLines(2, outputFile);
		}


		else if (command_mapping[command] == 2)
		{
			parseLines(1, outputFile);
		}

		else if (command_mapping[command] == 1)
		{
			parseLines(0, outputFile);
			std::cout << "Program executed sucessfully."; nl
			std::cout << "Trace file generated successfully."; nl
		}
	}


	std::cout << "Number of instructions executed : " << parsed_lines;
	nl
}


// Function to read machine code from the object file (.o) and load it into the emulator's memory
void takeInput(std::string filename)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    unsigned int cur;
    int counter = 0;

    // Read machine code from the binary object file
    while (file.read((char *)&cur, sizeof(int)))
    {
        macCodes.push_back(cur);   // Store machine code in vector for reference
        memorySpace[counter++] = cur; // Load machine code into emulator's memory space
    }
}
