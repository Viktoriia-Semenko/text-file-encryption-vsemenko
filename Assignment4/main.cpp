#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <dlfcn.h>

using namespace std;

class CaesarCipher
{
private:
    int buffer_size;
    void* handle;
    typedef char* (*encrypt_func)(char*, int);
    typedef char* (*decrypt_func)(char*, int);

    encrypt_func encrypt_pointer;
    decrypt_func decrypt_pointer;

public:
    explicit CaesarCipher(const char* path_to_lib, int buffer = 256) : buffer_size(buffer)
    {
        handle = dlopen(path_to_lib, RTLD_LAZY);
        if (!handle) {
            cerr << "Lib not found" << dlerror() << endl;
            exit(EXIT_FAILURE);
        }

        encrypt_pointer = (encrypt_func)dlsym(handle, "encrypt");
        decrypt_pointer = (decrypt_func)dlsym(handle, "decrypt");

        if (encrypt_pointer == nullptr || decrypt_pointer == nullptr) {
            cerr << "Proc not found" << dlerror() << endl;
            dlclose(handle);
            exit(EXIT_FAILURE);
        }
    }
    ~CaesarCipher(){
        if (handle){
            dlclose(handle);
        }
    }

    void encrypt(char* text, int key){
        char* result = encrypt_pointer(text, key);
        if (result) {
            strncpy(text, result, buffer_size - 1);
            text[buffer_size - 1] = '\0';
            free(result);
        } else {
            cerr << "Encryption failed" << endl;
        }
    }

    void decrypt(char* text, int key){
        char* result = decrypt_pointer(text, key);
        if (result) {
            strncpy(text, result, buffer_size - 1);
            text[buffer_size - 1] = '\0';
            free(result);
        } else {
            cerr << "Decryption failed" << endl;
        }
    }

};

class UndoRedoBuffer
{
private:
    char*** three_states;
    char*** redo_states;
    int row_number;
    int buffer_size;
    int index;
    int count_states;
    int redo_index;
    int count_redo_states;
    const int undo_buffer_size = 3;

public:
    UndoRedoBuffer(int rows, int buffer)
            : buffer_size(buffer), row_number(rows), index(0), redo_index(0), count_states(0), count_redo_states(0)
    {
        three_states = (char***)malloc(undo_buffer_size * sizeof(char**));
        redo_states = (char***)malloc(undo_buffer_size * sizeof(char**));

        if (three_states == nullptr || redo_states == nullptr){
            cerr << "Memory allocation failed" << endl;
            exit(EXIT_FAILURE);
        }

        // виділення памʼяті для трьох команд
        for (int i = 0; i < undo_buffer_size; i++) {
            three_states[i] = (char**)malloc(row_number * sizeof(char*));
            redo_states[i] = (char**)malloc(row_number * sizeof(char*));

            if (three_states[i] == nullptr || redo_states[i] == nullptr){
                cerr << "Memory allocation failed" << endl;
                exit(EXIT_FAILURE);
            }
            for (int j = 0; j < row_number; j++) {
                three_states[i][j] = (char*)malloc(buffer_size * sizeof(char));
                redo_states[i][j] = (char*)malloc(buffer_size * sizeof(char));
                if (three_states[i][j] == nullptr || redo_states[i][j] == nullptr){
                    cerr << "Memory allocation failed" << endl;
                    exit(EXIT_FAILURE);
                }
                three_states[i][j][0] = '\0';
            }

        }
    }

    ~UndoRedoBuffer() {
        for (int i = 0; i < undo_buffer_size; i++) {
            for (int j = 0; j < row_number; j++) {
                free(three_states[i][j]);
                free(redo_states[i][j]);
                three_states[i][j] = nullptr;
                redo_states[i][j] = nullptr;
            }
            free(three_states[i]);
            free(redo_states[i]);
            three_states[i] = nullptr;
            redo_states[i] = nullptr;
        }
        free(three_states);
        free(redo_states);
        three_states = nullptr;
        redo_states = nullptr;
    }

    void save_state(char** text) {
        for (int i = 0; i < row_number; i++) {
            strncpy(three_states[index][i], text[i], buffer_size - 1);
            three_states[index][i][buffer_size - 1] = '\0';

        }
        index = (index + 1) % undo_buffer_size; // оновлення індексу, щоб вказати потім на наступний буфер (їх три)
        if (count_states < undo_buffer_size) {
            count_states++;
        }
    }
    void save_redo_state(char** text) {
        for (int i = 0; i < row_number; i++) {
            strncpy(redo_states[redo_index][i], text[i], buffer_size - 1);
            redo_states[redo_index][i][buffer_size - 1] = '\0';
        }
        redo_index = (redo_index + 1) % undo_buffer_size;
        if (count_redo_states < undo_buffer_size) {
            count_redo_states++;
        }
    }

