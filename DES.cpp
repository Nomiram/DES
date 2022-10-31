#define FDEBUG 1
#if FDEBUG
#define DPRINTF(str, arg...) printf(str, arg)
#define DCOUT(str,bs) {bitset<64> out; for(int i=0; i<bs.size();i++){out[i]=bs[bs.size()-1-i];};cout<<str<<hex<< out.to_ullong()<<endl;}
#define RCOUT(str,bs) {bitset<64> out; for(int i=0; i<bs.size();i++){out[i]=bs[bs.size()-1-i];};cout<<str<< out<<endl;}
#endif
#if FDEBUG==0
#define DPRINTF(str, arg...) 
#endif
#include "DES.h"
#include <iostream>
#include <bitset>
using namespace std;
bitset<64> DES_encrypt(bitset<64>,bitset<64>);
bitset<32> F(bitset<32>, bitset<48>);
bitset<28> leftshift(bitset<28> k, int shift);
void generateKeys(bitset <64> key, bitset <48>* subkey);
int main(int argc, char* argv[])
{
    /*
       if (argc > 1)
       {
             cout << argv[1]<< endl;
       } else
               {
                 cout << "Not arguments" << endl;
               }
    */
    setlocale(LC_ALL,"russian");
    unsigned long long int TEST1   = 0x123456ABCD132536;
    unsigned long long int TESTKEY = 0xAABB09182736CCDD;
    printf("sizeof: %i\n", sizeof(TEST1)*8);
    printf("hex TEST: %I64x\n", TEST1);
    printf("hex KEY: %I64x\n", TESTKEY);
    printf("hex: %I64x\n",DES_encrypt(bitset<64>{TEST1}, bitset<64>{TESTKEY}));
    // system("pause");
    return 0;
}
bitset<64> DES_encrypt(bitset<64> text, bitset<64> key){
  bitset<32> left;
  bitset<32> tmp;
  bitset<32> right;
  bitset <48> subkey [16];
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
  // cout <<"text "<< text << endl;
  // cout <<"key  "<< key << endl;
  generateKeys(key,subkey);
  //Начальная перестановка
  for (int i = 0; i < 16; i++)
    {
      // DPRINTF("%x\n", subkey[i]);
    }
  for (int i = 0; i < 64; i++)
	  start_tmp[i] = text[IP[i] - 1];
  for (int i = 0; i < 32; i++)
      left[i] = start_tmp[i];
  for (int i = 0; i < 32; i++)
      right[i] = start_tmp[i+32];
  // DPRINTF("%lx | %lx\n", left, right);
  printf("initial permutation\n");
  // printf("%lx | %lx\n", left, right);
    DCOUT("!left:  ",left);
    DCOUT("!right: ",right);
  printf("16 rounds\n");
  // 16 раундов 
	for (int round = 0; round < 16; round++)
	{
		tmp = right;
		right = left ^ F(right, subkey[round]);
		left = tmp;
    DCOUT("!left:  ",left);
    DCOUT("!right: ",right);
    DCOUT("!skey:  ",subkey[round]);
    // printf("%lx | %lx | %I64x\n", left, right, subkey[round]);
  }
    return start_tmp;

}
void generateKeys(bitset <64> key, bitset <48>* subkey) {
	bitset<56> real_key;
	bitset<28> left;
	bitset<28> right;
	bitset<48> compressKey;
 
	
 cout <<"key: "<<key<<endl;
  for (int i = 0; i < 56; i++){
		real_key[i] = key[PC_1[i] - 1];
		// real_key[(real_key.size()-1) - (i)] = key[(key.size()-1) - (PC_1[i] - 1)];

  }
  DCOUT("rk: ",real_key);
//   cout <<"perm key: ";
//     for (int i = 0; i < 56; i++){
// 		cout << key[(key.size()-1) - (PC_1[i] - 1)];

//   }
//   cout << endl;
//  cout <<"real key: "<<hex<< real_key.to_ullong()<<endl;
//  cout <<"real key: "<<real_key<<endl;
//  cout <<"real key: "<<real_key.count()<<endl;
	for (int round = 0; round < 16; round++)
	{
		for (int i = 0; i < 28; i++){
			left[i] = real_key[i];
			right[i] = real_key[i+28];
    }
    // cout <<"keys: "<<hex<< left.to_ullong() <<" "<< left.to_ullong()<< endl; 
		 // Сдвиг влево
    if(round == 0 || round == 1 || round == 8 || round == 15){
		  left  = leftshift(left,  1);
		  right = leftshift(right, 1);
    }
    else{
      left  = leftshift(left,  2);
		  right = leftshift(right, 2);
    }
		 // Подключаем, заменяем и выбираем ПК-2 для перестановки и сжатия
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
	// Расширение электронного блока
	bitset<48> expandR;  
	for (int i = 0; i < 48; i++)
		expandR[i] = R[E[i]-1];  
  
  RCOUT("R  ",R);
  RCOUT("eR ",expandR);
  DCOUT("right_expanded ", expandR);
	 // XOR
	expandR = expandR ^ k;
	 // вместо этого S-поле
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
	 // Замена P-бокса
	bitset<32> tmp = output;
	for (int i = 0; i < 32; i++)
		output[i] = tmp[P[i] - 1];
 
	return output;
}
bitset<28> leftshift(bitset<28> k, int shift) {
  ///Сдвиг влево превращается в сдвиг вправо 
  ///из-за того, что ключ инвертирован

      for(int i=0; i<shift; i++) 
    {
        bool temp = k[0];
        k >>= 1; // - сдвиг вправо
        k[k.size()-1] = temp;
    }
    //       for(int i=0; i<shift; i++) 
    // {
    //     bool temp = k[k.size()-1];
    //     k <<= 1; // << - сдвиг влево
    //     k[0] = temp;
    // }
    return k;
}