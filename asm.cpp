/*------------------------------------------------------------*/
// Name: Anchal Dubey
// Roll No. 2201CS15
// CS-210 Mini Project 
/*-----------------------------------------------------------*/

#include<bits/stdc++.h>
#include<fstream>
using namespace std;
#define ll long long int
#define unint unsigned int
#define fir first
#define sec second
#define se set<int>
#define vint vector<int>
#define pa_in_in pair<int,int>
#define pa_in_st pair<int,string>
#define br break;

/*------------------------------------------------------------------------------*/

// For storing the code line
typedef struct code_line{
    string lab_el; // Serves as a named reference point
    string mnemonic;// Reserved name for a class of instructions that have same purpose(serves as an abbreviation)
    string operand; 
    int operand_type;// Like if it is hex or oct
    bool if_label;// Checks if the label is present in that line or not
}code_line;

/*-------------------------------------------------------------------------------*/

// Decimal to hexadecimal conversion
string dec_to_hex(int num,int add=24){
    // default value of add is set to 24.
    // if add is 32 it is treated as unsigned int
    // str() sets the string object whose content is present in the stream
    stringstream s;
    if(add==32){
        unint n=num;
        s<<hex<<num;
    }
    else{
        if(num<0) num+=(1<<add); // Converts to its equivalent positive value
        s<<hex<<num; // Converts num to hexadecimal string using stringstream
    }
    return s.str();
}

/*-------------------------------------------------------------------------------------*/

// Declaring all the vectors and maps
vector<code_line> info;
vector<string>clean_line; // Code line without spaces
map<string,pair<string,int>> op_code; // Used for storing mnemonic and opcode
vector<pa_in_st> err_store; // For storing the errors
map<string,int> label; // For storing labels with their declaration
set<string>used_label;
vector<pa_in_st> mac_Code; //For storing machine code
vint pc; // For program counter for each line
bool if_HALT=false; // Checking for halt

string fl_name;

/*---------------------------------------------------------------------------------------*/

// Verifies if the name is valid variable or not
//Eg : var12 is valid but 1var is not valid
int valid_name(string s){
    for(auto it:s){
        if((it>='A' && it<='Z') || (it>='a' && it<='z') || it=='_' || (it>='0' && it<='9')) continue;
        return 0;
    }
    if((s[0]>='A' && s[0]<='Z') || (s[0]>='a' && s[0]<='z') || s[0]=='_') return 1;

    return 0;
}

/*----------------------------------------------------------------------------------------*/

// Checks if the string is in decimal or not
int isDec(string num){
    int flag=1;
    for(int k=0;k<num.size();k++) flag &=(num[k]>='0' and num[k]<='9'); // Checks if it is in the range of decimal else flag becomes 0

    return flag;
}

/*------------------------------------------------------------------------------------------*/

// Checks if the string is in hexadecimal or not
int isHex(string t){
    int flag=1;
    if(t.size()<3) return 0;

    flag &=(t[0]=='0') and ((t[1]=='x' || t[1]=='X'));

    for(int k=2;k<t.size();k++){
        int s=(t[k]>='0' && t[k]<='9');
        s |= ((t[k]>='A' && t[k]<='F') || ((t[k]>='a' && t[k]<='f')));
        flag &= s;
    }

    return flag;
}

/*---------------------------------------------------------------------------------------------*/

// Checks if the string is in octal or not
int isOct(string t){
    int flag=1;
    if(t.size()<2) return 0;

    for(int k=1;k<t.size();k++) flag &= (t[k]>='0' && t[k]<='7');

    flag &= (t[0]=='0');

    return flag;
}

/*----------------------------------------------------------------------------------------------*/