    bool load_state(char** text) {
        if (count_states == 0) {
            cout << "No undo could be done." << endl;
            return false;
        }
        index = (index + 2) % 3; // переміщення індексу назад до останнього збереженого стану
        for (int i = 0; i < row_number; i++) {
            strncpy(text[i], three_states[index][i], buffer_size - 1);
            text[i][buffer_size - 1] = '\0';
        }
        count_states--;
        return true;
    }
    bool load_redo_state(char** text) {
        if (count_redo_states == 0) {
            cout << "No redo could be done." << endl;
            return false;
        }
        redo_index = (redo_index + 2) % 3;
        for (int i = 0; i < row_number; i++) {
            strncpy(text[i], redo_states[redo_index][i], buffer_size - 1);
            text[i][buffer_size - 1] = '\0';
        }
        count_redo_states--;
        return true;
    }

};

class ConsoleInput
{
public:
    void append_text(){
        cout << "Enter a text to append: ";
        cin.ignore();
    }
    void line_index(int& line, int& index){
        cout << "Choose line and index: ";
        cin >> line >> index;
        cin.ignore();
    }
    void file_name(char* save_name){
        cout << "Enter the file name: ";
        cin >> save_name;
    }
    void search_text(){
        cout << "Enter text to search: ";
        cin.ignore();
    }
    void line_index_num(int& line, int& index, int& num_of_symbols){
        cout << "Choose line, index and number of symbols: ";
        cin >> line >> index >> num_of_symbols;
    }
    // new
    void input_file(char* file_path){
        cout << "Enter input file path: ";
        cin.getline(file_path, 100);
    }
    void output_file(char* path) {
        cout << "Enter output file path: ";
        cin.getline(path, 100);
    }

    void encrypt_decrypt_choice(int &choice) {
        cout << "Enter 1 to encrypt or 2 to decrypt: ";
        cin >> choice;
        cin.ignore();
    }

    void key_for_cipher(int &key) {
        cout << "Enter the key: ";
        cin >> key;
        cin.ignore();
    }
};

class FileHandler
{
private:
    ConsoleInput console_input;

    static bool file_exists(const char *filename){
        FILE* file_pointer = fopen(filename, "r");
        bool is_exists = false;
        if (file_pointer != nullptr){
            is_exists = true;
            fclose(file_pointer);
        }
        return is_exists;
    }

public:
    void save_info(char** text, int line_count) {
        char save_name[100];
        console_input.file_name(save_name);

        FILE* file;
        if (file_exists(save_name)) {
            cout << "Do you want to overwrite this file (y/n)?: ";
            char response;
            cin.ignore();
            cin.get(response);

            if (response == 'y') {
                file = fopen(save_name, "w");
            } else if (response == 'n') {
                file = fopen(save_name, "a");
                fseek(file, 0, SEEK_END);
            } else {
                std::cout << "Invalid answer" << std::endl;
                return;
            }

        } else {
            file = fopen(save_name, "w");
        }
        if (file == nullptr) {
            cerr << "Error while opening file" << endl;
            return;
        }

        for (int i = 0; i < line_count; ++i) {
            fprintf(file, "%s\n", text[i]);
        }
        fclose(file);
        cout << "Text has been saved successfully" << endl;
    }

    void load_info() {
        char load_name[100];

        console_input.file_name(load_name);

        FILE* file = fopen(load_name, "r");
        if (file == nullptr) {
            cerr << "Error opening file." << endl;
            return;
        }

        char my_string[256];
        while (fgets(my_string, sizeof(my_string), file) != nullptr) {
            cout << my_string;
        }
        fclose(file);
        cout << "Text has been loaded successfully" << endl;
    }
};

class Text
{
private:
//    char** text;
    char* clipboard; // буфер обміну
    int row_number;
    int buffer_size;
//    int line_count;
    UndoRedoBuffer undo_redo_buffer;
    ConsoleInput console_input;

