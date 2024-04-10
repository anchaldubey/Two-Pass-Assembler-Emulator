/*--------------------------------------------------------------------------------*/
// Name: Anchal Dubey
// Roll No. 2201CS15
// CS-210 Mini Project
/*--------------------------------------------------------------------------------*/

#include<bits/stdc++.h>
#include<fstream>
using namespace std;
#define ll long long int
#define fir first
#define sec second
#define vint vector<int>
#define se set<int>
#define pa_in_in pair<int,int>
#define pa_in_st pair<int,string>
#define bre break;

/*-------------------------------------------------------------------------------*/

// Converts decimal to hexadecimal
string dec_to_hex(unsigned int n){
    stringstream s;
    s<<hex<<n;
    string t=s.str();
    reverse(t.begin(),t.end());
    while(t.size()<8) t+='0';
    reverse(t.begin(),t.end());

    return t; 
}

/*------------------------------------------------------------------------------*/

vector<string>cod;
int mem[1<<24]; // Main memory
int A,B,pc,sp,indx,cont,exec; // sp is stack pointer || pc is program counter || A and B are registers
array<int,2>chang;

/*-----------------------------------------------------------------------------*/

vector<string>mnem;
void push_in_mnem(vector<string>&mnem){
    mnem.push_back("ldc");
    mnem.push_back("adc");
    mnem.push_back("ldl");
    mnem.push_back("stl");
    mnem.push_back("ldnl");
    mnem.push_back("stnl");
    mnem.push_back("add");
    mnem.push_back("sub");
    mnem.push_back("shl");
    mnem.push_back("shr");
    mnem.push_back("adj");
    mnem.push_back("a2sp");
    mnem.push_back("sp2a");
    mnem.push_back("call");
    mnem.push_back("return");
    mnem.push_back("brz");
    mnem.push_back("brlz");
    mnem.push_back("br");
    mnem.push_back("HALT");
}

/*--------------------------------------------------------------------------------*/

// Declaring all the functions to be used in the code
void ldc(int n);
void adc(int n);
void adj(int n);
void ldl(int off_set);
void stl(int off_set);
void ldnl(int off_set);
void stnl(int off_set);
void call(int off_set);
void ret_urn(int off_set);
void brz(int off_set);
void brlz(int off_set);
void br(int off_set);
void sub(int off);
void add(int off);
void shl(int off);
void shr(int off);
void a2sp(int off);
void sp2a(int off);

using func_pointer=void(*)(int);
func_pointer call_func[]={ldc,adc,ldl,stl,ldnl,stnl,add,sub,shl,shr,adj,a2sp,sp2a,call,ret_urn,brz,brlz,br};

void mac_cod_in();
void mem_display();
void reg_display();
void print_read();
void print_write();
void in_the_set();
int run_the_code(int ope,int count=(1<<25));
bool run_emu();

/*---------------------------------------------------------------------------------*/

// Defining the int main function
int main(){
    // ios_base::sync_with_stdio(false);
    // cin.tie(NULL);
    // cout.tie(NULL);


    push_in_mnem(mnem);
    mac_cod_in();
    
    // Messages to be printed intially for information
    cout<<"--------------------------------------"<<endl;
    cout<<"Namaste and Welcome!!"<<endl;
    cout<<"--------------------------------------"<<endl;
    cout<<"Functions that can be implemented are:"<<endl;
    cout<<"**************************************"<<endl;
    cout<<"(1) -dump : For memory dump"<<endl;
    cout<<"(2) -t : Emulating code one line at a time"<<endl;
    cout<<"(3) -run : To emulate till the end"<<endl;
    cout<<"(4) -reg : To show registers and SP values"<<endl;
    cout<<"(5) -isa : To display instruction set"<<endl;
    cout<<"(6) -read : For read operations"<<endl;
    cout<<"(7) -write : For write operations"<<endl;

    while(true){
       run_emu();
    }
    return 0;
}

/*-------------------------------------------------------------------------------*/

// Function for emulator run 
bool run_emu(){
    cout<<"Enter the command(0 to exit): ";
    string t;
    cin>>t;

    if(t=="0") exit(0);
    else if(t=="-dump") mem_display();
    else if(t=="-reg") reg_display();
    else if(t=="-t"){
        int flag=run_the_code(0,1);
        return flag;
    }
    else if(t=="-run"){
        int flag=run_the_code(0);
        return flag;
    }
    else if(t=="-isa") in_the_set();
    else if(t=="-read"){
        int flag=run_the_code(1);
        return flag;
    }
    else if(t=="-write"){
        int flag=run_the_code(2);
        return flag;
    }
    else cout<<"ERROR!! Please enter valid instruction"<<endl;
    return true;
}

/*-------------------------------------------------------------------------------*/

// Function for input
void mac_cod_in(){
    string in_str;
    cout<<"Enter the machine code file name with extension: ";
    cin>>in_str;

    ifstream fil(in_str,ios::in | ios::binary);
    unsigned int rd;
    int sr=0;
    while(fil.read((char*)&rd,sizeof(int))){
        mem[sr++]=rd;
        cod.push_back(dec_to_hex(rd));
    }
}

