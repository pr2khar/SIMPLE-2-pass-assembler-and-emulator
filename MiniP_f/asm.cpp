//TITLE: assembler																					
//Declaration of Authorship -----
											
//AUTHOR :  PRAKHAR SHUKLA
//ROLL NUMBER : 2201CS54

//This cpp file, asm.cpp, is part of the MiniProject Phase - I of CS210 at the 
//department of Computer Science and Engg, IIT Patna.




#include<bits/stdc++.h> //Include all standard libraries.
using namespace std; 


//macros defined
#define loop(i, a, b) for (int i = a; i < b; i++)
#define revloop(i, b, a) for (int i = b; i >= a; i--)
#define vi vector<int>
#define vs vector<string>
#define mii map<int, int>
#define msi map<string, int>
#define mis map<int, string>
#define mss map<string, string>
#define msp map<string,pair<string,int>>
#define all(x) x.begin(), x.end()

//maximum range of Program Counter
const int MAX_MEMORY = 100000;


//Defining Mnemonic
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

//Defining information contained in every error message generation
struct errorInfo
{

    string error_type; 

    int line_number;
    
};

struct warningInfo
{

    string label_unused; 

    int line_number_warn;
    
};

//Storing program counter and line number where label is present
struct labelInfo
{
    int pc_value;
    int lineNum;
};

 
struct linediv
{
    //Division of line into constituents and storing separately
    string label_in_line;
    string mnemonic_in_line;
    string operand_in_line;
    //Storing operand type in every line
    int oprType;
    //checking whether label is present in given line
    bool label_or_no;
};

//struct defining the information contained in every line of listing file
struct list_line_data 
{
	string line_address;
    string macCode;
    string statement; 
};

//creating map to store mnemonics and their attributes
map<string, mnemonic> opcode;



string fName;
vector<errorInfo> error;
vector<warningInfo> warning;
msi labelPresence;
msi labelUsage;
map<string, labelInfo> labels;
vector<string> without_comm;
vector<string> clean;
map<int, linediv> source;
vector<string> MachineCodes;
vector<list_line_data> list_file_inputs;
mii generateLineForError;
mii generate2ndpasserr;
bool haltPresent = false;

//compare function to sort errors so that they can be displayed in order of lines
bool compare(errorInfo a, errorInfo b)
{
    if((a.line_number>=0) && (b.line_number>=0))
        return a.line_number <= b.line_number;
    else
        return a.line_number >= b.line_number;
}

//initialize mnemonic information table
void opct()
{
    //OPCode table
    // No OPCODE for data and SET
    opcode["data"]={"",2},opcode["ldc"]={"00",2},opcode["adc"]={"01",2},opcode["ldl"]={"02",3},opcode["stl"]={"03",3},opcode["ldnl"]={"04",3};
    opcode["stnl"]={"05",3},opcode["add"]={"06",1},opcode["sub"]={"07",1};
    opcode["shl"]={"08",1},opcode["shr"]={"09",1},opcode["adj"]={"0a",2};
    opcode["a2sp"]={"0b",1},opcode["sp2a"]={"0c",1},opcode["call"]={"0d",3};
    opcode["return"]={"0e",1},opcode["brz"]={"0f",3},opcode["brlz"]={"10",3};
    opcode["br"]={"11",3},opcode["HALT"]={"12",1},opcode["SET"]={"",2};
}