    void save_state() {
        undo_redo_buffer.save_state(text);
    }
    void save_redo_state() {
        undo_redo_buffer.save_redo_state(text);
    }

public:
    char** text;
    int line_count;
    explicit Text (int rows = 10, int buffer = 256)
            : text(nullptr), row_number(rows), buffer_size(buffer), line_count(0), undo_redo_buffer(rows, buffer), clipboard(nullptr)
    {
        text = (char**)malloc(row_number * sizeof(char*));
        if (text == nullptr){
            cerr << "Cannot allocate memory for this input" << endl;
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < row_number; i++){
            text[i] = (char*) malloc(buffer_size * sizeof(char));
            if (text[i] == nullptr){
                cerr << "Cannot allocate memory for this input" << endl;
                exit(EXIT_FAILURE);
            }
            text[i][0] = '\0'; // ініціалізуємо кожен рядок як порожній
        }

        clipboard = (char*)malloc(buffer_size * sizeof(char));
        if (clipboard == nullptr) {
            cerr << "Cannot allocate memory for clipboard" << endl;
            exit(EXIT_FAILURE);
        }
        clipboard[0] = '\0';
    }

    ~Text() {
        for (int i = 0; i < row_number; i++) {
            free(text[i]);
            text[i] = nullptr;
        }
        free(text);
        text = nullptr;
        free(clipboard);
        clipboard = nullptr;
    }

    void append_text_to_end(){
        save_state();
        //save_redo_state();
        char* buffer = nullptr; // цей вказівник ще не використовується
        size_t local_buffer_size = 0;
        ssize_t input_length; // довжина рядка що зчитали

        console_input.append_text();

        // динамічно виділяємо памʼять та зберігаємо текст в буфері
        input_length = getline(&buffer, &local_buffer_size, stdin);

        if (input_length == -1){
            cerr << "Error while reading input.\n" << endl;
            free(buffer);
            return;
        }

        buffer[input_length - 1] = '\0'; // видалення нового рядка

        // якщо немає ніякого текст, то починаємо зпочатку
        if (line_count == 0){
            strncpy(text[line_count], buffer, local_buffer_size); // копіюємо введений текст у перший рядок
            line_count ++;
        } else{
            size_t current_length = strnlen(text[line_count - 1], buffer_size); // довжина останнього рядка (без вставленого тексту)
            size_t new_length = current_length + strnlen(buffer, local_buffer_size) + 1; // довжина того шо було + новий текст + \0

            // якщо нова довжина більша за виділену кількість в буфері
            if (new_length > local_buffer_size) {
                // перевиділяємо памʼять для вказівника
                text[line_count - 1] = (char*) realloc(text[line_count - 1], new_length * sizeof(char));
                if (text[line_count - 1] == nullptr) {
                    cerr << "Memory reallocation failed\n" << endl;
                    free(buffer);
                    return;
                }
            }
            strncat(text[line_count - 1], buffer, local_buffer_size); // додаємо введений текст до кінця останнього рядка
        }
        free(buffer);
        save_redo_state();
    }

    void start_new_line(){
        save_state();
        //save_redo_state();
        if (line_count >= row_number){
            row_number *= 2; // якщо недостатньо рядків, то виділяємо в два рази більше
            text = (char**)realloc(text, row_number * sizeof(char*));
            if (text == nullptr){
                cerr << "Fail to reallocate memory\n" << endl;
                exit(EXIT_FAILURE);
            }
            for (int i = line_count; i < row_number; i++) {
                text[i] = (char*)malloc(buffer_size * sizeof(char)); // виділяємо памʼять для нових рядків
                if (text[i] == nullptr){
                    cerr << "Memory allocating is failed\n" << endl;
                    exit(EXIT_FAILURE);
                }
                text[i][0] = '\0';
            }
        }
        text[line_count][0] = '\0';
        line_count++; // до підрахунку рядків додаємо рядок
        cout << "New line is started." << endl;
        //save_redo_state();
    }