// Defining the opcodes and mnemonics
// Type notations:
// 0 : No operands considered. Eg: add,sub
// 1 : One operand and 1 value. Eg: adc,ldc
// 2 : One operand and an offset. Eg: ldnl,stnl
void Optable(){
    op_code["ldc"]={"00",1},
    op_code["ldl"]={"02",2},
    op_code["adc"]={"01",1},
    op_code["stl"]={"03",2},
    op_code["ldnl"]={"04",2},
    op_code["stnl"]={"05",2},
    op_code["add"]={"06",0},
    op_code["sub"]={"07",0},
    op_code["shl"]={"08",0},
    op_code["shr"]={"09",0},
    op_code["adj"]={"0A",1},
    op_code["a2sp"]={"0B",0},
    op_code["sp2a"]={"0C",0},
    op_code["call"]={"0D",2},
    op_code["return"]={"0E",0},
    op_code["brz"]={"0F",2},
    op_code["brlz"]={"10",2},
    op_code["br"]={"11",2},
    op_code["HALT"]={"12",0},
    op_code["data"]={"",1},
    op_code["SET"]={"",1};
}

/*---------------------------------------------------------------------------------------------------*/

// Declaring all the functions that are to be used in the code
void insert_error(int line_no,string type);
string str_wo_space(string str,int line_no);
void push_set_inst(vector<string> &v,string token,string t,int k);
void implement_set();
void processing_label();
void complete_data(int k,string s,string t,string u,int type);
int type_calculate(string t);
void form_the_table();
void create_data_segment();
void First_Pass();
int spot_err();
string append_zero(string t,int z = 6);
void Second_Pass();

/*--------------------------------------------------------------------------------------------------------*/

// Defining the int main function
int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);


    First_Pass(); // Carries out the first pass of the assembler

    if(spot_err()){
        Second_Pass(); // Carries out the second pass and writes to the files only if no errors are found
        // Writing to the file:
        ofstream list_output("ListCode.l");
        for(auto it:mac_Code){
            list_output<<append_zero(dec_to_hex(pc[it.fir]));
            list_output<<" "<<it.sec;
            list_output<<" ";
            list_output<<clean_line[it.fir]<<endl;
        }
        list_output.close();

        ofstream mac_cod_out;
        mac_cod_out.open("MachineCode.o",ios::binary | ios::out);

        for(auto it:mac_Code){
            unint y;
            if(it.sec.empty() || it.sec=="         ") continue;

            stringstream s;
            s<<hex<<it.sec;
            s>>y;

            static_cast<int>(y);
            mac_cod_out.write((const char*)&y,sizeof(unint));
        }
      mac_cod_out.close();
      cout<<"Logfile is created"<<endl;
      cout<<"Machine code is created"<<endl;
      cout<<"Listing code is created"<<endl;
    }
    system("pause");
    return 0;
}



/*--------Defining all the functions--------*/



// Inserting the errors
void insert_error(int line_no,string type){
    string s=to_string(line_no);
    string t="ERROR!! At line: "+s+"    Type: "+type;
    err_store.push_back({line_no+1,t});
}

/*------------------------------------------------------------------------------------*/

// This returns the string without space i.e. removes unnecessary space
string str_wo_space(string str,int line_no){
    for(int k=0;k<2;k++){
        reverse(str.begin(),str.end());
        while(str.back()==' ' || str.back()== '\t') str.pop_back();
    }
    string store;
    for(int k=0;k<str.size();k++){
        char ch=str[k];
        if(ch==';') br // Comments begin with ; so anything after that is ignored
        if(ch==':'){   // Label names are followed by : 
            store+=':';
            if(k==str.size()-1 || str[k+1]!=' ') store+=" ";
            continue;
        }
        if(ch!=' ' && ch!='\t'){  // White spaces and tabs are ignored
            store+=ch;
            continue;
        }
        store+=" ";
        int z=k;
        while(z<str.size() && ch==str[z]) z++;
        k=z-1;
    }

    while(!store.empty() && (store.back()==' ' || store.back()=='\t')) store.pop_back();

    int pad=0;
    for(auto it:store){
      if(it==' ') pad++;
    }
    
    if(pad>2) insert_error(line_no+1,"ERROR!! Wrong syntax");

    return store;
}

/*--------------------------------------------------------------------------------------------*/