//function to check whether a given character is a digit
bool digitYN(char x) 
{
	if((x >= '0') && (x <= '9'))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//function to check whether a given character is a digit
bool alphaYN(char x) 
{
	if(((x >= 'a') && (x <= 'z')))
    {
        return true;
    }
    else if(((x >= 'A') && (x<= 'Z')))
    {
        return true;
    }
    else
    {
        return false;
    }
}


// checking if string is in Octal form
bool checkOctal(string s)
{
    int n = s.size();
    bool ok = true;
    if(n < 2)
    {
    	return false;
    }
    if(s[0] == '0')
    {
        loop(i, 1, n)
        {
            if((s[i] >= '0') && (s[i] <= '7'))
            {
                continue;
            }
            else
            {
                ok = false;
                break;
            }
        }
    }
    else
    {
        ok = false;
    }
    return ok;
}


// checking if string is in Hexadecimal form
bool checkHexadecimal(string s)
{   
    int n = s.size();
    bool ok = true ;
    if(n < 3)
    {
    	return false;
    }
    if(s[0]=='0')
    {
        if((s[1]=='x') || (s[1]=='X'))
        {
            loop(i,2,n)
            {
                if((digitYN(s[i]))|| ((s[i] >= 'a') && (s[i] <= 'f')) || (((s[i] >= 'A') && (s[i] <= 'F'))))
                {
                    continue;
                }
                else
                {
                    ok = false;
                    break;
                }
            }
        }
        else
        {
            ok = false;
        }
    }
    else
    {
        ok = false;
    }
    return ok;
}


// checking if string is in Decimal form
bool checkDecimal(string s)
{    
    int n = s.size();
	bool ok = true;
	loop(i,0,n)
    {
		if(digitYN(s[i]))
        {
            continue;
        }
        else
        {
            ok = false;
            break;
        }
    }
	return ok;
}


// checking if label name is valid
bool isValidLabelName(string s)
{    
    int n = s.size();
    if((alphaYN(s[0])) || (s[0] == '_'))
    {
	    loop(i,1,n)
        {
		    if((alphaYN(s[i])) || (s[i] == '_') || (digitYN(s[i])))
            {
			    continue;
            }
		    else 
            {
                return false;
            }
	    }
    }
    else
    {
        return false;
    }

    return true;
}



int operandType(string s)
{
    // -1 if empty or invalid
    // 0 if existing label
    // 1 is label does not exist
    // 8 if Octal
    // 10 if decimal
    // 16 if Hexadecimal
    if(s.empty())
    {
        return -1;
    }
    if((isValidLabelName(s))&&(labelPresence[s]!=0))
    {
        return 0;
    }
    if((isValidLabelName(s))&&(labelPresence[s]==0))
    {
        return 1;
    }
    if(checkHexadecimal(s))
    {
        return 16;
    }
    if(checkOctal(s))
    {
        return 8;
    }
    if(checkDecimal(s))
    {
        return 10;
    }

    return -1;
}

int HtD(string& hexString) 
{
    // Remove the '0x' prefix if present
    string hexNumber = hexString.substr(2); // Assuming the input starts with '0x'

    // Pad the hexadecimal number with leading zeros to make it 8 characters long
    if (hexNumber.length() < 8) 
    {
        hexNumber = string(8 - hexNumber.length(), '0') + hexNumber;
    }

    // Convert the hexadecimal number to decimal
    stringstream ss;
    ss << hex << hexNumber;
    int decimalValue;
    ss >> decimalValue;
    return decimalValue;
}


int OtD(string& octalString) 
{
    //convert Octal string to decimal number using stoi and base = 8
    return stoi(octalString, nullptr, 8);
}


int string_to_dec(string s, int lno)
{
    int ans = 0;
    if((s[0] != '-') && (s[0] != '+'))
    {
        //when no sign is given
        if(s == "0")
        {
            return 0;
        }

        else
        {
            if(operandType(s) == 0)
            {
                //operand is a label
                return ans = labels[s].pc_value;
            }
            else if(operandType(s) == 1)
            {
                //operand is a label name which is not defined
                error.push_back({"Label does not exist.", lno});
            }
            else if(operandType(s) == 16)
            {
                //operand is hexadecimal
                return ans = HtD(s);
            }
            else if(operandType(s) == 8)
            {
                //operand is octal
                return ans = OtD(s);
            }
            else if(operandType(s) == 10)
            {
                //operan is an integer
                return ans = stoi(s);
            }
            else
            {
                //invalid operand type
                error.push_back({"Operand is invalid/missing/not a number.", lno});
            }

        }
    }
    else if(s[0] == '-')
    {
        //same as above for negative operands
        string negs = s.substr(1);
        if(negs == "0")
        {
            return 0;
        }

        else
        {
            if(operandType(negs) == 0)
            {
                error.push_back({"Unexpected '-' detected.", lno});
            }
            else if(operandType(negs) == 1)
            {
                error.push_back({"Unexpected '-' detected.", lno});
            }
            else if(operandType(negs) == 16)
            {
                return ans = ((-1)*HtD(negs));
            }
            else if(operandType(negs) == 8)
            {
                return ans = ((-1)*OtD(negs));
            }
            else if(operandType(negs) == 10)
            {
                return ans = ((-1)*stoi(negs));
            }
            else
            {
                error.push_back({"Operand is missing/invalid.", lno});
            }

        }
    }
    else if(s[0] == '+')
    {
        //same as above for positive operands
        string poss = s.substr(1);
        if(poss == "0")
        {
            return 0;
        }

        else
        {
            if(operandType(poss) == 0)
            {
                error.push_back({"Unexpected '+' detected.", lno});
            }
            else if(operandType(poss) == 1)
            {
                error.push_back({"Unexpected '+' detected.", lno});
            }
            else if(operandType(poss) == 16)
            {
                return ans = HtD(poss);
            }
            else if(operandType(poss) == 8)
            {
                return ans = OtD(poss);
            }
            else if(operandType(poss) == 10)
            {
                return ans = stoi(poss);
            }
            else
            {
                error.push_back({"Operand is missing/invalid.", lno});
            }

        }
    }
}

string intToHexString(int num) 
{
    //converting int to hexadecimal string
    stringstream sss;
    //sss << hex << num;
    sss << hex << setw(8) << setfill('0') << num;
    return sss.str();
}

string intstringToHexString(string s)
{
    //converting int string to hexadecimal string
    int temp = stoi(s);
    string res = intToHexString(temp);
    return res;
}

string trim(string& str) 
{
    // Find the first non-space character
    auto start = str.find_first_not_of(" \t");
    // Find the last non-space character
    auto end = str.find_last_not_of(" \t");

    // If no non-space characters, return an empty string
    if (start == string::npos || end == string::npos) 
    {
        return "";
    }

    // Return the trimmed string
    return str.substr(start, end - start + 1);
}

string removeWhitespaceBeforeColon(string& str) 
{
    string result = str; // Copy the original string
    size_t colonPos = result.find(":");

    // Check if a colon is found and if the characters around the colon are whitespaces
    if (colonPos != string::npos) {
        // Check if there is a whitespace before the colon
        if (colonPos > 0 && isspace(result[colonPos - 1])) 
        {
            result.erase(colonPos - 1, 1); 
        }
    }

    return result;
}

string removeWhitespaceAfterColon(string& str) 
{
    string result = str; // Copy the original string
    size_t colonPos = result.find(":");

    // Check if a colon is found and if the character after the colon is a whitespace
    if (colonPos != string::npos) 
    {
        if(colonPos + 1 < result.length() && isspace(result[colonPos + 1]))
        {        
            result.erase(colonPos + 1, 1);
        }
    }

    return result;
}

string extractBeforeColon(string& str) 
{
    // Find the position of the colon
    size_t colonPos = str.find(":");
    // If colon is not found, return an empty string
    if (colonPos == string::npos) 
    {
        return "";
    }
    // Extract the substring before the colon
    return str.substr(0, colonPos);
}

string extractAfterColonBeforeWhitespace(string& str) 
{
    size_t colonPos = str.find(":");
    size_t whitespacePos = str.find_first_of(" \t", colonPos);

    // If colon is not found, find the first whitespace
    if (colonPos == string::npos) 
    {
        whitespacePos = str.find_first_of(" \t");
        if (whitespacePos == string::npos) 
        {
            return str; // No whitespace, return the whole string
        }
        return str.substr(0, whitespacePos);
    }

    // If no whitespace after colon, return an empty string
    if (whitespacePos == string::npos) 
    {
        return "";
    }

    // Return the substring after the colon but before the next whitespace
    return str.substr(colonPos + 1, whitespacePos - colonPos - 1);
}


string generateOperands(string& str) 
{
    // Find the position of the first whitespace
    size_t whitespacePos = str.find(" ");

    // If whitespace is not found, return an empty string
    if (whitespacePos == string::npos) 
    {
        return "";
    }

    // Return the substring after the first whitespace
    return str.substr(whitespacePos + 1);
}

bool checkForWhitespaceOrComma(string& str) 
{
    // Check for a whitespace or a comma
    size_t whitespacePos = str.find(" ");
    size_t commaPos = str.find(",");

    // If either a whitespace or a comma is found, return false (indicating an error)
    if (whitespacePos != string::npos || commaPos != string::npos) 
    {
        return false;
    }

    // If neither is found, return true (indicating no error)
    return true;
}

string reduceWhitespace(string& input)
{
    //reduce all contiguous white space to a single whitespace (if exists)
    string result;
    bool lastCharWasSpace = false;

    for (char c : input) 
    {
        if (isspace(c)) 
        {
            if (!lastCharWasSpace) 
            {
                result += ' ';
                lastCharWasSpace = true;
            }
        } 
        else 
        {
            result += c;
            lastCharWasSpace = false;
        }
    }

    return result;
}

void first_pass(vs without_comm, int *progctr)
{
    int src_no = 0;
    loop(i,0,without_comm.size())
    {        
        int linenum = i;
        string no_spaces;
        no_spaces = reduceWhitespace(without_comm[i]); //reduce whitespaces to one wherever present
        no_spaces = removeWhitespaceAfterColon(no_spaces); //remove whitespaces after colon
        no_spaces = removeWhitespaceBeforeColon(no_spaces); //remove whitespaces before colon
        no_spaces = reduceWhitespace(no_spaces); //again reduce whitespaces


        string label; string mnemo; string operands; //containers for label, mnemonic and operands for every line
        bool labelPresent = true;


        label = extractBeforeColon(no_spaces);
        mnemo = extractAfterColonBeforeWhitespace(no_spaces);
        operands = generateOperands(no_spaces);



        if(label.empty())
        {
            //boolean to store labelPresence
            labelPresent = false;
        }

        else
        {
            if(isValidLabelName(label) == false)
            {
                //invalid label name
                error.push_back({"Label name is invalid." , generateLineForError[linenum]});
            }
            else
            {
                if(labelPresence[label] == 0)
                {
                    labels.insert({label,{*progctr, generateLineForError[linenum]}});
                    labelPresence[label]++;
                }
                else
                {
                    //duplicate label
                    error.push_back({"Label name already in use.", generateLineForError[linenum]});
                }
            }
        }

        
        if(mnemo.size())
        {
            //if mnemonic exists on line
            if(opcode.find(mnemo) == opcode.end())
            {
                //invalid instruction
                error.push_back({"Invalid mnemonic/instruction.", generateLineForError[linenum]});
            }
            else
            {
                // generating errors where number of operands does not match number of operands allowed by mnemonic on line
                if((opcode[mnemo].mnemonic_number == 2) || (opcode[mnemo].mnemonic_number == 3))
                {
                    if(operands == "")
                    {
                        error.push_back({"No operand detected.", generateLineForError[linenum]});
                    }
                    else if(!checkForWhitespaceOrComma(operands))
                    {
                        error.push_back({"Unexpected operand(s) detected.", generateLineForError[linenum]});
                    }
                }
                else if(opcode[mnemo].mnemonic_number == 1)
                {
                    if(operands != "")
                    {
                        error.push_back({"Unexpected operand(s) detected.", generateLineForError[linenum]});
                    }
                }
            }
        }

        if(!mnemo.empty())
        {
            //store line data
            source.insert({(*progctr), {label, mnemo, operands, operandType(operands), labelPresent}});
            generate2ndpasserr[src_no] = linenum;
            src_no++;
            //increment PC
            (*progctr)++;
        }            
    }
}

void SETimplementation()
{
    //implement set
    loop(i, 0, MAX_MEMORY)
    {
        int linenum = i;
        if((source[i].label_in_line.empty())&&(source[i].mnemonic_in_line.empty()))
        {
            break;
        }
        if((source[i].mnemonic_in_line == "SET") && (labelPresence[source[i].label_in_line] != 0))
        {
            //reset label defined on line to value
            labels[source[i].label_in_line].pc_value = string_to_dec((source[i].operand_in_line), generateLineForError[generate2ndpasserr[linenum]]);
        }
    }
}


void second_pass()
{
    int pctr = 0; // Program counter to track the memory address
    loop(i, 0, MAX_MEMORY)
    {
        int linenum = i;
        // Check if both label and mnemonic are empty, indicating the end of the source code
        if ((source[i].label_in_line.empty()) && (source[i].mnemonic_in_line.empty()))
        {
            break;
        }
        string machine_code = "";
        int mnemotype; string mnemoop;
        // Check if the mnemonic is empty
        if (source[i].mnemonic_in_line.empty())
        {
            mnemotype = -1; // Set mnemotype to -1 if no mnemonic is present
        }    
        else
        {    
            mnemotype = opcode[source[i].mnemonic_in_line].mnemonic_number;
            mnemoop = opcode[source[i].mnemonic_in_line].opc;

            if (mnemotype == 1)
            {
                // Handle HALT instruction
                if (source[i].mnemonic_in_line == "HALT")
                {
                    haltPresent = true; // Set flag to indicate HALT instruction presence
                }
                machine_code = "000000" + mnemoop; // Generate machine code for HALT
            }
            else if (mnemotype == 2)
            {
                // Handle data and SET instructions
                if ((source[i].mnemonic_in_line == "data") || (source[i].mnemonic_in_line == "SET"))
                { 
                    int oper = string_to_dec(source[i].operand_in_line, generateLineForError[generate2ndpasserr[linenum]]);
                    labelUsage[source[i].operand_in_line]++;
                    machine_code = intToHexString(oper); // Convert operand to hexadecimal
                }
                else
                {
                    // Handle other mnemonics with or without labels
                    if (!labelPresence[source[i].operand_in_line])
                    {
                        int oper = string_to_dec(source[i].operand_in_line, generateLineForError[generate2ndpasserr[linenum]]);
                        machine_code = intToHexString(oper); // Convert operand to hexadecimal
                    }
                    else
                    {
                        int oper = string_to_dec(source[i].operand_in_line, generateLineForError[generate2ndpasserr[linenum]]);
                        labelUsage[source[i].operand_in_line]++;
                        machine_code = intToHexString(oper);
                        machine_code = intToHexString(labels[source[i].operand_in_line].pc_value);
                    }
                    machine_code = machine_code.substr(2);
                    machine_code += mnemoop;
                }
            }
            else if (mnemotype == 3)
            {
                // Handle mnemonics with relative addressing
                if (!labelPresence[source[i].operand_in_line])
                {
                    int oper = string_to_dec(source[i].operand_in_line, generateLineForError[generate2ndpasserr[linenum]]);
                    machine_code = intToHexString(oper);
                }
                else
                {
                    machine_code = intToHexString(labels[source[i].operand_in_line].pc_value - pctr - 1);
                    labelUsage[source[i].operand_in_line]++;
                }
                machine_code = machine_code.substr(2);
                machine_code += mnemoop;
            }

            while (machine_code.size() != 8)
            {
                machine_code = "0" + machine_code; // Pad machine code with leading zeros
            } 
            // Uncomment the following line to print machine code for debugging
            // cout << machine_code << endl;

            // Add machine code, label, mnemonic, and operand to respective listing file data
            MachineCodes.push_back(machine_code);
            if (!source[i].label_in_line.empty()) 
            {
                source[i].label_in_line += ": ";
            }
            if (!source[i].mnemonic_in_line.empty()) 
            {
                source[i].mnemonic_in_line += " ";
            }
            string line_list = source[i].label_in_line + source[i].mnemonic_in_line + source[i].operand_in_line; 

            if (mnemotype != -1)
            {
                list_file_inputs.push_back({intToHexString(pctr), machine_code, line_list});
                pctr++;
            }
        }
    }
}



void write(string ffname) 
{
    string log_f = ".log";
    string lst = ".lst";
    string machinecode = ".o";

    // Open log file for writing
    ofstream coutLog(ffname + log_f);
    
    // Display message indicating no errors
    coutLog << "NO ERRORS DETECTED." << endl << endl << endl;

    // Display warnings and count the total number of warnings
    if (haltPresent)
    {
        coutLog << warning.size() << " warnings detected." << endl;
    }
    else
    {
        coutLog << (warning.size() + 1) << " warnings detected." << endl;
        coutLog << "HALT instruction missing." << endl;
    }

    // Display details for each warning
    for (auto w : warning) 
    {
        coutLog << "Unused label \"" << w.label_unused << " : \" detected on line number " << w.line_number_warn << endl;
    }

    // Close the log file and display a confirmation message
    coutLog.close();
    cout << ffname << ".log Log file generated" << endl;

    // Open listing file for writing
    ofstream coutList(ffname + lst);
    
    // Write each line of the listing file with line address, machine code, and statement
    for (auto cur : list_file_inputs) 
    {
        coutList << cur.line_address << " " << cur.macCode << " " << cur.statement << endl;
    }

    // Close the listing file and display a confirmation message
    coutList.close();
    cout << ffname << ".lst Listing file generated" << endl;

    // Open machine code object file for writing in binary mode
    ofstream coutMCode;
    coutMCode.open(ffname + machinecode, ios::binary | ios::out);

    // Convert hexadecimal machine code to unsigned int and write to the object file
    for (auto &code : MachineCodes) 
    {
        unsigned int y;
        std::stringstream ss;
        ss << std::hex << code;
        ss >> y;
        static_cast<int>(y);
        coutMCode.write((const char*)&y, sizeof(unsigned));
    }

    // Close the object file and display a confirmation message
    coutMCode.close();
    cout << ffname << ".o Machine code object file generated" << endl;
}


string extractBeforeSemicolon(string& str) 
{
    // Find the position of the semicolon
    size_t semicolonPos = str.find(";");

    // If semicolon is not found, return the whole string
    if (semicolonPos == string::npos) 
    {
        return str;
    }

    // Return the substring before the semicolon
    return str.substr(0, semicolonPos);
}

void printErr()
{
    //print error to terminal
    sort(all(error), compare);
    cout<<"---------------------------------- ERRORS ----------------------------------"<<endl;
    for(auto it : error)
    {
        cout<<"Error type : "<<it.error_type<<" "<<"Error at Line Number : "<<it.line_number<<"."<<endl;
    }
}


int main()
{
    // Prompt user to input the assembly file
    ifstream infile;
    cout << "Enter file to assemble:" << endl;
    cin >> fName;
    infile.open(fName);

    // Check if input file exists
    if (infile.fail())
    {
        cout << "Input file does not exist!" << endl;
        exit(0);
    }

    // Initialize opcode table
    opct();
    
    // Extract filename without extension for use in log and lst file writing
    string fNameWithoutExtension;
    size_t pos1 = fName.find(".");
    if (pos1 != string::npos)
        fNameWithoutExtension = fName.substr(0, pos1);

    string line_read;
    int program_ctr = 0;
    int actual_line = 1;
    int progLine = 0;

    // Read lines from the input file and perform initial processing
    while (getline(infile, line_read))
    {
        string str_without_comm;

        // Extract text before semicolon (comments)
        str_without_comm = extractBeforeSemicolon(line_read);
        // Remove terminal spaces and check for empty lines
        string no_terminal_spaces = trim(str_without_comm);
        if (!no_terminal_spaces.empty())
        {
            without_comm.push_back(no_terminal_spaces);
            generateLineForError[progLine] = actual_line;
            progLine++;
        }

        actual_line++;
    }

    // Perform first pass, SET implementation, and second pass
    first_pass(without_comm, &program_ctr);
    SETimplementation();
    second_pass();

    // Check for errors and generate warnings
    if (error.empty())
    {
        for (auto it : labelPresence)
        {
            if (it.second)
            {
                if (labelUsage[it.first] == 0)
                {
                    warning.push_back({it.first, labels[it.first].lineNum});
                }
            }
        }

        // Generate log, listing, and object files
        write(fNameWithoutExtension);
    }
    else
    {
        // Print errors if any
        printErr();
    }

    return 0;
}