    void insert_text_by_line() {
        save_state();
        // save_redo_state();
        int line, index;
        char* buffer = nullptr;
        size_t local_buffer_size = 0;
        ssize_t input_length;

        console_input.line_index(line, index);

        if (line >= line_count || index > strnlen(text[line], buffer_size)) {
            cout << "You entered invalid line or index." << endl;
            return;
        }

        cout << "Enter text to insert: ";
        cin.ignore();
        input_length = getline(&buffer, &local_buffer_size, stdin);
        if (input_length == -1) {
            cerr << "Error reading input" << endl;
            free(buffer);
            return;
        }

        buffer[input_length - 1] = '\0';

        size_t current_length = strnlen(text[line], buffer_size);
        size_t local_buffer_length = strnlen(buffer, local_buffer_size);
        size_t new_length = current_length + local_buffer_length + 1;

        if (new_length > buffer_size) {
            text[line] = (char*)realloc(text[line], new_length * sizeof(char));
            if (text[line] == nullptr) {
                cerr << "Memory reallocation failed." << endl;
                return;
            }
        }

        char* temporary_buffer = (char*)malloc((current_length - index + 1) * sizeof(char));
        if (temporary_buffer == nullptr) {
            cerr << "Memory allocation failed" << endl;
            free(buffer);
            return;
        }

        strncpy(temporary_buffer, text[line] + index, current_length - index);
        temporary_buffer[current_length - index] = '\0';
        text[line][index] = '\0';
        strncat(text[line], buffer, local_buffer_length);
        strncat(text[line], temporary_buffer, current_length - index);

        free(temporary_buffer);
        free(buffer);

        cout << "Text has been inserted." << endl;
        // save_redo_state();
    }

