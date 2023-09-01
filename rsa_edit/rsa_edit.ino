/*Inspired by http://www.trytoprogram.com/cpp-examples/cplusplus-program-encrypt-decrypt-string/ */
#include<string.h>
#include<math.h>

// 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97
int p = 5;
int q = 29;
int n, fn;
int i;
int check;
long int e[50]; //Untuk menqimpan Public key
long int d[50]; //Untuk menqimpan Private key
long int temp[50];
long int j;
char c[50]; //Ciphertext
char m[50]; //Plaintext
String msg = "E9DA2715"; //Input pesan

int check_prime(long int pri) {
  int i;
  for (i = 2; i <= pri - 1; i++) {
    if (pri % i == 0)
      return 0;
  }
  if (i == pri)
    return 1;
}

//Function untuk menghasilkan public dan private key
int encryption_key() {
  int k;
  k = 0;
  for (i = 2; i < fn; i++)
  {
    if (fn % i == 0)
      continue;

    check = check_prime(i);

    if (check == 1 && i != p && i != q)
    {
      e[k] = i;
      //Serial.print("e["); Serial.print(k);  Serial.print("]="); Serial.println(e[k]);
      check = cd(e[k]);
      if (check > 0)
      {
        d[k] = check;
        //Serial.print("d["); Serial.print(k);  Serial.print("]="); Serial.println(e[k]);
        k++;
      }
      if (k == 99)
        break;
    }
  }
  return k;
}
long int cd (long int a) {
  long int k = 1;
  while (1) {
    k = k + fn;
    if (k % a == 0)
      return (k / a);
  }
}
void encrypt() {


  long int pt, ct, key = e[0];
  long int k, len;
  int i = 0;
  len = msg.length();

  Serial.print("e = "); Serial.println(key);

  Serial.println();  
  Serial.println("---------Step 4");

  Serial.print("Message To Encrypt = ");  Serial.println(msg);
  Serial.print("Message In DEC = ");
  for (int i = 0; msg[i] != 0 ; i++) {
    Serial.print(msg[i] & 0xFF, DEC);
    Serial.print(" ");
  }
  Serial.println(); 
  Serial.println();
  
  while (i != len)
  {
    pt = m[i];
    //pt = pt - 96; //Untuk mencegah character melebihi batas unsigned dan menjaga valua tetap dalam range
    pt = pt;
    k = 1;
    for (j = 0; j < key; j++) {
      k = k * pt;
      k = k % n;
    }
    temp[i] = k;
    //ct = k + 96;
    ct = k;
    c[i] = ct;
    i++;
  }
  c[i] = -1;
  Serial.print("The Encrypted Message Is = ");
  for (i = 0; c[i] != -1; i++) {
    Serial.print(c[i]);
  }
  Serial.println();


  Serial.print("The Encrypted Message In DEC = ");
  for (int i = 0; c[i] != -1 ; i++) {
    Serial.print(c[i] & 0xFF, DEC);
    Serial.print(" ");
  }
  Serial.println();
}
void decrypt() {
  long int pt, ct, key = d[0];
  long int k;
  int i = 0;

  Serial.println(); 
  Serial.println("---------Step 5");
  Serial.print("d = "); Serial.println(key);

  while (c[i] != -1)
  {
    ct = temp[i];
    k = 1;
    for (j = 0; j < key; j++)
    {
      k = k * ct;
      k = k % n;
    }
    //pt = k + 96;
    pt = k;
    m[i] = pt;
    i++;
  }
  m[i] = -1;
  Serial.print("\nThe Decrypted Message Is = ");
  for (i = 0; m[i] != -1; i++) {
    Serial.print(m[i]);
  }

}

//Reference https://www.cs.drexel.edu/~jpopyack/IntroCS/HW/RSAWorksheet.html
void setup() {
  Serial.begin(115200);
  Serial.println("---------RSA Calculator");

  Serial.println();
  Serial.println("---------Step 1");

  Serial.print ("First Prime Number (P) = "); Serial.println(p);
  check = check_prime(p);
  if (check == 1) {
    Serial.println("P is a Prime Number");
  } else {
    Serial.println("P is NOT a Prime Number");
    exit(0);
  }


  Serial.print("Second Prime Number (Q) = "); Serial.println(q);
  check = check_prime(q);
  if (check == 1) {
    Serial.println("Q is a Prime Number");
  } else {
    Serial.println("Q is NOT a Prime Number");
    exit(0);
  }

  int msg_len = msg.length() + 1;
  char msg_arr[16];
  msg.toCharArray(msg_arr, msg_len);
  for (i = 0; msg_arr[i] != 0 ; i++)
  {
    m[i] = msg [i];
    //Serial.println(m[i]);
  }
  n = p * q; //Untuk cari nilai N
  Serial.print("Nilai N = "); Serial.println(n);

  fn = ((p - 1) * (q - 1)); //Untuk mencari nilai dari private key
  Serial.print("Nilai r = "); Serial.println(fn);

//  Serial.println();
//  Serial.println("---------Step 2");
  //Serial.print("Nilai K = "); Serial.println(encryption_key());

  encryption_key();
  
  unsigned long time1 = micros();

  Serial.println();
  Serial.println("---------Step 3");
  
  encrypt();
  Serial.print("Waktu Encrypt (uS) = ");  Serial.println(micros() - time1);
  unsigned long time2 = micros();
  decrypt();
  Serial.print("\nWaktu Decrypt (uS) = ");  Serial.print(micros() - time2);
}

void loop() {
}
