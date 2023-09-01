  /*Inspired by http://www.trytoprogram.com/cpp-examples/cplusplus-program-encrypt-decrypt-string/ */
#include<string.h>
#include<math.h>

int p, q;
int n, fn;
int i;
int check;
long int e[50]; //Untuk menqimpan Public key
long int d[50]; //Untuk menqimpan Private key
long int temp[50];
long int j;
char c[50]; //Ciphertext
char m[50]; //Plaintext
String msg = "9DA2715"; //Input pesan
int check_prime(long int); //Untuk cek angka prima
long int cd(long int); //Untuk cek nilai K
void encryption_key();
void encrypt();
void decrypt();
void setup() {
  Serial.begin(115200);
  Serial.println("First Prime Number");
  p = 17;
  Serial.println(p);
  check = check_prime(p);
  if (check == 1) {
    Serial.println("P is a Prime Number");
  } else {
    Serial.println("P is NOT a Prime Number");
    exit(0);
  }

  Serial.println("Second Prime Number");
  q = 13;
  Serial.println(q);
  check = check_prime(q);
  if (check == 1) {
    Serial.println("Q is a Prime Number");
  } else {
    Serial.println("Q is NOT a Prime Number");
    exit(0);
  }
  
  Serial.println("Message or String To Encrypt\n");
  Serial.println(msg);
  int msg_len = msg.length() + 1;
  char msg_arr[16];
  msg.toCharArray(msg_arr, msg_len);
  for (i = 0; msg_arr[i] != 0 ; i++)
  {
    m[i] = msg [i];
  }
  n = p * q; //Untuk cari nilai N
  fn = ((p - 1) * (q - 1)); //Untuk mencari nilai dari private key
  encryption_key();
  unsigned long time1 = micros();
  encrypt();
  Serial.print("Waktu Encrypt : ");
  Serial.print(micros() - time1);
  Serial.println(" uS");
  unsigned long time2 = micros();
  decrypt();
  Serial.print("\nWaktu Decrypt : ");
  Serial.print(micros() - time2);
  Serial.println(" uS");
}

void loop() {
}

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
void encryption_key()
{
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
      check = cd(e[k]);
      if (check > 0)
      {
        d[k] = check;
        k++;
      }
      if (k == 99)
        break;
    }
  }
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

  while (i != len)
  {
    pt = m[i];
    pt = pt - 96; //Untuk mencegah character melebihi batas unsigned dan menjaga valua tetap dalam range
    k = 1;
    for (j = 0; j < key; j++) {
      k = k * pt;
      k = k % n;
    }
    temp[i] = k;
    ct = k + 96;
    c[i] = ct;
    i++;
  }
  c[i] = -1;
  Serial.println("\nThe Encrypted Message Is");
  for (i = 0; c[i] != -1; i++) {
    Serial.print(c[i]);
  }
  Serial.println("\nThe Encrypted Message In HEX");
  for (int i = 0; c[i] != -1 ; i++) {
    Serial.println(c[i] & 0xFF, DEC);
  }
  Serial.println("-----END OF HEX-----");
}
void decrypt() {
  long int pt, ct, key = d[0];
  long int k;
  int i = 0;
  while (c[i] != -1)
  {
    ct = temp[i];
    k = 1;
    for (j = 0; j < key; j++)
    {
      k = k * ct;
      k = k % n;
    }
    pt = k + 96;
    m[i] = pt;
    i++;
  }
  m[i] = -1;
  Serial.println("\nThe Decrypted Message Is");
  for (i = 0; m[i] != -1; i++) {
    Serial.print(m[i]);
  }
  
}