    void search_text() {
        char buffer[256];
        console_input.search_text();

        fgets(buffer, buffer_size, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        bool is_found = false;
        for (int i = 0; i < line_count; ++i) {
            char* position = text[i];
            while ((position = strstr(position, buffer)) != nullptr) {
                cout << "Text is present in this position: " << i << " " << position - text[i] << endl;
                position += strnlen(buffer, buffer_size);
                is_found = true;
            }
        }
        if (!is_found) {
            cout << "There is no match for the given text." << endl;
        }
    }

    void delete_text(int line = -1, int index = -1, int num_of_symbols = -1, bool ask_for_input = true){
        save_state();
        // save_redo_state();
        // input
        if (ask_for_input) {
            console_input.line_index_num(line, index, num_of_symbols);
        }

        // validation for the input
        if (line >= line_count || index > strnlen(text[line], buffer_size)) {
            cout << "You entered invalid line or index." << endl;
            return;
        }
        else if (index + num_of_symbols > strnlen(text[line], buffer_size)) {
            cout << "The number of symbols you want to deleted is bigger than number of symbols in buffer" << endl;
            return;
        }

        size_t current_length = strnlen(text[line], buffer_size); // довжина тексту в буфері зараз

        char* temporary_buffer = (char*)malloc((current_length) * sizeof(char));
        if (temporary_buffer == nullptr){
            cerr << "Memory allocation failed" << endl;
            return;
        }
        strncpy(temporary_buffer, text[line], index); // текст який був до індексу, після якого видаляти
        temporary_buffer[index] = '\0'; // нульове закінчення
        strncat(temporary_buffer, text[line] + index + num_of_symbols, strnlen(text[line], buffer_size) - index - num_of_symbols);
        // додаємо текст який залишився після видаленого

        strncpy(text[line], temporary_buffer, buffer_size - 1);
        text[line][buffer_size - 1] = '\0';

        free(temporary_buffer);
        cout << "Text has been deleted successfully." << endl;
        //save_redo_state();

    }

    void undo_command() {
        if (!undo_redo_buffer.load_state(text)) { // якщо буде false
            cout << "Undo failed. No previous index is available." << endl;
        } else {
            cout << "Undo successful." << endl;
        }
    }

    void redo_command() {
        if (!undo_redo_buffer.load_redo_state(text)) {
            cout << "Redo failed. No next index is available." << endl;
        } else {
            cout << "Redo successful." << endl;
        }
    }

    void copy_text(int line = -1, int index = -1, int num_of_symbols = -1, bool ask_for_input = true) {
        save_state();
        //save_redo_state();

        if (ask_for_input) {
            console_input.line_index_num(line, index, num_of_symbols);
        }


        if (line >= line_count || index > strnlen(text[line], buffer_size)) {
            cout << "You entered invalid line or index." << endl;
            return;
        }
        if (index + num_of_symbols > strnlen(text[line], buffer_size)) {
            cout << "The number of symbols you want to cut is bigger than the number of symbols in buffer" << endl;
            return;
        }

        strncpy(clipboard, text[line] + index, num_of_symbols); // копіємо обраний текст в буфер обміну
        clipboard[num_of_symbols] = '\0'; // зануляємо текст що додали в буфер
        cout << "Text has been copied to clipboard." << endl;
        //save_redo_state();
    }

    void cut_text() {
        int line, index, num_of_symbols;
        console_input.line_index_num(line, index, num_of_symbols);
        copy_text(line, index, num_of_symbols, false);
        delete_text(line, index, num_of_symbols, false);
    }

    void paste_text() {
        save_state();
        //save_redo_state();
        int line, index;
        console_input.line_index(line, index);

        if (line >= line_count || index > strnlen(text[line], buffer_size)) {
            cout << "You entered invalid line or index." << endl;
            return;
        }

        size_t clipboard_length = strnlen(clipboard, buffer_size); // довжина тексту в буфері обміну
        size_t current_length = strnlen(text[line], buffer_size); // довжина тексту в буфері
        size_t new_length = current_length + clipboard_length + 1; // нова довжина + \0

        if (new_length > buffer_size) { // виділяємо памʼять
            text[line] = (char*)realloc(text[line], new_length * sizeof(char));
            if (text[line] == nullptr) {
                cerr << "Memory reallocation failed." << endl;
                return;
            }
        }

        char* temporary_buffer = (char*)malloc((current_length - index + 1) * sizeof(char));
        // зберігання тексту що йде після вставки
        if (temporary_buffer == nullptr) {
            cerr << "Memory allocation failed" << endl;
            return;
        }

        strncpy(temporary_buffer, text[line] + index, current_length - index); // текст що йде після вставленого
        temporary_buffer[current_length - index] = '\0';
        text[line][index] = '\0';
        strncat(text[line], clipboard, clipboard_length); // додавання тексту з буфера обміну до рядка
        strncat(text[line], temporary_buffer, current_length - index); // додавання тексту що йде після вставленого

        free(temporary_buffer);
        cout << "Text has been pasted from clipboard." << endl;
        // save_redo_state();
    }

    void insert_with_replacement() {
        save_state();
        //save_redo_state();
        char* buffer = nullptr; // зберігання інпут тексту
        size_t local_buffer_size = 0;

        int line, index;
        console_input.line_index(line, index);

        if (line >= line_count || index > strnlen(text[line], buffer_size)) {
            cout << "You entered an invalid line or index." << endl;
            return;
        }

        cout << "Write text: ";
        ssize_t input_length = getline(&buffer, &local_buffer_size, stdin);

        if (input_length == -1) {
            cerr << "Error while reading input.\n" << endl;
            free(buffer);
            return;
        }

        buffer[input_length - 1] = '\0';

        size_t new_length = index + strnlen(buffer, local_buffer_size); // довжина вставленого тексту після вставки
        size_t current_length = strnlen(text[line], buffer_size); // початкова довжина тексту, який в буфері без змін

        if (new_length >= buffer_size) {
            text[line] = (char*)realloc(text[line], (new_length + 1) * sizeof(char));
            if (text[line] == nullptr) {
                cerr << "Memory reallocation failed." << endl;
                free(buffer);
                return;
            }
            buffer_size = new_length + 1;
        }

        size_t replace_length = current_length - index; // обчислення довжини тексту, який треба замінити
        if (replace_length > strnlen(buffer, local_buffer_size)) // чи довжина заміни більша за довжину нового тексту
            replace_length = strnlen(buffer, local_buffer_size); // тоді довжина заміна - довжина тексту інпут

        strncpy(text[line] + index, buffer, replace_length); // копіювання в рядок починаючи з індекса вставки
        if (replace_length < strnlen(buffer, local_buffer_size))
            strncat(text[line], buffer + replace_length, strnlen(buffer + replace_length, local_buffer_size));
        // додавання тексту що залишився після вставки нового

        free(buffer);

        cout << "Text has been inserted with replacement." << endl;
        //save_redo_state();
    }
};

class CommandLineInterface
{
private:
    Text text;
    CaesarCipher cipher;
    ConsoleInput console_input;
public:
    static void print_help() {
        cout << "This program is the 'Simple Text Editor'\n"
             << "It implements the following commands:\n"
             << "0 - See the commands\n"
             << "1 - Append text symbols to the end\n"
             << "2 - Start the new line\n"
             << "3 - Saving the information\n"
             << "4 - Loading the information\n"
             << "5 - Print the current text to console\n"
             << "6 - Insert the text by line and symbol index\n"
             << "7 - Search for the text\n"
             << "8 - Delete the text by line and index\n"
             << "9 - Undo latest command\n"
             << "10 - Redo latest command\n"
             << "11 - Cut text\n"
             << "12 - Copy text\n"
             << "13 - Paste text\n"
             << "14 - Insert text with replacement\n"
             << "15 - Encrypt text\n"
             << "16 - Decrypt text\n";
    }

