#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

using namespace std;

//encryption
void encrypt(string filename, int n, string message);
//decryption
void decrypt(string input, string output, long long p, long long q);
//modular exoponentiation, calculate C = M^e mod n
long long modExp(long long M, long long e, long long n);
//calculate decryption key d
long long decryptKey(long long p, long long q);
//gcd
long long gcd(long long a, long long b);

int main(int argc, char* argv[]) {
    //takes in two command line arguments, p and q
    if (argc != 3) {
        cout << "Error: invalid number of command line arguments" << endl;
        return 0;
    }
    //convert p and q to long long
    long long p = stoll(argv[1]);
    long long q = stoll(argv[2]);
    
    //prompt user for command
    while(true) {
        string commandLine;
        getline(cin, commandLine);
        //pasrse command line
        stringstream ss(commandLine);
        string command;
        ss >> command;
        //end gracefully if EXIT
        if (command == "EXIT") {
            break;
        }
        //encrypt
        else if (command == "ENCRYPT") {
            string filename, nStr, message;
            ss >> filename >> nStr;
            ss.ignore();
            getline(ss, message);
            //convert n to int
            int n = stoll(nStr);
            if (filename.length() == 0 || nStr.length() == 0 || message.length() == 0) {
                cout << "Error: invalid command line arguments" << endl;
                continue;
            }
            encrypt(filename, n, message);
        }
        //decrypt
        else if (command == "DECRYPT") {
            string input, output;
            ss >> input >> output;
            if (input.length() == 0 || output.length() == 0) {
                cout << "Error: invalid command line arguments" << endl;
                continue;
            }
            decrypt(input, output, p, q);
        }
    }
    return 0;
}

void encrypt(string filename, int n, string message) {
    long long e = 65537;
    //if n < 27 throw error, prompt new command
    if (n < 27) {
        cout << "Error: n must be greater than 27, please re-enter" << endl;
        return;
    }
    //x = 1+log_100 (n/27), round down
    size_t x = floor(1 + log10(n/27)/log10(100));

    //write C to output file
    ofstream output(filename);
    //iterate through entiring message, converting x chars at a time
    for (size_t i = 0; i < message.length(); i += x) {
        //get x chars
        string temp = message.substr(i, x);
        //if message length < x, add white space to the end
        while (temp.length() < x) {
            temp += ' ';
        }
        //convert each character to two chars, white sapce is 00, a is 01, b is 02, etc.
        //create a new string to hold the encoded message
        string encoded = "";
        for (size_t j = 0; j < temp.length(); j++) {
            //white space
            if (temp[j] == ' ') {
                encoded += "00";
            }
            //a to z
            else if (temp[j] >= 'a' && temp[j] <= 'z') {
                int val = temp[j] - 'a' + 1;
                //if val < 10, add a 0 in front
                if (val < 10) {
                    encoded += "0" + to_string(val);
                }
                else {
                    encoded += to_string(val);
                }
            }
            else {
                cout << "Error: invalid character in message" << endl;
                return;
            }
        }
        //convert encoded string to int M
        long long M = stoll(encoded);

        //calculate C = M^e mod n
        long long C = modExp(M, e, n);
        output << C << " ";
    }
    output.close();
}

long long modExp(long long M, long long e, long long n) {
    long long C = 1; //result
    long long power = M % n;
    //convert e to binary
    //string binary = "";
    while (e > 0) {
        if (e % 2 == 1) {
            C = (C * power) % n;
        }
        power = (power * power) % n;
        e /= 2;
    }
    return C;
}

void decrypt(string input, string output, long long p, long long q) {
    long long d = decryptKey(p, q);
    //cout << "Decryption key d: " << d << endl;
    long long n = p * q;
    //cout << "n: " << n << endl;
    //read in input file, read each long long
    ifstream infile(input);
    vector<long long> nums;
    long long C;
    while (infile >> C) {
       // cout << "Encrypted C: " << C << endl;
        nums.push_back(C);
    }
    infile.close();

    //write decoded to output file
    ofstream outfile(output);

    //decrypt each num until encrypted message ends
    for (size_t i = 0; i < nums.size(); i++) {
        //calculate M = C^d mod n using Modular Exponentiation Algo
        //cout << "Encrypted C: " << nums[i] << endl;
        long long M = modExp(nums[i], d, n);
        //cout << "Decrypted M: " << M << endl;
        //convert M to x letters and spaces
        //convert M to string
        string temp = to_string(M);
        size_t x = floor(1 + log10(n/27)/log10(100));
        while (temp.length() < 2*x) {
            temp = "0" + temp;
        }
        //cout << "Decrypted temp: " << temp << endl;
        //segment M by 2 digits
        string decoded = "";
        for (size_t j = 0; j < temp.length(); j += 2) {
            //get 2 digits
            string digits = temp.substr(j, 2);
            int value = stoll(digits);
            if (value == 0) {
                decoded += ' ';
            }
            else {
                decoded += (char)(value + 'a' - 1);
            }
        }
        //cout << "Decoded segment: " << decoded << endl;
        outfile << decoded;
    }
    outfile.close();
}

long long decryptKey(long long p, long long q) {
    long long e = 65537;
    //L = lcm(p-1, q-1) = (p-1)*(q-1)/gcd(p-1, q-1)
    long long L = (p-1)*(q-1)/gcd(p-1, q-1);
    if (L <= e) {
        cout << "Error: L must be greater than e" << endl;
        return 0;
    }
    //d satisfies d*e mod L = 1, find through extended euclidean algorithm
    long long s = 0, old_s = 1; //old_s is y
    long long t = 1, old_t = 0; //old_t is d
    long long r = e, old_r = L; //old_r is gcd
    while (r != 0) {
        long long quotient = floor(old_r/r);
        long long temp = r;
        r = old_r - quotient * r;
        old_r = temp;
        temp = s;
        s = old_s - quotient * s;
        old_s = temp;
        temp = t;
        t = old_t - quotient * t;
        old_t = temp;
    }
    //if d is negative, add e*L until it's a positive int < e*L
    while (old_t < 0) {
        old_t += L;
    }
    if ((old_t * e) % L != 1) {
        cout << "Error: d is incorrect" << endl;
        return 0;
    }

    //if gcd is not 1, throw error
    if (old_r != 1) {
        cout << "Error: gcd of e and L must be 1" << endl;
        return 0;
    }
    return old_t;
}

long long gcd(long long a, long long b) {
    long long x = a, y = b;
    while (y != 0) {
        long long r = x % y;
        x = y;
        y = r;
    }
    return x;
}