// Implementing SET mnemonic in assembler
void push_set_inst(vector<string> &v,string token,string t,int k){
    int siz=t.size();
    if(siz<=k+5) return ;   // If there are not enough characters following SET to form a valid instruction the function returns
    v.push_back("adj 10000");   // SP at random memory
    v.push_back("stl -1");      // To load A
    v.push_back("stl 0");       // To load B
    string s="ldc "+t.substr(k+6,t.size()-(k+6));   // Immediate value following the set derivative and load it on stack
    v.push_back(s);
    string a="ldc "+token.substr(0,k);   // To load label's address on the stack
    v.push_back(a);
    v.push_back("stnl 0");        // Load A and B
    v.push_back("ldl 0");
    v.push_back("ldl -1");
    v.push_back("adj -10000");    // Adjusting the stack pointer back
}

/*---------------------------------------------------------------------------------------------*/

// Implements the SET mnemonic given
void implement_set(){
    vector<string>v;    // To temporarily store the updated lines
    int siz=clean_line.size();
    for(int k=0;k<siz;k++){
       string current;
       int flag=0;
       int siz2=clean_line[k].size();
       for(int i=0;i<siz2;i++){
        current+=clean_line[k][i];
        if(clean_line[k][i]==':'){   // Marks the end of the label
            current.pop_back();
            if(siz2>i+5 && clean_line[k].substr(i+2,3)=="SET"){
                flag=1;
                if(abs(label[current])==i){  // If label position matches current position i it means that it is a valid SET instruction 
                    int b=v.size()-1;
                    label[current]=b;        // Updates the label map
                    string e=clean_line[k].substr(0,i+1)+" data ";
                    e+=clean_line[k].substr(i+6,siz2-(i+6));
                    v.push_back(e);
                }
                else push_set_inst(v,current,clean_line[k],i);  // If not a valid SET instruction calls the push_set_instruction function
                br
            }
        }
       } 
       if(!flag && !clean_line[k].empty()) v.push_back(clean_line[k]);
    }
    clean_line=v;
}

/*-----------------------------------------------------------------------------------------------------*/

// Handles labels by creating and storing a map that associates each label with its declared position
void processing_label(){
    for(int k=0;k<clean_line.size();k++){
        string current;       //  Stores the current label being processed
        int m=clean_line[k].size();
        string lm=clean_line[k];
        for(int i=0;i<m;i++){
            if(clean_line[k][i]==':'){     // Marks the end of a label name so checks current for label
                int check=valid_name(current);
                if(check==0){
                    insert_error(k+1,"ERROR!! Invalid name of the label");
                    br
                }
                if(label.count(current)!=0){
                    if(m>i+4 && lm.substr(i+2,3)=="SET") continue;
                    if(m>i+5 && lm.substr(i+2,4)=="data" && label[current]<0){  // Redeclares label only if it was earlier set with a negative index
                        label[current]=k;
                        continue;
                    }
                    string ad="There are multiple declaration of label : "+current;  // If label is being redeclared multiple times without redeclaration it reports an error
                    insert_error(k+1,ad);
                }
                if(m>i+4 && lm.substr(i+2,3)=="SET"){
                    label[current]=-i;
                    continue;
                }
                label[current]=i;   // Updates the label map
                br
            }
            current+=clean_line[k][i];
        }
    }
}

/*-------------------------------------------------------------------------------*/

// Completes the info vector to make the code clean
void complete_data(int k,string s,string t,string u,int type){
    info[k].lab_el=s;
    info[k].mnemonic=t;
    info[k].operand=u;
    info[k].operand_type=type;
}

/*-------------------------------------------------------------------------------*/

// Returns the type of operand. Eg: label/hex/decimal
int type_calculate(string t){
    if(t.empty()) return 0;
    if(t[0]=='+' || t[0]=='-'){
        reverse(t.begin(),t.end());
        t.pop_back();
        reverse(t.begin(),t.end());
    }
    if(t.empty()) return -1;
    else if(isDec(t)==1) return 10;
    else if(isOct(t)==1) return 8;
    else if(isHex(t)==1) return 16;
    else if(valid_name(t)==1) return 1;
    return -1;
}

