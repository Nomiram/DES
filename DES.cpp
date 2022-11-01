#define FDEBUG 0
#if FDEBUG==1
#define DPRINTF(str, arg...) printf(str, arg)
#define DCOUT(str,bs) {bitset<64> out; for(int i=0; i<bs.size();i++){out[i]=bs[bs.size()-1-i];};cout<<str<<hex<< out.to_ullong()<<endl;}
#define RCOUT(str,bs) {bitset<64> out; for(int i=0; i<bs.size();i++){out[i]=bs[bs.size()-1-i];};cout<<str<< out<<endl;}
#endif
#if FDEBUG==0
#define DPRINTF(str, arg...) 
#define DCOUT(str,bs) 
#define RCOUT(str,bs) 
#endif
#include "DES.h"
#include <iostream>
#include <fstream>
#include <bitset>
#include <cstring>
#include <ctime>
using namespace std;
bitset<64> DES(bitset<64>,bitset<64>,bool);
bitset<32> F(bitset<32>, bitset<48>);
bitset<28> leftshift(bitset<28> k, int shift);
void generateKeys(bitset <64> key, bitset <48>* subkey);
int example();
int main(int argc, char* argv[])
{
    if(argc > 1 && string(argv[1]) == string("example"))
    {
      example();
      system("pause");
      return 0;
    }
    
    
    if(argc > 3 && ((string(argv[1]) == string("encrypt"))||(string(argv[1]) == string("decrypt"))))
    {
      bool flag_encrypt;
      if(string(argv[1]) == string("encrypt")){
        cout<<"Encryption....";  
        flag_encrypt = true;
      }
      else
      {
        cout<<"Decryption....";  
        flag_encrypt = false;
      }
    // bitset<64> temp;
    unsigned int start_time =  clock();
    unsigned long long int TEXT;
    unsigned long long int TESTKEY = 0xAABB09182736CCDD;
    bitset<64> key{TESTKEY} ;
    fstream file;
    fstream fileout;
    file.open(argv[2], ios::binary | ios::in);
    fileout.open(argv[3], ios::binary | ios::out);
    if(!file){
      cout<<"file "<< argv[2] <<" not found"<<endl;
      return 9090;
    }
      if(!fileout){
      cout<<"file "<< argv[3] <<" not found"<<endl;
      return 9090;
    }
    char plain[8];
    char tmp[8];
    while (!file.eof()){
    memset(plain, 0, sizeof(plain));
    memset(tmp, 0, sizeof(tmp));
    file.read((char*)plain, sizeof(plain));
    // cout<<"plain "<<endl;
    for(int i=0; i< sizeof(plain);i++){
      // cout << hex << (int)plain[i];
      tmp[sizeof(plain)-i-1] = plain[i];
    }
    // cout<<endl;
    memmove(&TEXT, tmp, sizeof(tmp));
    if(file.eof() && !flag_encrypt) break;
    // cout<<"text1: "<<hex<<TEXT<<endl;
    // cout<<"text2: "<<hex<<temp.to_ullong()<<endl;
    bitset<64> result = DES({TEXT}, key, flag_encrypt);
    // cout << "result: " << hex << result.to_ullong() << endl;
    TEXT = 0;
    TEXT = result.to_ullong();
    char tmp2[8];
    memset(tmp, 0, sizeof(tmp));
    memset(tmp2, 0, sizeof(tmp));
    memmove(tmp, &TEXT, sizeof(tmp));
    for(int i=0; i< sizeof(tmp);i++){
      tmp2[sizeof(tmp)-i-1] = tmp[i];
    }
    memmove(&TEXT, tmp2, sizeof(tmp2));
    fileout.write((char*)&TEXT, sizeof(TEXT));
    if (fileout.bad()){
    cout<<"fail "<< fileout.exceptions() <<endl;
    }
    }
    cout<<"done successfully"<<endl;
    unsigned int end_time = clock(); 
    double time = end_time - start_time;
    cout<<"time: "<<time/1000.0<<" s "<<endl;

    file.close();
    fileout.close();
    return 0;
    }

    cout << "Usage: " << endl;
    cout << "DES.exe example - show example" << endl;
    cout << "DES.exe encrypt fileinname fileoutname - encrypt file" << endl;
    cout << "DES.exe decrypt fileinname fileoutname - decrypt file" << endl;
    system("pause");
    return 1;
 

}
int example(){
    setlocale(LC_ALL,"russian");
    unsigned long long int PLAINTEXT   = 0x123456ABCD132536;
    unsigned long long int TESTKEY = 0xAABB09182736CCDD;
    printf("sizeof: %i\n", sizeof(PLAINTEXT)*8);
    printf("hex TEST: %I64x\n", PLAINTEXT);
    printf("hex KEY: %I64x\n", TESTKEY);
    printf("result: %I64x\n",DES(bitset<64>{PLAINTEXT}, bitset<64>{TESTKEY}, true));
    // system("pause");
    return 0;
}
  bitset<64> DES(bitset<64> text, bitset<64> key, bool encrypt = true){
  bitset<32> left;
  bitset<32> right;
  bitset <48> subkey [16];
  //reverse direction for key and plaintext
  //this is necessary because in DES numbering \
  starts from the beginning, and in bitset from the end
  {bitset<64> out; 
  for(int i=0; i<key.size();i++)
  {out[i]=key[key.size()-1-i];};
  key=out;
  }
  {bitset<64> out; 
  for(int i=0; i<text.size();i++)
  {out[i]=text[text.size()-1-i];};
  text=out;
  }
  bitset<64> start_tmp = text;
  DCOUT("key:  ", key);
  DCOUT("text: ", text);
  generateKeys(key,subkey);
  //Initial permutation
  for (int i = 0; i < 64; i++)
	  start_tmp[i] = text[IP[i] - 1];
  for (int i = 0; i < 32; i++)
      left[i] = start_tmp[i];
  for (int i = 0; i < 32; i++)
      right[i] = start_tmp[i+32];
  DPRINTF("Initial permutation\n","");
    DCOUT("!left:  ",left);
    DCOUT("!right: ",right);
  DPRINTF("16 rounds\n","");
  // 16 rounds 
	for (int round = 0; round < 16; round++)
	{
		bitset<32> tmp = right;
    if(encrypt){
		  right = left ^ F(right, subkey[round]);
    }
    else{
		  right = left ^ F(right, subkey[15-round]);
    }
		left = tmp;
    DCOUT("!left:  ",left);
    DCOUT("!right: ",right);
    DCOUT("!skey:  ",subkey[round]);
  }
  
	// Merge
  bitset<64> result;
	for (int i = 0; i < 32; i++)
		result[i] = right[i];
	for (int i = 32; i < 64; i++)
		result[i] = left[i - 32];
	bitset<64> tmp = result;
	for (int i = 0; i < 64; i++)
		result[i] = tmp[IP_1[i] - 1];
	// Revers reverse direction for result
  //
  {bitset<64> out; 
  for(int i=0; i<result.size();i++)
  {out[i]=result[result.size()-1-i];};
  result=out;
  }
  return result;

}
void generateKeys(bitset <64> key, bitset <48>* subkey) {
	bitset<56> real_key;
	bitset<28> left;
	bitset<28> right;
	bitset<48> compressKey;
 
	
//  cout <<"key: "<<key<<endl;
  for (int i = 0; i < 56; i++){
		real_key[i] = key[PC_1[i] - 1];
		// real_key[(real_key.size()-1) - (i)] = key[(key.size()-1) - (PC_1[i] - 1)];

  }
  DCOUT("rk: ",real_key);
	for (int round = 0; round < 16; round++)
	{
		for (int i = 0; i < 28; i++){
			left[i] = real_key[i];
			right[i] = real_key[i+28];
    }
		 // Left shift
    if(round == 0 || round == 1 || round == 8 || round == 15){
		  left  = leftshift(left,  1);
		  right = leftshift(right, 1);
    }
    else{
      left  = leftshift(left,  2);
		  right = leftshift(right, 2);
    }
		 // 
		for (int i=0; i < 28; i++)
			real_key[i] = left[i];
		for (int i = 28; i < 56; i++)
			real_key[i] = right[i - 28];
		for (int i = 0; i < 48; i++)
		{
			int m = PC_2[i];
			compressKey[i] = real_key[m - 1];
		}                                   
		subkey[round] = compressKey;
	}
 
}

