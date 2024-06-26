#include <iostream>
#include <cstring>

using namespace std;

extern "C" {

char *encrypt(char *raw_text, int key) {
    size_t text_length = strlen(raw_text);
    char *encrypted_text = (char *) malloc((text_length + 1) * sizeof(char));

    for (int i = 0; i < text_length; i++) {
        char letter = raw_text[i];

        if ('A' <= letter && letter <= 'Z') { // 65 - 'A', 90 - 'Z' in ASCII
            encrypted_text[i] = (letter - 'A' + key) % 26 + 'A';
        } else if ('a' <= letter && letter <= 'z') { // 97 - 'a', 122 - 'z'
            encrypted_text[i] = (letter - 'a' + key) % 26 + 'a';
        } else {
            encrypted_text[i] = letter;
        }
    }
    encrypted_text[text_length] = '\0';

    return encrypted_text;
}

char *decrypt(char *encrypted_text, int key) {
    size_t text_length = strlen(encrypted_text);
    char *decrypted_text = (char *) malloc((text_length + 1) * sizeof(char));

    for (int i = 0; i < text_length; i++) {
        char letter = encrypted_text[i];

        if ('A' <= letter && letter <= 'Z') { // 65 - 'A', 90 - 'Z' in ASCII
            decrypted_text[i] = (letter - 'A' - key + 26) % 26 + 'A';
        } else if ('a' <= letter && letter <= 'z') { // 97 - 'a', 122 - 'z'
            decrypted_text[i] = (letter - 'a' - key + 26) % 26 + 'a';
        } else {
            decrypted_text[i] = letter;
        }
    }
    decrypted_text[text_length] = '\0';

    return decrypted_text;
}
}