/*-------------------------------------------------------------------------------*/

// Process the data in tabular manner as follows:
// Label   Mnemonic   Operand   Operand type(Hex,Oct)
void form_the_table(){
    int program_counter=0;
    for(int k=0;k<clean_line.size();k++){
        string res[10]={"","","",""};
        string current="";
        int indx=1;
        for(int i=0;i<clean_line[k].size();i++){
            if(clean_line[k][i]==':'){
                res[0]=current;
                current="";
                i++;
                continue;
            }
            else if(clean_line[k][i]==' '){
                res[indx++]=current;
                current="";
                continue;
            }
            current+=clean_line[k][i];
            if(i==clean_line[k].size()-1) res[indx++]=current;
        }
        if(!res[1].empty()) info[k].if_label=true;   // To check if the current line contains the label
        else info[k].if_label=false;

        if(res[1]=="HALT") if_HALT=true;      // If the mnemonic of the instruction is HALT
        if(!res[0].empty()) label[res[0]]=program_counter;   // If the label is present updates it with corresponding program counter value
        pc[k]=program_counter;

        if(indx==1){
            complete_data(k,res[0],"","",0);  // If indx is 1 implies there is only label present // Also operand type is 0 indicating no operand
            continue;
        }
        program_counter++;

        if(!op_code.count(res[1])){        // Checks if the mnemonic extracted exists in the Optable
            insert_error(k+1,"ERROR!! The mnemonic is inavalid");
            //cout<<res[0]<<" "<<res[1]<<endl;
            continue;
        }
        //cout<<res[0]<<" "<<res[1]<<endl;
        int mini=min(op_code[res[1]].sec,1);   // Determines minimum number of operands required for the mnemonic
        if(mini!=indx-2){                // -2 is done for label and mnemonic. If they don't match then error is inserted
            insert_error(k+1,"ERROR!! Invalid Opcode and Syntax combination");
            continue;
        }
        complete_data(k,res[0],res[1],res[2],type_calculate(res[2]));   // Completing the data in the info table
        // If the operand type is 1 it indicates it is a label and hence then checks for its validity in label map
        if(info[k].operand_type==1 && !label.count(info[k].operand)) insert_error(k+1,"Label/data doesn't exist"); 
        // If the operand type is -1 it indicates invalid number
        else if(info[k].operand_type==-1) insert_error(k+1,"ERROR!! The number is invalid");
        if(info[k].operand_type==1) used_label.insert(info[k].operand);
    }
}

/*-----------------------------------------------------------------------------------------*/

// Separates code in data segment for execution of instructions
void create_data_segment(){
    vector<string>instructions; // Stores non-data segment instructions
    vector<string>data_seg;    // Stores data segment instruction

    for(int k=0;k<clean_line.size();k++){
        int flag=0;
        int siz=clean_line[k].size();
        int siz2=clean_line[k].size();
        for(int i=0;i<siz2;i++){
            if(clean_line[k].substr(i,4)=="data" && i+4<siz2){
                data_seg.push_back(clean_line[k]);
                flag=1;
                br
            }
            if(clean_line[k].back()==':' && k+1<siz &&clean_line[k+1].substr(0,4)=="data"){
                data_seg.push_back(clean_line[k]);
                flag=1;
                br
            }
        }
        if(flag==0) instructions.push_back(clean_line[k]);
    }
    instructions.insert(instructions.end(),data_seg.begin(),data_seg.end());
    clean_line=instructions;  // Removes the data segment line and replaces clean_line with instructions
}

/*--------------------------------------------------------------------------------------------------*/