    void print_text(char** text, int line_count) {
        if (text == nullptr) {
            cout << "Text is empty" << endl;
            return;
        }
        for (int i = 0; i < line_count; ++i) {
            if (text[i] != nullptr){
                cout << text[i] << endl;
            }
        }
    }

    void encrypt_decrypt(){
        int choice, key;
        char input_path[100], output_path[100];

        console_input.encrypt_decrypt_choice(choice);
        console_input.input_file(input_path);
        console_input.output_file(output_path);
        console_input.key_for_cipher(key);

        FILE* input_file = fopen(input_path, "r");
        if (input_file == nullptr) {
            cout << "Error opening input file" << endl;
            return;
        }

        FILE* output_file = fopen(output_path, "w");
        if (output_file == nullptr) {
            cout << "Error opening output file." << endl;
            fclose(input_file);
            return;
        }

        char line[1024];
        while (fgets(line, sizeof(line), input_file)) {
            if (choice == 1) {
                cipher.encrypt(line, key);
            } else if (choice == 2) {
                cipher.decrypt(line, key);
            }
            fputs(line, output_file);
        }

        fclose(input_file);
        fclose(output_file);
        cout << "Operation is successful" << endl;
    }

    explicit CommandLineInterface(const CaesarCipher& cipher) : cipher(cipher) {
    }
};

enum Commands {
    COMMAND_HELP = 0,
    COMMAND_APPEND = 1,
    COMMAND_NEW_LINE = 2,
    COMMAND_SAVE = 3,
    COMMAND_LOAD = 4,
    COMMAND_PRINT = 5,
    COMMAND_INSERT_LI = 6,
    COMMAND_SEARCH = 7,
    COMMAND_DELETE = 8,
    COMMAND_UNDO = 9,
    COMMAND_REDO = 10,
    COMMAND_CUT = 11,
    COMMAND_COPY = 12,
    COMMAND_PASTE = 13,
    COMMAND_INSERT_REPLACE = 14,
    COMMAND_ENCRYPT_DECRYPT = 15
};

int main() {
    int rows = 10;
    int buffer_size = 256;
    int user_command;

    Text text(rows, buffer_size);
    CommandLineInterface command_line(CaesarCipher(nullptr));
    FileHandler file_handler;
    CaesarCipher caesar_cipher("libcaesar.so");

    command_line.print_help();
    while (true) {
        char continue_input[2];
        cout << "Enter the command: ";
        if (scanf("%d", &user_command) != 1) {
            cout << "Invalid input. Please, enter the number given in the help-menu.\n" << endl;
            int a;
            while ((a = getchar()) != '\n') {}
            continue;
        }
        switch (user_command) {
            case COMMAND_HELP:
                command_line.print_help();
                break;
            case COMMAND_APPEND:
                text.append_text_to_end();
                break;
            case COMMAND_NEW_LINE:
                text.start_new_line();
                break;
            case COMMAND_SAVE:
                file_handler.save_info(text.text, text.line_count);
                break;
            case COMMAND_LOAD:
                file_handler.load_info();
                break;
            case COMMAND_PRINT:
                command_line.print_text(text.text, text.line_count);
                break;
            case COMMAND_INSERT_LI:
                text.insert_text_by_line();
                break;
            case COMMAND_SEARCH:
                text.search_text();
                break;
            case COMMAND_DELETE:
                text.delete_text();
                break;
            case COMMAND_UNDO:
                text.undo_command();
                break;
            case COMMAND_REDO:
                text.redo_command();
                break;
            case COMMAND_CUT:
                text.cut_text();
                break;
            case COMMAND_COPY:
                text.copy_text();
                break;
            case COMMAND_PASTE:
                text.paste_text();
                break;
            case COMMAND_INSERT_REPLACE:
                text.insert_with_replacement();
                break;
            case COMMAND_ENCRYPT_DECRYPT:
                command_line.encrypt_decrypt();
            default:
                printf("This command is not implemented\n");
        }
        cout << "\nDo you want to continue?:(y/n) ";
        cin >> continue_input;
        if (strcmp(continue_input, "y") == 0) {
            continue;
        } else if (strcmp(continue_input, "n") == 0) {
            break;
        } else cout << "Enter a valid answer\n" << endl;
    }
    return 0;
}