bitset<32> F(bitset<32> R, bitset<48> k){
	// Expand block
	bitset<48> expandR;  
	for (int i = 0; i < 48; i++)
		expandR[i] = R[E[i]-1];  
  
  RCOUT("R  ",R);
  RCOUT("eR ",expandR);
  DCOUT("right_expanded ", expandR);
	 // XOR
	expandR = expandR ^ k;
	 // 
	bitset<32> output;
	int x = 0;
	for (int i = 0; i < 48; i = i + 6)
	{
		int row = expandR[i] * 2 + expandR[i + 5];
		int col = expandR[i + 1] * 8 + expandR[i + 2] * 4 + expandR[i + 3] * 2 + expandR[i + 4];
		int num = S_BOX[i / 6][row][col];
		bitset<4> temp(num);
		output[x + 3] = temp[0];
		output[x + 2] = temp[1];
		output[x + 1] = temp[2];
		output[x] = temp[3];
		x += 4;
	}
	 // P box
	bitset<32> tmp = output;
	for (int i = 0; i < 32; i++)
		output[i] = tmp[P[i] - 1];
 
	return output;
}
bitset<28> leftshift(bitset<28> k, int shift) {
   ///Left shift becomes right shift
   /// because the key is inverted
    for(int i=0; i<shift; i++) 
    {
        bool temp = k[0];
        k >>= 1; // - right shift
        k[k.size()-1] = temp;
    }
    return k;
}