// First pass of assembler
void First_Pass(){
    Optable();
    ifstream file_input;
    cout<<"Enter the file to be assembled: "<<endl;
    cin>>fl_name;

    file_input.open(fl_name);

    if(file_input.fail()){
        cout<<"The file entered doesn't exist"<<endl;
        return ;
    }
    string str;
    while(getline(file_input,str)){
        string current=str_wo_space(str,clean_line.size()); // Removes the unnecessary space from the code
        clean_line.push_back(current);
    }
    processing_label(); // Handles label related task
    if(err_store.empty()) implement_set(); // If there are no errors then implement set function is called
    info.resize(clean_line.size()); // info and pc are resized on the basis of number of lines in the code
    pc.resize(clean_line.size());
    create_data_segment();     // The create data segment function is called
    form_the_table();         // The data is processed in tabular format
}

/*-----------------------------------------------------------------------------------------------------*/

// Adds the errors and messages in the log file
int spot_err(){
    ofstream Errors("Log_File.log");
    Errors<<"Log_File.log is created"<<endl;

    int cnt=0;

    if(err_store.empty()){     // If no errors are found only then the list_code and machine_code are generated
        cout<<"No Errors!!!";
        cout<<endl;
        if(label.size()!=used_label.size()){
            for(auto it:label){
                if(used_label.count(it.fir)==0) cnt++;
            }
        }
        if(!if_HALT) cnt++;
        cout<<cnt<<" number of warnings are detected"<<endl;
        Errors<<cnt<<" number of warnings are detected"<<endl;
        if(!if_HALT){
            Errors<<"Warning!! Halt is not found"<<endl;
        }
        if(label.size()!=used_label.size()){
            for(auto it:label){
                if(used_label.count(it.first)==0) Errors<<it.fir<<" is an unused label"<<endl;
            }
        }
        Errors<<"MachineCode.o is created"<<endl;
        Errors<<"ListCode.l is created"<<endl;
        Errors.close();
        return 1;
    }
    sort(err_store.begin(),err_store.end());       // All the errors are sorted and is iterated over and written to the log_file
    cout<<err_store.size()<<" "<<"Error found!!"<<endl;

    for(auto it: err_store) Errors<<it.sec<<endl;
    Errors.close();
    return 0;
}

/*-------------------------------------------------------------------------------------------------------*/

// Function to append 0 to make it of 24 bit
// eg: F to 00000F
// Since each character if hexadecimal 6 character becomes 24 bit
string append_zero(string t,int z){
    // Reverse the string
    reverse(t.begin(),t.end());

    // Appending 0's until the string reaches the desired size
    while(t.size()<z) t+='0';

    // Reversing back to the original string
    reverse(t.begin(),t.end());

    return t;
}

/*-------------------------------------------------------------------------------------------------------*/

// Second pass of the assembler
// Converts code to machine code
// Also generates listing code
void Second_Pass(){
    for(int k=0;k<info.size();k++){   // Iterates through each line of code in info
        if(clean_line[k].empty()) continue;
        string new_s=append_zero(dec_to_hex(pc[k]));
        string current_cod;
        int dec;

        if(info[k].mnemonic==""){
            current_cod="         ";
            mac_Code.push_back({k,current_cod});
            continue;
        }
        if(info[k].operand_type==1){
            if(op_code[info[k].mnemonic].sec==2){  // Checks if second pass is required
                int value;
                value=label[info[k].operand];
                dec=value-(pc[k]+1);  // Calculates the displacement
            }
            else dec=label[info[k].operand];
            string temp=append_zero(dec_to_hex(dec));
            current_cod=temp+op_code[info[k].mnemonic].fir;
            mac_Code.push_back({k,current_cod});
        }
        else if(info[k].operand_type==0){   // If operand type is 0 implies no operand is required
            current_cod="000000";
            current_cod+=op_code[info[k].mnemonic].fir;
            mac_Code.push_back({k,current_cod});
        }
        else{
            int z=6;
            int add=24;
            if(info[k].mnemonic=="data") z=8,add=32;
            dec=stoi(info[k].operand,0,info[k].operand_type);
            current_cod=append_zero(dec_to_hex(dec,add),z);
            current_cod+=op_code[info[k].mnemonic].fir;
            mac_Code.push_back({k,current_cod});      // Stores the index k along with the generated machine code(current_cod)
        }
    }
}

/*---------------------------------------------------------------------END------------------------------------------------------------------------------*/