/*-------------------------------------------------------------------------------*/

// Printing the memory
void mem_display(){
    for(int k=0;k<cod.size();k++){
        cout<<dec_to_hex(k);
        cout<<" ";
        for(int i=k;i<min(k+4,(int)cod.size());i++){
            cout<<dec_to_hex(mem[i]);
            cout<<" ";
        }
        k=k+3;
        cout<<endl;
    }
}

/*-----------------------------------------------------------------------------*/

// For performing the operation
int run_the_code(int ope,int count){
    int it=0;

    while(count-- && pc<cod.size()){
        cont++;
        //cout<<count<<endl;
        if(pc>=cod.size() || cont>(int)3e7){
            cout<<"ERROR!! Segmentation fault"<<endl;
            return 0;
        }
        string current=cod[pc];
        int __op=stoi(current.substr(6,2),0,16);

        if(__op==18){
            cout<<"HALT is found"<<endl;
            cout<<cont;
            cout<<" number of statements were executed";
            cout<<endl;
            return 1;
        }
        int m=stoi(current.substr(0,6),0,16);
        if(m >= (1 << 23)) m-=(1 << 24);
        if(count==0) reg_display();
        (call_func[__op])(m);
        if(ope==1){
            if(exec==1){
                print_read();
                exec=0;
            }
        }
        else if(ope==2 && exec==2){
            if(exec==2){
                print_write();
                exec=0;
            }
        }
        pc++;
        indx++;
    }
    return 1;
}

/*--------------------------------------------------------------------------*/

void in_the_set(){
    cout<<"Machine code operand mnemonic"<<endl;
    cout<<"0     ldc     value"<<endl;
    cout<<"1     adc     value"<<endl;
    cout<<"2     ldl     value"<<endl;
    cout<<"3     stl     value"<<endl;
    cout<<"4     ldnl    value"<<endl;
    cout<<"5     stnl    value"<<endl;
    cout<<"6     add          "<<endl;
    cout<<"7     sub          "<<endl;
    cout<<"9     shr          "<<endl;
    cout<<"10    adj     value"<<endl;
    cout<<"11    a2sp         "<<endl;
    cout<<"12    sp2a         "<<endl;
    cout<<"13    call   offset"<<endl;
    cout<<"14    return       "<<endl;
    cout<<"15    brz    offset"<<endl;
    cout<<"16    brlz   offset"<<endl;
    cout<<"17    br     offset"<<endl;
    cout<<"18    HALT         "<<endl;
}

/*--------------------------------------------------------------------------*/

void print_read(){
    cout<<"Reading mem[" <<dec_to_hex(pc) << "], has value: ";
    cout<<dec_to_hex(chang[0])<<endl;
}

/*--------------------------------------------------------------------------*/

void print_write(){
    cout<<"Writing mem[" <<dec_to_hex(pc) << "], from ";
    cout<<dec_to_hex(chang[0])<<" to ";
    cout<<dec_to_hex(chang[1])<<endl;
}

/*--------------------------------------------------------------------------*/

void reg_display(){
    cout<<"A : ";
    cout<<dec_to_hex(A)<<"     ";
    cout<<"B : ";
    cout<<dec_to_hex(B)<<"     ";
    cout<<"SP : ";
    cout<<dec_to_hex(sp)<<"    ";
    cout<<"PC : ";
    cout<<dec_to_hex(pc+1)<<"    ";
    cout<<mnem[pc]<<endl;
}

/*---------------------------------------------------------------------------*/

void ldc(int n){
    B=A;
    A=n;
}

void adc(int n){
    A=A+n;
}

void adj(int n){
    sp=sp+n;
}

void ldl(int off_set){
    B=A;
    A=mem[sp+off_set];
    chang={sp+off_set,0};
    exec=1;
}

void stl(int off_set){
    chang={mem[sp+off_set],A};
    mem[sp+off_set]=A;
    exec=2;
    A=B;
}

void ldnl(int off_set){
    A=mem[A+off_set];
    chang={sp+off_set,0};
    exec=1;
}

void stnl(int off_set){
    chang={mem[A+off_set],B};
    mem[A+off_set]=B;
    exec=2;
}

void call(int off_set){
    B=A;
    A=pc;
    pc=pc+off_set;
}

void ret_urn(int off_set){
    pc=A;
    A=B;
}

void brz(int off_set){
    if(A==0) pc=pc+off_set;
}

void brlz(int off_set){
    if(A<0) pc=pc+off_set;
}

void br(int off_set){
    pc=pc+off_set;
}

void add(int off){
    A=A+B;
}

void sub(int off){
    A=B-A;
}

void shl(int off){
    A=B<<A;
}

void shr(int off){
    A=B>>A;
}

void a2sp(int off){
    sp=A;
    A=B;
}

void sp2a(int off){
    B=A;
    A=sp;
}

